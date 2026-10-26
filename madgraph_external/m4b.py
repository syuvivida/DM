#!/usr/bin/env python3
"""
Invariant mass of the four leading b-tagged jets from a Delphes ROOT file.

Usage:
    python3 m4b.py tag_1_delphes_events.root [--out m4b] [--ptmin 20] [--etamax 2.5]
                   [--btag-bit 0] [--nb 4] [--max-events N] [--jet-branch Jet]

Requirements: pip install uproot awkward numpy matplotlib

Selection (per event):
  * jets from the `Jet` branch with pT > ptmin and |eta| < etamax
  * b-tagged: bit `btag-bit` of Jet.BTag is set (bit 0 in your delphes_card.dat)
  * at least `nb` b-tagged jets (default 4); the 4 leading-pT b-jets are used
    (with --nb 3, the 4th jet is the leading non-b jet)

Outputs:
  <out>.png  : m4b histogram, plus m(bb) of the two pairs from the min |m1 - m2| pairing
  <out>.npz  : arrays m4b, mbb1, mbb2, pt4b, event weight
  printed    : cut flow and summary
"""
import argparse
import sys

import awkward as ak
import numpy as np
import uproot


def four_vectors(pt, eta, phi, mass):
    px = pt * np.cos(phi)
    py = pt * np.sin(phi)
    pz = pt * np.sinh(eta)
    e = np.sqrt(px**2 + py**2 + pz**2 + mass**2)
    return px, py, pz, e


