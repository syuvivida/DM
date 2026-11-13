#!/usr/bin/env python3
"""Compare the UFO analytic widths/BRs (ufo_widths.py, same formulas as Pseudoscalar2HDMI_widths.h) with the
MadGraph 4D scans of noAligned_AHhha/4DScan_sinp_lam3_tanb_sinbma/data.
usage: python3 validate_vs_madgraph.py <path to 4DScan_sinp_lam3_tanb_sinbma/data>"""
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ufo_widths as U

base = sys.argv[1]
for tag, mA, mhc in [("MA410_MH260", 410, 410), ("MA410_MH260_MHC260", 410, 260),
                     ("MA700_MH260", 700, 700), ("MA700_MH260_MHC260", 700, 260)]:
    d = np.genfromtxt(os.path.join(base, "xSec_noaligned_AHhha_%s_4Dscan.dat" % tag), skip_header=1)
    w = np.genfromtxt(os.path.join(base, "width_noaligned_AHhha_%s_4Dscan.dat" % tag), skip_header=1)
    rel = []
    for row, wr in zip(d, w):
        sinp, lam3, tb, sbma = row[:4]
        inp = dict(sinp=sinp, lam3=lam3, tanbeta=tb, sinbma=sbma, mh2=260, mh3=mA, mhc=mhc, mh4=100, MXd=10)
        wA, wH = U.widths(inp, "h3"), U.widths(inp, "h2")
        rel.append((wA[("h2", "h4")] / sum(wA.values()) / row[4] - 1, wH[("h1", "h1")] / sum(wH.values()) / row[5] - 1))
    rel = np.abs(np.array(rel))
    print("%-20s n=%d  |BR(A->Ha) ratio-1| median %.1e, 95%% %.1e   |BR(H->hh) ratio-1| median %.1e, 95%% %.1e"
          % (tag, len(rel), np.median(rel[:, 0]), np.percentile(rel[:, 0], 95), np.median(rel[:, 1]), np.percentile(rel[:, 1], 95)))
