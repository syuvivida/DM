# Smaller backgrounds for the 4b + MET search: single top, tt̄Z, tt̄H, ZH, WH, ZZ, WZ

Gridpack production on lxplus + HTCondor, set up like the tt̄, Z → νν̄ and W → ℓν scripts. One folder holds
everything.

| File | What it does |
|---|---|
| `proc_*.dat` | The eight process cards |
| `samples.sh` | The sample table: process card, output folder, matching, forced H → b b̄, default MET slices |
| `pythia8_cp5.dat`, `pythia8_mlm.dat`, `pythia8_hbb.dat` | Pythia8 card pieces: CP5 tune, MLM matching, forced H → b b̄. The build script joins the pieces each sample needs |
| `make_bkg_gridpacks.sh` | Builds one gridpack per MET slice, plus the Pythia8 command file for the jobs |
| `gridpack_bkg.sub` | Runs that build on HTCondor: one job per sample |
| `run_bkg_job.sh` | One production job: gridpack → LHE → Pythia8 → Delphes → EOS |
| `submit_bkg.sh` | Writes the job lists and submit files for production, and submits |
| `set_run_card.py`, `delphes_card.dat` | Same files as in `znunu_cards` |

## The samples

All are leading order in the 5-flavour scheme (`sm-no_b_mass`, NNPDF31 NNLO, lhaid 303600), 13.6 TeV.

| Sample | Process | Decays | Extra jets | MET slices (GeV) | Output folder |
|---|---|---|---|---|---|
| `st_tw` | tW⁻ + t̄W⁺ | in MadGraph, ≥ 1 leptonic W (e, μ, τ) | none | 100–200, 200–300, > 300 | `ST_tW_MET` |
| `st_tch` | t-channel tq + t̄q | in MadGraph, t → b ℓ ν | none | 100–200, 200–300, > 300 | `ST_tch_MET` |
| `ttz` | tt̄Z | Z → νν̄ in MadGraph; tops in Pythia8 (all modes) | none | inclusive | `TTZ_nunu_MET` |
| `tth` | tt̄H | tops in Pythia8 (all modes); H → b b̄ forced in Pythia8 | none | inclusive | `TTH_bb_MET` |
| `zh` | ZH | Z → νν̄ in MadGraph; H → b b̄ forced in Pythia8 | 0, 1 (MLM) | > 100 | `ZH_nunubb_MET` |
| `wh` | W±H | W → ℓν in MadGraph; H → b b̄ forced in Pythia8 | 0, 1 (MLM) | > 50 | `WH_lnubb_MET` |
| `zz` | ZZ | Z → νν̄, Z → b b̄ in MadGraph | 0, 1 (MLM) | > 100 | `ZZ_nunubb_MET` |
| `wz` | W±Z | W → ℓν, Z → b b̄ in MadGraph | 0, 1 (MLM) | > 50 | `WZ_lnubb_MET` |

Why it is set up this way:

* **MET slices.** The parton-level MET cut (`misset`) only sees neutrinos that are in the MadGraph process.
  - Single top is sliced like tt̄.
  - For `zh` and `zz` the MET is pT(Z → νν̄), as in Z + jets, so one slice above 100 GeV is enough.
  - For `wh` and `wz` (lost lepton) the slice starts at 50 GeV, as in W + jets.
  - `ttz` and `tth` are inclusive: the tops decay in Pythia8, so their neutrinos are not in the cut.
* **No extra jets for tW.** tW + 1 jet contains tt̄ diagrams (g g → t W⁻ b̄) and would double count the tt̄ samples.
* **All standard parton cuts are off** (`ptj = ptb = 0`, no ΔR cuts). With `cut_decays = True`, which the MET cut
  needs, they would also act on the b quarks and W-decay quarks from top and Z decays.
* **Forced H → b b̄.** Pythia8 does not rescale the cross section for a forced decay. The job therefore multiplies
  the cross section in the banner by BR(H → b b̄) = 0.5824 for `tth`, `zh` and `wh` (it detects the
  `25:onMode = off` line in the Pythia8 command file). The banner lists the raw LHE cross section and the factor.
* **MLM matching** for the dibosons: `xqcut = 15` GeV in MadGraph, `qCut = 30` GeV in Pythia8, as for V + jets.
  Check the differential jet rates of one test run: they should be smooth around 30 GeV.
* **Not included:** the loop-induced g g → ZH (about 10–15% of ZH, more at high pT), s-channel single top, tt̄W.

