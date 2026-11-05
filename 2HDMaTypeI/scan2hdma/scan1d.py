import sys; import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan2hdma as S
from bench import BENCH, lumi
K = 0.776
def show(name, vals, tie=None):
    print("\n--- %s ---   sigma x BR [fb] = sigA x BR(A->Ha) x BR(H->hh) x BR(a->xx)" % name)
    for v in vals:
        p = dict(BENCH); p[name] = v
        if tie: p.update(tie(v))
        try:
            r = S.point(lumi, K, **p)
        except Exception as e:
            print("%10s  error %s" % (v, e)); continue
        top = sorted(r["wA"].items(), key=lambda kv: -kv[1])[:3]
        topA = ", ".join("%s:%.2f" % ("".join(str(x) for x in k), w / r["GA"]) for k, w in top)
        print("%8.3f  sigA=%7.1f fb  BR(Ha)=%.3f BR(hh)=%.3f BR(xx)=%.3f  ->  %6.2f fb   GA/mA=%.2f  [A: %s]" % (
            v, r["sigA"] * 1e3, r["brAHa"], r["brHhh"], r["brachi"], r["sigBR"] * 1e3, r["GA"] / p["mh3"], topA))
show("tanbeta", [0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 5.0])
show("sinp", [0.1, 0.2, 0.3, 0.35, 0.5, 0.6, 0.707, 0.85])
show("lam3", [-2, 0, 1, 2, 3, 5, 8])
show("laP1", [-3, 0, 3, 6, 10])
show("laP2", [-3, 0, 3, 6, 10])
show("sinbma", [1.0, 0.99875, 0.995, 0.98, 0.9539])
show("mh4", [70, 100, 130, 150, 200, 250, 300])
show("mh2", [251, 260, 280, 300, 350, 400])
show("mh3", [400, 450, 500, 550, 610, 700, 800, 1000], tie=lambda v: dict(mhc=v))
show("MXd", [1, 10, 30, 45])
