#!/usr/bin/env python3
"""2D maps in the (mA, mH) plane of the maximal sigma x BR and of the parameters that give it.
usage: python3 plot_bestPar_2D.py bestPar_nonAligned_AHhha.dat [--hbb]   (--hbb: include BR(h->bb)^2 = 0.5824^2)"""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

fn = sys.argv[1]
hbb = "--hbb" in sys.argv
d = np.genfromtxt(fn, names=True)
mA, mH, ok = d["mA"], d["mH"], d["allowed"] > 0
xs = d["xsecBR_pb"] * (0.5824 ** 2 if hbb else 1.) * 1e3   # fb
ma = np.median(d["ma"])

panels = [("xsecBR", xs, r"max $\sigma\times$BR [fb]" + (r" incl. BR($h\to b\bar b$)$^2$" if hbb else ""), True, "%.3g"),
          ("sinp", d["sinp"], r"$\sin\theta$", False, "%.2f"),
          ("sinbma", np.sqrt(np.clip(1 - d["sinbma"] ** 2, 0, 1)), r"$\cos(\beta-\alpha)$", False, "%.3f"),
          ("tanb", d["tanb"], r"$\tan\beta$", False, "%.2f"),
          ("lam3", d["lam3"], r"$\lambda_3$", False, "%.1f"),
          ("laP1", d["laP1"], r"$\lambda_{P1}$", False, "%.1f"),
          ("laP2", d["laP2"], r"$\lambda_{P2}$", False, "%.1f"),
          ("dM", d["mA"] - d["mHc"], r"$m_A - m_{H^\pm}$ [GeV]", False, "%.0f"),
          ("BR_AHa", d["BR_AHa"], r"BR($A\to Ha$)", False, "%.2f"),
          ("BR_Hhh", d["BR_Hhh"], r"BR($H\to hh$)", False, "%.2f"),
          ("GammaA", d["GammaA_over_mA"], r"$\Gamma_A/m_A$", False, "%.3f"),
          ("drho", d["drho"] * 1e3, r"$\Delta\rho\times 10^3$", False, "%.2f")]

fig, axs = plt.subplots(3, 4, figsize=(20, 13))
for ax, (name, v, label, log, fmt) in zip(axs.flat, panels):
    good = ok & np.isfinite(v) & ((v > 0) if log else True)
    sc = ax.scatter(mA[good], mH[good], c=v[good], s=330, marker="s", cmap="viridis", edgecolors="k",
                    norm=LogNorm() if log and good.sum() > 1 else None)
    ax.scatter(mA[~ok], mH[~ok], s=200, marker="x", c="gray", label="no allowed point")
    for x, y, z, g in zip(mA, mH, v, ok):
        if g:
            ax.annotate(fmt % z, (x, y), ha="center", va="center", fontsize=6, color="w")
    fig.colorbar(sc, ax=ax).set_label(label)
    xx = np.linspace(mA.min() - 30, mA.max() + 30, 5)
    ax.plot(xx, xx - ma, "r-", lw=1)
    ax.set_xlim(mA.min() - 30, mA.max() + 30)
    ax.set_ylim(mH.min() - 20, mH.max() + 30)
    ax.set_xlabel(r"$m_A$ [GeV]")
    ax.set_ylabel(r"$m_H$ [GeV]")
    ax.set_title(label, fontsize=10)
    if (~ok).any():
        ax.legend(fontsize=7, loc="upper left")
fig.suptitle(r"2HDM+a Type I, non-aligned: max $\sigma(gg\to A)\times$BR($A\to Ha$, $H\to hh$, $a\to\chi\chi$), "
             r"13.6 TeV, $m_a$=%g GeV" % ma, fontsize=12)
fig.tight_layout()
outname = fn.rsplit(".", 1)[0] + ("_hbb" if hbb else "") + ".pdf"
fig.savefig(outname)
print("wrote", outname)