def inv_mass(px, py, pz, e):
    m2 = e**2 - px**2 - py**2 - pz**2
    return np.sqrt(np.maximum(m2, 0.0))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("file")
    ap.add_argument("--out", default="m4b")
    ap.add_argument("--jet-branch", default="Jet")
    ap.add_argument("--ptmin", type=float, default=20.0)
    ap.add_argument("--etamax", type=float, default=2.5)
    ap.add_argument("--btag-bit", type=int, default=0)
    ap.add_argument("--nb", type=int, default=4, help="minimum number of b-tagged jets among the 4 used (3 or 4)")
    ap.add_argument("--max-events", type=int, default=None)
    ap.add_argument("--bins", type=int, default=60)
    ap.add_argument("--range", type=float, nargs=2, default=(0.0, 800.0))
    args = ap.parse_args()

    J = args.jet_branch
    tree = uproot.open(args.file)["Delphes"]
    branches = [f"{J}.PT", f"{J}.Eta", f"{J}.Phi", f"{J}.Mass", f"{J}.BTag", "Event.Weight"]
    a = tree.arrays(branches, entry_stop=args.max_events)
    nev = len(a)

    pt, eta, phi, mass = a[f"{J}.PT"], a[f"{J}.Eta"], a[f"{J}.Phi"], a[f"{J}.Mass"]
    isb = ((a[f"{J}.BTag"] >> args.btag_bit) & 1) == 1
    w = ak.to_numpy(ak.fill_none(ak.firsts(a["Event.Weight"]), 1.0))

    good = (pt > args.ptmin) & (abs(eta) < args.etamax)
    nbjet = ak.sum(good & isb, axis=1)
    njet = ak.sum(good, axis=1)

    # order: selected b-jets (by pT), then selected non-b jets (by pT), then jets failing pT/eta
    rank = ak.where(good & isb, 0, ak.where(good, 1, 2))
    order = ak.argsort(rank * 1e7 - pt, axis=1)
    sel_pt, sel_eta, sel_phi, sel_m = pt[order], eta[order], phi[order], mass[order]
    sel_good, sel_b = good[order], isb[order]

    passed = (nbjet >= args.nb) & (njet >= 4)
    pt4 = ak.to_numpy(sel_pt[passed][:, :4])
    eta4 = ak.to_numpy(sel_eta[passed][:, :4])
    phi4 = ak.to_numpy(sel_phi[passed][:, :4])
    m4 = ak.to_numpy(sel_m[passed][:, :4])
    if not np.all(ak.to_numpy(sel_good[passed][:, :4])):
        sys.exit("internal error: non-selected jet among the four used")

    px, py, pz, e = four_vectors(pt4, eta4, phi4, m4)
    m4b = inv_mass(px.sum(1), py.sum(1), pz.sum(1), e.sum(1))
    pt4b = np.hypot(px.sum(1), py.sum(1))

    # pair into two bb systems (h -> bb candidates): minimise |m_bb1 - m_bb2|
    pairings = [((0, 1), (2, 3)), ((0, 2), (1, 3)), ((0, 3), (1, 2))]
    mpairs = []
    for (i, j), (k, l) in pairings:
        m1 = inv_mass(px[:, i] + px[:, j], py[:, i] + py[:, j], pz[:, i] + pz[:, j], e[:, i] + e[:, j])
        m2 = inv_mass(px[:, k] + px[:, l], py[:, k] + py[:, l], pz[:, k] + pz[:, l], e[:, k] + e[:, l])
        mpairs.append((m1, m2))
    dm = np.stack([abs(m1 - m2) for m1, m2 in mpairs], axis=1)
    best = np.argmin(dm, axis=1) if len(dm) else np.array([], dtype=int)
    m1 = np.choose(best, [p[0] for p in mpairs]) if len(best) else np.array([])
    m2 = np.choose(best, [p[1] for p in mpairs]) if len(best) else np.array([])
    mbb_lead = np.maximum(m1, m2)
    mbb_sub = np.minimum(m1, m2)

    wp = w[ak.to_numpy(passed)]

    # ---- cut flow ----
    print(f"file: {args.file}")
    print(f"events read                         : {nev}")
    print(f">= 4 jets (pT>{args.ptmin}, |eta|<{args.etamax})      : {int(ak.sum(njet >= 4))}")
    for k in range(5):
        print(f"   events with exactly {k} b-tagged jets : {int(ak.sum(nbjet == k))}")
    print(f"   events with >= 5 b-tagged jets      : {int(ak.sum(nbjet >= 5))}")
    print(f"selected (>= {args.nb} b-tags, 4 jets)       : {len(m4b)}  ({100.0 * len(m4b) / max(nev, 1):.2f} %)")
    if len(m4b):
        print(f"m4b   mean {np.mean(m4b):7.1f}  median {np.median(m4b):7.1f}  GeV")
        print(f"m_bb  (leading/subleading pair) median {np.median(mbb_lead):6.1f} / {np.median(mbb_sub):6.1f} GeV")

    np.savez(f"{args.out}.npz", m4b=m4b, mbb1=mbb_lead, mbb2=mbb_sub, pt4b=pt4b, weight=wp)

    # ---- plots ----
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib not installed: skipping the plot")
        return
    fig, ax = plt.subplots(1, 2, figsize=(11, 4.2))
    ax[0].hist(m4b, bins=args.bins, range=args.range, histtype="step", lw=1.8, color="#1f4e8c")
    ax[0].set_xlabel(r"$m_{4b}$ [GeV]")
    ax[0].set_ylabel("events / bin")
    ax[0].set_title(f"4 leading b-jets (≥{args.nb} b-tags), N = {len(m4b)}")
    ax[1].hist(mbb_lead, bins=50, range=(0, 300), histtype="step", lw=1.8, label="leading pair", color="#1f4e8c")
    ax[1].hist(mbb_sub, bins=50, range=(0, 300), histtype="step", lw=1.8, label="subleading pair", color="#c0392b")
    ax[1].axvline(125, ls=":", color="gray")
    ax[1].set_xlabel(r"$m_{bb}$ [GeV]  (min $|m_1-m_2|$ pairing)")
    ax[1].legend(frameon=False)
    for x in ax:
        x.spines[["top", "right"]].set_visible(False)
    fig.tight_layout()
    fig.savefig(f"{args.out}.png", dpi=150)
    print(f"wrote {args.out}.png and {args.out}.npz")


if __name__ == "__main__":
    main()
