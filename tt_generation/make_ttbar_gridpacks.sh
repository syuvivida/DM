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
#   HERE     directory holding the proc/pythia8 cards and set_run_card.py (default: the script's directory).
#            MUST be set in HTCondor jobs: condor copies the script to a scratch directory without the cards.
#   NCORE    cores MadGraph may use (default: OMP_NUM_THREADS, which HTCondor sets to request_cpus, else 4)
#   FORCE    set FORCE=1 to rebuild slices whose outputs already exist (default: skip them, so an
#            interrupted job can simply be rerun and continues with the missing slices)
# A gridpack is only reused if it was built with the current run-card settings: they are recorded in
# <sample>_MET<lo>to<hi>_settings.txt. A gridpack with other settings, or without that file (built before
# 16 Nov 2026, with parton cuts on the top-decay products), is moved to <gridpack_dir>/old_settings/ and rebuilt.
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
HERE=${HERE:-$(cd "$(dirname "$0")" && pwd)}
NCORE=${NCORE:-${OMP_NUM_THREADS:-4}}
for f in pythia8_card_MLM.dat set_run_card.py; do
  [ -f "$HERE/$f" ] || { echo "ERROR: $HERE/$f not found; set HERE to the ttBar directory"; exit 1; }
done
SAMPLE=${1:?give semilep, dilep or ttbb}
GPDIR=$(mkdir -p "${2:-$HERE/gridpacks}" && cd "${2:-$HERE/gridpacks}" && pwd)
SLICES=${SLICES:-"100:200 200:300 300:-1"}

case $SAMPLE in
  semilep) PROC=proc_tt_semilep.dat; DIR=TT_semilep_MET; MLM=1 ;;
  dilep)   PROC=proc_tt_dilep.dat;   DIR=TT_dilep_MET;   MLM=1 ;;
  ttbb)    PROC=proc_ttbb_lep.dat;   DIR=TTbb_lep_MET;   MLM=0 ;;
  *) echo "unknown sample $SAMPLE"; exit 1 ;;
esac
[ -f "$HERE/$PROC" ] || { echo "ERROR: $HERE/$PROC not found; set HERE to the ttBar directory"; exit 1; }

# --- same generator settings as make_ttbar_samples.sh (do not change one without the other) ---
# All standard parton cuts are off. cut_decays = True makes them act on the decay products too, so any jet or b cut
# here would also cut on the b quarks and W-decay quarks from the top decays.
#   semilep, dilep: extra jets are regulated by the MLM kt cut (xqcut), which does not touch decay products.
#   ttbb (4-flavour): the b mass regulates the extra b b~ pair, so no cut is needed; the sample then also covers
#                     tt~ + one extra b jet (other b soft, forward, or merged), which the >= 3 b-jet split needs.
COMMON="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf lhaid=303600 use_syst=False systematics_program=none
        cut_decays=True auto_ptj_mjj=False ptl=0 etal=-1 drll=0 drjl=0 drbl=0 mmll=0
        ptj=0 etaj=-1 drjj=0 mmjj=0 ptb=0 etab=-1 drbb=0 drbj=0 mmbb=0"
if [ "$MLM" = 1 ]; then
  RUN="$COMMON ickkw=1 xqcut=30 maxjetflavor=5"
  PYCARD="$HERE/pythia8_card_MLM.dat"
else
  RUN="$COMMON ickkw=0 xqcut=0 maxjetflavor=4"
  PYCARD=$(mktemp); grep -v "^JetMatching" "$HERE/pythia8_card_MLM.dat" > "$PYCARD"
fi

cd "$MG"
[ -d "$DIR" ] || ./bin/mg5_aMC "$HERE/$PROC"
cd "$DIR"
rm -f RunWeb                                       # stale lock left by an interrupted MadGraph run
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat
rm -f Cards/delphes_card.dat                       # Delphes runs in the batch jobs, not here

