"""
2D scan in (m_A, m_H) of the Type-I 2HDM+a (Pseudoscalar_2HDMI UFO).

At every mass point, with m_a fixed, the parameters
    tanbeta, sinp (sin theta), sinbma (via cos(beta-alpha)), lam3, laP1, laP2
and, with "fit_dmAc": 1, also dmAc = m_A - m_H+- (within dmAc_range), are optimized to maximize
    sigma(gg->A) x BR(A->Ha) x BR(H->hh) x BR(a->chi chi)      [13.6 TeV, MadGraph-LO normalized]
subject to the same theory/experimental constraints as refine_typeI.py, plus electroweak precision |Delta T| < T_max
(m_H+- is no longer tied to m_A0 or m_H, so custodial symmetry does not protect T).

usage:
    python scan_mA_mH.py                      # default grid and constraints
    python scan_mA_mH.py scan_cfg.json        # your settings (any subset of the keys in CFG below)
outputs (prefix set by "out"):
    <out>.csv   one row per mass point: best sigma x BR, BRs, side channels, best couplings (or "nan" if no allowed point)
    <out>.png   map of the best sigma x BR, if matplotlib is available
"""
import sys, os, math, json, csv, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
import multiprocessing as mp

CFG = dict(
    mA=[300., 800., 25.],      # [min, max, step] GeV
    mH=[260., 610., 25.],      # [min, max, step] GeV  (m_H > 2 m_h = 250 needed for H->hh)
    ma=143.,                   # fixed m_a
    dmAc=50.,                  # m_H+- = m_A - dmAc (fixed value, or start value when fit_dmAc = 1)
    fit_dmAc=0,                # 1: optimize dmAc = m_A - m_H+- at every mass point too
    dmAc_range=[-100., 400.],  # allowed range of dmAc when it is optimized [GeV]
    MXd=10., gPXd=1.,
    # constraints
    tb_min=2.0, tb_max=20.,    # tan(beta) range (lower end: flavour)
    cba_max=0.15,              # |cos(beta-alpha)| upper bound (Higgs couplings)
    mhc_min=150.,              # skip points with m_H+- below this
    monoh=20.,                 # cap on sigma(A->ha->bb+MET) [fb]; 1e9 = off
    hh=1,                      # resonant-HH limit on direct gg->H->hh
    unit=1,                    # tree-level unitarity
    lam_max=4 * math.pi,       # perturbativity of the Z2-basis quartics and lam3, laP1, laP2
    width_max=0.2,             # Gamma_A / m_A
    T_max=0.2,                 # |Delta T| (electroweak precision, one loop, alignment limit); 1e9 = off
    # optimizer
    n_iter=1500,               # random-walk steps per start
    n_start=3,                 # independent starts per mass point (best kept)
    seed=1,
    workers=0,                 # parallel processes (0 = all cores)
    out="scan_mA_mH",
)

KEYS6 = ["tanbeta", "sinp", "cba", "lam3", "laP1", "laP2"]
STEP = dict(tanbeta=.15, sinp=.03, cba=.01, lam3=.4, laP1=.5, laP2=.5, dmAc=10.)


def keys(cfg):
    return KEYS6 + (["dmAc"] if cfg["fit_dmAc"] else [])
STARTS = [dict(tanbeta=2.0, sinp=0.45, cba=0.14, lam3=4.5, laP1=0.0, laP2=4.5),
          dict(tanbeta=2.5, sinp=0.35, cba=0.10, lam3=2.0, laP1=1.0, laP2=6.0),
          dict(tanbeta=3.0, sinp=0.30, cba=0.12, lam3=1.0, laP1=0.5, laP2=2.0),
          dict(tanbeta=2.0, sinp=0.60, cba=0.15, lam3=8.0, laP1=0.0, laP2=8.0)]


def _init_worker():
    global S, O, LUMI
    import scan2hdma as S
    import scan_opt as O
    LUMI = S.Lumi()