## 1. Once, on lxplus

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh
export MG=/eos/user/s/syu/MG5_aMC_v3_5_7
```
Copy the whole `other_bkg_cards` folder to `$MG/other_bkg_cards`. Pythia8 and Delphes in `$MG` are the ones you
already built for tt̄.
Then make the scripts executable, in case the copy dropped the permission: `chmod +x $MG/other_bkg_cards/*.sh`.

## 2. Build the gridpacks

On the batch system (all eight samples, one job each):
```bash
cd $MG/other_bkg_cards
mkdir -p output error log
condor_submit gridpack_bkg.sub
```
or interactively, one sample at a time: `bash make_bkg_gridpacks.sh st_tw`.

Result: `gridpacks/<sample>_MET<lo>to<hi>_gridpack.tar.gz` and `_pythia8.cmd`, e.g. `st_tw_MET300toinf`,
`ttz_MET0toinf`, `zh_MET100toinf`. A job that was cut off can be submitted again: finished slices are skipped
(`FORCE=1` rebuilds them). To build only some samples, edit the last line of `gridpack_bkg.sub`.

## 3. Test one job, then submit production

```bash
export DELPHES=$MG/Delphes
bash run_bkg_job.sh gridpacks/st_tw_MET300toinf_gridpack.tar.gz gridpacks/st_tw_MET300toinf_pythia8.cmd \
     delphes_card.dat 200 1 /tmp/bkg_test ST_tW_MET
# time it, then choose the events per job so that a job takes 2-6 h
bash submit_bkg.sh "st_tw st_tch" 40 10000 1000     # 40 jobs x 10k events per MET slice
bash submit_bkg.sh "ttz tth zh wh zz wz" 20 10000 1000
bash submit_bkg.sh all 20 10000 1000                # or everything with the same numbers
```
- **Output:** `$OUT/<folder>/Events/<sample>_MET<lo>to<hi>_r<seed>/`, where `OUT` defaults to
  `root://eosuser.cern.ch//eos/user/s/syu/other_bkg`. The layout is the MadGraph one.
- **Adding jobs later:** use a new first seed (e.g. 2000), so run names and seeds do not repeat.
- **Options:** `FLAVOUR=longlunch|workday|tomorrow`, `DRYRUN=1` to write the job lists and submit files without
  submitting. All gridpacks are checked first: if one is missing, nothing is submitted.

## 4. Reading the samples in hhmet_sensitivity.py

Add the new folders to `BKG_DIRS`. The label "5F" only means "use this sample as it is" (no heavy-flavour split):
```python
    "ST_tW_MET":     ("single top", "5F"),
    "ST_tch_MET":    ("single top", "5F"),
    "TTZ_nunu_MET":  ("ttZ/ttH", "5F"),
    "TTH_bb_MET":    ("ttZ/ttH", "5F"),
    "ZH_nunubb_MET": ("VH", "5F"),
    "WH_lnubb_MET":  ("VH", "5F"),
    "ZZ_nunubb_MET": ("VV", "5F"),
    "WZ_lnubb_MET":  ("VV", "5F"),
```
The run names follow the `MET<lo>to<hi>_r<seed>` pattern the script parses, and the banner line it reads
("Matched Integrated weight") already includes the matching efficiency and the H → b b̄ factor.

## 5. Normalisation

The banner cross sections are leading order. Scale each sample to a higher-order reference:
K = σ_ref × BR / σ_LO, with σ_LO from a run without MET cut.
- `ttz` and `tth` are already inclusive: use their banner value directly.
- For the sliced samples, build one extra inclusive gridpack, e.g. `SLICES="0:-1" bash make_bkg_gridpacks.sh st_tw`,
  and run one small job (`SLICES="0:-1" bash submit_bkg.sh st_tw 1 10000 9000`).

Reference cross sections at 13.6 TeV. Only the tW value was checked against a source; take the others from the
LHC Top WG and LHC Higgs WG tables before use.

| Process | σ_ref (inclusive) | BR to apply | Source / status |
|---|---|---|---|
| tW (t + t̄) | 87.9 pb | 1 − (0.674)² ≈ 0.55 (≥ 1 leptonic W) | approx. N3LO, quoted in CMS TOP-23-008 |
| t-channel (t + t̄) | ≈ 230 pb | 0.326 (t → b ℓ ν, 3 flavours) | from memory: check LHC Top WG |
| tt̄Z | ≈ 0.95 pb | 0.200 (Z → νν̄) | from memory: check |
| tt̄H | ≈ 0.57 pb | 0.5824 (already in the banner) | from memory: check LHC Higgs WG |
| ZH | ≈ 0.94 pb | 0.200 × 0.5824 | from memory: check LHC Higgs WG |
| W±H | ≈ 1.46 pb | 0.326 × 0.5824 | from memory: check LHC Higgs WG |
| ZZ | ≈ 17 pb | 2 × 0.200 × 0.151 ≈ 0.060 | from memory: check |
| W±Z | ≈ 54 pb | 0.326 × 0.151 ≈ 0.049 | from memory: check |
