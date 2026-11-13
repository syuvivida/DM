"""
Random-search optimization of sigma(gg->A) x BR(A->Ha) x BR(H->hh) x BR(a->chi chi) in the 2HDM+a
(Pseudoscalar_2HDMI UFO), with theory and (simplified) experimental constraints.
"""
import sys, math, random, json
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
import scan2hdma as S
import contextlib, io

K_MG = 0.776          # MadGraph LO / formula at the reference point
FOURPI = 4 * math.pi
rng = np.random.default_rng(7)

# random directions in the space of two complex doublets (8 real numbers), reused for every point
_dirs = rng.normal(size=(20000, 8))
H1 = _dirs[:, 0:4:2] + 1j * _dirs[:, 1:4:2]   # (2 complex components)
H2 = _dirs[:, 4:8:2] + 1j * _dirs[:, 5:8:2]
a = np.sum(np.abs(H1) ** 2, 1); b = np.sum(np.abs(H2) ** 2, 1)
c = np.sum(np.conj(H1) * H2, 1)  # H1^dag H2


# Model convention (checked against l1 = mh^2/(2v^2) at alignment):
# V4 = l1 a^2 + l2 b^2 + l3 a b + l4 |c|^2 + 2 Re[l5 c^2 + l6 a c + l7 b c],  a=|H1|^2, b=|H2|^2, c=H1^dag H2 (Higgs basis)
def V4(l, A, B, C):
    l1, l2, l3, l4, l5, l6, l7 = l
    return l1 * A ** 2 + l2 * B ** 2 + l3 * A * B + l4 * np.abs(C) ** 2 + 2 * np.real(l5 * C ** 2 + l6 * A * C + l7 * B * C)


def quartic_min(l):
    return float(np.min(V4(l, a, b, c) / (a + b) ** 2))


def z2_basis(l, tb):
    """Rotate to the Z2 basis (Phi1, Phi2) and fit V = lam1/2 A^2 + lam2/2 B^2 + lam3 AB + lam4|C|^2 + lam5/2 (C^2+cc)
    (+ lam6, lam7 terms as a check)."""
    be = math.atan(tb); cb, sb = math.cos(be), math.sin(be)
    P1 = H1[:500]; P2 = H2[:500]              # random Phi1, Phi2
    h1 = cb * P1 + sb * P2; h2 = -sb * P1 + cb * P2
    v = V4(l, np.sum(abs(h1) ** 2, 1), np.sum(abs(h2) ** 2, 1), np.sum(np.conj(h1) * h2, 1))
    A = np.sum(abs(P1) ** 2, 1); B = np.sum(abs(P2) ** 2, 1); C = np.sum(np.conj(P1) * P2, 1)
    X = np.stack([A * A / 2, B * B / 2, A * B, abs(C) ** 2, np.real(C * C), A * np.real(C), B * np.real(C)], 1)
    lam, *_ = np.linalg.lstsq(X, v, rcond=None)
    return lam   # lam1..lam5, lam6, lam7


def constraints(env, p, r, cfg):
    l = [complex(env["l%d" % i]).real for i in range(1, 8)]
    lam = z2_basis(l, p["tanbeta"])
    reasons = []
    if max(abs(x) for x in list(lam[:5]) + [p["lam3"], p["laP1"], p["laP2"]]) > cfg["lam_max"]:
        reasons.append("perturbativity")
    if quartic_min(l) <= 0:
        reasons.append("unbounded potential")
    if p["laP1"] < 0 or p["laP2"] < 0:
        reasons.append("unbounded P direction")
    if r["GA"] / p["mh3"] > cfg["width_max"]:
        reasons.append("Gamma_A/m_A")
    return reasons


def sample(cfg):
    mA = rng.uniform(*cfg["mA"])
    mH = rng.uniform(*cfg["mH"])
    ma = rng.uniform(*cfg["ma"])
    p = dict(S.DEFAULTS)
    p.update(tanbeta=math.exp(rng.uniform(math.log(cfg["tb"][0]), math.log(cfg["tb"][1]))),
             sinp=rng.uniform(*cfg["sinp"]), sinbma=math.sqrt(1 - rng.uniform(0, cfg["cba_max"]) ** 2),
             lam3=rng.uniform(*cfg["lam3"]), laP1=rng.uniform(*cfg["laP"]), laP2=rng.uniform(*cfg["laP"]),
             mh1=125., mh2=mH, mh3=mA, mh4=ma, MXd=10., gPXd=1., MT=172., MB=4.7)
    # custodial symmetry: m_H+- equal to the 2HDM pseudoscalar mass (before mixing) or to m_H
    p["mhc"] = None
    return p


def run(cfg, n):
    lumi = S.Lumi()
    good = []
    rej = {}
    for i in range(n):
        p = sample(cfg)
        if p["mh3"] < p["mh2"] + p["mh4"] + 5:   # A -> H a must be open
            continue
        env0 = S.evaluate(**{**p, "mhc": 700.})
        mA0 = complex(env0["mA"]).real
        p["mhc"] = mA0 if cfg["custodial"] == "A" else p["mh2"]
        if p["mhc"] < cfg["mhc_min"]:
            rej["m_H+- < %g" % cfg["mhc_min"]] = rej.get("m_H+- < %g" % cfg["mhc_min"], 0) + 1
            continue
        try:
            env = S.evaluate(**p)
            r = S.point(lumi, K_MG, **p)
        except Exception:
            continue
        why = constraints(env, p, r, cfg)
        for w in why:
            rej[w] = rej.get(w, 0) + 1
        if why:
            continue
        good.append(dict(sigBR_fb=r["sigBR"] * 1e3, sigA_fb=r["sigA"] * 1e3, brAHa=r["brAHa"], brHhh=r["brHhh"],
                         brachi=r["brachi"], GA_over_mA=r["GA"] / p["mh3"],
                         **{k: p[k] for k in ("tanbeta", "sinp", "sinbma", "lam3", "laP1", "laP2", "mh2", "mh3", "mh4", "mhc")}))
    good.sort(key=lambda d: -d["sigBR_fb"])
    return good, rej


