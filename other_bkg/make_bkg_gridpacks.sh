#!/bin/bash
# Build MadGraph gridpacks for the smaller backgrounds of the 4b + MET search: single top (tW, t-channel),
# tt~Z(nu nu), tt~H(bb), ZH, WH, ZZ, WZ. One gridpack per sample and MET slice, for production on the CERN batch
# system with run_bkg_job.sh. RUN THIS ON LXPLUS (EL9), not on the Mac: a gridpack contains compiled code and
# only runs on the system it was built on.
#
# usage:  bash make_bkg_gridpacks.sh <sample> [gridpack_dir]
#         sample = st_tw | st_tch | ttz | tth | zh | wh | zz | wz      (see samples.sh)
# environment:
#   MG       MadGraph directory on lxplus (needs Pythia8 installed in it: "install pythia8" in mg5_aMC)
#   LCGVIEW  LCG view to source (default LCG_110a, el9, gcc14); use the same one in the batch jobs
#   SLICES   parton-level MET slices "lo:hi" (hi = -1: no upper limit); default per sample, see samples.sh
#   HERE     directory holding the cards, samples.sh and set_run_card.py (default: the script's directory).
#            MUST be set in HTCondor jobs: condor copies the script to a scratch directory without the cards.
#   NCORE    cores MadGraph may use (default: OMP_NUM_THREADS, which HTCondor sets to request_cpus, else 4)
#   FORCE    set FORCE=1 to rebuild slices whose outputs already exist (default: skip them, so an
#            interrupted job can simply be rerun and continues with the missing slices)
#
# For each slice it writes into <gridpack_dir> (default <HERE>/gridpacks):
#   <sample>_MET<lo>to<hi>_gridpack.tar.gz   the gridpack (LHE generation only)
#   <sample>_MET<lo>to<hi>_pythia8.cmd       the Pythia8 command file MadGraph itself writes for these cards
#                                            (made with a 100-event normal run); the batch jobs reuse it
set -e
LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}
# shellcheck disable=SC1090
source "$LCGVIEW"
MG=${MG:?set MG to your MadGraph directory on lxplus}
HERE=${HERE:-$(cd "$(dirname "$0")" && pwd)}
NCORE=${NCORE:-${OMP_NUM_THREADS:-4}}
SAMPLE=${1:?give a sample: st_tw st_tch ttz tth zh wh zz wz}
[ -f "$HERE/samples.sh" ] || { echo "ERROR: $HERE/samples.sh not found; set HERE to the other_bkg_cards directory"; exit 1; }
# shellcheck disable=SC1091
source "$HERE/samples.sh"
sample_config "$SAMPLE"
GPDIR=$(mkdir -p "${2:-$HERE/gridpacks}" && cd "${2:-$HERE/gridpacks}" && pwd)
SLICES=${SLICES:-$DEFSLICES}

# --- generator settings ---
# All standard cuts are switched off: with cut_decays = True (needed for the MET cut on the neutrinos) they would
# also act on the decay products (b quarks from top and Z decays, quarks from W decays).
RUN="ebeam1=6800 ebeam2=6800 pdlabel=lhapdf lhaid=303600 use_syst=False systematics_program=none
     cut_decays=True auto_ptj_mjj=False maxjetflavor=5
     ptj=0 etaj=-1 drjj=0 mmjj=0 ptb=0 etab=-1 drbb=0 drbj=0 mmbb=0 ptl=0 etal=-1 drll=0 drjl=0 drbl=0 mmll=0"
if [ "$MLM" = 1 ]; then RUN="$RUN ickkw=1 xqcut=15"; else RUN="$RUN ickkw=0 xqcut=0"; fi
for f in "$PROC" set_run_card.py pythia8_cp5.dat pythia8_mlm.dat pythia8_hbb.dat; do
  [ -f "$HERE/$f" ] || { echo "ERROR: $HERE/$f not found; set HERE to the other_bkg_cards directory"; exit 1; }
done

# Pythia8 card of this sample: CP5 tune (+ MLM matching) (+ forced H -> b b~)
PYCARD=$(mktemp)
cat "$HERE/pythia8_cp5.dat" > "$PYCARD"
[ "$MLM" = 1 ] && cat "$HERE/pythia8_mlm.dat" >> "$PYCARD"
[ "$HBB" = 1 ] && cat "$HERE/pythia8_hbb.dat" >> "$PYCARD"

cd "$MG"
[ -d "$DIR" ] || ./bin/mg5_aMC "$HERE/$PROC"
cd "$DIR"
rm -f RunWeb                                       # stale lock left by an interrupted MadGraph run
[ -f Cards/madspin_card.dat ] && mv Cards/madspin_card.dat Cards/madspin_card_unused.dat   # no MadSpin
rm -f Cards/delphes_card.dat                       # Delphes runs in the batch jobs, not here

seed=$((8000 + RANDOM % 1000))
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=${SAMPLE}_MET${lo}to${hi/-1/inf}
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
  python3 "$HERE/set_run_card.py" Cards/run_card.dat $RUN misset=$lo missetmax=$hi iseed=$seed \
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
  cp "$PYCARD" Cards/pythia8_card.dat
  python3 "$HERE/set_run_card.py" Cards/run_card.dat gridpack=False nevents=100 iseed=$((seed + 500))
  echo "=== pythia8 command file for $tag"
  ./bin/generate_events "cmd_$tag" -f --nb_core=$NCORE
  cmd=$(ls Events/cmd_${tag}/*pythia8.cmd 2>/dev/null | head -1)
  [ -n "$cmd" ] || { echo "ERROR: no pythia8 .cmd file in Events/cmd_${tag}"; exit 1; }
  if [ "$HBB" = 1 ] && ! grep -qiE "^ *25:onMode *= *off" "$cmd"; then
    echo "ERROR: the forced H -> b b~ lines are missing in $cmd"; exit 1
  fi
  cp "$cmd" "$CMD_OUT"
  rm -rf "Events/cmd_${tag}" "Events/gp_${tag}"
  echo "    -> $GP_OUT, $CMD_OUT"
done
rm -f "$PYCARD"
first=${SLICES%% *}; tag1=${SAMPLE}_MET${first%%:*}to$(echo "${first##*:}" | sed 's/^-1$/inf/')
echo "done. Test one job interactively before submitting:"
echo "  bash run_bkg_job.sh $GPDIR/${tag1}_gridpack.tar.gz $GPDIR/${tag1}_pythia8.cmd delphes_card.dat 200 1 /tmp/bkg_test $DIR"
