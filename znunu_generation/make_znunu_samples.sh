#!/bin/bash
# Generate MET-biased Z -> nu nu~ + jets / + b b~ backgrounds for the 4b + MET search (MadGraph5_aMC@NLO + Pythia8).
#
# usage:  bash make_znunu_samples.sh <sample> [nevents_per_run] [n_runs] [first_run]
#         sample = jets  (Z(nu nu) + 0,1,2 jets, 5-flavour, MLM)
#                | bb    (Z(nu nu) + b b~ + 0,1 jet, 4-flavour, massive b, MLM on the light jet)
#         first_run: number of the first run (default 1).  Use it to ADD runs to an existing sample, e.g.
#                    "bash make_znunu_samples.sh bb 50000 4 5" makes runs r5..r8 with new seeds.
# environment:
#   MG       MadGraph directory (default /Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7)
#   SLICES   parton-level MET slices "lo:hi" in GeV (hi = -1: no upper limit), default "100:200 200:300 300:-1".
#            For Z -> nu nu the parton-level MET is exactly pT(Z).
#   LHAID4F  4-flavour PDF for the bb sample (default 320900 = NNPDF31_nnlo_as_0118_nf_4; check with "lhapdf list")
#
# Each run is started with "./bin/generate_events <name> -f" (no interactive card editor).  If Cards/delphes_card.dat
# exists in the process folder, Delphes runs too.
set -e
MG=${MG:-/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7}
HERE=$(cd "$(dirname "$0")" && pwd)
SAMPLE=${1:?give jets or bb}
NEV=${2:-50000}
NRUN=${3:-1}
FIRST=${4:-1}
SLICES=${SLICES:-"100:200 200:300 300:-1"}
LHAID4F=${LHAID4F:-320900}

case $SAMPLE in
  jets) PROC=proc_znunu_jets.dat; DIR=ZNuNu_jets_MET;  PY=pythia8_card_znunu_jets.dat
        RUN="lhaid=303600 ickkw=1 xqcut=15 maxjetflavor=5 ptj=20 etaj=-1 drjj=0 ptb=0 etab=-1 drbb=0 drbj=0" ;;
  bb)   PROC=proc_znunu_bb.dat;   DIR=ZNuNu_bb4F_MET;  PY=pythia8_card_znunu_bb4F.dat
        RUN="lhaid=$LHAID4F ickkw=1 xqcut=15 maxjetflavor=4 ptj=20 etaj=-1 drjj=0 ptb=0 etab=-1 drbb=0 drbj=0" ;;
  *) echo "unknown sample $SAMPLE (use jets or bb)"; exit 1 ;;
esac
COMMON="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf use_syst=False systematics_program=none cut_decays=True
        auto_ptj_mjj=False nevents=$NEV"

cd "$MG"
if [ ! -d "$DIR" ]; then
  ./bin/mg5_aMC "$HERE/$PROC"
fi
cd "$DIR"
cp "$HERE/$PY" Cards/pythia8_card.dat
cp "$HERE/delphes_card.dat" Cards/delphes_card.dat
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat   # decay is in the process

seed=$((2000 + 100 * FIRST))          # different first_run -> different seeds
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}
  for i in $(seq "$FIRST" $((FIRST + NRUN - 1))); do
    seed=$((seed + 1))
    name=znunu_${SAMPLE}_MET${lo}to${hi/-1/inf}_r${i}
    if [ -d "Events/$name" ]; then echo "=== $name exists, skipped"; continue; fi
    python3 "$HERE/set_run_card.py" Cards/run_card.dat $COMMON $RUN misset=$lo missetmax=$hi iseed=$seed
    echo "=== $name"
    ./bin/generate_events "$name" -f
  done
done
echo "done: events in $MG/$DIR/Events/"
