"""
Fast estimate of  sigma(gg -> A) x BR(A -> H a) x BR(H -> h h) x BR(a -> chi chi)  in the 2HDM+a
(UFO model Pseudoscalar_2HDMI, as used in MadGraph).

  * widths / BRs: tree-level 2-body partial widths from the model's own decays.py (what MadGraph's
    "auto" widths use), after evaluating all internal parameters of parameters.py
  * sigma(gg -> A): LO narrow-width formula with top+bottom loops; the A-quark couplings are taken
    from the model (via Gamma(A->tt), Gamma(A->bb)); gluon luminosity from LHAPDF;
    normalized to a MadGraph LO cross section at a reference point (K_MG)
"""
import cmath
import math
import sys

import os
# UFO model directory: $HDMA_MODEL, default ./Pseudoscalar_2HDMI next to this file
MODEL_DIR = os.environ.get("HDMA_MODEL", os.path.join(os.path.dirname(os.path.abspath(__file__)), "Pseudoscalar_2HDMI"))
# directory containing the LHAPDF set NNPDF31_nnlo_as_0118: $HDMA_PDFPATH (optional if LHAPDF finds it already)
PDF_PATH = os.environ.get("HDMA_PDFPATH")
sys.path.insert(0, MODEL_DIR)
import parameters as _P  # noqa: E402
import decays as _D      # noqa: E402

GEV2_TO_PB = 0.3893794e9

_funcs = dict(cmath=cmath, complexconjugate=lambda z: complex(z).conjugate(), re=lambda z: complex(z).real,
              im=lambda z: complex(z).imag, sec=lambda z: 1 / cmath.cos(z), csc=lambda z: 1 / cmath.sin(z),
              cot=lambda z: 1 / cmath.tan(z), asec=lambda z: cmath.acos(1 / z), acsc=lambda z: cmath.asin(1 / z),
              theta_function=lambda x, y, z: y if x else z, cond=lambda c, a, b: a if c == 0 else b,
              reglog=lambda z: 0 if z == 0 else cmath.log(z), abs=abs, complex=complex)

DEFAULTS = {p.name: p.value for p in _P.all_parameters if p.nature == "external"}


def evaluate(**ext):
    env = dict(_funcs)
    for p in _P.all_parameters:
        if p.nature == "external":
            env[p.name] = ext.get(p.name, DEFAULTS[p.name])
    for p in _P.all_parameters:
        if p.nature == "internal":
            env[p.name] = eval(p.value, env)
    return env


import re as _re
import numpy as _np


class _NPMath:  # numpy stand-in for cmath, so a width expression can be evaluated on arrays of daughter masses
    sqrt = staticmethod(lambda z: _np.sqrt(_np.asarray(z, dtype=complex)))
    pi = math.pi
    e = math.e
    cos = staticmethod(_np.cos); sin = staticmethod(_np.sin); tan = staticmethod(_np.tan)
    exp = staticmethod(_np.exp); log = staticmethod(_np.log)
    asin = staticmethod(_np.arcsin); acos = staticmethod(_np.arccos); atan = staticmethod(_np.arctan)


FIXED_WIDTHS = {23: 2.4952, 24: 2.085}      # Z, W (GeV)
OFFSHELL_NSIG = 15.                          # Breit-Wigner range: |m* - m| < 15 Gamma
_GL_X, _GL_W = _np.polynomial.legendre.leggauss(400)   # many nodes: the integrand has a sqrt endpoint at threshold
_compiled = {}


def _daughter_fn(expr, M, prods):
    """Return code for the 2-body width with the daughter masses replaced by the variables MOFF1, MOFF2."""
    key = (expr, M)
    if key in _compiled:
        return _compiled[key]
    n1, n2 = prods[0].mass.name, prods[1].mass.name
    lam = "cmath.sqrt(%s**4 + MOFF1**4 + MOFF2**4 - 2*%s**2*MOFF1**2 - 2*%s**2*MOFF2**2 - 2*MOFF1**2*MOFF2**2)" % (M, M, M)
    e = expr
    if n1 == n2:
        e = e.replace("cmath.sqrt(%s**4 - 4*%s**2*%s**2)" % (M, M, n1), lam)
        e = _re.sub(r"\b%s\b" % n1, "(0.5*(MOFF1+MOFF2))", e)
    else:
        e = _re.sub(r"\b%s\b" % n1, "MOFF1", e)
        e = _re.sub(r"\b%s\b" % n2, "MOFF2", e)
    code = compile(e, "<width>", "eval")
    _compiled[key] = code
    return code


