// Find the 2HDM+a (Type I, non-aligned) parameters that maximise
//     sigma(gg -> A) x BR(A -> H a) x BR(H -> h h) x BR(a -> chi chi)
// for a given (mA, mH), in the spirit of HAAahah/scripts/minimizer_HAAahah.C.
//
// Free parameters (7):  sin(theta) (sinp), sin(beta-alpha) (sinbma), tan(beta), lambda_3, lambda_P1, lambda_P2,
//                       and the mass splitting dM = mA - mH+-.
// Fixed: ma = 140 GeV, mchi = 10 GeV, g_chi = 1, mh = 125 GeV (ma as in scan2hdma).
// Ranges as in scan2hdma: 2 <= tan(beta) <= 20, 0 <= cos(beta-alpha) <= 0.15.
//
// Widths: partial widths of the UFO model Pseudoscalar_2HDMI (the model of the MadGraph samples), translated to C++ in
// Pseudoscalar2HDMI_widths.h by make_cpp_widths.py, INCLUDING the off-shell (3-body) widths that MadGraph adds for
// closed channels with an unstable daughter (e.g. H -> a a* -> a chi chi~), computed as in scan2hdma.py.
// Production: sigma(gg -> A) = sigmaNorm * sigma0(mA) * cos^2(theta) / tan^2(beta), sigma0 from the MadGraph 13.6 TeV
// 2D scan; sigmaNorm = 0.896 brings it to the MadGraph cross sections of the 17 signal samples (ratio 0.896 +- 0.0015).
//
// Constraints (one-sided penalties, zero inside the allowed region):
//   Gamma_H / mH < fracWH (0.05), Gamma_A / mA < fracWA (0.10),
//   perturbativity: |lambda_1..5| in the Z2 basis (computed from the masses and angles), |lam3|, |laP1|, |laP2| < 4 pi,
//   tree-level unitarity: largest |eigenvalue| of the 2HDM scattering matrix < 8 pi,
//   potential bounded from below (Z2-basis conditions; lambda_P1, lambda_P2 >= 0 by their range),
//   |Delta rho| < drhoMax (1e-3, i.e. |T| < ~0.13; PDG: T = 0.03 +- 0.12),
//   mH+- >= mHcMin (150 GeV).
// The perturbativity, unitarity and boundedness checks and the off-shell widths are ports of scan2hdma
// (scan_opt.py, scan2hdma.py). Not included: the gg -> H -> hh and mono-h limits of scan2hdma.
// If no point satisfies them, the least-violating point is reported (allowed = 0).
//
// compile:  g++ -O2 minimizer_AHhha_nonAligned.C -o minimize_nonAligned `root-config --cflags --libs`
//           (standalone Minuit2 without ROOT: add -DMINUIT2_STANDALONE -I<minuit2>/include -L<minuit2>/lib -lMinuit2 -lMinuit2Math)
// run:      ./minimize_nonAligned mA mH [options]     (./minimize_nonAligned without arguments lists the options)
#include "Math/Minimizer.h"
#include "Math/Functor.h"
#ifdef MINUIT2_STANDALONE
#include "Minuit2/Minuit2Minimizer.h"
#else
#include "Math/Factory.h"
#endif
#include <algorithm>
#include <array>
#include <complex>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Pseudoscalar2HDMI_widths.h"

static inline double sq_(double x) { return x * x; }

// ---------------------------------------------------------------------------------------------------------------
// configuration (all can be changed on the command line)
// ---------------------------------------------------------------------------------------------------------------
static const int NPAR = 7;
static const char* kNames[NPAR] = {"sinp", "sinbma", "tanb", "lam3", "laP1", "laP2", "dM_A_Hc"};

