# Maximal σ×BR for gg → A → H a → (hh)(χχ̄): 2HDM+a Type I, non-aligned

This is the non-aligned counterpart of `HAAahah/scripts/minimizer_HAAahah.C`. For each (m_A, m_H) it finds the
values of **seven parameters** that maximise the cross section below: sinθ, sin(β−α), tanβ, λ₃, λ_P1, λ_P2, and
m_A − m_H±.

  σ(gg→A) × BR(A→Ha) × BR(H→hh) × BR(a→χχ̄)

under width, perturbativity, tree-level unitarity, stability (bounded-from-below) and electroweak-precision (Δρ)
constraints.

## What changed in this version

1. **Physical quartic couplings.** λ₁…λ₅ in the Z2 basis are computed from the masses and angles (via the UFO's
   Higgs-basis couplings l₁…l₇ and a rotation by β). The checks are those of scan2hdma (`scan_opt.py`):
   - perturbativity: |λ₁…λ₅|, |λ₃|, |λ_P1|, |λ_P2| < 4π (before, only the Higgs-basis l₁…l₇, λ_P1, λ_P2 were checked);
   - tree-level unitarity: the largest |eigenvalue| of the 2HDM scattering matrix < 8π (new);
   - potential bounded from below: λ₁, λ₂ > 0, λ₃ > −√(λ₁λ₂), λ₃ + λ₄ − |λ₅| > −√(λ₁λ₂) (new; λ_P1, λ_P2 ≥ 0
     by their ranges).
2. **Ranges as in scan2hdma:** m_a = 140 GeV (was 100), 2 ≤ tanβ ≤ 20 (was 1–10), 0 ≤ cos(β−α) ≤ 0.15
   (was 0.2), i.e. sin(β−α) ≥ 0.988686.
3. **Off-shell widths.** A closed channel with an unstable daughter (a, H, Z, W) is integrated over a Breit-Wigner
   mass of that daughter, as MadGraph does for its 3-body widths (e.g. H → a a* → a χχ̄ when 2m_a > m_H). This is a
   port of `scan2hdma.py widths(offshell=True)`. `make_cpp_widths.py` now also writes every channel of H, A and a
   with explicit daughter masses.
4. **σ normalisation.** σ(gg→A) is multiplied by 0.896, the ratio MadGraph / σ₀-table over the 17 signal samples
   (0.896 ± 0.0015, independent of m_A and Γ_A). It was 12% too high.
5. **Wider search:** 300,000 random points and 30 Migrad starts per mass point (was 60,000 and 8): with unitarity
   the allowed region is narrow and the smaller search missed better points. About 8 s per mass point.

Not included (on purpose): the gg → H → hh and mono-h limits that scan2hdma also applies.

## Validation

With the parameters of the 17 MadGraph signal samples (scan2hdma cards), the minimizer's model gives:

| Quantity | Agreement with MadGraph / scan2hdma |
|---|---|
| σ×BR | within 1.1% of MadGraph (was 11–45% too high) |
| σ(gg→A) | within 0.5% of MadGraph |
| BR(H→hh) at m_H = 255–260 GeV | within 1% of MadGraph (was 13–28% too high without the off-shell widths) |
| Γ_A/m_A | identical to MadGraph |
| unitarity eigenvalue / 8π | identical to scan2hdma at all 37 points |

## Files

| File | Purpose |
|---|---|
| `minimizer_AHhha_nonAligned.C` | The minimizer (ROOT Minuit2): `./minimize_nonAligned mA mH [options]`. |
| `Pseudoscalar2HDMI_widths.h` | Partial widths of H, A and a, **generated from the UFO model** of your MadGraph samples, including versions with explicit daughter masses for the off-shell integrals. Do not edit by hand. |
| `make_cpp_widths.py` | Regenerates the header from `models/Pseudoscalar_2HDMI` (`parameters.py`, `decays.py`). |
| `ufo_widths.py`, `validate_vs_madgraph.py` | 2-body widths in Python and the comparison with your MadGraph 4D scans (unchanged). |
| `getBestPar_mA_mH.sh` | Runs the (m_A, m_H) plane and writes `bestPar_nonAligned_AHhha.dat` (about 8 min). |
| `plot_bestPar_2D.py` | 2D maps of the maximal σ×BR and of the parameters that give it. `--hbb` multiplies by BR(h→bb̄)². |

## Running

`make_cpp_widths.py`, `ufo_widths.py` and `validate_vs_madgraph.py` need the UFO next to them:
`ln -s /Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/models/Pseudoscalar_2HDMI .`
The minimizer itself only needs the generated header.

```bash
g++ -O2 -std=c++17 minimizer_AHhha_nonAligned.C -o minimize_nonAligned `root-config --cflags --libs`
./minimize_nonAligned 500 260                    # one point, about 8 s
bash getBestPar_mA_mH.sh                          # whole plane
python3 plot_bestPar_2D.py bestPar_nonAligned_AHhha.dat --hbb
```

Options (`./minimize_nonAligned` with no arguments prints them):

