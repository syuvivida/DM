import sys, math, json, numpy as np
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan_opt as O, scan2hdma as S
cfg = dict(lam_max=O.FOURPI, width_max=0.2, mhc_min=float(sys.argv[2]) if len(sys.argv) > 2 else 600., cba_max=0.10)
lumi = S.Lumi(); rng = np.random.default_rng(int(sys.argv[3]) if len(sys.argv) > 3 else 1)
USE_HH = int(sys.argv[4]) if len(sys.argv) > 4 else 1
UNIT = int(sys.argv[5]) if len(sys.argv) > 5 else 1
keys = ["tanbeta", "sinp", "cba", "lam3", "laP1", "laP2", "mh2", "mh3", "mh4"]
lo = dict(tanbeta=1., sinp=.02, cba=0., lam3=-3., laP1=0., laP2=0., mh2=250., mh3=300., mh4=60.)
hi = dict(tanbeta=10., sinp=.95, cba=cfg["cba_max"], lam3=O.FOURPI, laP1=O.FOURPI, laP2=O.FOURPI, mh2=1000., mh3=1500., mh4=400.)
step = dict(tanbeta=.15, sinp=.03, cba=.01, lam3=.4, laP1=.5, laP2=.5, mh2=8., mh3=10., mh4=8.)
def f(x):
    p = dict(S.DEFAULTS); p.update(MXd=10., gPXd=1., MT=172., MB=4.7, mh1=125.)
    p.update({k: x[k] for k in keys if k != "cba"}); p["sinbma"] = math.sqrt(1 - x["cba"] ** 2)
    if p["mh3"] < p["mh2"] + p["mh4"] + 5: return -1, None
    p["mhc"] = complex(S.evaluate(**{**p, "mhc": 700.})["mA"]).real   # m_H+- = m_A0 (custodial)
    if p["mhc"] < cfg["mhc_min"]: return -1, None
    env = S.evaluate(**p); r = S.point(lumi, O.K_MG, **p)
    if O.constraints(env, p, r, cfg): return -1, None
    if UNIT and O.unitarity_max(O.z2_basis([complex(env["l%d" % i]).real for i in range(1, 8)], p["tanbeta"])) > 8 * math.pi: return -1, None
    sH, _ = O.sigma_ggH(env, lumi)
    if USE_HH and sH * r["brHhh"] > O.hh_limit(p["mh2"]): return -1, None
    return r["sigBR"] * 1e3, (p, r)
x = json.loads(sys.argv[1]); best, info = f(x); T = 1.0
for it in range(3000):
    y = {k: min(hi[k], max(lo[k], x[k] + rng.normal() * step[k] * T)) for k in keys}
    v, inf = f(y)
    if v > best: x, best, info = y, v, inf
    if it % 1000 == 999: T *= 0.5
p, r = info
print("sigBR = %.2f fb  sigA = %.1f fb  BR(A->Ha)=%.3f BR(H->hh)=%.3f BR(a->xx)=%.3f GA/mA=%.3f" % (best, r["sigA"]*1e3, r["brAHa"], r["brHhh"], r["brachi"], r["GA"]/p["mh3"]))
print(json.dumps({k: round(float(x[k]), 4) for k in keys} | {"mhc": round(p["mhc"], 2)}))
env = S.evaluate(**p); l = [complex(env["l%d" % i]).real for i in range(1, 8)]
sH, ktH = O.sigma_ggH(env0 := S.evaluate(**p), lumi)
print("  direct sigma(ggH)xBR(hh) = %.3f pb (limit ~%.3f pb), kappa_t(H) = %.3f, kappa_t(A) = %.3f" % (sH*r["brHhh"], O.hh_limit(p["mh2"]), ktH, r["kt"]))
print("Z2-basis lam1..5:", O.z2_basis(l, p["tanbeta"])[:5].round(2), " quartic min:", round(O.quartic_min(l), 4), " max unitarity eigenvalue/8pi:", round(O.unitarity_max(O.z2_basis(l, p["tanbeta"]))/(8*math.pi), 3))