struct Config {
  // m_a, tan(beta) and cos(beta-alpha) ranges as in scan2hdma (scan_cfg_fit_dmAc.json / scan_mA_mH.py):
  // ma = 140 GeV, 2 <= tan(beta) <= 20, 0 <= cos(beta-alpha) <= 0.15 (sin(beta-alpha) >= 0.988686)
  double mA = 0, mH = 0, ma = 140., mchi = 10., gchi = 1.;
  // ranges of the free parameters
  double lo[NPAR] = {0.01, 0.98868599, 2., 1., 0., 0., -150.};
  double hi[NPAR] = {std::sqrt(2.) / 2., 1., 20., 10., 10., 10., 150.};
  double fracWH = 0.05, fracWA = 0.10, drhoMax = 1e-3, mHcMin = 150.;
  double lamMax = 4 * M_PI;   // perturbativity: |lambda_1..5| (Z2 basis), |lam3|, |laP1|, |laP2| < lamMax
  double unitMax = 8 * M_PI;  // tree-level unitarity: largest |eigenvalue| of the 2HDM scattering matrix < unitMax
  double sigmaNorm = 0.896;   // sigma(gg->A) normalisation: MadGraph / sigma0 table (17 MadGraph runs: 0.896 +- 0.0015)
  int offshell = 1;           // 1: include the off-shell (3-body) widths as MadGraph / scan2hdma do
  int nGL = 100;              // Gauss-Legendre nodes of the off-shell integrals (400 in scan2hdma; same result to <1e-4)
  double penalty = 1e5;     // weight of the constraint penalties
  double tolerance = 1e-3;  // relative overshoot of a limit still counted as allowed
  int nSample = 300000;     // random starting-point search (the allowed region is narrow once unitarity is required)
  int nStart = 30;          // Migrad runs from the best samples
  int print = 0;
};
static Config gCfg;

// sigma(gg -> A) [pb] at 13.6 TeV for cos(theta) = 1, tan(beta) = 1 (MadGraph, from the 2D scan at tb = 2, sinp = 0.1)
static const double kMA[] = {410, 460, 510, 560, 610, 660, 700, 750, 800};
static const double kSigma0[] = {10.545, 5.8431, 3.3951, 2.0590, 1.2922, 0.83561, 0.60035, 0.40591, 0.27925};
static const int kN = sizeof(kMA) / sizeof(kMA[0]);

double sigma0_ggA(double mA) {
  int i = 0;
  if (mA >= kMA[kN - 1]) i = kN - 2;
  else if (mA > kMA[0]) while (kMA[i + 1] < mA) ++i;
  if (mA < kMA[0] || mA > kMA[kN - 1]) {
    static bool warned = false;
    if (!warned) std::cerr << "WARNING: mA = " << mA << " outside the sigma0 table [" << kMA[0] << ", " << kMA[kN - 1]
                           << "]: extrapolated" << std::endl;
    warned = true;
  }
  const double t = (mA - kMA[i]) / (kMA[i + 1] - kMA[i]);
  return std::exp((1 - t) * std::log(kSigma0[i]) + t * std::log(kSigma0[i + 1]));
}

// ---------------------------------------------------------------------------------------------------------------
// Delta rho. Aligned 2HDM+a part as in HAAahah/scripts/rho.C; outside alignment the h/H weights sin^2(beta-alpha),
// cos^2(beta-alpha) and the gauge-boson terms of the 2HDM are added; the CP-odd state of the doublet is shared between
// A (cos^2 theta) and a (sin^2 theta).
// ---------------------------------------------------------------------------------------------------------------
double Fpv(double m1, double m2) {  // F(x, y) = (x + y)/2 - x y / (x - y) ln(x / y), x = m1^2, y = m2^2
  const double x = m1 * m1, y = m2 * m2;
  if (std::fabs(x - y) < 1e-9 * (x + y)) return 0.;
  return 0.5 * (x + y) - x * y / (x - y) * std::log(x / y);
}

double deltaRho(double sinp, double sinbma, double mA, double mH, double mHc, double ma, double vev, double MW, double MZ) {
  const double mh = 125.;
  const double c2t = 1 - sinp * sinp, s2t = sinp * sinp;
  const double s2ba = sinbma * sinbma, c2ba = 1 - s2ba;
  const double odd_Hc = c2t * Fpv(mHc, mA) + s2t * Fpv(mHc, ma);
  const double odd_H = c2t * Fpv(mA, mH) + s2t * Fpv(ma, mH);
  const double odd_h = c2t * Fpv(mA, mh) + s2t * Fpv(ma, mh);
  const double v = odd_Hc + s2ba * (Fpv(mHc, mH) - odd_H) + c2ba * (Fpv(mHc, mh) - odd_h) +
                   3 * c2ba * (Fpv(MZ, mH) - Fpv(MW, mH) - Fpv(MZ, mh) + Fpv(MW, mh));
  return v / (16. * M_PI * M_PI * vev * vev);
}

