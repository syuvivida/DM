#!/usr/bin/env python3
"""
Write one MadGraph param_card per (m_A, m_H) point of the minimizer table bestPar_nonAligned_AHhha.dat
(minimizer_AHhha_nonAligned.C / getBestPar_mA_mH.sh), and a script that generates the signal events for every card
in the existing MadGraph process directory (gg > h3 [noborn=QCD], MadSpin: h3 > h2 h4, h2 > h1 h1, h4 > xd~ xd,
Pythia8 with h1 -> b b~, Delphes).

usage:
    python3 make_cards_from_minimizer.py bestPar_nonAligned_AHhha.dat TEMPLATE [options]

    TEMPLATE   a param_card.dat of the Pseudoscalar_2HDMI model, e.g. workdir_AHhhaa_4b/Cards/param_card.dat
               (a run banner *_banner.txt also works: its <slha> block is used)
options:
    --outdir DIR        where the cards and scripts go (default: cards_minimizer)
    --points LIST       only these points, e.g. "500:260,650:310" (default: all)
    --min-sigbr X       only points with sigma x BR >= X fb (default 0)
    --top N             only the N points with the largest sigma x BR
    --include-disallowed  also write cards for points with allowed = 0 (default: skipped)
    --suffix S          appended to the run names, so they do not overwrite runs made with other parameters
                        (default: _min; e.g. mA500_mH260_min, MadSpin output mA500_mH260_min_decayed_1)
    --mxd M --gpxd G    DM mass and coupling (default 10 and 1, as in the minimizer)
    --proc PROC         MadGraph process directory (default: the workdir_AHhhaa_4b next to this MadGraph)
    --nevents N         events per run (default: keep the run_card value)

Each card has: tanbeta, sinbma, sinp, lam3, lap1, lap2 from the minimizer; mh2 = m_H, mh3 = m_A, mh4 = m_a,
mhc = m_H+- from the table; mxd, gpxd; DECAY 25, 35, 36, 37, 55 set to Auto, so that MadGraph recomputes all widths
(including the 3-body ones) for every point.

Writes into DIR:
    param_card_<tag>.dat   one card per point
    summary.csv            the points, their parameters and the minimizer's expected sigma x BR
    run_all.sh             runs all points in PROC one after the other; skips points that already have Delphes
                           output, so it can simply be restarted after an interruption
"""
import argparse
import csv
import os
import re

# param_card name (as written in the comment after the value) -> column of bestPar_nonAligned_AHhha.dat
PARAMS = {"tanbeta": "tanb", "sinbma": "sinbma", "sinp": "sinp", "lam3": "lam3", "lap1": "laP1", "lap2": "laP2",
          "mh2": "mH", "mh3": "mA", "mh4": "ma", "mhc": "mHc"}
AUTO_WIDTHS = (25, 35, 36, 37, 55)   # h, H, A, H+-, a
DEFAULT_PROC = "/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/workdir_AHhhaa_4b"


def read_table(path):
    rows = []
    with open(path) as f:
        head = f.readline().split()
        for line in f:
            v = line.split()
            if len(v) == len(head):
                rows.append({k: float(x) for k, x in zip(head, v)})
    return rows


def read_template(path):
    text = open(path).read()
    if "<slha>" in text:
        text = text.split("<slha>")[1].split("</slha>")[0].lstrip("\n")
    return text.splitlines()


def make_card(lines, values, auto=AUTO_WIDTHS):
    """Card text with the parameters in `values` (param_card name -> number) replaced and the widths of the
    particles in `auto` set to Auto (their BR lines removed)."""
    out, seen, skip_br = [], set(), False
    for line in lines:
        s = line.strip()
        m = re.match(r"DECAY\s+(-?\d+)\s+(\S+)(.*)", s, re.I)
        if m:
            skip_br = abs(int(m.group(1))) in auto
            if skip_br:
                out.append("DECAY  %d   Auto %s" % (int(m.group(1)), m.group(3).strip()))
                continue
        elif skip_br and s and not s.startswith("#") and not re.match(r"BLOCK", s, re.I):
            continue                                   # BR line of an Auto width: drop
        elif skip_br and re.match(r"BLOCK", s, re.I):
            skip_br = False
        pm = re.match(r"^(\s*(?:-?\d+\s+)+)(\S+)(\s*#\s*)(\S+)(.*)$", line)
        if pm and pm.group(4).lower() in values and not re.match(r"\s*DECAY", line, re.I):
            name = pm.group(4).lower()
            line = "%s%.6e%s%s%s" % (pm.group(1), values[name], pm.group(3), pm.group(4), pm.group(5))
            seen.add(name)
        out.append(line)
    missing = set(values) - seen
    if missing:
        raise ValueError("parameters not found in the template card: %s" % ", ".join(sorted(missing)))
    return "\n".join(out) + "\n"


