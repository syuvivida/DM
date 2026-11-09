# MET-biased Z → νν̄ + b-jet backgrounds for the 4b + MET search (MadGraph5_aMC@NLO 3.5.7 + Pythia8)

| File | What it is |
|---|---|
| `proc_znunu_bb.dat` | Z(→νν̄) + b b̄ + 0, 1 light jet. 4-flavour scheme (massive b, no b in the proton), MLM matching on the light jet. **The main Z background with 2 real b jets.** |
| `proc_znunu_jets.dat` | Z(→νν̄) + 0, 1, 2 jets. 5-flavour scheme (b in p and j), MLM matching. Covers Z + light jets (mistags), Z + c, and soft or collinear g → b b̄. |
| `pythia8_card_znunu_bb4F.dat` | CP5 tune + MLM settings for the 4F sample (qCut = 30 GeV, nQmatch = 4, nJetMax = 1) |
| `pythia8_card_znunu_jets.dat` | CP5 tune + MLM settings for the 5F sample (qCut = 30 GeV, nQmatch = 5, nJetMax = 2) |
| `set_run_card.py` | Sets run_card values without the interactive card editor |
| `make_znunu_samples.sh` | Builds the process folder, sets the cards and generates the MET slices |

## Why it is set up this way

* **The decay is inside the hard process** (`z > vl vl~`), with ν = ν_e, ν_μ, ν_τ, so the parton-level MET cut works.
  - The cut is `misset`, together with `cut_decays = True`.
  - For Z → νν̄ the parton-level MET is exactly pT(Z). Each MET slice is therefore a pT(Z) slice, and the
    high-MET tail gets enough events.
* **MET slices.** The default is `100:200 200:300 300:-1` GeV. The real MET differs from pT(Z) only by detector
  resolution and neutrinos from b decays, so the lowest edge can sit closer to the analysis cut than for tt̄.
  Check once with a small `0:100` slice that it contributes ≲ 1% after your selection.
* **Two samples, two jobs.**
  - **Z + b b̄ (4F)** describes hard, well-separated b jets with the full b-mass matrix element. This is the
    configuration a ≥ 2–3 b-tag selection keeps.
  - **Z + jets (5F)** is needed for events with fake b-tags (light and charm jets) and for b quarks from the
    shower.
* **Overlap removal (required).** Both samples contain Z + b b̄. Keep Z + b b̄ only from the 4F sample: in the
  5F Z + jets sample, drop events with ≥ 1 b hadron not coming from the Z. Using generator-level b-hadron
  matching to jets with pT > 20 GeV is the usual choice.
* **4-flavour PDF.** The 4F sample uses NNPDF31_nnlo_as_0118_nf_4 in the matrix element (`LHAID4F`, default
  320900). The shower uses the standard 5F CP5 PDF, as CMS does.
  - Check the set is installed with `lhapdf list | grep nf_4`, or let MadGraph download it on the first run.
  - If it can't, use `LHAID4F=303600 bash make_znunu_samples.sh bb ...`, at the cost of a small inconsistency.
* **MLM matching.** `xqcut = 15` GeV in MadGraph and `qCut = 30` GeV in Pythia. Check the differential jet
  rates (`Events/<run>/*djrs*`): they should be smooth around 30 GeV. If they aren't, move qCut, keeping it at
  1.5–2 × xqcut.

## Running

```bash
cd /path/to/znunu_cards
export MG=/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7

bash make_znunu_samples.sh bb   50000 4      # Z(nu nu) + b b~ 4F: 4 runs x 50k events per MET slice
bash make_znunu_samples.sh jets 50000 4      # Z(nu nu) + jets 5F
SLICES="0:-1" bash make_znunu_samples.sh bb 5000    # small inclusive run, for the normalization check below
```

* **Where the events go:** `$MG/ZNuNu_bb4F_MET/Events/znunu_bb_MET<lo>to<hi>_r<i>/` (and likewise for `jets`).
* **Adding more runs later:** pass a 4th argument, the first run number. For example,
  `bash make_znunu_samples.sh bb 50000 4 5` makes r5–r8 with new seeds. Runs that already exist are skipped.
* **Delphes:** copy `Delphes/cards/delphes_card_CMS.dat` to `Cards/delphes_card.dat` in the process folder before
  running, and each run then also goes through Delphes.

## Normalization

1. **Per slice:** weight each event by `σ_slice / N_slice`. Use the **matched** cross section from the Pythia8 log
   or banner (`tag_1_merged_xsecs.txt`), and N after matching. Add the runs of a slice by dividing by their total
   N.
2. **Higher orders:**
   - **Z + jets:** a common approach scales the 5F sample by the NNLO/LO ratio of inclusive Z production. Take
     σ_NNLO(pp → Z → νν̄) at 13.6 TeV from the CMS/LHC standard-model cross-section tables, divided by the matched
     σ of a small inclusive run (`SLICES="0:-1"`). That ratio is typically ~1.2–1.3.
   - **Z + b b̄:** its K-factor is less certain. NLO/LO is about 1.2–1.5, and measured Z + b b̄ rates have
     differed from predictions by tens of %. Use a dedicated normalization uncertainty (≈ 20–30%), or constrain
     it in data with Z(→ℓℓ) + b b̄ control regions.
3. **Statistics:** aim for at least ~10 raw events per slice that matters, after your full 4b + MET selection.
   Add runs to the slices that dominate.

## Other Z / MET + b backgrounds you may want next
ZZ → νν̄ b b̄, ZH(→ b b̄) with Z → νν̄, tt̄Z(→ νν̄), and W(→ ℓν, lost lepton) + b b̄. They can be set up the same way.
