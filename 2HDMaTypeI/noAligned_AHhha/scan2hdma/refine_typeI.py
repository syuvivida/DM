"""Constrained maximization of sigma(gg->A)xBR(A->Ha)xBR(H->hh)xBR(a->chichi) for the Type-I 2HDM+a UFO.
usage: python refine_typeI.py start.json cfg.json [seed]     (inline JSON strings also accepted)
cfg keys: custodial ('A': mHc = m_A0, 'H': mHc = mH), tb_min, cba_max, mhc_min, hh (1/0), unit (1/0)
"""
import sys, math, json, numpy as np
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan_opt as O, scan2hdma as S
cfg = dict(monoh=1e9, custodial="H", tb_min=2.0, cba_max=0.15, mhc_min=150., hh=1, unit=1, lam_max=O.FOURPI, width_max=0.2)
def load(a):
    return json.load(open(a)) if os.path.isfile(a) else json.loads(a)


cfg.update(load(sys.argv[2]) if len(sys.argv) > 2 else {})
rng = np.random.default_rng(int(sys.argv[3]) if len(sys.argv) > 3 else 1)
lumi = S.Lumi()
keys = ["tanbeta", "sinp", "cba", "lam3", "laP1", "laP2", "mh2", "mh3", "mh4"] + (["mhc"] if cfg["custodial"] == "free" else [])
lo = dict(tanbeta=cfg["tb_min"], sinp=.02, cba=0., lam3=-3., laP1=0., laP2=0., mh2=250., mh3=300., mh4=60., mhc=cfg["mhc_min"])
hi = dict(tanbeta=20., sinp=.95, cba=cfg["cba_max"], lam3=O.FOURPI, laP1=O.FOURPI, laP2=O.FOURPI, mh2=1000., mh3=1500., mh4=400., mhc=1500.)
step = dict(tanbeta=.15, sinp=.03, cba=.01, lam3=.4, laP1=.5, laP2=.5, mh2=8., mh3=10., mh4=8., mhc=10.)


def build(x):
    p = dict(S.DEFAULTS); p.update(MXd=10., gPXd=1., MT=172., MB=4.7, mh1=125.)
    p.update({k: x[k] for k in keys if k != "cba"}); p["sinbma"] = math.sqrt(1 - x["cba"] ** 2)
    if cfg["custodial"] == "A":
        p["mhc"] = complex(S.evaluate(**{**p, "mhc": 700.})["mA"]).real
    elif cfg["custodial"] == "H":
        p["mhc"] = p["mh2"]
    return p


def f(x):
    p = build(x)
    if p["mh3"] < p["mh2"] + p["mh4"] + 5 or p["mhc"] < cfg["mhc_min"]: return -1, None
    env = S.evaluate(**p); r = S.point(lumi, O.K_MG, **p)
    if O.constraints(env, p, r, cfg): return -1, None
    l = [complex(env["l%d" % i]).real for i in range(1, 8)]
    if cfg["unit"] and O.unitarity_max(O.z2_basis(l, p["tanbeta"])) > 8 * math.pi: return -1, None
    br_ha = r["wA"].get((25, 55), 0.) / sum(r["wA"].values())
    if r["sigA"] * 1e3 * br_ha * 0.58 > cfg["monoh"]: return -1, None   # sigma(A->ha->bb+MET) cap [fb]
    sH, _ = O.sigma_ggH(env, lumi)
    if cfg["hh"] and sH * r["brHhh"] > O.hh_limit(p["mh2"]): return -1, None
    return r["sigBR"] * 1e3, (p, r, env, sH)


x = load(sys.argv[1])
if cfg["custodial"] == "free": x.setdefault("mhc", x["mh2"])
best, info = f(x)
if info is None:   # find a feasible start nearby
    for _ in range(4000):
        y = {k: min(hi[k], max(lo[k], x[k] + rng.normal() * step[k] * 5)) for k in keys}
        v, inf = f(y)
        if inf: x, best, info = y, v, inf; break
if info is None:
    print("no feasible point found near start"); sys.exit()
T = 1.0
for it in range(3000):
    y = {k: min(hi[k], max(lo[k], x[k] + rng.normal() * step[k] * T)) for k in keys}
    v, inf = f(y)
    if v > best: x, best, info = y, v, inf
    if it % 1000 == 999: T *= 0.5
p, r, env, sH = info
l = [complex(env["l%d" % i]).real for i in range(1, 8)]
lam = O.z2_basis(l, p["tanbeta"])
wA = r["wA"]; tot = sum(wA.values())
top = sorted(((v / tot, k) for k, v in wA.items()), reverse=True)[:4]
print("sigBR = %.2f fb | sigA = %.1f fb  BR(A->Ha)=%.3f BR(H->hh)=%.3f BR(a->xx)=%.3f  GA/mA=%.3f" % (
    best, r["sigA"] * 1e3, r["brAHa"], r["brHhh"], r["brachi"], r["GA"] / p["mh3"]))
print("  sigma(A->ha->bb+MET) = %.1f fb" % (r["sigA"]*1e3*wA.get((25,55),0.)/tot*0.58))
print("  A decays:", ", ".join("%s %.2f" % (k, b) for b, k in top))
print("  direct ggH x BR(hh) = %.3f pb (limit ~%.3f)  | Z2 lam1..5 =" % (sH * r["brHhh"], O.hh_limit(p["mh2"])), lam[:5].round(2),
      " unitarity/8pi = %.2f" % (O.unitarity_max(lam) / (8 * math.pi)))
print("  PARAMS", json.dumps({k: round(float(p[k]), 4) for k in ("tanbeta", "sinp", "sinbma", "lam3", "laP1", "laP2", "mh2", "mh3", "mh4", "mhc")}))
