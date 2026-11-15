#!/bin/bash
# Build MadGraph gridpacks for the MET-sliced tt~ backgrounds (one gridpack per sample and MET slice), for
# production on the CERN batch system with run_ttbar_job.sh. RUN THIS ON LXPLUS (EL9), not on the Mac: a gridpack
# contains compiled code and only runs on the system it was built on.
#
# usage:  bash make_ttbar_gridpacks.sh <sample> [gridpack_dir]
#         sample = semilep | dilep | ttbb
# environment:
#   MG       MadGraph directory on lxplus (needs Pythia8 installed in it: "install pythia8" in mg5_aMC)
#   LCGVIEW  LCG view to source (default LCG_110a, el9, gcc14); use the same one in the batch jobs
#   SLICES   parton-level MET slices "lo:hi" (hi = -1: no upper limit), default "100:200 200:300 300:-1"
#
# For each slice it writes into <gridpack_dir> (default ./gridpacks):
#   <sample>_MET<lo>to<hi>_gridpack.tar.gz   the gridpack (LHE generation only)
#   <sample>_MET<lo>to<hi>_pythia8.cmd       the Pythia8 command file MadGraph itself writes for these cards
#                                            (made with a 100-event normal run), so the batch jobs shower exactly
#                                            as the interactive samples were showered
set -e
LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}
# shellcheck disable=SC1090
source "$LCGVIEW"
MG=${MG:?set MG to your MadGraph directory on lxplus}
HERE=$(cd "$(dirname "$0")" && pwd)
SAMPLE=${1:?give semilep, dilep or ttbb}
GPDIR=$(mkdir -p "${2:-$HERE/gridpacks}" && cd "${2:-$HERE/gridpacks}" && pwd)
SLICES=${SLICES:-"100:200 200:300 300:-1"}

case $SAMPLE in
  semilep) PROC=proc_tt_semilep.dat; DIR=TT_semilep_MET; MLM=1 ;;
  dilep)   PROC=proc_tt_dilep.dat;   DIR=TT_dilep_MET;   MLM=1 ;;
  ttbb)    PROC=proc_ttbb_lep.dat;   DIR=TTbb_lep_MET;   MLM=0 ;;
  *) echo "unknown sample $SAMPLE"; exit 1 ;;
esac

# --- same generator settings as make_ttbar_samples.sh (do not change one without the other) ---
COMMON="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf lhaid=303600 use_syst=False systematics_program=none
        cut_decays=True ptl=0 etal=-1 drll=0 drjl=0 drbl=0 mmll=0 ptb=0 etab=-1 drbj=0"
if [ "$MLM" = 1 ]; then
  RUN="$COMMON ickkw=1 xqcut=30 maxjetflavor=5 ptj=20 etaj=-1 drjj=0 drbb=0 auto_ptj_mjj=False"
  PYCARD="$HERE/pythia8_card_MLM.dat"
else
  RUN="$COMMON ickkw=0 maxjetflavor=4 ptj=20 etaj=-1 drjj=0 ptb=20 etab=3.0 drbb=0.3 drbj=0"
  PYCARD=$(mktemp); grep -v "^JetMatching" "$HERE/pythia8_card_MLM.dat" > "$PYCARD"
fi

cd "$MG"
[ -d "$DIR" ] || ./bin/mg5_aMC "$HERE/$PROC"
cd "$DIR"
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat
rm -f Cards/delphes_card.dat                       # Delphes runs in the batch jobs, not here

seed=$((7000 + RANDOM % 1000))
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=${SAMPLE}_MET${lo}to${hi/-1/inf}
  seed=$((seed + 1))

  # 1) gridpack: no shower (with -f, MadGraph runs Pythia8 only if Cards/pythia8_card.dat exists)
  rm -f Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat $RUN misset=$lo missetmax=$hi iseed=$seed gridpack=True nevents=10000
  echo "=== gridpack $tag"
  ./bin/generate_events "gp_$tag" -f
  gp=$(ls -t ./*gp_${tag}*gridpack*.tar.gz 2>/dev/null | head -1)
  [ -n "$gp" ] || { echo "ERROR: no gridpack tarball found for $tag in $PWD"; exit 1; }
  mv "$gp" "$GPDIR/${tag}_gridpack.tar.gz"

  # 2) 100-event normal run with Pythia8, only to keep the Pythia8 command file MadGraph writes
  cp "$PYCARD" Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat gridpack=False nevents=100 iseed=$((seed + 500))
  echo "=== pythia8 command file for $tag"
  ./bin/generate_events "cmd_$tag" -f
  cmd=$(ls Events/cmd_${tag}/*pythia8.cmd 2>/dev/null | head -1)
  [ -n "$cmd" ] || { echo "ERROR: no pythia8 .cmd file in Events/cmd_${tag}"; exit 1; }
  cp "$cmd" "$GPDIR/${tag}_pythia8.cmd"
  rm -rf "Events/cmd_${tag}" "Events/gp_${tag}"
  echo "    -> $GPDIR/${tag}_gridpack.tar.gz, $GPDIR/${tag}_pythia8.cmd"
done
echo "done. Test one job interactively before submitting:"
echo "  bash run_ttbar_job.sh $GPDIR/${SAMPLE}_MET300toinf_gridpack.tar.gz $GPDIR/${SAMPLE}_MET300toinf_pythia8.cmd delphes_card.dat 200 1 /tmp/ttbar_test TT_${SAMPLE}_MET"
