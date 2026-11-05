"""
Write one MadGraph param_card per (m_A, m_H) point of a scan_mA_mH.py result, with the optimized parameters,
and (optionally) a MadGraph script that generates events for every card.

usage:
    python make_cards.py scan_mA_mH_fit_dmAc.csv TEMPLATE_param_card.dat [options]

    TEMPLATE  a param_card.dat of the Pseudoscalar_2HDMI model, e.g. workdir_AHhha/Cards/param_card.dat
              (a run banner *_banner.txt also works: its <slha> block is used)
options:
    --outdir DIR       where the cards go (default: cards)
    --min-sigbr X      only points with sigma x BR >= X fb (default 0)
    --top N            only the N points with the largest sigma x BR
    --mxd M --gpxd G   also set the DM mass / coupling (default: keep the template's values)
    --mg-script PROC   also write DIR/run_all.sh for the MadGraph process directory PROC (run with: bash DIR/run_all.sh).
                       For each card it copies the card to PROC/Cards/param_card.dat and runs
                       PROC/bin/generate_events <tag> -f , i.e. without the interactive card editor.  (Loading a card
                       through the editor makes MadGraph import LHAPDF's python module to update alpha_s, which
                       fails with "initialization of lhapdf did not return an extension module" when that module was
                       built for another python.)  Also writes the equivalent run_all.mg5 for mg5_aMC.
    --nevents N        events per run (default: keep the run_card value)

Each card has:  tanbeta, sinbma, sinp, lam3, lap1, lap2 from the optimization;  mh2 = m_H, mh3 = m_A,
mh4 = m_a, mhc = m_H+- from the CSV;  DECAY 25, 35, 36, 37, 55 set to Auto, so MadGraph recomputes the widths
(including the 3-body ones) for every point.
"""
import sys, os, re, csv, argparse

# param_card name (as written in the comment after the value)  ->  CSV column
PARAMS = {"tanbeta": "tanbeta", "sinbma": "sinbma", "sinp": "sinp", "lam3": "lam3", "lap1": "laP1", "lap2": "laP2",
          "mh2": "mH", "mh3": "mA", "mh4": "ma", "mhc": "mHc"}
AUTO_WIDTHS = (25, 35, 36, 37, 55)   # h, H, A, H+-, a


def read_template(path):
    text = open(path).read()
    if "<slha>" in text:
        text = text.split("<slha>")[1].split("</slha>")[0].lstrip("\n")
    return text.splitlines()


def make_card(lines, values, auto=AUTO_WIDTHS):
    """Return the card text with the parameters in `values` (param_card name -> number) replaced and the widths of
    the particles in `auto` set to Auto (their BR lines removed)."""
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
        # parameter line:   <index(es)>  <value>  # <name> [...]
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


def tag(mA, mH):
    f = lambda x: ("%g" % x).replace(".", "p")
    return "mA%s_mH%s" % (f(mA), f(mH))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("csv"); ap.add_argument("template")
    ap.add_argument("--outdir", default="cards")
    ap.add_argument("--min-sigbr", type=float, default=0.)
    ap.add_argument("--top", type=int, default=0)
    ap.add_argument("--mxd", type=float); ap.add_argument("--gpxd", type=float)
    ap.add_argument("--mg-script", metavar="PROC")
    ap.add_argument("--nevents", type=int)
    a = ap.parse_args()

    lines = read_template(a.template)
    rows = [r for r in csv.DictReader(open(a.csv)) if r["sigBR_fb"] not in ("nan", "")]
    rows = [r for r in rows if float(r["sigBR_fb"]) >= a.min_sigbr]
    rows.sort(key=lambda r: -float(r["sigBR_fb"]))
    if a.top:
        rows = rows[:a.top]
    os.makedirs(a.outdir, exist_ok=True)

    written = []
    for r in rows:
        vals = {card: float(r[col]) for card, col in PARAMS.items()}
        if a.mxd is not None: vals["mxd"] = a.mxd
        if a.gpxd is not None: vals["gpxd"] = a.gpxd
        t = tag(float(r["mA"]), float(r["mH"]))
        head = ("# 2HDM+a point %s from %s (scan_mA_mH.py optimization)\n"
                "# expected sigma(gg->A) x BR(A->Ha->hh+chichi) = %s fb (13.6 TeV, MG-LO normalized); "
                "BR(A->Ha) = %s, BR(H->hh) = %s\n" % (t, os.path.basename(a.csv), r["sigBR_fb"], r["brAHa"], r["brHhh"]))
        path = os.path.join(a.outdir, "param_card_%s.dat" % t)
        with open(path, "w") as f:
            f.write(head + make_card(lines, vals))
        written.append((t, os.path.abspath(path), r))

    with open(os.path.join(a.outdir, "summary.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["tag", "card", "mA", "mH", "mHc", "ma", "sigBR_fb", "tanbeta", "sinp", "sinbma", "lam3", "laP1", "laP2"])
        for t, p, r in written:
            w.writerow([t, p] + [r[k] for k in ("mA", "mH", "mHc", "ma", "sigBR_fb", "tanbeta", "sinp", "sinbma",
                                                "lam3", "laP1", "laP2")])
    print("wrote %d cards to %s/ (list in %s/summary.csv)" % (len(written), a.outdir, a.outdir))

    if a.mg_script:
        proc = a.mg_script.rstrip("/")
        sh = os.path.join(a.outdir, "run_all.sh")
        with open(sh, "w") as f:
            f.write("#!/bin/bash\n# generated by make_cards.py: one MadGraph run per param_card\nset -e\n")
            f.write('PROC="%s"\n' % proc)
            f.write('cp "$PROC/Cards/param_card.dat" "$PROC/Cards/param_card.dat.before_make_cards"\n')
            if a.nevents:
                f.write('sed -E -i.bak "s/^ *[0-9]+ *= *nevents/ %d = nevents/" "$PROC/Cards/run_card.dat"\n' % a.nevents)
            for t, p, r in written:
                f.write('\necho "=== %s  (expected sigma x BR = %s fb)"\n' % (t, r["sigBR_fb"]))
                f.write('cp "%s" "$PROC/Cards/param_card.dat"\n' % p)
                f.write('(cd "$PROC" && ./bin/generate_events %s -f)\n' % t)
            f.write('\ncp "$PROC/Cards/param_card.dat.before_make_cards" "$PROC/Cards/param_card.dat"\n')
        os.chmod(sh, 0o755)
        print("wrote %s  (run with: bash %s)" % (sh, sh))
        sp = os.path.join(a.outdir, "run_all.mg5")
        with open(sp, "w") as f:
            for t, p, r in written:
                f.write("launch %s -n %s\n0\n%s\n" % (proc, t, p))
                if a.nevents:
                    f.write("set nevents %d\n" % a.nevents)
                f.write("0\n")
        print("wrote %s  (mg5_aMC alternative; needs a working LHAPDF python module)" % sp)


if __name__ == "__main__":
    main()
