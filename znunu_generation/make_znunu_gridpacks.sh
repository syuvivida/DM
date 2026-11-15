#!/bin/bash
# Build MadGraph gridpacks for the MET-sliced Z(-> nu nu~) backgrounds (one gridpack per sample and MET slice), for
# production on the CERN batch system with run_znunu_job.sh. RUN THIS ON LXPLUS (EL9), not on the Mac: a gridpack
# contains compiled code and only runs on the system it was built on.
#
# usage:  bash make_znunu_gridpacks.sh <sample> [gridpack_dir]
#         sample = jets  (Z(nu nu) + 0,1,2 jets, 5-flavour, MLM)
#                | bb    (Z(nu nu) + b b~ + 0,1 jet, 4-flavour, massive b, MLM on the light jet)
# environment:
#   MG       MadGraph directory on lxplus (needs Pythia8 installed in it: "install pythia8" in mg5_aMC)
#   LCGVIEW  LCG view to source (default LCG_110a, el9, gcc14); use the same one in the batch jobs
#   SLICES   parton-level MET (= pT(Z)) slices "lo:hi" (hi = -1: no upper limit), default "100:200 200:300 300:-1"
#   LHAID4F  4-flavour PDF for the bb sample (default 320900 = NNPDF31_nnlo_as_0118_nf_4)
#   HERE     directory holding the proc/pythia8 cards and set_run_card.py (default: the script's directory).
#            MUST be set in HTCondor jobs: condor copies the script to a scratch directory without the cards.
#   NCORE    cores MadGraph may use (default: OMP_NUM_THREADS, which HTCondor sets to request_cpus, else 4)
#   FORCE    set FORCE=1 to rebuild slices whose outputs already exist (default: skip them, so an
#            interrupted job can simply be rerun and continues with the missing slices)
#
# For each slice it writes into <gridpack_dir> (default <HERE>/gridpacks):
#   znunu_<sample>_MET<lo>to<hi>_gridpack.tar.gz   the gridpack (LHE generation only)
#   znunu_<sample>_MET<lo>to<hi>_pythia8.cmd       the Pythia8 command file MadGraph itself writes for these cards
#                                                  (made with a 100-event normal run), so the batch jobs shower
#                                                  exactly as the interactive samples were showered
set -e
LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}
# shellcheck disable=SC1090
source "$LCGVIEW"
MG=${MG:?set MG to your MadGraph directory on lxplus}
HERE=${HERE:-$(cd "$(dirname "$0")" && pwd)}
NCORE=${NCORE:-${OMP_NUM_THREADS:-4}}
SAMPLE=${1:?give jets or bb}
GPDIR=$(mkdir -p "${2:-$HERE/gridpacks}" && cd "${2:-$HERE/gridpacks}" && pwd)
SLICES=${SLICES:-"100:200 200:300 300:-1"}
LHAID4F=${LHAID4F:-320900}

# --- same generator settings as make_znunu_samples.sh (do not change one without the other) ---
case $SAMPLE in
  jets) PROC=proc_znunu_jets.dat; DIR=ZNuNu_jets_MET; PY=pythia8_card_znunu_jets.dat
        RUN="lhaid=303600 ickkw=1 xqcut=15 maxjetflavor=5 ptj=20 etaj=-1 drjj=0 ptb=0 etab=-1 drbb=0 drbj=0" ;;
  bb)   PROC=proc_znunu_bb.dat;   DIR=ZNuNu_bb4F_MET; PY=pythia8_card_znunu_bb4F.dat
        RUN="lhaid=$LHAID4F ickkw=1 xqcut=15 maxjetflavor=4 ptj=20 etaj=-1 drjj=0 ptb=0 etab=-1 drbb=0 drbj=0" ;;
  *) echo "unknown sample $SAMPLE (use jets or bb)"; exit 1 ;;
esac
COMMON="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf use_syst=False systematics_program=none cut_decays=True
        auto_ptj_mjj=False"
for f in "$PROC" "$PY" set_run_card.py; do
  [ -f "$HERE/$f" ] || { echo "ERROR: $HERE/$f not found; set HERE to the znunu_cards directory"; exit 1; }
