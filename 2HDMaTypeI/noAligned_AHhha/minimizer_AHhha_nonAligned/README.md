# Maximal σ×BR for gg → A → H a → (hh)(χχ̄): 2HDM+a Type I, non-aligned

This is the non-aligned counterpart of `HAAahah/scripts/minimizer_HAAahah.C`. For each (m_A, m_H) it finds the
values of **seven parameters** that maximise the cross section below: sinθ, sin(β−α), tanβ, λ₃, λ_P1, λ_P2, and
m_A − m_H±.

  σ(gg→A) × BR(A→Ha) × BR(H→hh) × BR(a→χχ̄)

under width, perturbativity, electroweak-precision (Δρ) and stability constraints.

## Files

| File | Purpose |
|---|---|
| `minimizer_AHhha_nonAligned.C` | The minimizer (ROOT Minuit2). Usage is the same as the original: `./minimize_nonAligned mA mH`. |
| `Pseudoscalar2HDMI_widths.h` | Tree-level 2-body partial widths of H, A and a, **generated from the UFO model** used for your MadGraph samples. Do not edit by hand. |
| `make_cpp_widths.py` | Regenerates the header from `models/Pseudoscalar_2HDMI` (`parameters.py`, `decays.py`). |
| `ufo_widths.py`, `validate_vs_madgraph.py` | The same widths evaluated directly in Python, and the comparison with your MadGraph 4D scans. |
| `getBestPar_mA_mH.sh` | Runs the scan over the (m_A, m_H) plane and writes `bestPar_nonAligned_AHhha.dat`. |
| `plot_bestPar_2D.py` | Draws 2D maps of the maximal σ×BR and of the parameters that give it. Add `--hbb` to multiply by BR(h→bb̄)². |

## Running

`make_cpp_widths.py`, `ufo_widths.py` and `validate_vs_madgraph.py` need the UFO next to them:
`ln -s /Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/models/Pseudoscalar_2HDMI .`
The minimizer itself only needs the generated header.

```bash
g++ -O2 minimizer_AHhha_nonAligned.C -o minimize_nonAligned `root-config --cflags --libs`
./minimize_nonAligned 500 260                    # one point
bash getBestPar_mA_mH.sh                          # whole plane, about 3 s
python3 plot_bestPar_2D.py bestPar_nonAligned_AHhha.dat --hbb
```

Options (`./minimize_nonAligned` with no arguments prints them):

| Option | Default | Meaning |
|---|---|---|
| `--sinp lo hi` | 0.01 0.7071 | Range of sinθ |
| `--sinbma lo hi` | 0.98 1 | Range of sin(β−α), i.e. 0 ≤ cos(β−α) ≤ 0.2. The UFO uses cos(β−α) ≥ 0. |
| `--tb lo hi` | 1 10 | Range of tanβ |
| `--lam3 lo hi` | 1 10 | Range of λ₃ |
| `--laP1 lo hi` | 0 10 | Range of λ_P1. Must be ≥ 0: the model has no P⁴ term, so negative values leave the potential unbounded from below. |
| `--laP2 lo hi` | 0 10 | Range of λ_P2 (same reason) |
| `--dM lo hi` | −150 150 | Range of m_A − m_H±. Use `--dM 0 0` to fix m_H± = m_A. |
| `--fracWH` | 0.05 | Upper limit on Γ_H/m_H |
| `--fracWA` | 0.10 | Upper limit on Γ_A/m_A |
| `--drhoMax` | 1e-3 | Limit on \|Δρ\|, about \|T\| < 0.13. Use `--drhoMax 1` to switch it off. |
| `--mHcMin` | 150 | Lower limit on m_H± |
| `--ma`, `--mchi` | 100, 10 | Masses of a and χ |
| `--nSample`, `--nStart` | 60000, 8 | Random starting-point search, and the number of Migrad runs started from its best points |

A parameter is fixed by giving equal limits. For example, `--laP1 0 0 --laP2 0 0 --dM 0 0 --drhoMax 1` reproduces
the earlier 4-parameter setup.

## Physics inputs

- **Widths:** the analytic tree-level 2-body widths of the UFO model `Pseudoscalar_2HDMI`. These are the same
  couplings MadGraph uses, including all cos(β−α) ≠ 0 terms (λ₆, λ₇, the H–h mixing, and H→hh, WW, ZZ).
  - Compared with your 4D scans (4 × 525 MadGraph points), the median agreement of BR(A→Ha) and BR(H→hh)
    is 10⁻⁶–10⁻⁴, and 95% of points agree to better than 2%.
  - When m_H± = m_A, MadGraph's AUTO width also includes off-shell three-body decays, most likely A → W H±*.
    At tanβ ≲ 3 this can make Γ_A up to 2× larger than the 2-body sum, and BR(A→Ha) correspondingly smaller,
    in a minority of points.
