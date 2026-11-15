# W(→ ℓν) + jets / + b b̄ ("lost lepton") gridpack production on lxplus + HTCondor

These scripts go in the same folder as the existing `wlnu_cards` files (process cards, Pythia8 cards,
`set_run_card.py`, `delphes_card.dat`). They mirror the Z → νν̄ and tt̄ gridpack scripts.

| File | What it does |
|---|---|
| `make_wlnu_gridpacks.sh` | Builds one gridpack per MET slice, plus the Pythia8 command file MadGraph writes for these cards |
| `gridpack_wlnu.sub` | Runs `make_wlnu_gridpacks.sh` on HTCondor: one job for `jets`, one for `bb` |
| `run_wlnu_job.sh` | One production job: gridpack → LHE → Pythia8 (MLM) → Delphes → EOS |
| `submit_wlnu.sh` | Writes the job list and the submit file for production, and submits |

The generator settings in `make_wlnu_gridpacks.sh` are the same as in `make_wlnu_samples.sh`:
- `jets`: W± + 0, 1, 2 jets, 5-flavour; `bb`: W± + b b̄ + 0, 1 jet, 4-flavour. ℓ = e, μ, τ.
- No lepton cuts at generation (`ptl = 0`, `etal = -1`, `drjl = drll = 0`): the lepton veto is applied in the analysis.
- xqcut = 15 GeV, qCut = 30 GeV.
- MET slices 50–150, 150–250, > 250 GeV (parton-level MET = pT of the W's neutrino).

If you change a setting, change it in both scripts.

## 1. Once, on lxplus

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh
export MG=/eos/user/s/syu/MG5_aMC_v3_5_7
lhapdf list --installed | grep NNPDF31_nnlo_as_0118_nf_4     # the 4F PDF of the bb sample must be listed
```
Copy the whole `wlnu_cards` folder (cards + these scripts) to `$MG/wlnu_cards`. Pythia8 and Delphes in `$MG`
are the ones you already built for tt̄.

## 2. Build the gridpacks

On the batch system (both samples):
```bash
cd $MG/wlnu_cards
mkdir -p output error log
condor_submit gridpack_wlnu.sub
```
or interactively, one sample at a time:
```bash
bash make_wlnu_gridpacks.sh bb
bash make_wlnu_gridpacks.sh jets
```
Result: `gridpacks/wlnu_{bb,jets}_MET{50to150,150to250,250toinf}_gridpack.tar.gz` and `_pythia8.cmd`.
A job that was cut off can be submitted again: finished slices are skipped (`FORCE=1` rebuilds them).

- If the process folders `WLNu_jets_MET` and `WLNu_bb4F_MET` do not exist in `$MG` yet, the script creates them.
  Do not copy them from the Mac: they hold code compiled for macOS.
- If the `bb` build stops with an `AlphaS_FlavorScheme` error from the 4F PDF, apply the same `.info` fix as on the
  Mac (see `README_wlnu.md`), in `WLNu_bb4F_MET/lib/PDFsets/NNPDF31_nnlo_as_0118_nf_4/`, and resubmit.

## 3. Test one job, then submit production

```bash
export DELPHES=$MG/Delphes
bash run_wlnu_job.sh gridpacks/wlnu_bb_MET250toinf_gridpack.tar.gz gridpacks/wlnu_bb_MET250toinf_pythia8.cmd \
     delphes_card.dat 200 1 /tmp/wlnu_test WLNu_bb4F_MET
# time it, then choose the events per job so that a job takes 2-6 h
bash submit_wlnu.sh bb   40 10000 1000     # 40 jobs x 10k events per MET slice, seeds 1000-1039
bash submit_wlnu.sh jets 40 10000 1000
```
- **Output:** `$OUT/WLNu_bb4F_MET/Events/wlnu_bb_MET250toinf_r<seed>/` (and `WLNu_jets_MET/...`). `OUT` defaults
  to `root://eosuser.cern.ch//eos/user/s/syu/wlnu`. The layout is the MadGraph one.
- **Normalisation:** each job's banner has the gridpack cross section and the matched cross section of that job,
  σ_LHE × N(after matching) / N(LHE).
- **Adding jobs later:** use a new first seed (e.g. 2000), so run names and seeds do not repeat.
- **Options:** `FLAVOUR=longlunch|workday|tomorrow`, `SLICES="250:-1"` for one slice only, `DRYRUN=1` to write the
  job list and submit file without submitting.
- **Low-MET check:** `SLICES="0:50" bash make_wlnu_gridpacks.sh bb` builds the extra 0–50 GeV slice, to confirm it
  contributes ≲ 1–2% after the full selection (pass the same `SLICES` to `submit_wlnu.sh`).
- **Overlap removal still applies:** keep W + b b̄ only from the 4F sample (see `README_wlnu.md`).
