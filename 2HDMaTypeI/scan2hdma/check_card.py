"""Compare the scanner's widths and BRs with a MadGraph param_card or run banner (auto widths).
usage: python check_card.py path/to/run_XX_tag_1_banner.txt   (or a param_card.dat written by MadGraph)"""
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan2hdma as S

text = open(sys.argv[1]).read()
if "<slha>" in text:
    text = text.split("<slha>")[1].split("</slha>")[0]
ext, decays, cur = {}, {}, None
names = {p.name.lower(): p.name for p in S._P.all_parameters if p.nature == "external"}
for line in text.splitlines():
    t = line.split("#")[0].split()
    if not t:
        continue
    if t[0].upper() == "BLOCK":
        cur = None; continue
    if t[0].upper() == "DECAY":
        cur = int(t[1]); decays[cur] = {"width": float(t[2]), "br": {}}; continue
    if cur is not None:                      # BR line: BR NDA ID1 ID2 ...
        ids = tuple(sorted(abs(int(x)) for x in t[2:2 + int(t[1])]))
        decays[cur]["br"][ids] = decays[cur]["br"].get(ids, 0.) + float(t[0]); continue
    m = re.search(r"#\s*(\S+)", line)       # parameter line: index value # name
    if m and m.group(1).lower() in names and len(t) >= 2:
        ext[names[m.group(1).lower()]] = float(t[-1])

env = S.evaluate(**ext)
w = S.widths(env)
for pdg, nm in ((55, "a"), (35, "H"), (36, "A")):
    pw = w[pdg]; tot = sum(pw.values()); mg = decays.get(pdg, {"width": float("nan"), "br": {}})
    print("%s (%d): Gamma = %.4f GeV   MadGraph %.4f" % (nm, pdg, tot, mg["width"]))
    for k in sorted(pw, key=pw.get, reverse=True)[:6]:
        # MadGraph lists the closed channel H -> a a* as the 3-body decay H -> a chi chi~
        mgbr = mg["br"].get(k, 0.) + (mg["br"].get((52, 52, 55), 0.) if k == (55, 55) else 0.)
        print("    %-10s BR = %.4f   MadGraph %.4f" % (k, pw[k] / tot, mgbr))
