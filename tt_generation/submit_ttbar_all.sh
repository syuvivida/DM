#!/bin/bash
# Submit the gridpack production of all three tt~ samples (semilep, dilep, ttbb) in one go.
# It calls submit_ttbar.sh once per sample, so each sample gets its own job list, submit file and cluster.
#
# usage: bash submit_ttbar_all.sh <n_jobs_per_slice> <events_per_job> [first_seed]
#   e.g. bash submit_ttbar_all.sh 40 10000 1000     -> 40 jobs x 10k events per MET slice, for each sample
# environment: the same as for submit_ttbar.sh (MG, DELPHES, LCGVIEW, OUT, SLICES, FLAVOUR, GPDIR), plus
#   SAMPLES  samples to submit (default "semilep dilep ttbb")
# All gridpacks are checked first: if one is missing, nothing is submitted.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
NJOB=${1:?jobs per slice}; NEV=${2:?events per job}; SEED0=${3:-1000}
SAMPLES=${SAMPLES:-"semilep dilep ttbb"}
SLICES=${SLICES:-"100:200 200:300 300:-1"}
GPDIR=${GPDIR:-$HERE/gridpacks}
: "${MG:?set MG}"

missing=0
for sample in $SAMPLES; do
  for s in $SLICES; do
    lo=${s%%:*}; hi=${s##*:}; tag=${sample}_MET${lo}to${hi/-1/inf}
    for f in "$GPDIR/${tag}_gridpack.tar.gz" "$GPDIR/${tag}_pythia8.cmd"; do
      [ -s "$f" ] || { echo "missing $f"; missing=1; }
    done
  done
done
[ $missing = 0 ] || { echo "nothing submitted: build the missing gridpacks first (condor_submit gridpack_ttbar.sub)"; exit 1; }

for sample in $SAMPLES; do
  echo "=== $sample"
  SLICES="$SLICES" GPDIR="$GPDIR" bash "$HERE/submit_ttbar.sh" "$sample" "$NJOB" "$NEV" "$SEED0"
done