// ---------------------------------------------------------------------------------------------------------------
// Widths with off-shell daughters (port of scan2hdma.py widths(), offshell=True). Parents in the order a, H, A, so
// that the widths of a and H are known when they appear as unstable daughters. Z and W have fixed widths; h and H+-
// are treated as stable daughters (as in scan2hdma). An open channel is the plain 2-body width; a closed channel with
// an unstable daughter is integrated over a Breit-Wigner mass of that daughter (running width), the other daughter
// on shell, summed over which daughter is off shell: MadGraph's 3-body widths such as H -> a a* -> a chi chi~.
// ---------------------------------------------------------------------------------------------------------------
static std::vector<double> gGLx, gGLw;
void initGaussLegendre(int n) {
  gGLx.assign(n, 0.); gGLw.assign(n, 0.);
  for (int i = 0; i < n; ++i) {
    double x = std::cos(M_PI * (i + 0.75) / (n + 0.5)), dp = 0;
    for (int it = 0; it < 100; ++it) {
      double p0 = 1., p1 = x;
      for (int k = 2; k <= n; ++k) { const double p2 = ((2 * k - 1) * x * p1 - (k - 1) * p0) / k; p0 = p1; p1 = p2; }
      dp = n * (x * p1 - p0) / (x * x - 1);
      const double dx = p1 / dp;
      x -= dx;
      if (std::fabs(dx) < 1e-15) break;
    }
    gGLx[i] = x; gGLw[i] = 2. / ((1 - x * x) * dp * dp);
  }
}

// sqrt(s) nodes and weights for an off-shell daughter (mass m, width g) between its decay threshold thr and mmax
void bwNodes(double m, double g, double mmax, double thr, bool vec, std::vector<double>& xs, std::vector<double>& ws) {
  xs.clear(); ws.clear();
  const double lo = sq_(std::max(thr, 1e-3)), hi = mmax * mmax;
  if (mmax <= std::max(thr, 1e-3) || hi <= lo) return;
  const double t1 = std::atan((lo - m * m) / (m * g)), t2 = std::atan((hi - m * m) / (m * g));
  auto beta = [thr](double q2) { return std::sqrt(std::max(1. - thr * thr / q2, 0.)); };
  const double b0 = beta(m * m);
  for (size_t i = 0; i < gGLx.size(); ++i) {
    const double t = 0.5 * (t2 - t1) * gGLx[i] + 0.5 * (t1 + t2);
    const double w = 0.5 * (t2 - t1) * gGLw[i] / M_PI;
    const double s = std::min(std::max(m * m + m * g * std::tan(t), lo), hi);
    const double r = vec ? s / (m * m) : s / (m * m) * (b0 > 0 ? beta(s) / b0 : 1.);
    xs.push_back(std::sqrt(s)); ws.push_back(w * r);
  }
}

struct Widths {
  std::map<std::pair<int, int>, double> pw;
  double tot = 0;
  double get(int a, int b) const { auto it = pw.find({std::min(a, b), std::max(a, b)}); return it == pw.end() ? 0. : it->second; }
};

// gam: widths of the possible unstable daughters (filled as the parents are computed), thr: their decay thresholds
template <class PW>
Widths partialWidths(int pdg, double mPar, const std::vector<p2hdm::Chan>& chans, PW pw,
                     std::map<int, double>& gam, std::map<int, double>& thr) {
  Widths W;
  std::vector<double> xs, ws;
  for (size_t k = 0; k < chans.size(); ++k) {
    const auto& c = chans[k];
    const int ids[2] = {c.id1, c.id2};
    const double ms[2] = {c.m1, c.m2};
    double gs[2];
    bool unstable[2];
    for (int i = 0; i < 2; ++i) {
      auto it = gam.find(ids[i]);
      gs[i] = (gCfg.offshell && it != gam.end()) ? it->second : 0.;
      unstable[i] = gs[i] > 1e-3 * std::max(ms[i], 1e-9);
    }
    double w = 0;
    if (ms[0] + ms[1] < mPar) {
      w = pw((int)k, ms[0], ms[1]);
      if (!std::isfinite(w)) w = 0;
    } else if (gCfg.offshell && (unstable[0] || unstable[1])) {
      for (int i = 0; i < 2; ++i) {
        if (!unstable[i]) continue;
        const int j = 1 - i;
        const bool vec = ids[i] == 23 || ids[i] == 24;
        auto ti = thr.find(ids[i]);
        bwNodes(ms[i], gs[i], mPar - ms[j], ti == thr.end() ? 0. : ti->second, vec, xs, ws);
        for (size_t n = 0; n < xs.size(); ++n) {
          const double v = i == 0 ? pw((int)k, xs[n], ms[j]) : pw((int)k, ms[j], xs[n]);
          if (std::isfinite(v)) w += v * ws[n];
        }
      }
    } else continue;
    if (w > 0) { W.pw[{std::min(ids[0], ids[1]), std::max(ids[0], ids[1])}] += w; W.tot += w; }
  }
  gam[pdg] = W.tot;
  if (W.tot > 0) {  // threshold of the dominant channel(s): used for the running width when this particle is off shell
    double t = 1e30;
    for (const auto& c : chans) {
      auto it = W.pw.find({std::min(c.id1, c.id2), std::max(c.id1, c.id2)});
      if (it != W.pw.end() && it->second > 0.05 * W.tot) t = std::min(t, c.m1 + c.m2);
    }
    thr[pdg] = t;
  }
  return W;
}

