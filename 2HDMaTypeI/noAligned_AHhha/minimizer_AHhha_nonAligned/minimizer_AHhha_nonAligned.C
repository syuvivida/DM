// Find the 2HDM+a (Type I, non-aligned) parameters that maximise
//     sigma(gg -> A) x BR(A -> H a) x BR(H -> h h) x BR(a -> chi chi)
// for a given (mA, mH), in the spirit of HAAahah/scripts/minimizer_HAAahah.C.
//
// Free parameters (7):  sin(theta) (sinp), sin(beta-alpha) (sinbma), tan(beta), lambda_3, lambda_P1, lambda_P2,
//                       and the mass splitting dM = mA - mH+-.
// Fixed: ma = 100 GeV, mchi = 10 GeV, g_chi = 1, mh = 125 GeV.
//
// Widths: tree-level 2-body partial widths of the UFO model Pseudoscalar_2HDMI (the model of the MadGraph samples),
// translated to C++ in Pseudoscalar2HDMI_widths.h by make_cpp_widths.py (validated against the MadGraph 4D scans).
// Production: sigma(gg -> A) = sigma0(mA) * cos^2(theta) / tan^2(beta), sigma0 from the MadGraph 13.6 TeV 2D scan.
//
// Constraints (one-sided penalties, zero inside the allowed region):
//   Gamma_H / mH < fracWH (0.05), Gamma_A / mA < fracWA (0.10),
//   |lambda_1..7|, |lambda_P1|, |lambda_P2| < 4 pi (perturbativity),
//   |Delta rho| < drhoMax (1e-3, i.e. |T| < ~0.13; PDG: T = 0.03 +- 0.12),
//   lambda_P1, lambda_P2 >= 0 by their range (no P^4 term in the model: needed for a potential bounded from below),
//   mH+- >= mHcMin (150 GeV).
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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Pseudoscalar2HDMI_widths.h"

// ---------------------------------------------------------------------------------------------------------------
// configuration (all can be changed on the command line)
// ---------------------------------------------------------------------------------------------------------------
static const int NPAR = 7;
static const char* kNames[NPAR] = {"sinp", "sinbma", "tanb", "lam3", "laP1", "laP2", "dM_A_Hc"};

struct Config {
  double mA = 0, mH = 0, ma = 100., mchi = 10., gchi = 1.;
  // ranges of the free parameters
  double lo[NPAR] = {0.01, 0.98, 1., 1., 0., 0., -150.};
  double hi[NPAR] = {std::sqrt(2.) / 2., 1., 10., 10., 10., 10., 150.};
  double fracWH = 0.05, fracWA = 0.10, drhoMax = 1e-3, mHcMin = 150.;
  double penalty = 1e5;     // weight of the constraint penalties
  double tolerance = 1e-3;  // relative overshoot of a limit still counted as allowed
  int nSample = 60000;      // random starting-point search
  int nStart = 8;           // Migrad runs from the best samples
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
// model evaluation
// ---------------------------------------------------------------------------------------------------------------
struct Point {
  double par[NPAR];
  double mHc, xsec, sigmaA, brAHa, brHhh, brachichi, gA, gH, ga, maxLam, drho;
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
  const auto wA = m.widths_A(), wH = m.widths_H(), wa = m.widths_a();
  auto get = [](const std::map<std::string, double>& w, const char* k) { auto it = w.find(k); return it == w.end() ? 0. : it->second; };
  p.gA = p2hdm::total(wA); p.gH = p2hdm::total(wH); p.ga = p2hdm::total(wa);
  p.brAHa = p.gA > 0 ? get(wA, "h2 h4") / p.gA : 0;
  p.brHhh = p.gH > 0 ? get(wH, "h1 h1") / p.gH : 0;
  p.brachichi = p.ga > 0 ? get(wa, "Xd Xd~") / p.ga : 0;
  p.sigmaA = sigma0_ggA(gCfg.mA) * (1. - sinp * sinp) / (tb * tb);
  p.xsec = p.sigmaA * p.brAHa * p.brHhh * p.brachichi;
  for (double l : {m.l1, m.l2, m.l3, m.l4, m.l5, m.l6, m.l7, laP1, laP2}) p.maxLam = std::max(p.maxLam, std::fabs(l));
  p.drho = deltaRho(sinp, sinbma, gCfg.mA, gCfg.mH, p.mHc, gCfg.ma, m.vev, m.MW, m.MZ);
  return p;
}

// relative violations of the constraints (0 when satisfied)
std::vector<double> violations(const Point& p) {
  auto over = [](double x, double lim) { return x > lim ? (x - lim) / lim : 0.; };
  return {over(p.gH / gCfg.mH, gCfg.fracWH), over(p.gA / gCfg.mA, gCfg.fracWA), over(p.maxLam, 4 * M_PI),
          over(std::fabs(p.drho), gCfg.drhoMax), over(gCfg.mHcMin, p.mHc)};
}
static const char* kConstraintNames[] = {"Gamma_H/mH", "Gamma_A/mA", "max|lambda|", "|Delta rho|", "mH+- >= mHcMin"};

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
            << ", Delta rho = " << p.drho << ", mH+- = " << p.mHc << std::endl;
  for (int v = 0; v < NPAR; ++v) {
    const bool atLimit = std::fabs(best[v] - lo[v]) < 1e-3 * (hi[v] - lo[v]) || std::fabs(best[v] - hi[v]) < 1e-3 * (hi[v] - lo[v]);
    std::cout << kNames[v] << " = " << best[v] << " +/- " << bestErr[v] << (atLimit ? "   (at the limit of its range)" : "")
              << std::endl;
  }
  // one line for the scan script
  printf("RESULT %g %g %g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %.6g %d\n", gCfg.mA,
         gCfg.mH, gCfg.ma, p.mHc, best[0], best[1], best[2], best[3], best[4], best[5], p.xsec, p.sigmaA, p.brAHa, p.brHhh,
         p.brachichi, p.gA / gCfg.mA, p.gH / gCfg.mH, p.drho, p.maxLam, ok ? 1 : 0);
}

void usage() {
  std::cout << "Usage: ./minimize_nonAligned mA mH [options]\n"
               "  ranges of the free parameters (lo hi):\n"
               "    --sinp 0.01 0.7071  --sinbma 0.98 1  --tb 1 10  --lam3 1 10  --laP1 0 10  --laP2 0 10\n"
               "    --dM -150 150        (dM = mA - mH+-; fix mH+- = mA with --dM 0 0)\n"
               "  constraints: --fracWH 0.05  --fracWA 0.10  --drhoMax 1e-3  --mHcMin 150\n"
               "  other: --ma 100  --mchi 10  --nSample 60000  --nStart 8  --print 0\n";
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
    printf("RESULT %g %g %g 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n", gCfg.mA, gCfg.mH, gCfg.ma);
    return 0;
  }
  minimize_7d();
  return 0;
}
