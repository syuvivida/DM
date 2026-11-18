#!/bin/bash
# Write the job list and the HTCondor submit file for gridpack production, then submit.
#
# usage: bash submit_bkg.sh <samples> <n_jobs_per_slice> <events_per_job> [first_seed]
#   samples = one sample, a quoted list, or "all":   st_tw st_tch ttz tth zh wh zz wz
#   e.g. bash submit_bkg.sh st_tw 40 10000 1000              -> 40 jobs x 10k events in every MET slice of st_tw
#        bash submit_bkg.sh "ttz tth" 20 10000 1000
#        bash submit_bkg.sh all 20 10000 1000
# environment: MG, DELPHES, LCGVIEW (as for run_bkg_job.sh);
#              OUT (default root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/other_bkg),
#              SLICES (default per sample, see samples.sh; only sensible with a single sample),
#              FLAVOUR (default workday), GPDIR (default ./gridpacks),
#              DRYRUN=1 writes the job lists and the .sub files without submitting
# Seeds must be unique per slice: use a new first_seed when you add jobs later (the run name is ..._r<seed>).
# All gridpacks are checked first: if one is missing, nothing is submitted.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
# shellcheck disable=SC1091
source "$HERE/samples.sh"
SAMPLES=${1:?samples (or all)}; NJOB=${2:?jobs per slice}; NEV=${3:?events per job}; SEED0=${4:-1000}
[ "$SAMPLES" = all ] && SAMPLES=$ALL_SAMPLES
GPDIR=$(cd "${GPDIR:-$HERE/gridpacks}" && pwd)
OUT=${OUT:-root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/other_bkg}
FLAVOUR=${FLAVOUR:-workday}
USERSLICES=$SLICES
: "${MG:?set MG}"
[ -f "$HERE/delphes_card.dat" ] || { echo "missing $HERE/delphes_card.dat"; exit 1; }

missing=0
for sample in $SAMPLES; do
  sample_config "$sample"
  for s in ${USERSLICES:-$DEFSLICES}; do
    lo=${s%%:*}; hi=${s##*:}; tag=${sample}_MET${lo}to${hi/-1/inf}
    for f in "$GPDIR/${tag}_gridpack.tar.gz" "$GPDIR/${tag}_pythia8.cmd"; do
      [ -s "$f" ] || { echo "missing $f"; missing=1; }
    done
  done
done
[ $missing = 0 ] || { echo "nothing submitted: build the missing gridpacks first (make_bkg_gridpacks.sh / gridpack_bkg.sub)"; exit 1; }

mkdir -p "$HERE/logs"
for sample in $SAMPLES; do
  sample_config "$sample"
  LIST=$HERE/jobs_${sample}_s${SEED0}.txt
  : > "$LIST"
  for s in ${USERSLICES:-$DEFSLICES}; do
    lo=${s%%:*}; hi=${s##*:}; tag=${sample}_MET${lo}to${hi/-1/inf}
    for i in $(seq 0 $((NJOB - 1))); do
      echo "$GPDIR/${tag}_gridpack.tar.gz $GPDIR/${tag}_pythia8.cmd $((SEED0 + i))" >> "$LIST"
    done
  done
  cat > "$HERE/bkg_${sample}.sub" <<EOS
executable            = $HERE/run_bkg_job.sh
arguments             = \$(gridpack) \$(cmd) $HERE/delphes_card.dat $NEV \$(seed) $OUT $DIR
environment           = "MG=$MG DELPHES=${DELPHES:-$MG/Delphes} LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}"
output                = $HERE/logs/\$(ClusterId).\$(ProcId).out
error                 = $HERE/logs/\$(ClusterId).\$(ProcId).err
log                   = $HERE/logs/\$(ClusterId).log
request_cpus          = 1
+JobFlavour           = "$FLAVOUR"
MY.SendCredential     = True
max_retries           = 2
queue gridpack, cmd, seed from $LIST
EOS
  echo "=== $sample: $(wc -l < "$LIST") jobs ($NJOB per slice x $NEV events), output to $OUT/$DIR/Events/"
  if [ "${DRYRUN:-0}" = 1 ]; then echo "    DRYRUN: wrote bkg_${sample}.sub and $(basename "$LIST"), not submitted"; continue; fi
  condor_submit "$HERE/bkg_${sample}.sub"
done
