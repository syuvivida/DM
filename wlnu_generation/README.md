# MET-biased W → ℓν + b-jet ("lost lepton") backgrounds for the 4b + MET search (MadGraph5_aMC@NLO 3.5.7 + Pythia8)

| File | What it is |
|---|---|
| `proc_wlnu_bb.dat` | W±(→ ℓν) + b b̄ + 0, 1 light jet, ℓ = e, μ, τ. 4-flavour scheme (massive b), MLM matching on the light jet. **The main W background with two real b jets.** |
| `proc_wlnu_jets.dat` | W±(→ ℓν) + 0, 1, 2 jets. 5-flavour scheme (b in p and j), MLM matching. Covers W + light/charm jets (mistags) and soft g → b b̄. |
| `pythia8_card_wlnu_bb4F.dat` | CP5 tune + MLM settings for the 4F sample (qCut = 30 GeV, nQmatch = 4, nJetMax = 1) |
| `pythia8_card_wlnu_jets.dat` | CP5 tune + MLM settings for the 5F sample (qCut = 30 GeV, nQmatch = 5, nJetMax = 2) |
| `set_run_card.py` | Sets run_card values without the interactive card editor |
| `make_wlnu_samples.sh` | Builds the process folder, sets the cards and generates the MET slices |

## Why it is set up this way

* **The lepton is not "lost" at generation.**
  - All W → ℓν events are generated, with **no lepton cuts** (`ptl = 0`, `etal = -1`, `drjl = drll = 0`).
  - The analysis then applies its isolated-e/μ veto (at generator level, or in Delphes).
  - The events that survive are the "lost-lepton" background: an e/μ outside the acceptance, below the pT
    threshold, or not isolated (often inside a b jet); and above all **W → τν with a hadronic τ**, which passes
    any e/μ veto.

  Cutting on the lepton at generation would remove exactly these events. That's why τ is included in ℓ.
* **Decays inside the hard process** (`w+ > l+ vl`) and `cut_decays = True`, so the parton-level MET cut
  (`misset`) acts on the neutrino.
* **The slices start lower than for Z → νν̄** (default `50:150 150:250 250:-1` GeV).
  - At parton level, `misset` is the pT of the W's neutrino only.
  - In the detector, a lost e/μ adds to the MET, and in W → τν the MET also includes the τ's neutrino(s). So
    events with parton-level MET below the analysis cut (150–200 GeV) can still pass it.
  - **Check once:** generate a `0:50` slice and confirm it contributes ≲ 1–2% after your full selection. If it
    contributes more, lower the first edge.
* **Both charges** (W⁺ and W⁻) are in the same sample.
* **Two samples** (as for Z → νν̄):
  - the 4F W + b b̄ sample for hard, separated b jets;
  - the 5F W + jets sample for mistags.

  **Overlap removal is required.** Keep W + b b̄ only from the 4F sample: drop events from the 5F sample that
  have ≥ 1 b hadron not coming from the W decay.
* **4-flavour PDF:** `LHAID4F` (default 320900, NNPDF31_nnlo_as_0118_nf_4). This set is already installed in
  your `withPythia8D` MadGraph, with its `.info` file fixed. If MadGraph has to copy it into a new process folder
  and complains about `AlphaS_FlavorScheme`, copy the fixed `.info` from
  `HEPTools/lhapdf6_py3/share/LHAPDF/NNPDF31_nnlo_as_0118_nf_4/` into `<process>/lib/PDFsets/NNPDF31_nnlo_as_0118_nf_4/`.

## Running

```bash
cd /path/to/wlnu_cards
export MG=/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7

bash make_wlnu_samples.sh bb   50000 4      # W(l nu) + b b~ 4F: 4 runs x 50k events per MET slice
bash make_wlnu_samples.sh jets 50000 4      # W(l nu) + jets 5F
SLICES="0:-1" bash make_wlnu_samples.sh jets 5000   # inclusive run: K-factor normalization
SLICES="0:50" bash make_wlnu_samples.sh bb   10000  # low-MET check (see above)
```
* **Output:** events go to `$MG/WLNu_bb4F_MET/Events/wlnu_bb_MET<lo>to<hi>_r<i>/` (and likewise for `jets`).
* **Adding runs:** pass a 4th argument, the first run number, e.g. `... bb 50000 4 5` for runs r5–r8 with new
  seeds. Existing runs are skipped.
* **Delphes:** copy `Delphes/cards/delphes_card_CMS.dat` to `Cards/delphes_card.dat` in the process folder,
  and Delphes then runs after Pythia8.

## Normalization

1. **Per slice:** weight each event by `σ_slice / N_slice`. Use the **matched** cross section from the Pythia8
   log (`tag_1_merged_xsecs.txt`), and N after matching.
2. **W + jets:** scale to NNLO with the inclusive run, K = σ_NNLO(pp → W → ℓν) / σ_LO,matched(inclusive).
   - Use σ_NNLO at 13.6 TeV from the CMS/LHC SM tables, for W⁺ + W⁻ and three lepton flavours. Typically K ≈ 1.1–1.3.
3. **W + b b̄:** the NLO/LO K-factor is large (≈ 2 for W b b̄, because new qg channels open at NLO).
   - Assign a 30–40% normalization uncertainty, or constrain it with a 1-lepton + 2b control region (the same
     selection with the lepton veto inverted).
   - That control region is also the standard way to measure the lost-lepton background in data.

## Related backgrounds with a W and b jets (generate separately if they matter)
tt̄ (done; the dominant lost-lepton source), single top (tW and t-channel), and tt̄W.