def tag(mA, mH, suffix):
    f = lambda x: ("%g" % x).replace(".", "p")
    return "mA%s_mH%s%s" % (f(mA), f(mH), suffix)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("table")
    ap.add_argument("template")
    ap.add_argument("--outdir", default="cards_minimizer")
    ap.add_argument("--points", default="")
    ap.add_argument("--min-sigbr", type=float, default=0.)
    ap.add_argument("--top", type=int, default=0)
    ap.add_argument("--include-disallowed", action="store_true")
    ap.add_argument("--suffix", default="_min")
    ap.add_argument("--mxd", type=float, default=10.)
    ap.add_argument("--gpxd", type=float, default=1.)
    ap.add_argument("--proc", default=DEFAULT_PROC)
    ap.add_argument("--nevents", type=int)
    a = ap.parse_args()

    rows = [r for r in read_table(a.table) if r["xsecBR_pb"] > 0]
    if not a.include_disallowed:
        rows = [r for r in rows if r.get("allowed", 1) > 0]
    if a.points:
        want = {tuple(float(x) for x in p.split(":")) for p in a.points.split(",") if p}
        rows = [r for r in rows if (r["mA"], r["mH"]) in want]
        found = {(r["mA"], r["mH"]) for r in rows}
        for p in sorted(want - found):
            print("WARNING: point mA=%g mH=%g not in the table (or not allowed)" % p)
    rows = [r for r in rows if r["xsecBR_pb"] * 1e3 >= a.min_sigbr]
    rows.sort(key=lambda r: -r["xsecBR_pb"])
    if a.top:
        rows = rows[:a.top]
    if not rows:
        raise SystemExit("no point selected")

    lines = read_template(a.template)
    os.makedirs(a.outdir, exist_ok=True)
    written = []
    for r in rows:
        vals = {card: r[col] for card, col in PARAMS.items()}
        vals["sinbma"] = min(vals["sinbma"], 1.0)
        vals["mxd"], vals["gpxd"] = a.mxd, a.gpxd
        t = tag(r["mA"], r["mH"], a.suffix)
        head = ("# 2HDM+a point %s from %s (minimizer_AHhha_nonAligned.C)\n"
                "# expected sigma(gg->A) x BR(A->Ha) x BR(H->hh) x BR(a->chichi) = %.4g fb (13.6 TeV, MadGraph-LO "
                "normalised); BR(A->Ha) = %.4g, BR(H->hh) = %.4g, Gamma_A/mA = %.4g\n"
                % (t, os.path.basename(a.table), r["xsecBR_pb"] * 1e3, r["BR_AHa"], r["BR_Hhh"], r["GammaA_over_mA"]))
        path = os.path.join(a.outdir, "param_card_%s.dat" % t)
        with open(path, "w") as f:
            f.write(head + make_card(lines, vals))
        written.append((t, path, r))

    cols = ["mA", "mH", "ma", "mHc", "tanb", "sinp", "sinbma", "lam3", "laP1", "laP2", "BR_AHa", "BR_Hhh",
            "GammaA_over_mA"]
    with open(os.path.join(a.outdir, "summary.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["tag", "card", "expected_sigBR_fb"] + cols)
        for t, p, r in written:
            w.writerow([t, os.path.basename(p), "%.5g" % (r["xsecBR_pb"] * 1e3)] + ["%.6g" % r[c] for c in cols])
    print("wrote %d cards to %s/ (list in %s/summary.csv)" % (len(written), a.outdir, a.outdir))

    proc = a.proc.rstrip("/")
    sh = os.path.join(a.outdir, "run_all.sh")
    with open(sh, "w") as f:
        f.write("#!/bin/bash\n"
                "# generated by make_cards_from_minimizer.py: one MadGraph run (+ MadSpin, Pythia8, Delphes) per param_card.\n"
                "# Points that already have Delphes output are skipped, so the script can be restarted after an interruption.\n"
                "# The param_card (and run_card, if --nevents was given) of the process directory are restored at the end.\n"
                "set -e\n")
        f.write('PROC="%s"\n' % proc)
        f.write('HERE="$(cd "$(dirname "$0")" && pwd)"   # the cards are next to this script\n')
        f.write('[ -x "$PROC/bin/generate_events" ] || { echo "no MadGraph process directory at $PROC"; exit 1; }\n')
        f.write('cp "$PROC/Cards/param_card.dat" "$PROC/Cards/param_card.dat.before_minimizer_cards"\n')
        f.write('cp "$PROC/Cards/run_card.dat" "$PROC/Cards/run_card.dat.before_minimizer_cards"\n')
        f.write('restore() {\n  cp "$PROC/Cards/param_card.dat.before_minimizer_cards" "$PROC/Cards/param_card.dat"\n'
                '  cp "$PROC/Cards/run_card.dat.before_minimizer_cards" "$PROC/Cards/run_card.dat"\n}\ntrap restore EXIT\n')
        if a.nevents:
            f.write('sed -E -i.bak "s/^ *[0-9]+ *= *nevents/ %d = nevents/" "$PROC/Cards/run_card.dat"\n' % a.nevents)
        f.write('done_already() { ls "$PROC"/Events/"$1"_decayed_*/*delphes_events.root >/dev/null 2>&1; }\n')
        for t, p, r in written:
            f.write('\nif done_already %s; then echo "=== %s: already done, skipping"; else\n' % (t, t))
            f.write('  echo "=== %s  (expected sigma x BR = %.4g fb)"\n' % (t, r["xsecBR_pb"] * 1e3))
            f.write('  cp "$HERE/%s" "$PROC/Cards/param_card.dat"\n' % os.path.basename(p))
            f.write('  (cd "$PROC" && ./bin/generate_events %s -f)\nfi\n' % t)
        f.write('\necho "all done; compare with the minimizer: python3 check_mg_vs_minimizer.py $HERE/summary.csv $PROC"\n')
    os.chmod(sh, 0o755)
    print("wrote %s  (run with: bash %s)" % (sh, sh))


if __name__ == "__main__":
    main()
