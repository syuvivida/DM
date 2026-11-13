# scan2hdma: fast sigma x BR estimates and optimizer for the 2HDM+a (Pseudoscalar_2HDMI UFO, Type I)

Signal: sigma(gg->A) x BR(A->Ha) x BR(H->hh) x BR(a->chi chi), 13.6 TeV, normalized to MadGraph LO (K_MG = 0.776).

## Requirements
Python 3 with numpy and the LHAPDF python module, plus the PDF set NNPDF31_nnlo_as_0118 (lhaid 303600).
Easiest (conda-forge):
    conda create -n hdma -c conda-forge python=3.12 numpy lhapdf
    conda activate hdma
    lhapdf install NNPDF31_nnlo_as_0118        # or point HDMA_PDFPATH at a folder that already has it

## Environment variables (optional)
    HDMA_PDFPATH  folder containing NNPDF31_nnlo_as_0118/ (e.g. .../HEPTools/lhapdf6_py3/share/LHAPDF)
    HDMA_MODEL    UFO model folder (default: ./Pseudoscalar_2HDMI next to the scripts)

## Check the setup (reproduces the MadGraph widths/BRs of the reference card)
    python bench.py

## Compare with a MadGraph run (auto widths)
    python check_card.py /path/to/workdir/Events/run_01/run_01_tag_1_banner.txt
Prints the scanner's widths/BRs for a, H, A next to MadGraph's.  Closed 2-body channels with an unstable daughter
(e.g. H -> a a* -> a chi chi~ when 2 m_a > m_H) are included as 3-body widths, as MadGraph does.

## Optimize (Type I)
    python refine_typeI.py examples/start_typeI.json examples/cfg_typeI.json 11
Arguments: start point, constraint config, random seed (different seeds -> different random walks).
Inline JSON strings also work instead of file names. ~30 s per run.

start JSON: tanbeta, sinp (sin theta), cba (cos(beta-alpha)), lam3, laP1, laP2, mh2 (H), mh3 (A), mh4 (a)
cfg JSON:
    custodial  "A": mHc = m_A0 (2HDM A mass before a-A mixing) | "H": mHc = mH | "free": mhc is a fit parameter
    tb_min     lower bound on tan(beta) (flavour)
    cba_max    upper bound on cos(beta-alpha) (Higgs couplings)
    mhc_min    lower bound on m_H+-
    monoh      cap on sigma(A->ha->bb+MET) in fb (rough mono-h guard; 1e9 = off)
    hh         1 = apply resonant-HH limit on direct gg->H->hh (approximate curve in scan_opt.py)
    unit       1 = require tree-level unitarity
Output: best sigma x BR, BRs, side channels, and a PARAMS line to copy into MadGraph ("set <name> <value>").

## 2D scan in (m_A, m_H) with per-point optimization of the couplings
    python scan_mA_mH.py examples/scan_cfg.json
Fixes m_a and m_H+- = m_A - dmAc; optimizes tanbeta, sinp, sinbma, lam3, laP1, laP2 at every grid point under the
constraints above plus |Delta T| < T_max.  Runs in parallel on all cores ("workers").  Writes scan_mA_mH.csv
(best sigma x BR, BRs, side channels and best couplings per point; "nan" = no allowed couplings) and scan_mA_mH.png.
Cost ~ n_iter x n_start x 10 ms per point per core (1000 x 2: ~20 s).

### Also optimizing m_A - m_H+-
    python scan_mA_mH.py examples/scan_cfg_fit_dmAc.json
With "fit_dmAc": 1 the mass splitting dmAc = m_A - m_H+- (range "dmAc_range") is optimized together with the six
couplings at every point (starts from m_H+- = m_H, m_H+- = m_A and the given dmAc).  The best dmAc is the last CSV
column and the second number in each cell of the plot.

## MadGraph param_cards for every scan point
    python make_cards.py scan_mA_mH_fit_dmAc.csv /path/to/workdir_AHhha/Cards/param_card.dat \
        --outdir cards --mg-script /path/to/workdir_AHhha --nevents 10000
    bash cards/run_all.sh                    # copies each card into Cards/ and runs bin/generate_events <tag> -f
Writes cards/param_card_mA<mA>_mH<mH>.dat (optimized tanbeta, sinbma, sinp, lam3, lap1, lap2 and the masses; widths of
h, H, A, H+-, a set to Auto), cards/summary.csv, run_all.sh (recommended; no interactive card editor, so MadGraph does not need the LHAPDF python
module) and run_all.mg5 (mg5_aMC alternative).
Options: --top N, --min-sigbr X, --mxd, --gpxd.  After the runs, check each banner with check_card.py.
