#!/usr/bin/env python3
"""
After the MadGraph runs: compare MadGraph's sigma x BR with the minimizer's prediction, point by point, and write
the cross sections needed to normalise the samples.

usage:
    python3 check_mg_vs_minimizer.py cards_minimizer/summary.csv [PROC]

For each tag in summary.csv it reads the banner of the MadSpin-decayed run PROC/Events/<tag>_decayed_*/ and computes
    sigma x BR = sigma(gg->A) x BR(A->Ha) x BR(H->hh) x BR(a->chichi)
from the integrated weight and the (Auto) decay tables in the banner. Pythia8 forces h -> b b~, so the cross section
of the 4b + MET sample is sigma x BR x BR(h->bb)^2. Use the SM value 0.5824 ("4b SM"); MadGraph's tree-level
BR(h->bb) of this UFO (about 0.84, no h -> gg, gamma gamma, ...) is printed only for reference.
Writes <summary dir>/mg_xsec.csv.
"""
import csv
import glob
import os
import re
import sys

DEFAULT_PROC = "/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/workdir_AHhhaa_4b"
BR_HBB_SM = 0.5824


def banner_numbers(path):
    b = open(path).read()
    m = re.search(r"Integrated weight \(pb\)\s*:\s*([0-9.eE+-]+)", b)
    if not m:
        return None
    sig = float(m.group(1))

    def br(parent, daughters):
        blk = re.search(r"DECAY\s+%d\s+([0-9.eE+-]+)(.*?)(?=\nDECAY|\n<|\Z)" % parent, b, re.S)
        if not blk:
            return float("nan"), float("nan")
        tot = 0.0
        for line in blk.group(2).splitlines():
            mm = re.match(r"\s*([0-9.eE+-]+)\s+2\s+(-?\d+)\s+(-?\d+)", line)
            if mm and sorted([abs(int(mm.group(2))), abs(int(mm.group(3)))]) == sorted(daughters):
                tot += float(mm.group(1))
        return tot, float(blk.group(1))

    bAHa, wA = br(36, [35, 55])
    bHhh, wH = br(35, [25, 25])
    bchi, wa = br(55, [52, 52])
    bhbb, wh = br(25, [5, 5])
    return dict(sigmaA_pb=sig, BR_AHa=bAHa, BR_Hhh=bHhh, BR_achichi=bchi, BR_hbb_MG=bhbb, GammaA=wA, GammaH=wH,
                sigBR_fb=sig * bAHa * bHhh * bchi * 1e3)


def main():
    summary = sys.argv[1]
    proc = (sys.argv[2] if len(sys.argv) > 2 else DEFAULT_PROC).rstrip("/")
    rows = list(csv.DictReader(open(summary)))
    out = []
    print("%-18s %10s %10s %6s | %9s %8s %8s | %12s %12s" % ("tag", "expect fb", "MG fb", "MG/exp", "sigA pb",
          "BR_AHa", "BR_Hhh", "4b SM fb", "4b MG-LO fb"))
    for r in rows:
        t = r["tag"]
        bans = sorted(glob.glob(os.path.join(proc, "Events", t + "_decayed_*", "*banner.txt")))
        if not bans:
            print("%-18s %10s   no MadSpin banner in %s/Events/%s_decayed_*" % (t, r["expected_sigBR_fb"], proc, t))
            continue
        n = banner_numbers(bans[-1])
        if n is None:
            print("%-18s  could not read %s" % (t, bans[-1]))
            continue
        exp = float(r["expected_sigBR_fb"])
        four_b_sm = n["sigBR_fb"] * BR_HBB_SM ** 2
        four_b_mg = n["sigBR_fb"] * n["BR_hbb_MG"] ** 2
        print("%-18s %10.4g %10.4g %6.3f | %9.4g %8.4f %8.4f | %12.4g %12.4g" % (t, exp, n["sigBR_fb"], n["sigBR_fb"] / exp,
              n["sigmaA_pb"], n["BR_AHa"], n["BR_Hhh"], four_b_sm, four_b_mg))
        out.append(dict(tag=t, mA=r["mA"], mH=r["mH"], expected_sigBR_fb=exp, banner=bans[-1],
                        sigBR_4b_SMhbb_fb=four_b_sm, sigBR_4b_MGhbb_fb=four_b_mg, **n))
    if out:
        path = os.path.join(os.path.dirname(os.path.abspath(summary)), "mg_xsec.csv")
        with open(path, "w", newline="") as f:
            w = csv.DictWriter(f, list(out[0]))
            w.writeheader()
            w.writerows(out)
        print("wrote", path)
        bad = [o["tag"] for o in out if abs(o["sigBR_fb"] / o["expected_sigBR_fb"] - 1) > 0.05]
        if bad:
            print("NOTE: MadGraph differs from the minimizer by more than 5% for:", ", ".join(bad))


if __name__ == "__main__":
    main()
