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