seed=$((7000 + RANDOM % 1000))
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=${SAMPLE}_MET${lo}to${hi/-1/inf}
  seed=$((seed + 1))
  GP_OUT="$GPDIR/${tag}_gridpack.tar.gz"; CMD_OUT="$GPDIR/${tag}_pythia8.cmd"; SET_OUT="$GPDIR/${tag}_settings.txt"
  want=$(echo $RUN misset=$lo missetmax=$hi)       # the settings this gridpack must have (one line)

  # resume: skip what is already done (a gridpack tarball counts only if it is a complete archive)
  gp_ok=0
  if [ -s "$GP_OUT" ] && tar -tzf "$GP_OUT" >/dev/null 2>&1; then gp_ok=1; fi
  if [ $gp_ok = 1 ] && [ "$(cat "$SET_OUT" 2>/dev/null)" != "$want" ]; then
    echo "=== $tag: the existing gridpack was built with other run-card settings (or before they were recorded):"
    echo "    moved to $GPDIR/old_settings/, rebuilding"
    mkdir -p "$GPDIR/old_settings"
    for f in "$GP_OUT" "$CMD_OUT" "$SET_OUT"; do
      if [ -e "$f" ]; then mv -f "$f" "$GPDIR/old_settings/"; fi
    done
    gp_ok=0
  fi
  if [ "${FORCE:-0}" != 1 ] && [ $gp_ok = 1 ] && [ -s "$CMD_OUT" ]; then
    echo "=== $tag: gridpack and pythia8.cmd already exist, skipping (FORCE=1 to rebuild)"
    continue
  fi
  rm -rf "Events/gp_${tag}" "Events/cmd_${tag}"     # leftovers of an interrupted attempt

  # 1) gridpack: no shower (with -f, MadGraph runs Pythia8 only if Cards/pythia8_card.dat exists)
  rm -f Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat $RUN misset=$lo missetmax=$hi iseed=$seed gridpack=True nevents=10000
  if [ "${FORCE:-0}" != 1 ] && [ $gp_ok = 1 ]; then
    echo "=== $tag: gridpack exists, only making the pythia8.cmd"
  else
    echo "=== gridpack $tag"
    ./bin/generate_events "gp_$tag" -f --nb_core=$NCORE
    gp=$(ls -t ./*gp_${tag}*gridpack*.tar.gz 2>/dev/null | head -1)
    [ -n "$gp" ] || { echo "ERROR: no gridpack tarball found for $tag in $PWD"; exit 1; }
    mv "$gp" "$GP_OUT.part" && mv "$GP_OUT.part" "$GP_OUT"   # never leave a half-copied tarball
    echo "$want" > "$SET_OUT"
  fi

  # 2) 100-event normal run with Pythia8, only to keep the Pythia8 command file MadGraph writes
  cp "$PYCARD" Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat gridpack=False nevents=100 iseed=$((seed + 500))
  echo "=== pythia8 command file for $tag"
  ./bin/generate_events "cmd_$tag" -f --nb_core=$NCORE
  cmd=$(ls Events/cmd_${tag}/*pythia8.cmd 2>/dev/null | head -1)
  [ -n "$cmd" ] || { echo "ERROR: no pythia8 .cmd file in Events/cmd_${tag}"; exit 1; }
  cp "$cmd" "$CMD_OUT"
  rm -rf "Events/cmd_${tag}" "Events/gp_${tag}"
  echo "    -> $GPDIR/${tag}_gridpack.tar.gz, $GPDIR/${tag}_pythia8.cmd"
done
echo "done. Test one job interactively before submitting:"
echo "  bash run_ttbar_job.sh $GPDIR/${SAMPLE}_MET300toinf_gridpack.tar.gz $GPDIR/${SAMPLE}_MET300toinf_pythia8.cmd delphes_card.dat 200 1 /tmp/ttbar_test $DIR"
