# MET-biased tt̄ backgrounds for the 4b + MET search (MadGraph5_aMC@NLO 3.5.7 + Pythia8)

| File | What it is |
|---|---|
| `proc_tt_semilep.dat` | tt̄ → (bℓν)(bqq′) + 0, 1, 2 jets, ℓ = e, μ, τ, MLM matching |
| `proc_tt_dilep.dat` | tt̄ → (bℓν)(bℓν) + 0, 1, 2 jets, MLM matching |
| `proc_ttbb_lep.dat` | tt̄bb̄ (4-flavour, massive b) with ≥ 1 leptonic W: usually the leading top background with 3–4 b-tags |
| `pythia8_card_MLM.dat` | CP5 tune + MLM settings (qCut = 70 GeV, nJetMax = 2) |
| `set_run_card.py` | sets run_card values without the interactive card editor |
| `make_ttbar_samples.sh` | builds the process folder, sets the cards and generates the MET slices |

## Why it is set up this way

* **Decays are inside the hard process** (`t > b w+, w+ > l+ vl`), not in MadSpin. The parton-level MET cut
  (`misset`, the vector sum of the neutrino pT) can only act on neutrinos that exist when the events are
  generated, and MadSpin decays the tops afterwards. `cut_decays = True` makes the cuts act on the decay products.
* **No lepton cuts** (`ptl = 0`, `etal = -1`, `drll = drjl = mmll = 0`). Our search vetoes leptons, so the
  dangerous tt̄ events are those where the lepton is lost (soft, forward, non-isolated) or is a hadronic τ.
  Default lepton cuts would remove exactly those events.
* **τ included** in ℓ: τ → hadrons + ν is a major source of tt̄ events with real MET and no e/μ.
* **MET slices** (default `100:200 200:300 300:-1` GeV in parton-level MET): each slice is generated separately, so the
  high-MET tail that survives the analysis gets enough events. The lowest edge (100 GeV) sits below the analysis
  cut (150–200 GeV), because neutrinos from τ and b-hadron decays (added in the shower) and detector resolution
  add to the MET. **Check the turn-on once:** generate a small slice `0:100` and confirm that it contributes
  ≲ 1–2% of the events passing your full selection. If it contributes more, lower the first edge.
* **tt̄ + 0, 1, 2 jets, MLM** (`xqcut = 30`, Pythia `qCut = 70`): extra hard radiation (and g → bb̄) gives the
  additional b jets the 4b selection needs. Check the matching with the differential jet rate plots
  (`Events/<run>/*djrs*`): they should be smooth around qCut.
* **tt̄bb̄ in the 4-flavour scheme** (`ptb > 20`, `drbb > 0.3` on the extra b pair): describes the hard extra b jets
  better than the shower does. **Overlap:** remove events with ≥ 3 b hadrons at generator level from the tt̄ + jets
  samples (or the extra-b category from tt̄bb̄), so the two samples are not double counted.

## Running

```bash
# in the folder with these files (MG = your MadGraph directory)
export MG=/Users/yush/2HDMa/MG5_aMC_v3_5_7
bash make_ttbar_samples.sh semilep 50000 4      # 4 runs x 50k events per MET slice
bash make_ttbar_samples.sh dilep   50000 2
bash make_ttbar_samples.sh ttbb    20000 2
SLICES="0:-1" bash make_ttbar_samples.sh semilep 5000   # small inclusive run for the normalization (below)
```

Runs are started with `./bin/generate_events <name> -f`, without the interactive card editor, so the
LHAPDF Python-module problem does not stop them. Each run gets its own seed, and the events are in
`$MG/TT_semilep_MET/Events/<sample>_MET<lo>to<hi>_r<i>/`.

## Normalization

1. **Cross section of each slice:** take it from the banner of each run. For MLM-matched samples use the
   **matched** cross section from the Pythia8 log/banner, not the MadGraph value before matching. Weight each event by
   `w = σ_slice / N_slice` (N = events after matching), and add the runs of one slice together by dividing by the total N.
2. **Scale to NNLO+NNLL:**
   `K = σ_NNLO(tt̄) × BR(channel) / σ_LO,matched(inclusive run, same channel)`
   * σ_NNLO+NNLL(tt̄, 13.6 TeV, m_t = 172.5 GeV) ≈ **924 pb** (LHC Top Working Group). Check the current value on the
     TopNNLORefXsec TWiki.
   * BR(W → ℓν, ℓ = e+μ+τ) = 3 × 0.1086 = 0.3258. **Semileptonic: 2 × 0.3258 × 0.6742 = 0.4393; dileptonic: 0.3258² = 0.1061**.
   * Multiply every slice by the same K. For tt̄bb̄, the NLO/LO K-factor is about 1.5–2 and there are large uncertainties
     (~30–40%). Quote it as a systematic.