def _bounds(cfg):
    lo = dict(tanbeta=cfg["tb_min"], sinp=0.02, cba=0.0, lam3=-3.0, laP1=0.0, laP2=0.0, dmAc=cfg["dmAc_range"][0])
    hi = dict(tanbeta=cfg["tb_max"], sinp=0.95, cba=cfg["cba_max"], lam3=cfg["lam_max"], laP1=cfg["lam_max"],
              laP2=cfg["lam_max"], dmAc=cfg["dmAc_range"][1])
    return lo, hi


def _F(x, y):
    """Veltman function F(x, y) = (x+y)/2 - x y/(x-y) ln(x/y), F(x, x) = 0  (x, y = masses squared)."""
    if abs(x - y) < 1e-6 * max(x, y):
        return 0.
    return 0.5 * (x + y) - x * y / (x - y) * math.log(x / y)


def delta_T(mH, mA, ma, mHc, sinp, MW=80.38, sw2=0.231):
    """One-loop Delta T of the 2HDM+a in the alignment limit (A, a = mixtures of the 2HDM A0 and the singlet P)."""
    c2, s2 = 1 - sinp ** 2, sinp ** 2
    H, A, a, C = mH ** 2, mA ** 2, ma ** 2, mHc ** 2
    br = _F(C, H) + c2 * _F(C, A) + s2 * _F(C, a) - c2 * _F(H, A) - s2 * _F(H, a)
    return br / (16 * math.pi * sw2 * MW ** 2)


def _evaluate(x, mA, mH, cfg):
    """Return (sigBR in fb, info) for couplings x at (mA, mH); sigBR = -1 if the point is not allowed."""
    p = dict(S.DEFAULTS)
    p.update(MXd=cfg["MXd"], gPXd=cfg["gPXd"], MT=172., MB=4.7, mh1=125.,
             mh2=mH, mh3=mA, mh4=cfg["ma"], mhc=mA - x.get("dmAc", cfg["dmAc"]),
             tanbeta=x["tanbeta"], sinp=x["sinp"], sinbma=math.sqrt(1 - x["cba"] ** 2),
             lam3=x["lam3"], laP1=x["laP1"], laP2=x["laP2"])
    if p["mhc"] < cfg["mhc_min"]:
        return -1, None
    dT = delta_T(mH, mA, cfg["ma"], p["mhc"], x["sinp"])
    if abs(dT) > cfg["T_max"]:
        return -1, None
    try:
        env = S.evaluate(**p)
        r = S.point(LUMI, O.K_MG, **p)
    except Exception:
        return -1, None
    if O.constraints(env, p, r, cfg):
        return -1, None
    l = [complex(env["l%d" % i]).real for i in range(1, 8)]
    lam = O.z2_basis(l, p["tanbeta"])
    umax = O.unitarity_max(lam)
    if cfg["unit"] and umax > 8 * math.pi:
        return -1, None
    totA = sum(r["wA"].values())
    br_ha = r["wA"].get((25, 55), 0.) / totA if totA > 0 else 0.
    monoh = r["sigA"] * 1e3 * br_ha * 0.58
    if monoh > cfg["monoh"]:
        return -1, None
    sH, _ = O.sigma_ggH(env, LUMI)
    if cfg["hh"] and sH * r["brHhh"] > O.hh_limit(mH):
        return -1, None
    info = dict(sigA_fb=r["sigA"] * 1e3, brAHa=r["brAHa"], brHhh=r["brHhh"], brachi=r["brachi"],
                GA_over_mA=r["GA"] / mA, GH=r["GH"], monoh_fb=monoh, ggH_hh_pb=sH * r["brHhh"],
                hh_limit_pb=O.hh_limit(mH), unit_over_8pi=umax / (8 * math.pi),
                sinbma=p["sinbma"], mhc=p["mhc"], dmAc=mA - p["mhc"], dT=dT)
    return r["sigBR"] * 1e3, info