| Option | Default | Meaning |
|---|---|---|
| `--sinp lo hi` | 0.01 0.7071 | Range of sinθ (scan2hdma: 0.02 0.95) |
| `--sinbma lo hi` | 0.988686 1 | Range of sin(β−α), i.e. 0 ≤ cos(β−α) ≤ 0.15 |
| `--cbaMax x` | 0.15 | Same as `--sinbma sqrt(1−x²) 1` |
| `--tb lo hi` | 2 20 | Range of tanβ |
| `--lam3 lo hi` | 1 10 | Range of λ₃ (scan2hdma: −3 4π) |
| `--laP1 lo hi`, `--laP2 lo hi` | 0 10 | Ranges of λ_P1, λ_P2; must be ≥ 0 (no P⁴ term in the model) (scan2hdma: 0 4π) |
| `--dM lo hi` | −150 150 | Range of m_A − m_H± (scan2hdma: −100 400) |
| `--fracWH` | 0.05 | Upper limit on Γ_H/m_H |
| `--fracWA` | 0.10 | Upper limit on Γ_A/m_A (scan2hdma: 0.2) |
| `--drhoMax` | 1e-3 | Limit on \|Δρ\| (scan2hdma: \|ΔT\| < 0.2, about \|Δρ\| < 1.56e-3) |
| `--mHcMin` | 150 | Lower limit on m_H± |
| `--lamMax` | 4π | Perturbativity limit |
| `--unitMax` | 8π | Unitarity limit on the largest eigenvalue |
| `--ma`, `--mchi` | 140, 10 | Masses of a and χ |
| `--sigmaNorm` | 0.896 | Normalisation of σ(gg→A) to MadGraph |
| `--offshell` | 1 | 0 = 2-body widths only (the old behaviour) |
| `--nGL` | 100 | Gauss-Legendre nodes of the off-shell integrals (400 gives the same result to < 10⁻⁴) |
| `--nSample`, `--nStart` | 300000, 30 | Random starting-point search and the number of Migrad runs from its best points |

To use **all** of scan2hdma's ranges and limits (except gg→H→hh and mono-h):
```bash
./minimize_nonAligned mA mH --fracWA 0.2 --sinp 0.02 0.95 --dM -100 400 --lam3 -3 12.566 \
    --laP1 0 12.566 --laP2 0 12.566 --drhoMax 0.00156
```

The RESULT line (and the table) has two new last columns: `unit_over_8pi` (largest unitarity eigenvalue / 8π)
and `bounded_margin` (> 0: potential bounded from below). `maxLambda` is now the largest of |λ₁…λ₅| (Z2 basis),
|λ₃|, |λ_P1|, |λ_P2|.

## Physics inputs

- **Widths:** the UFO model `Pseudoscalar_2HDMI` partial widths, the couplings MadGraph uses, including all
  cos(β−α) ≠ 0 terms, plus the off-shell 3-body widths described above. The parents are computed in the order a, H,
  A, so that the widths of a and H are known when they appear as daughters; h and H± are treated as stable
  daughters, Z and W have fixed widths (as in scan2hdma).
- **Production:** σ(gg→A) = 0.896 · σ₀(m_A) · cos²θ / tan²β, σ₀ from the MadGraph 13.6 TeV 2D scan, interpolated
  log-linearly for 410–800 GeV.
- **Δρ:** the aligned 2HDM+a formula of `HAAahah/scripts/rho.C`, extended outside alignment (unchanged).
- **Constraints** are one-sided penalties; the objective is −log(σ×BR). A point is "allowed" if it violates no limit
  by more than 0.1% (absolute 10⁻³ for the bounded-from-below margin). If nothing is allowed, the least-violating
  point is printed with `allowed = 0`.

## What the scan shows (default settings: 49 of 54 points allowed)

- **The largest σ×BR** is near threshold: 445 fb at (410, 255) and 405 fb at (410, 260); with BR(h→bb̄)² 151 and
  137 fb. At m_H = 260 it falls to 126 fb at m_A = 510, 27 fb at 610 and 1.7 fb at 700. With the old version (m_a =
  100, no unitarity) the value at (410, 260) was 1300 fb.
- **No allowed point** at (750, 255), (750, 260), (800, 255), (800, 260), (800, 300): Γ_A/m_A > 10%.
  With `--fracWA 0.2` they become allowed (e.g. 5.5 fb at (750, 260)).
- **Where the optimum sits** (out of 49 allowed points): tanβ = 2 (lower edge) at 33, cos(β−α) = 0.15 at 44,
  unitarity at its limit at 24, potential at the edge of boundedness at 22, |Δρ| = 10⁻³ at 17, Γ_A/m_A = 10% at 10.
  The new 4π perturbativity limit is never the binding one: unitarity is stronger.
- **Typical best values** (median, 10–90%): sinθ 0.54 (0.48–0.65), tanβ 2.0 (2.0–2.4), cos(β−α) 0.15, λ₃ 5.4
  (3.6–9.2), λ_P1 0 (0–6.7), λ_P2 4.6 (1.0–6.1), m_A − m_H± 114 GeV (81–144).

## Comparison with scan2hdma (36 of its 37 mass points, m_a = 140)

- **With the defaults above:** the minimizer finds 0.7–1.3× scan2hdma's σ×BR for m_A ≤ 600 GeV. At larger m_A and
  light H it finds less or nothing (none at (750, 260), (800, 260), (800, 310)), because of its tighter ranges:
  Γ_A/m_A < 10% (scan2hdma 20%), sinθ ≤ 0.71 (0.95), m_A − m_H± ≤ 150 GeV (400).
- **With all of scan2hdma's ranges** (command above): every point is allowed and the minimizer finds 0.9–1.8×
  scan2hdma's σ×BR (median 1.09), plus one outlier at (800, 310), 0.83 vs 0.12 fb. Somewhat larger values are
  expected, because the gg→H→hh and mono-h limits are not applied here; at m_H ≥ 310 GeV the gg→H→hh limit is often
  what limits scan2hdma.
