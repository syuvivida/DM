#!/usr/bin/env python3
"""One PNG per parameter of the best-fit points on the (mA, mH) plane.
Usage: python3 plot_bestPar_2D.py [bestPar_nonAligned_AHhha.dat] [outdir]
The axes are categorical (one equally spaced row per mH value, one column per mA value), so that
mH = 255 and 260 GeV get separate rows and the numbers do not overlap. Points with allowed = 0
are hatched."""
import sys, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
from matplotlib.patches import Rectangle

fin = sys.argv[1] if len(sys.argv) > 1 else "bestPar_nonAligned_AHhha.dat"
outdir = sys.argv[2] if len(sys.argv) > 2 else "plots_bestPar"
os.makedirs(outdir, exist_ok=True)
d = np.genfromtxt(fin, names=True)

PARS = [  # file name, label, values, number format, log colour scale
    ("mA_minus_mHc", r"$m_A - m_{H^\pm}$ [GeV]", d["mA"] - d["mHc"], "%.1f", False),
    ("sintheta",     r"$\sin\theta$",            d["sinp"],          "%.3f", False),
    ("sinbma",       r"$\sin(\beta-\alpha)$",    d["sinbma"],        "%.4f", False),
    ("tanbeta",      r"$\tan\beta$",             d["tanb"],          "%.2f", False),
    ("lambdaP1",     r"$\lambda_{P1}$",          d["laP1"],          "%.1e", True),
    ("lambdaP2",     r"$\lambda_{P2}$",          d["laP2"],          "%.2f", False),
    ("lambda3",      r"$\lambda_3$",             d["lam3"],          "%.2f", False),
]
mAs, mHs = np.unique(d["mA"]), np.unique(d["mH"])
ix = {v: i for i, v in enumerate(mAs)}
iy = {v: i for i, v in enumerate(mHs)}
ma = np.unique(d["ma"])
allowed = d["allowed"] if "allowed" in d.dtype.names else np.ones(len(d))

for fname, label, val, fmt, log in PARS:
    fig, ax = plt.subplots(figsize=(11, 7.5))
    pos = val[val > 0]
    norm = LogNorm(pos.min(), pos.max()) if (log and len(pos)) else Normalize(val.min(), val.max())
    cmap = plt.get_cmap("viridis")
    for k in range(len(d)):
        x, y, v = ix[d["mA"][k]], iy[d["mH"][k]], val[k]
        c = cmap(norm(v)) if (not log or v > 0) else (0.8, 0.8, 0.8, 1)
        ax.add_patch(Rectangle((x - 0.47, y - 0.45), 0.94, 0.9, facecolor=c, edgecolor="white",
                               hatch="///" if allowed[k] == 0 else None, lw=1))
        lum = 0.299 * c[0] + 0.587 * c[1] + 0.114 * c[2]
        ax.text(x, y, fmt % v, ha="center", va="center", fontsize=10,
                color="black" if (allowed[k] == 0 or lum >= 0.5) else "white",
                bbox=dict(boxstyle="round,pad=0.15", fc="white", ec="none", alpha=0.8) if allowed[k] == 0 else None)
    sm = plt.cm.ScalarMappable(norm=norm, cmap=cmap)
    cb = fig.colorbar(sm, ax=ax, pad=0.02)
    cb.set_label(label, fontsize=13)
    ax.set_xlim(-0.5, len(mAs) - 0.5); ax.set_ylim(-0.5, len(mHs) - 0.5)
    ax.set_xticks(range(len(mAs))); ax.set_xticklabels(["%g" % v for v in mAs])
    ax.set_yticks(range(len(mHs))); ax.set_yticklabels(["%g" % v for v in mHs])
    ax.set_xlabel(r"$m_A$ [GeV]", fontsize=13); ax.set_ylabel(r"$m_H$ [GeV]", fontsize=13)
    for s in ax.spines.values(): s.set_visible(False)
    ax.tick_params(length=0)
    title = "Best-fit %s" % label
    if len(ma) == 1: title += r",  $m_a$ = %g GeV" % ma[0]
    ax.set_title(title, fontsize=13)
    if (allowed == 0).any():
        ax.text(1.0, -0.09, "hatched: allowed = 0", transform=ax.transAxes, ha="right", fontsize=9, color="0.3")
    ax.text(0.0, -0.09, "axes not to scale: one row/column per mass value", transform=ax.transAxes,
            fontsize=9, color="0.3")
    fig.tight_layout()
    out = os.path.join(outdir, "bestPar_%s.pdf" % fname)
    fig.savefig(out, dpi=150); plt.close(fig)
    print("wrote", out)