def _bw_nodes(m, g, mmax, thr=0., kind="scalar"):
    """Gauss-Legendre nodes for an off-shell daughter of mass m and width g, with sqrt(s) from its decay threshold
    thr up to mmax.  Integration variable t = atan((s - m^2)/(m g)); the running-width factor
    r = sqrt(s) Gamma(sqrt(s)) / (m Gamma) is applied, with Gamma(sqrt(s)) ~ sqrt(s) beta(s) for a (pseudo)scalar
    decaying to a pair of mass thr/2, and ~ sqrt(s) for Z/W.  Returns (sqrt(s), weights)."""
    if mmax <= max(thr, 1e-3):
        return _np.zeros(0), _np.zeros(0)
    lo = max(thr, 1e-3) ** 2
    hi = mmax ** 2
    if hi <= lo:
        return _np.zeros(0), _np.zeros(0)
    t1, t2 = math.atan((lo - m * m) / (m * g)), math.atan((hi - m * m) / (m * g))
    t = 0.5 * (t2 - t1) * _GL_X + 0.5 * (t1 + t2)
    w = 0.5 * (t2 - t1) * _GL_W / math.pi
    s = _np.clip(m * m + m * g * _np.tan(t), lo, hi)
    if kind == "vector":
        r = s / (m * m)
    else:
        beta = lambda q2: _np.sqrt(_np.clip(1 - thr * thr / q2, 0, None))
        b0 = beta(m * m)
        r = s / (m * m) * (beta(s) / b0 if b0 > 0 else 1.)   # sqrt(s) Gamma(sqrt(s)) / (m Gamma), Gamma(q) ~ q beta
    return _np.sqrt(s), w * r


def _flat_prods(d):
    for prods in d.partial_widths:
        for q in prods:
            yield q


def widths(env, pdgs=(55, 35, 36, 25), offshell=True):
    """Partial widths from the model's decays.py.  With offshell=True, a channel whose unstable daughter(s)
    (a, H, A, H+-, Z, W) are within a few widths of threshold, or below it, is integrated over Breit-Wigner
    daughter masses: this reproduces MadGraph's 3-body widths such as H -> a a* -> a chi chi~ ."""
    out = {}
    gam = dict(FIXED_WIDTHS)
    thr = {23: 0., 24: 0.}   # decay threshold (mass sum of the dominant channel) of each unstable particle
    decs = {d.particle.pdg_code: d for d in _D.all_decays}
    npenv = dict(env); npenv["cmath"] = _NPMath; npenv["abs"] = _np.abs
    for pdg in pdgs:
        d = decs.get(pdg)
        if d is None:
            continue
        M = d.particle.mass.name
        m_par = complex(env[M]).real
        pw = {}
        for prods, expr in d.partial_widths.items():
            ms = [complex(env[q.mass.name]).real for q in prods]
            gs = [gam.get(abs(q.pdg_code), 0.) if offshell else 0. for q in prods]
            unstable = [g > 1e-3 * max(m, 1e-9) for m, g in zip(ms, gs)]
            w = 0.
            closed = sum(ms) >= m_par
            if not closed:                       # open: plain 2-body width (as MadGraph)
                try:
                    w = complex(eval(expr, env)).real
                except (ZeroDivisionError, ValueError):
                    w = 0.
            elif offshell and len(prods) == 2 and any(unstable):
                # closed: one daughter off-shell (Breit-Wigner, running width), the other on-shell, summed over
                # which daughter is off-shell (gives the factor 2 for identical daughters).  This is MadGraph's
                # 3-body width, e.g. H -> a a* -> a chi chi~ .
                code = _daughter_fn(expr, M, prods)
                ids = [abs(q.pdg_code) for q in prods]
                for i in (0, 1):
                    if not unstable[i]:
                        continue
                    j = 1 - i
                    kind = "vector" if ids[i] in (23, 24) else "scalar"
                    x, wt = _bw_nodes(ms[i], gs[i], m_par - ms[j], thr.get(ids[i], 0.), kind)
                    if not wt.size:
                        continue
                    other = _np.full_like(x, ms[j])
                    npenv["MOFF1"], npenv["MOFF2"] = (x, other) if i == 0 else (other, x)
                    with _np.errstate(all="ignore"):
                        val = _np.real(_np.asarray(eval(code, npenv), dtype=complex))
                    w += float(_np.sum(_np.where(_np.isfinite(val), val, 0.) * wt))
            else:
                continue
            if w > 0:
                key = tuple(sorted(abs(p.pdg_code) for p in prods))
                pw[key] = pw.get(key, 0.) + w   # sum charge-conjugate channels (H+W- and H-W+)
        out[pdg] = pw
        gam[pdg] = sum(pw.values())
        if pw:   # threshold of the dominant channel, used for the running width of this particle when off-shell
            tot_ = sum(pw.values())
            cands = [k for k in pw if pw[k] > 0.05 * tot_]
            def _msum(k):
                return sum(complex(env[decs[k].particle.mass.name]).real if k in decs else
                           next(complex(env[q.mass.name]).real for q in _flat_prods(d) if abs(q.pdg_code) == k)
                           for k in k)
            thr[pdg] = min(_msum(k) for k in cands)
    return out


