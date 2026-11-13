# MadGraph signal samples with the minimizer's parameters

Uses the existing process directory `workdir_AHhhaa_4b` (g g > h3 [noborn=QCD]; MadSpin: h3 > h2 h4, h2 > h1 h1,
h4 > xd~ xd; Pythia8 with h1 -> b b~ forced; Delphes), exactly as the scan2hdma samples.

```bash
cd /Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/minimizer_mc
# 1) cards (all allowed points of the table, or a selection)
python3 make_cards_from_minimizer.py bestPar_nonAligned_AHhha.dat ../workdir_AHhhaa_4b/Cards/param_card.dat \
    --nevents 10000                                   # all 49 allowed points
#   e.g. only some points:   --points "410:260,510:260,610:260,660:300"
#   or the 10 largest:       --top 10        or above a value:  --min-sigbr 5   (fb)
# 2) generate (sequential; restart it any time: finished points are skipped)
bash cards_minimizer/run_all.sh
# 3) compare MadGraph with the minimizer and get the cross sections for normalising the samples
python3 check_mg_vs_minimizer.py cards_minimizer/summary.csv
```

- **Run names** get the suffix `_min` (e.g. `mA510_mH260_min`, MadSpin output `Events/mA510_mH260_min_decayed_1`),
  so they never overwrite the scan2hdma samples in the same process directory. Change it with `--suffix`.
- **Cards:** tanbeta, sinbma, sinp, lam3, lap1, lap2, mh2 = m_H, mh3 = m_A, mh4 = m_a (140), mhc = m_H± from the
  table; mxd = 10, gpxd = 1 (as in the minimizer); the widths of h, H, A, H± and a are set to Auto, so MadGraph
  recomputes them (including the 3-body widths) for each point. Points with `allowed = 0` are skipped unless
  `--include-disallowed` is given.
- **`run_all.sh`** saves the process directory's param_card and run_card first and restores them at the end
  (also after an error or Ctrl-C).
- **Normalisation:** `check_mg_vs_minimizer.py` writes `cards_minimizer/mg_xsec.csv` with MadGraph's
  σ(gg→A) × BR(A→Ha) × BR(H→hh) × BR(a→χχ̄) for each point. Because Pythia8 forces h → bb̄, the 4b + MET sample
  corresponds to that times BR(h→bb̄)²: use the SM value 0.5824² (column `sigBR_4b_SMhbb_fb`); the UFO's tree-level
  BR(h→bb̄) ≈ 0.84 is printed only for reference.
  The minimizer reproduces MadGraph to about 1%; a difference above 5% is flagged.
- **Time:** about as long as each scan2hdma point took (one MadGraph + MadSpin + Pythia8 + Delphes run per point).
  Running all 49 points takes correspondingly long, so a selection with `--points` or `--top` is a good start.