// ---------------------------------------------------------------------------------------------------------------
// Quartic couplings in the Z2 basis, perturbativity, tree-level unitarity, bounded-from-below potential
// (port of scan_opt.py: z2_basis, unitarity_max, quartic_min). The UFO gives the Higgs-basis couplings l1..l7 with
//   V4 = l1 a^2 + l2 b^2 + l3 a b + l4 |c|^2 + 2 Re[l5 c^2 + l6 a c + l7 b c],  a = |H1|^2, b = |H2|^2, c = H1^+ H2;
// rotating by beta to the Z2 basis (Phi1, Phi2) and fitting
//   V4 = lam1/2 A^2 + lam2/2 B^2 + lam3 A B + lam4 |C|^2 + lam5 Re C^2 + lam6 A Re C + lam7 B Re C
// on fixed random field configurations (exact: V4 is a quadratic form in these invariants).
// ---------------------------------------------------------------------------------------------------------------
typedef std::complex<double> cd;
struct Doublets { cd h1[2], h2[2]; };
static std::vector<Doublets> gZ2conf;           // configurations for the fit
static std::vector<std::vector<double>> gZ2pinv;  // 7 x N pseudo-inverse of the design matrix
static std::vector<Doublets> gDirs;             // random directions for the bounded-from-below test (fallback)

static double V4(const double* l, double a, double b, cd c) {
  return l[0] * a * a + l[1] * b * b + l[2] * a * b + l[3] * std::norm(c) +
         2 * std::real(l[4] * c * c + l[5] * a * c + l[6] * b * c);
}
static void invariants(const cd* x, const cd* y, double& a, double& b, cd& c) {
  a = std::norm(x[0]) + std::norm(x[1]); b = std::norm(y[0]) + std::norm(y[1]);
  c = std::conj(x[0]) * y[0] + std::conj(x[1]) * y[1];
}

void initZ2() {
  std::mt19937 rng(7);
  std::normal_distribution<double> nd(0., 1.);
  auto draw = [&]() { Doublets d; for (int k = 0; k < 2; ++k) { d.h1[k] = cd(nd(rng), nd(rng)); d.h2[k] = cd(nd(rng), nd(rng)); } return d; };
  const int N = 60;
  std::vector<std::vector<double>> X;
  for (int n = 0; n < N; ++n) {
    gZ2conf.push_back(draw());
    double A, B; cd C;
    invariants(gZ2conf.back().h1, gZ2conf.back().h2, A, B, C);
    X.push_back({A * A / 2, B * B / 2, A * B, std::norm(C), std::real(C * C), A * std::real(C), B * std::real(C)});
  }
  // (X^T X)^-1 X^T by Gauss-Jordan
  double M[7][14] = {{0}};
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) for (int n = 0; n < N; ++n) M[i][j] += X[n][i] * X[n][j];
    M[i][7 + i] = 1;
  }
  for (int c = 0; c < 7; ++c) {
    int piv = c;
    for (int r = c + 1; r < 7; ++r) if (std::fabs(M[r][c]) > std::fabs(M[piv][c])) piv = r;
    for (int k = 0; k < 14; ++k) std::swap(M[c][k], M[piv][k]);
    const double d = M[c][c];
    for (int k = 0; k < 14; ++k) M[c][k] /= d;
    for (int r = 0; r < 7; ++r) if (r != c) { const double f = M[r][c]; for (int k = 0; k < 14; ++k) M[r][k] -= f * M[c][k]; }
  }
  gZ2pinv.assign(7, std::vector<double>(N, 0.));
  for (int i = 0; i < 7; ++i) for (int n = 0; n < N; ++n) for (int j = 0; j < 7; ++j) gZ2pinv[i][n] += M[i][7 + j] * X[n][j];
  for (int n = 0; n < 4000; ++n) gDirs.push_back(draw());
}