done

# the 4F PDF must be visible to LHAPDF here and in the jobs (the LCG view points LHAPDF to the sets on cvmfs)
if [ "$SAMPLE" = bb ] && [ "$LHAID4F" = 320900 ]; then
  found=0
  for d in ${LHAPDF_DATA_PATH//:/ } $(lhapdf-config --datadir 2>/dev/null); do
    [ -d "$d/NNPDF31_nnlo_as_0118_nf_4" ] && found=1
  done
  [ $found = 1 ] || echo "WARNING: NNPDF31_nnlo_as_0118_nf_4 not found in LHAPDF_DATA_PATH / lhapdf-config --datadir;" \
                         "MadGraph may stop. Check with: lhapdf list --installed | grep nf_4"
fi

cd "$MG"
[ -d "$DIR" ] || ./bin/mg5_aMC "$HERE/$PROC"
cd "$DIR"
rm -f RunWeb                                       # stale lock left by an interrupted MadGraph run
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat   # decay is in the process
rm -f Cards/delphes_card.dat                       # Delphes runs in the batch jobs, not here

seed=$((5000 + RANDOM % 1000))
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=znunu_${SAMPLE}_MET${lo}to${hi/-1/inf}
  seed=$((seed + 1))
  GP_OUT="$GPDIR/${tag}_gridpack.tar.gz"; CMD_OUT="$GPDIR/${tag}_pythia8.cmd"

  # resume: skip what is already done (a gridpack tarball counts only if it is a complete archive)
  gp_ok=0
  if [ -s "$GP_OUT" ] && tar -tzf "$GP_OUT" >/dev/null 2>&1; then gp_ok=1; fi
  if [ "${FORCE:-0}" != 1 ] && [ $gp_ok = 1 ] && [ -s "$CMD_OUT" ]; then
    echo "=== $tag: gridpack and pythia8.cmd already exist, skipping (FORCE=1 to rebuild)"
    continue
  fi
  rm -rf "Events/gp_${tag}" "Events/cmd_${tag}"     # leftovers of an interrupted attempt

  # 1) gridpack: no shower (with -f, MadGraph runs Pythia8 only if Cards/pythia8_card.dat exists)
  rm -f Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat $COMMON $RUN misset=$lo missetmax=$hi iseed=$seed \
          gridpack=True nevents=10000
  if [ "${FORCE:-0}" != 1 ] && [ $gp_ok = 1 ]; then
    echo "=== $tag: gridpack exists, only making the pythia8.cmd"
  else
    echo "=== gridpack $tag"
    ./bin/generate_events "gp_$tag" -f --nb_core=$NCORE
    gp=$(ls -t ./*gp_${tag}*gridpack*.tar.gz 2>/dev/null | head -1)
    [ -n "$gp" ] || { echo "ERROR: no gridpack tarball found for $tag in $PWD"; exit 1; }
    mv "$gp" "$GP_OUT.part" && mv "$GP_OUT.part" "$GP_OUT"   # never leave a half-copied tarball
  fi

  # 2) 100-event normal run with Pythia8, only to keep the Pythia8 command file MadGraph writes
  cp "$HERE/$PY" Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat gridpack=False nevents=100 iseed=$((seed + 500))
  echo "=== pythia8 command file for $tag"
  ./bin/generate_events "cmd_$tag" -f --nb_core=$NCORE
  cmd=$(ls Events/cmd_${tag}/*pythia8.cmd 2>/dev/null | head -1)
  [ -n "$cmd" ] || { echo "ERROR: no pythia8 .cmd file in Events/cmd_${tag}"; exit 1; }
  cp "$cmd" "$CMD_OUT"
  rm -rf "Events/cmd_${tag}" "Events/gp_${tag}"
  echo "    -> $GP_OUT, $CMD_OUT"
done
echo "done. Test one job interactively before submitting:"
echo "  bash run_znunu_job.sh $GPDIR/znunu_${SAMPLE}_MET300toinf_gridpack.tar.gz $GPDIR/znunu_${SAMPLE}_MET300toinf_pythia8.cmd delphes_card.dat 200 1 /tmp/znunu_test $DIR"