if __name__ == "__main__":
    cfg = dict(mA=(400., 900.), mH=(250., 450.), ma=(70., 250.), tb=(1.0, 10.), sinp=(0.05, 0.9), cba_max=0.10,
               lam3=(-3., 8.), laP=(0., 8.), lam_max=FOURPI, width_max=0.2, custodial="A", mhc_min=600.)
    if len(sys.argv) > 1:
        cfg.update(json.loads(sys.argv[1]))
    n = int(sys.argv[2]) if len(sys.argv) > 2 else 20000
    good, rej = run(cfg, n)
    print("config:", cfg)
    print("accepted points: %d of %d;  rejections: %s" % (len(good), n, rej))
    hdr = "sigBR[fb] sigA[fb] BR(Ha) BR(hh) BR(xx) GA/mA | tanb  sinp  cba   lam3  laP1  laP2 |  mH    mA    ma    mH+-"
    print(hdr)
    for d in good[:15]:
        print("%8.2f %8.1f %6.3f %6.3f %6.3f %5.2f | %4.2f %5.2f %5.3f %5.2f %5.2f %5.2f | %5.0f %5.0f %5.0f %5.0f" % (
            d["sigBR_fb"], d["sigA_fb"], d["brAHa"], d["brHhh"], d["brachi"], d["GA_over_mA"], d["tanbeta"], d["sinp"],
            math.sqrt(1 - d["sinbma"] ** 2), d["lam3"], d["laP1"], d["laP2"], d["mh2"], d["mh3"], d["mh4"], d["mhc"]))
    json.dump(dict(cfg=cfg, good=good[:200], rej=rej), open("scan_result_%s.json" % cfg["custodial"], "w"), indent=1)


# ---- direct gg -> H -> hh vs. resonant-HH limits ----
# approximate observed 95% CL limits on sigma(pp->X->hh) for a narrow spin-0 X, 13 TeV (ATLAS Run-2 combination,
# arXiv:2311.15956; values read off by eye -- replace with HEPData before using for anything quantitative)
HH_LIMIT_PB = [(251, 0.6), (300, 0.35), (350, 0.15), (400, 0.08), (500, 0.04), (600, 0.025), (800, 0.01), (1000, 0.006)]


def hh_limit(m):
    pts = HH_LIMIT_PB
    if m <= pts[0][0]: return pts[0][1]
    for (m1, y1), (m2, y2) in zip(pts, pts[1:]):
        if m <= m2:
            return math.exp(math.log(y1) + (math.log(y2) - math.log(y1)) * (m - m1) / (m2 - m1))
    return pts[-1][1]


def _AH_half(tau):
    return 2 * (tau + (tau - 1) * S._fA(tau)) / tau ** 2


def sigma_ggH(env, lumi, k_mg=K_MG):
    mH, mt, mb = env["mh2"].real, env["MT"].real, env["MB"].real
    tb = env["tanbeta"].real
    kt = (complex(env["TH1x2"]) - complex(env["TH2x2"]) / tb).real          # from the model's H t t coupling
    kb = kt                                                                   # type I (this UFO): H b b coupling = H t t coupling
    amp = kt * _AH_half(mH ** 2 / (4 * mt * mt)) + kb * _AH_half(mH ** 2 / (4 * mb * mb))
    a_s = lumi.alphas(0.5 * mH)
    gam = env["Gf"].real * a_s ** 2 * mH ** 3 / (36 * math.sqrt(2) * math.pi ** 3) * abs(0.75 * amp) ** 2
    return k_mg * math.pi ** 2 * gam / (8 * mH ** 3) * lumi.tau_dLdtau(mH, 0.5 * mH) * S.GEV2_TO_PB, kt


def unitarity_max(lam):
    """largest |eigenvalue| of the 2HDM tree-level scattering matrix (Z2 basis, lam1/2 normalization); require < 8 pi."""
    l1, l2, l3, l4, l5 = lam[:5]
    r = lambda x: math.sqrt(max(x, 0.))
    ev = [1.5 * (l1 + l2) + s * r(2.25 * (l1 - l2) ** 2 + (2 * l3 + l4) ** 2) for s in (1, -1)]
    ev += [0.5 * (l1 + l2) + s * 0.5 * r((l1 - l2) ** 2 + 4 * l4 ** 2) for s in (1, -1)]
    ev += [0.5 * (l1 + l2) + s * 0.5 * r((l1 - l2) ** 2 + 4 * l5 ** 2) for s in (1, -1)]
    ev += [l3 + 2 * l4 - 3 * l5, l3 - l5, l3 + 2 * l4 + 3 * l5, l3 + l5, l3 + l4, l3 - l4]
    return max(abs(e) for e in ev)