3. **How many events:** a slice is statistically adequate if its equivalent luminosity `N_slice / (K σ_slice)` is
   at least the analysis luminosity (e.g. 138 or 300 fb⁻¹) **after** your selection, i.e. at least ~10 raw events
   survive the full 4b + MET selection in each slice that matters. Start with the numbers above, look at the cut flow,
   and add runs (`n_runs`) to the slices that dominate the background.

## Analysis-level reminders

* Reapply the real MET cut on the particle-level (or Delphes) MET. The generator slices only make the generation efficient.
* Veto isolated e/μ the same way in signal and background. τ_h events pass the veto, as they should.
* Compare the MET distributions of adjacent slices at the slice boundaries. They should join smoothly after weighting.

## Gridpack production on the CERN batch system (lxplus + HTCondor)

| File | What it is |
|---|---|
| `make_ttbar_gridpacks.sh` | Builds one gridpack per MET slice, plus the Pythia8 command file MadGraph writes for these cards |
| `run_ttbar_job.sh` | One batch job: gridpack → LHE → Pythia8 (MadGraph's `MG5aMC_PY8_interface`, MLM) → Delphes → EOS |
| `submit_ttbar.sh` | Writes the job list and the HTCondor submit file (`+JobFlavour`), and submits |

The generator settings in `make_ttbar_gridpacks.sh` are the same as in `make_ttbar_samples.sh`, so the batch events
are equivalent to the interactive ones. If you change a setting, change it in both scripts.

### Once, on lxplus

A gridpack contains compiled code: **build it on lxplus (EL9)**, not on the Mac.
```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh
export MG=/afs/cern.ch/user/s/syu/.../MG5_aMC_v3_5_7       # MadGraph on lxplus
cd $MG && ./bin/mg5_aMC  ->  install pythia8                # if HEPTools/MG5aMC_PY8_interface is missing
cd $MG/Delphes && make distclean && ./configure && make -j8  # Delphes built with the same LCG view
```
Use the same LCG view everywhere: when building, in the gridpack, and in the jobs. Otherwise you get the
`libtbb.so.12` / `libtbb.so.2` mismatch again.

### Build the gridpacks (interactively, about an hour per sample)
```bash
cd ttBar
bash make_ttbar_gridpacks.sh semilep        # -> gridpacks/semilep_MET{100to200,200to300,300toinf}_gridpack.tar.gz + _pythia8.cmd
bash make_ttbar_gridpacks.sh dilep
bash make_ttbar_gridpacks.sh ttbb
```
For each slice the script does two things:
1. It runs MadGraph with `gridpack = True` and no shower.
2. It makes a 100-event normal run with Pythia8, only to keep the `pythia8.cmd` file that MadGraph writes
   (CP5 tune, MLM qCut and nJetMax, and so on).

The jobs reuse that file, changing only the input and output file names and the random seed.

### Test one job interactively (about 200 events), then submit
```bash
export DELPHES=$MG/Delphes
bash run_ttbar_job.sh gridpacks/semilep_MET300toinf_gridpack.tar.gz gridpacks/semilep_MET300toinf_pythia8.cmd \
     delphes_card.dat 200 1 /tmp/ttbar_test TT_semilep_MET
# time it, then choose the events per job so that a job takes 2-6 h
bash submit_ttbar.sh semilep 40 10000 1000     # 40 jobs x 10k events per slice, seeds 1000-1039, flavour "workday"
condor_q
```
- **Output:** `$OUT/TT_semilep_MET/Events/semilep_MET300toinf_r<seed>/`, where `OUT` defaults to
  `root://eosuser.cern.ch//eos/user/s/syu/ttbar`. The layout is the MadGraph one, so `hhmet_sensitivity.py skim --mg <OUT>`
  reads it directly.
- **Normalisation:** each job's banner has the gridpack cross section and the matched cross section of that job:
  σ_LHE × N(after matching) / N(LHE).
- **Adding jobs later:** use a new first seed (e.g. 2000), so the run names and seeds do not repeat.
- **Job flavour:** set it with `FLAVOUR=longlunch` (2 h), `workday` (8 h, the default) or `tomorrow` (1 day).
  `espresso` (20 min) is the default if you set nothing, and is too short.
- **Delphes card:** use the slim card (`slim_delphes_card.py`) to keep the files about 10× smaller.
