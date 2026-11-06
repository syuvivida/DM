#!/bin/bash
# Generate MET-biased tt~ backgrounds for the 4b + MET search with MadGraph5_aMC@NLO + Pythia8.
#
# usage:  bash make_ttbar_samples.sh <sample> [nevents_per_run] [n_runs]
#         sample = semilep | dilep | ttbb
# example: bash make_ttbar_samples.sh semilep 50000 10      (10 runs per MET slice, different seeds)
#
# Each MET slice is generated separately (misset <= parton-level MET < missetmax), so the high-MET tail gets
# enough events.  Combine the slices by weighting each event with  sigma_slice / N_slice  (the cross sections
# are in each run's banner), and remove any events below your analysis cut afterwards.
set -e
MG=${MG:-/Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7}        # your MadGraph directory (or export MG=...)
HERE=$(cd "$(dirname "$0")" && pwd)
SAMPLE=${1:?give semilep, dilep or ttbb}
NEV=${2:-50000}
NRUN=${3:-1}

case $SAMPLE in
  semilep) PROC=proc_tt_semilep.dat; DIR=TT_semilep_MET; MLM=1 ;;
  dilep)   PROC=proc_tt_dilep.dat;   DIR=TT_dilep_MET;   MLM=1 ;;
  ttbb)    PROC=proc_ttbb_lep.dat;   DIR=TTbb_lep_MET;   MLM=0 ;;
  *) echo "unknown sample $SAMPLE"; exit 1 ;;
esac

# parton-level MET slices: "min:max" in GeV (max = -1: no upper limit).  The lowest edge should be below the
# analysis MET cut (tau and b-hadron neutrinos and detector effects add to the MET after the shower).
SLICES=${SLICES:-"100:200 200:300 300:-1"}

cd "$MG"
if [ ! -d "$DIR" ]; then
  ./bin/mg5_aMC "$HERE/$PROC"
fi
cd "$DIR"

# --- run card: 13.6 TeV, NNPDF3.1 NNLO, no lepton/b cuts (keeps lost-lepton events), cuts on decay products ON
COMMON="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf lhaid=303600 use_syst=False systematics_program=none
        cut_decays=True ptl=0 etal=-1 drll=0 drjl=0 drbl=0 mmll=0 ptb=0 etab=-1 drbj=0 nevents=$NEV"
if [ "$MLM" = 1 ]; then
  RUN="$COMMON ickkw=1 xqcut=30 maxjetflavor=5 ptj=20 etaj=-1 drjj=0 drbb=0 auto_ptj_mjj=False"
  cp "$HERE/pythia8_card_MLM.dat" Cards/pythia8_card.dat
else      # tt~bb~, 4-flavour, no matching: regulate the extra b pair
  RUN="$COMMON ickkw=0 maxjetflavor=4 ptj=20 etaj=-1 drjj=0 ptb=20 etab=3.0 drbb=0.3 drbj=0"
  grep -v "^JetMatching" "$HERE/pythia8_card_MLM.dat" > Cards/pythia8_card.dat
fi
# with -f, MadGraph switches on the steps whose cards exist: Pythia8 (pythia8_card.dat) yes, MadSpin no
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat   # decays are in the process
  cp -p "$HERE/delphes_card.dat" Cards/delphes_card.dat
seed=$RANDOM
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}
  for i in $(seq 1 "$NRUN"); do
    seed=$((seed + 1))
    name=${SAMPLE}_MET${lo}to${hi/-1/inf}_r${i}
    python3 "$HERE/set_run_card.py" Cards/run_card.dat $RUN misset=$lo missetmax=$hi iseed=$seed
    echo "=== $name"
    ./bin/generate_events "$name" -f
  done
done
echo "done: events in $MG/$DIR/Events/"