// Higgs-basis l[0..6] -> Z2-basis lam[0..6] (lam1..lam7)
void z2Basis(const double* l, double tb, double* lam) {
  const double be = std::atan(tb), cb = std::cos(be), sb = std::sin(be);
  for (int i = 0; i < 7; ++i) lam[i] = 0;
  for (size_t n = 0; n < gZ2conf.size(); ++n) {
    const auto& P = gZ2conf[n];
    cd x[2], y[2];
    for (int k = 0; k < 2; ++k) { x[k] = cb * P.h1[k] + sb * P.h2[k]; y[k] = -sb * P.h1[k] + cb * P.h2[k]; }
    double a, b; cd c;
    invariants(x, y, a, b, c);
    const double v = V4(l, a, b, c);
    for (int i = 0; i < 7; ++i) lam[i] += gZ2pinv[i][n] * v;
  }
}

// largest |eigenvalue| of the tree-level 2 -> 2 scattering matrix (Z2 basis, lam1/2 normalisation; scan_opt.py)
double unitarityMax(const double* lam) {
  const double l1 = lam[0], l2 = lam[1], l3 = lam[2], l4 = lam[3], l5 = lam[4];
  auto r = [](double x) { return std::sqrt(std::max(x, 0.)); };
  double ev[12];
  int k = 0;
  for (int s : {1, -1}) {
    ev[k++] = 1.5 * (l1 + l2) + s * r(2.25 * sq_(l1 - l2) + sq_(2 * l3 + l4));
    ev[k++] = 0.5 * (l1 + l2) + s * 0.5 * r(sq_(l1 - l2) + 4 * l4 * l4);
    ev[k++] = 0.5 * (l1 + l2) + s * 0.5 * r(sq_(l1 - l2) + 4 * l5 * l5);
  }
  ev[k++] = l3 + 2 * l4 - 3 * l5; ev[k++] = l3 - l5; ev[k++] = l3 + 2 * l4 + 3 * l5;
  ev[k++] = l3 + l5; ev[k++] = l3 + l4; ev[k++] = l3 - l4;
  double m = 0;
  for (double e : ev) m = std::max(m, std::fabs(e));
  return m;
}

// bounded-from-below margin (> 0: bounded). Z2-symmetric quartics (lam6 = lam7 = 0): the exact conditions
// lam1 > 0, lam2 > 0, lam3 > -sqrt(lam1 lam2), lam3 + lam4 - |lam5| > -sqrt(lam1 lam2); otherwise the minimum of
// V4 / (a + b)^2 over random directions (as scan_opt.quartic_min)
double boundedMargin(const double* l, const double* lam) {
  if (std::fabs(lam[5]) + std::fabs(lam[6]) < 1e-6 * (1 + std::fabs(lam[0]) + std::fabs(lam[1]))) {
    if (lam[0] <= 0 || lam[1] <= 0) return std::min(lam[0], lam[1]);
    const double r = std::sqrt(lam[0] * lam[1]);
    return std::min({lam[0], lam[1], lam[2] + r, lam[2] + lam[3] - std::fabs(lam[4]) + r});
  }
  double q = 1e30;
  for (const auto& d : gDirs) {
    double a, b; cd c;
    invariants(d.h1, d.h2, a, b, c);
    q = std::min(q, V4(l, a, b, c) / sq_(a + b));
  }
  return q;
}

// ---------------------------------------------------------------------------------------------------------------
// model evaluation
// ---------------------------------------------------------------------------------------------------------------
struct Point {
  double par[NPAR];
  double mHc, xsec, sigmaA, brAHa, brHhh, brachichi, gA, gH, ga, maxLam, drho;
  double unit, bounded, lamZ2[7];
};