def br(pw, final):
    tot = sum(pw.values())
    return (pw.get(tuple(sorted(final)), 0.) / tot if tot > 0 else 0.), tot


# ---------------- gg -> A (LO, narrow width) ----------------
def _fA(tau):
    if tau <= 1:
        return complex(math.asin(math.sqrt(tau)) ** 2)
    b = math.sqrt(1 - 1 / tau)
    return -0.25 * (cmath.log((1 + b) / (1 - b)) - 1j * math.pi) ** 2


def _A_half(tau):  # CP-odd fermion loop form factor
    return 2 * _fA(tau) / tau


class Lumi:
    def __init__(self, pdfname="NNPDF31_nnlo_as_0118", path=None, sqrts=13600.):
        path = path or PDF_PATH
        import lhapdf
        if path:
            lhapdf.pathsPrepend(path)
        lhapdf.setVerbosity(0)
        self.pdf = lhapdf.mkPDF(pdfname, 0)
        self.s = sqrts ** 2
        self.cache = {}

    def tau_dLdtau(self, m, mu=None):
        """tau dL^gg/dtau at tau = m^2/s, factorization scale mu (default m)."""
        key = (round(m, 3), mu)
        if key in self.cache:
            return self.cache[key]
        mu = mu or m
        tau = m * m / self.s
        n = 400
        ln_lo, tot = math.log(tau), 0.
        for i in range(n):  # integrate over y = ln x1 in [ln tau, 0]
            y = ln_lo * (1 - (i + 0.5) / n)
            x1 = math.exp(y)
            x2 = tau / x1
            tot += self.pdf.xfxQ(21, x1, mu) * self.pdf.xfxQ(21, x2, mu)
        val = tot * (-ln_lo) / n          # = tau dL/dtau  (xf*xf already contains the x factors)
        self.cache[key] = val
        return val

    def alphas(self, mu):
        return self.pdf.alphasQ(mu)


def kappa_q(env, pw36, q):
    """A-quark coupling relative to a SM-like pseudoscalar, from the model's Gamma(A->qq)."""
    mA = env["mh3"].real
    mq = {6: env["MT"], 5: env["MB"]}[q].real
    if mA <= 2 * mq:
        return None
    beta = math.sqrt(1 - 4 * mq * mq / mA ** 2)
    g_sm = 3 * env["Gf"].real * mq * mq * mA * beta / (4 * math.sqrt(2) * math.pi)
    return math.sqrt(pw36.get((q, q), 0.) / g_sm)


def sigma_ggA_LO(env, pw36, lumi, mu_frac=0.5, kappa_t_fallback=None):
    """LO sigma(pp -> A) in pb (before the MadGraph normalization factor)."""
    mA = env["mh3"].real
    mt, mb = env["MT"].real, env["MB"].real
    kt = kappa_q(env, pw36, 6)
    if kt is None:  # below tt threshold: cos(theta)/tan(beta) in type-II 2HDM+a
        kt = kappa_t_fallback if kappa_t_fallback is not None else \
            math.sqrt(1 - env["sinp"].real ** 2) / env["tanbeta"].real
    kb = kappa_q(env, pw36, 5) or 0.
    amp = kt * _A_half(mA ** 2 / (4 * mt * mt)) + kb * _A_half(mA ** 2 / (4 * mb * mb))
    a_s = lumi.alphas(mu_frac * mA)
    gam_gg = env["Gf"].real * a_s ** 2 * mA ** 3 / (36 * math.sqrt(2) * math.pi ** 3) * abs(0.75 * amp) ** 2
    return math.pi ** 2 * gam_gg / (8 * mA ** 3) * lumi.tau_dLdtau(mA, mu_frac * mA) * GEV2_TO_PB, kt


def point(lumi, k_mg=1.0, offshell=True, **ext):
    env = evaluate(**ext)
    w = widths(env, offshell=offshell)
    brAHa, gA = br(w[36], (35, 55))
    brHhh, gH = br(w[35], (25, 25))
    brachi, ga = br(w[55], (52, 52))
    sig, kt = sigma_ggA_LO(env, w[36], lumi)
    sig *= k_mg
    return dict(sigA=sig, kt=kt, brAHa=brAHa, brHhh=brHhh, brachi=brachi, GA=gA, GH=gH, Ga=ga,
                sigBR=sig * brAHa * brHhh * brachi, wA=w[36], wH=w[35], wa=w[55])
