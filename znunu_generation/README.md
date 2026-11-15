# Z(→ νν̄) + jets / + b b̄ gridpack production on lxplus + HTCondor

These scripts go in the same folder as the existing `znunu_cards` files (process cards, Pythia8 cards,
`set_run_card.py`, `delphes_card.dat`). They mirror the tt̄ gridpack scripts.

| File | What it does |
|---|---|
| `make_znunu_gridpacks.sh` | Builds one gridpack per MET slice, plus the Pythia8 command file MadGraph writes for these cards |
| `gridpack_znunu.sub` | Runs `make_znunu_gridpacks.sh` on HTCondor: one job for `jets`, one for `bb` |
| `run_znunu_job.sh` | One production job: gridpack → LHE → Pythia8 (MLM) → Delphes → EOS |
| `submit_znunu.sh` | Writes the job list and the submit file for production, and submits |

The generator settings in `make_znunu_gridpacks.sh` are the same as in `make_znunu_samples.sh` (5F Z + 0,1,2 jets
and 4F Z + b b̄ + 0,1 jet, xqcut = 15 GeV, qCut = 30 GeV, MET slices 100–200, 200–300, > 300 GeV). If you change a
setting, change it in both scripts.

## 1. Once, on lxplus

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh
export MG=/eos/user/s/syu/MG5_aMC_v3_5_7
lhapdf list --installed | grep NNPDF31_nnlo_as_0118_nf_4     # the 4F PDF of the bb sample must be listed
```
Copy the whole `znunu_cards` folder (cards + these scripts) to `$MG/znunu_cards`. Pythia8 and Delphes in `$MG`
are the ones you already built for tt̄.

## 2. Build the gridpacks

On the batch system (both samples, about a day at most):
```bash
cd $MG/znunu_cards
mkdir -p output error log
condor_submit gridpack_znunu.sub
```
or interactively, one sample at a time:
```bash
bash make_znunu_gridpacks.sh bb
bash make_znunu_gridpacks.sh jets
```
Result: `gridpacks/znunu_{bb,jets}_MET{100to200,200to300,300toinf}_gridpack.tar.gz` and `_pythia8.cmd`.
A job that was cut off can be submitted again: finished slices are skipped (`FORCE=1` rebuilds them).

If the process folders `ZNuNu_jets_MET` and `ZNuNu_bb4F_MET` do not exist in `$MG` yet, the script creates them.
Do not copy them from the Mac: they hold code compiled for macOS.

## 3. Test one job, then submit production

```bash
export DELPHES=$MG/Delphes
bash run_znunu_job.sh gridpacks/znunu_bb_MET300toinf_gridpack.tar.gz gridpacks/znunu_bb_MET300toinf_pythia8.cmd \
     delphes_card.dat 200 1 /tmp/znunu_test ZNuNu_bb4F_MET
# time it, then choose the events per job so that a job takes 2-6 h
bash submit_znunu.sh bb   40 10000 1000     # 40 jobs x 10k events per MET slice, seeds 1000-1039
bash submit_znunu.sh jets 40 10000 1000
```
- **Output:** `$OUT/ZNuNu_bb4F_MET/Events/znunu_bb_MET300toinf_r<seed>/` (and `ZNuNu_jets_MET/...`). `OUT` defaults
  to `root://eosuser.cern.ch//eos/user/s/syu/znunu`. The layout is the MadGraph one.
- **Normalisation:** each job's banner has the gridpack cross section and the matched cross section of that job,
  σ_LHE × N(after matching) / N(LHE).
- **Adding jobs later:** use a new first seed (e.g. 2000), so run names and seeds do not repeat.
- **Options:** `FLAVOUR=longlunch|workday|tomorrow`, `SLICES="300:-1"` for one slice only, `DRYRUN=1` to write the
  job list and submit file without submitting.
- **Overlap removal still applies:** keep Z + b b̄ only from the 4F sample (see `README_znunu.md`).