Point evaluate(const double* x) {
  Point p{};
  for (int i = 0; i < NPAR; ++i) p.par[i] = x[i];
  const double sinp = x[0], sinbma = std::min(x[1], 1.), tb = x[2], lam3 = x[3], laP1 = x[4], laP2 = x[5];
  p.mHc = gCfg.mA - x[6];
  p2hdm::Model m;
  m.mh1 = 125.; m.mh2 = gCfg.mH; m.mh3 = gCfg.mA; m.mhc = p.mHc; m.mh4 = gCfg.ma; m.MXd = gCfg.mchi; m.gPXd = gCfg.gchi;
  m.sinp = sinp; m.sinbma = sinbma; m.tanbeta = tb; m.lam3 = lam3; m.laP1 = laP1; m.laP2 = laP2;
  m.compute();
  std::map<int, double> gam = {{23, m.WZ}, {24, m.WW}}, thr = {{23, 0.}, {24, 0.}};
  const Widths wa = partialWidths(55, m.mh4, m.chans_a(), [&m](int k, double x, double y) { return m.pw_a(k, x, y); }, gam, thr);
  const Widths wH = partialWidths(35, m.mh2, m.chans_H(), [&m](int k, double x, double y) { return m.pw_H(k, x, y); }, gam, thr);
  const Widths wA = partialWidths(36, m.mh3, m.chans_A(), [&m](int k, double x, double y) { return m.pw_A(k, x, y); }, gam, thr);
  p.gA = wA.tot; p.gH = wH.tot; p.ga = wa.tot;
  p.brAHa = p.gA > 0 ? wA.get(35, 55) / p.gA : 0;
  p.brHhh = p.gH > 0 ? wH.get(25, 25) / p.gH : 0;
  p.brachichi = p.ga > 0 ? wa.get(52, 52) / p.ga : 0;
  p.sigmaA = gCfg.sigmaNorm * sigma0_ggA(gCfg.mA) * (1. - sinp * sinp) / (tb * tb);
  p.xsec = p.sigmaA * p.brAHa * p.brHhh * p.brachichi;
  const double lH[7] = {m.l1, m.l2, m.l3, m.l4, m.l5, m.l6, m.l7};
  z2Basis(lH, tb, p.lamZ2);
  p.maxLam = std::max({std::fabs(lam3), std::fabs(laP1), std::fabs(laP2)});
  for (int i = 0; i < 5; ++i) p.maxLam = std::max(p.maxLam, std::fabs(p.lamZ2[i]));
  p.unit = unitarityMax(p.lamZ2);
  p.bounded = boundedMargin(lH, p.lamZ2);
  p.drho = deltaRho(sinp, sinbma, gCfg.mA, gCfg.mH, p.mHc, gCfg.ma, m.vev, m.MW, m.MZ);
  return p;
}

// relative violations of the constraints (0 when satisfied)
std::vector<double> violations(const Point& p) {
  auto over = [](double x, double lim) { return x > lim ? (x - lim) / lim : 0.; };
  return {over(p.gH / gCfg.mH, gCfg.fracWH), over(p.gA / gCfg.mA, gCfg.fracWA), over(p.maxLam, gCfg.lamMax),
          over(p.unit, gCfg.unitMax), p.bounded < 0 ? -p.bounded : 0., over(std::fabs(p.drho), gCfg.drhoMax),
          over(gCfg.mHcMin, p.mHc)};
}
static const char* kConstraintNames[] = {"Gamma_H/mH", "Gamma_A/mA", "perturbativity max|lambda|", "unitarity",
                                         "bounded from below", "|Delta rho|", "mH+- >= mHcMin"};

double penalties(const Point& p) {
  double s = 0;
  for (double v : violations(p)) s += v * v;
  return gCfg.penalty * s;
}

bool allowed(const Point& p) {
  for (double v : violations(p)) if (v > gCfg.tolerance) return false;
  return true;
}

// minimise -log(sigma x BR) + penalties (the log makes the objective independent of the size of sigma x BR)
double functionForMinimize(const double* par) {
  const Point p = evaluate(par);
  return (p.xsec > 0 ? -std::log(p.xsec) : 1e3) + penalties(p);
}

// ---------------------------------------------------------------------------------------------------------------
ROOT::Math::Minimizer* makeMinimizer() {
#ifdef MINUIT2_STANDALONE
  return new ROOT::Minuit2::Minuit2Minimizer(ROOT::Minuit2::kMigrad);
#else
  return ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
#endif
}