def _walk(x, mA, mH, cfg, rng):
    lo, hi = _bounds(cfg)
    KEYS = keys(cfg)
    clip = lambda y: {k: min(hi[k], max(lo[k], y[k])) for k in KEYS}
    x = clip(x)
    best, info = _evaluate(x, mA, mH, cfg)
    if info is None:  # look for an allowed point near the start
        for _ in range(cfg["n_iter"]):
            y = clip({k: x[k] + rng.normal() * STEP[k] * 5 for k in KEYS})
            v, inf = _evaluate(y, mA, mH, cfg)
            if inf is not None:
                x, best, info = y, v, inf
                break
    if info is None:
        return -1, None, None
    T = 1.0
    for it in range(cfg["n_iter"]):
        y = clip({k: x[k] + rng.normal() * STEP[k] * T for k in KEYS})
        v, inf = _evaluate(y, mA, mH, cfg)
        if v > best:
            x, best, info = y, v, inf
        if it % max(1, cfg["n_iter"] // 3) == max(1, cfg["n_iter"] // 3) - 1:
            T *= 0.5
    return best, x, info


def optimize_point(args):
    mA, mH, cfg, k = args
    rng = np.random.default_rng(cfg["seed"] * 100003 + k)
    best = (-1, None, None)
    # m_H+- starting values: custodial with H (m_H+- = m_H), custodial with A (m_H+- = m_A), and the dmAc given
    dm_starts = [mA - mH, 0., cfg["dmAc"]]
    for s in range(cfg["n_start"]):
        start = dict(STARTS[s % len(STARTS)])
        if cfg["fit_dmAc"]:
            start["dmAc"] = dm_starts[s % len(dm_starts)]
        res = _walk(start, mA, mH, cfg, rng)
        if res[0] > best[0]:
            best = res
    return mA, mH, best


def grid(r):
    lo, hi, st = r
    return list(np.round(np.arange(lo, hi + 0.5 * st, st), 6))


def main():
    cfg = dict(CFG)
    if len(sys.argv) > 1:
        a = sys.argv[1]
        cfg.update(json.load(open(a)) if os.path.isfile(a) else json.loads(a))
    pts = [(mA, mH) for mA in grid(cfg["mA"]) for mH in grid(cfg["mH"])
           if mA > mH + cfg["ma"] + 5 and (cfg["fit_dmAc"] or mA - cfg["dmAc"] >= cfg["mhc_min"])]
    print("%d mass points (m_A > m_H + m_a + 5 GeV), m_a = %g, %s" % (
        len(pts), cfg["ma"], "m_A - m_H+- optimized in [%g, %g]" % tuple(cfg["dmAc_range"]) if cfg["fit_dmAc"]
        else "m_H+- = m_A - %g" % cfg["dmAc"]))
    jobs = [(mA, mH, cfg, k) for k, (mA, mH) in enumerate(pts)]
    nw = cfg["workers"] or os.cpu_count() or 1
    t0 = time.time()
    rows = []
    with mp.get_context("fork").Pool(nw, initializer=_init_worker) as pool:
        for i, (mA, mH, (v, x, info)) in enumerate(pool.imap_unordered(optimize_point, jobs)):
            rows.append((mA, mH, v, x, info))
            print("[%3d/%d  %5.0fs] mA=%6.1f mH=%6.1f  sigBR = %s" % (
                i + 1, len(jobs), time.time() - t0, mA, mH, ("%.2f fb" % v) if info else "no allowed point"),
                flush=True)
    rows.sort(key=lambda r: (r[0], r[1]))

    cols = ["mA", "mH", "mHc", "ma", "sigBR_fb", "sigA_fb", "brAHa", "brHhh", "brachi", "GA_over_mA",
            "monoh_fb", "ggH_hh_pb", "hh_limit_pb", "unit_over_8pi", "dT",
            "tanbeta", "sinp", "sinbma", "cba", "lam3", "laP1", "laP2", "dmAc"]
    with open(cfg["out"] + ".csv", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(cols)
        for mA, mH, v, x, info in rows:
            if info is None:
                w.writerow(["%g" % mA, "%g" % mH, "nan" if cfg["fit_dmAc"] else mA - cfg["dmAc"], cfg["ma"]] +
                           ["nan"] * (len(cols) - 4))
                continue
            d = dict(info, **x)
            w.writerow(["%g" % mA, "%g" % mH, "%.1f" % info["mhc"], cfg["ma"], "%.4g" % v] +
                       ["%.5g" % d[c] for c in cols[5:]])
    print("wrote %s.csv" % cfg["out"])
    good = [r for r in rows if r[4] is not None]
    if good:
        b = max(good, key=lambda r: r[2])
        print("best point: mA=%g mH=%g sigBR=%.2f fb  %s" % (b[0], b[1], b[2], json.dumps({k: round(b[3][k], 4) for k in keys(cfg)})))
    _plot(rows, cfg)


def _plot(rows, cfg):
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        from matplotlib.colors import LogNorm
    except ImportError:
        print("matplotlib not available: no plot")
        return
    xs, ys = grid(cfg["mA"]), grid(cfg["mH"])
    Z = np.full((len(ys), len(xs)), np.nan)
    for mA, mH, v, x, info in rows:
        if info is not None and v > 0:
            Z[ys.index(mH), xs.index(mA)] = v
    if np.all(np.isnan(Z)):
        print("no allowed points: no plot")
        return
    dx, dy = cfg["mA"][2], cfg["mH"][2]
    fig, ax = plt.subplots(figsize=(7.5, 5.5))
    im = ax.imshow(Z, origin="lower", aspect="auto", cmap="viridis",
                   extent=[xs[0] - dx / 2, xs[-1] + dx / 2, ys[0] - dy / 2, ys[-1] + dy / 2],
                   norm=LogNorm(vmin=max(np.nanmin(Z), 1e-3), vmax=np.nanmax(Z)))
    for mA, mH, v, x, info in rows:
        if info is not None and v > 0:
            lab = "%.3g" % v + ("\n%+.0f" % info["dmAc"] if cfg["fit_dmAc"] else "")
            ax.text(mA, mH, lab, ha="center", va="center", fontsize=5.5,
                    color="white" if v < np.sqrt(np.nanmin(Z) * np.nanmax(Z)) else "black")
    mm = np.array([xs[0], xs[-1]])
    ax.plot(mm, mm - cfg["ma"], "r--", lw=1, label=r"$m_A = m_H + m_a$")
    ax.set_xlim(xs[0] - dx / 2, xs[-1] + dx / 2); ax.set_ylim(ys[0] - dy / 2, ys[-1] + dy / 2)
    ax.set_xlabel(r"$m_A$ [GeV]"); ax.set_ylabel(r"$m_H$ [GeV]")
    ax.set_title(r"max $\sigma(gg\to A)\times$BR$(A\to Ha\to hh\,\chi\bar\chi)$ [fb], 13.6 TeV" + "\n" +
                 (r"$m_a$=%g GeV, " % cfg["ma"]) +
                 (r"$m_A-m_{H^\pm}$ optimized (2nd number), " if cfg["fit_dmAc"] else r"$m_{H^\pm}=m_A-%g$ GeV, " % cfg["dmAc"]) +
                 r"$\tan\beta\geq%g$, $|c_{\beta-\alpha}|\leq%g$, $|\Delta T|\leq%g$" % (
                     cfg["tb_min"], cfg["cba_max"], cfg["T_max"]), fontsize=9)
    ax.legend(loc="upper left", fontsize=8)
    fig.colorbar(im, ax=ax, label=r"$\sigma\times$BR [fb]")
    fig.tight_layout()
    fig.savefig(cfg["out"] + ".png", dpi=150)
    print("wrote %s.png" % cfg["out"])


if __name__ == "__main__":
    main()