- **Production:** σ(gg→A) = σ₀(m_A)·cos²θ/tan²β.
  - Your 4D scans follow this scaling to 0.5%.
  - σ₀(m_A) at 13.6 TeV is taken from the 2D scan (tanβ = 2, sinθ = 0.1). It agrees to better than 1% across
    cos(β−α) and m_H.
  - It is interpolated log-linearly for 410–800 GeV and extrapolated (with a warning) outside that range.
  - With m_H± = m_H instead of m_A, MadGraph's σ is about 10% lower at m_A = 410, through the larger A width.
- **Δρ:** the aligned 2HDM+a formula of `HAAahah/scripts/rho.C`, reproduced to 10⁻¹⁴ at cos(β−α) = 0, extended
  outside alignment. The extension adds the sin²(β−α)/cos²(β−α)-weighted H and h terms and the gauge-boson
  terms of the 2HDM, with the CP-odd doublet state shared between A (cos²θ) and a (sin²θ).
- **Constraints:** Γ_H/m_H < 5% (as in the original minimizer), Γ_A/m_A < 10%, |λ₁…λ₇|, |λ_P1|, |λ_P2| < 4π,
  |Δρ| < 10⁻³, and m_H± ≥ 150 GeV. A full stability (bounded-from-below) check of the 2HDM part is not included.
  - They are applied as one-sided penalties, and the objective is −log(σ×BR).
  - A point is "allowed" if it violates no limit by more than 0.1%.
  - If nothing is allowed, the least-violating point is printed with `allowed = 0`.
- **Fixed:** m_h = 125, m_χ = 10, g_χ = 1. The ranges of sinθ, λ₃ and tanβ are as in `minimizer_HAAahah.C`.

## Differences from `minimizer_HAAahah.C`

1. **Seven free parameters** instead of three, and a Δρ constraint because m_H± is now free.
2. **Exact widths:** they come from the UFO and include all non-aligned terms, instead of being hand-coded.
   The hand-coded formulas only work in the alignment limit.
3. **One-sided penalties:** the original adds `penaltyFactor*(Γ_H/m_H − 0.05)` even when the constraint is
   satisfied. That term is of order 0.05, larger than σ×BR itself (about 10⁻³), so the original mostly
   minimises the width. Here the penalty is zero inside the allowed region, and the objective is −log(σ×BR),
   so its scale does not matter.
4. **Several starting points:** the objective has several local minima. 60,000 random points are evaluated
   first, and Migrad is started from the best 8 (about 0.4 s per mass point). The result beat an independent
   100,000-point random search (Python/UFO) at (500, 260): 0.323 vs 0.262 pb. At (650, 360) it found 0.033 vs
   0.016 pb.
5. **The errors are not meaningful:** the "±" values printed by Minuit come from HESSE at a constrained or
   boundary optimum. The likelihood is very flat, so σ×BR changes little over a wide range of parameters.
   Use `--print 1` and the scan maps to judge this, not the ± values.

## What the scan shows (default settings: 55 of 60 points allowed, about 25 s)

- **The largest σ×BR×BR(h→bb̄)²** is near threshold: about 440 fb at (410, 260) and 360 fb at (410, 300). At
  m_H = 260 it falls to about 93 fb at m_A = 510, 17 fb at 610 and 1.7 fb at 700.
- **No allowed point** at (750, 255), (750, 260), (800, 255), (800, 260) and (800, 300), because A → ZH makes
  Γ_A/m_A > 10%. With `--fracWA 0.2`, all 60 points are allowed, and σ×BR at m_A ≳ 600 GeV rises by 1.5–8×.
- **The constraints that limit the result** (how often each is at its limit, out of 55 allowed points):
  - perturbativity, max|λ| = 4π: 38
  - Γ_A/m_A = 10%: 28
  - Γ_H/m_H = 5%: 19
  - |Δρ| = 10⁻³: 13
- **Typical best values:**

  | Parameter | Median (10–90%) |
  |---|---|
  | sinθ | 0.48 (0.30–0.60) |
  | cos(β−α) | 0.15 (0.10–0.20) |
  | tanβ | 2.2 (1.9–2.5) |
  | m_A − m_H± | 108 GeV (34–148) |
  | BR(A→Ha) | 0.43 |
- **m_H± = m_A, as in the earlier scans, violates electroweak precision far from threshold.** For example, the
  4-parameter optimum at (700, 260) has Δρ = 0.013.