void minimize_7d() {
  double lo[NPAR], hi[NPAR];
  for (int i = 0; i < NPAR; ++i) { lo[i] = gCfg.lo[i]; hi[i] = gCfg.hi[i]; }
  hi[6] = std::min(hi[6], gCfg.mA - gCfg.mHcMin);  // mH+- >= mHcMin
  if (hi[6] <= lo[6]) { std::cerr << "empty range for mA - mH+-" << std::endl; return; }

  // 1) random sampling of the 7D box (the objective has several local minima)
  std::mt19937 rng(12345);
  std::uniform_real_distribution<double> u(0., 1.);
  std::vector<std::pair<double, std::vector<double>>> samples;
  samples.reserve(gCfg.nSample);
  for (int s = 0; s < gCfg.nSample; ++s) {
    std::vector<double> x(NPAR);
    for (int i = 0; i < NPAR; ++i) x[i] = lo[i] + (hi[i] - lo[i]) * u(rng);
    samples.emplace_back(functionForMinimize(x.data()), x);
  }
  std::sort(samples.begin(), samples.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

  // 2) Migrad from the best samples
  double bestF = 1e300, bestErr[NPAR] = {0};
  std::vector<double> best = samples[0].second;
  ROOT::Math::Functor f(&functionForMinimize, NPAR);
  for (int s = 0; s < gCfg.nStart && s < (int)samples.size(); ++s) {
    ROOT::Math::Minimizer* minimizer = makeMinimizer();
    if (!minimizer) { std::cerr << "Error: Could not create minimizer!" << std::endl; return; }
    minimizer->SetMaxFunctionCalls(1000000);
    minimizer->SetTolerance(0.001);
    minimizer->SetPrintLevel(gCfg.print);
    minimizer->SetFunction(f);
    for (int v = 0; v < NPAR; ++v)
      minimizer->SetLimitedVariable(v, kNames[v], samples[s].second[v], 0.02 * (hi[v] - lo[v]), lo[v], hi[v]);
    minimizer->Minimize();
    if (minimizer->MinValue() < bestF) {
      bestF = minimizer->MinValue();
      best.assign(minimizer->X(), minimizer->X() + NPAR);
      for (int v = 0; v < NPAR; ++v) bestErr[v] = minimizer->Errors()[v];
    }
    delete minimizer;
  }

  // 3) results
  const Point p = evaluate(best.data());
  const bool ok = allowed(p);
  const auto viol = violations(p);
  std::cout << "\n=== Minimization Results (mA = " << gCfg.mA << ", mH = " << gCfg.mH << ", ma = " << gCfg.ma << ") ==="
            << std::endl;
  std::cout << "Effective cross section [pb]: " << p.xsec << std::endl;
  if (!ok) {
    std::cout << "NO ALLOWED POINT FOUND; least-violating point shown. Relative violations:";
    for (size_t i = 0; i < viol.size(); ++i) if (viol[i] > 0) std::cout << " " << kConstraintNames[i] << " " << viol[i];
    std::cout << std::endl;
  }
  std::cout << "sigma(gg->A) [pb] = " << p.sigmaA << ", BR(A->Ha) = " << p.brAHa << ", BR(H->hh) = " << p.brHhh
            << ", BR(a->chichi) = " << p.brachichi << std::endl;
  std::cout << "Gamma_A/mA = " << p.gA / gCfg.mA << ", Gamma_H/mH = " << p.gH / gCfg.mH << ", max|lambda_i| = " << p.maxLam
            << " (limit " << gCfg.lamMax << "), unitarity max|e| / 8pi = " << p.unit / (8 * M_PI)
            << ", bounded-from-below margin = " << p.bounded << ", Delta rho = " << p.drho << ", mH+- = " << p.mHc << std::endl;
  std::cout << "Z2-basis quartics lambda_1..5 = " << p.lamZ2[0] << " " << p.lamZ2[1] << " " << p.lamZ2[2] << " "
            << p.lamZ2[3] << " " << p.lamZ2[4] << std::endl;
  for (int v = 0; v < NPAR; ++v) {
    const bool atLimit = std::fabs(best[v] - lo[v]) < 1e-3 * (hi[v] - lo[v]) || std::fabs(best[v] - hi[v]) < 1e-3 * (hi[v] - lo[v]);
    std::cout << kNames[v] << " = " << best[v] << " +/- " << bestErr[v] << (atLimit ? "   (at the limit of its range)" : "")
              << std::endl;
  }
  // one line for the scan script
  printf("RESULT %g %g %g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %d %.6g %.6g\n",
         gCfg.mA, gCfg.mH, gCfg.ma, p.mHc, best[0], best[1], best[2], best[3], best[4], best[5], p.xsec, p.sigmaA, p.brAHa,
         p.brHhh, p.brachichi, p.gA / gCfg.mA, p.gH / gCfg.mH, p.drho, p.maxLam, ok ? 1 : 0, p.unit / (8 * M_PI), p.bounded);
}

void usage() {
  std::cout << "Usage: ./minimize_nonAligned mA mH [options]\n"
               "  ranges of the free parameters (lo hi):\n"
               "    --sinp 0.01 0.7071  --sinbma 0.988686 1  --tb 2 20  --lam3 1 10  --laP1 0 10  --laP2 0 10\n"
               "    --cbaMax 0.15        (same as --sinbma sqrt(1-0.15^2) 1)\n"
               "    --dM -150 150        (dM = mA - mH+-; fix mH+- = mA with --dM 0 0)\n"
               "  constraints: --fracWH 0.05  --fracWA 0.10  --drhoMax 1e-3  --mHcMin 150  --lamMax 12.566  --unitMax 25.133\n"
               "  other: --ma 140  --mchi 10  --sigmaNorm 0.896  --offshell 1  --nGL 100  --nSample 300000  --nStart 30  --print 0\n";
}

int main(int argc, char* argv[]) {
  if (argc < 3) { usage(); return 1; }
  gCfg.mA = std::atof(argv[1]);
  gCfg.mH = std::atof(argv[2]);
  const std::vector<std::string> rangeOpt = {"--sinp", "--sinbma", "--tb", "--lam3", "--laP1", "--laP2", "--dM"};
  for (int i = 3; i < argc; ++i) {
    const std::string a = argv[i];
    auto next = [&]() { if (i + 1 >= argc) { std::cerr << "missing value after " << a << std::endl; std::exit(1); } return std::atof(argv[++i]); };
    const auto r = std::find(rangeOpt.begin(), rangeOpt.end(), a);
    if (r != rangeOpt.end()) { const int k = r - rangeOpt.begin(); gCfg.lo[k] = next(); gCfg.hi[k] = next(); }
    else if (a == "--fracWH") gCfg.fracWH = next();
    else if (a == "--fracWA") gCfg.fracWA = next();
    else if (a == "--drhoMax") gCfg.drhoMax = next();
    else if (a == "--mHcMin") gCfg.mHcMin = next();
    else if (a == "--ma") gCfg.ma = next();
    else if (a == "--mchi") gCfg.mchi = next();
    else if (a == "--cbaMax") { gCfg.lo[1] = std::sqrt(1 - sq_(next())); gCfg.hi[1] = 1.; }
    else if (a == "--lamMax") gCfg.lamMax = next();
    else if (a == "--unitMax") gCfg.unitMax = next();
    else if (a == "--sigmaNorm") gCfg.sigmaNorm = next();
    else if (a == "--offshell") gCfg.offshell = (int)next();
    else if (a == "--nGL") gCfg.nGL = (int)next();
    else if (a == "--nSample") gCfg.nSample = (int)next();
    else if (a == "--nStart") gCfg.nStart = (int)next();
    else if (a == "--print") gCfg.print = (int)next();
    else { std::cerr << "unknown option " << a << std::endl; usage(); return 1; }
  }
  // a fixed parameter (lo == hi): give Minuit a tiny range
  for (int k = 0; k < NPAR; ++k)
    if (gCfg.hi[k] - gCfg.lo[k] < 1e-9) gCfg.hi[k] = gCfg.lo[k] + 1e-6 * std::max(1., std::fabs(gCfg.lo[k]));
  if (gCfg.mA < gCfg.mH + gCfg.ma) {
    std::cerr << "A -> H a is closed for mA = " << gCfg.mA << " < mH + ma = " << gCfg.mH + gCfg.ma << std::endl;
    printf("RESULT %g %g %g 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n", gCfg.mA, gCfg.mH, gCfg.ma);
    return 0;
  }
  initGaussLegendre(gCfg.nGL);
  initZ2();
  minimize_7d();
  return 0;
}
