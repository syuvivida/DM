#!/bin/bash
# Write the job list and the HTCondor submit file for gridpack production, then submit.
#
# usage: bash submit_wlnu.sh <sample> <n_jobs_per_slice> <events_per_job> [first_seed]
#   sample = jets | bb
#   e.g. bash submit_wlnu.sh bb 40 10000 1000      -> 40 jobs x 10k events in every MET slice
# environment: MG, DELPHES, LCGVIEW (as for run_wlnu_job.sh);
#              OUT (default root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/wlnu),
#              SLICES (default "50:150 150:250 250:-1"), FLAVOUR (default workday), GPDIR (default ./gridpacks),
#              DRYRUN=1 writes the job list and the .sub file without submitting
# Seeds must be unique per slice: use a new first_seed when you add jobs later (the run name is ..._r<seed>).
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
SAMPLE=${1:?jets or bb}; NJOB=${2:?jobs per slice}; NEV=${3:?events per job}; SEED0=${4:-1000}
GPDIR=$(cd "${GPDIR:-$HERE/gridpacks}" && pwd)
OUT=${OUT:-root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/wlnu}
SLICES=${SLICES:-"50:150 150:250 250:-1"}
FLAVOUR=${FLAVOUR:-workday}
: "${MG:?set MG}"
case $SAMPLE in jets) PROCDIR=WLNu_jets_MET ;; bb) PROCDIR=WLNu_bb4F_MET ;; *) echo "use jets or bb"; exit 1 ;; esac

mkdir -p "$HERE/logs"
LIST=$HERE/jobs_wlnu_${SAMPLE}_s${SEED0}.txt
: > "$LIST"
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=wlnu_${SAMPLE}_MET${lo}to${hi/-1/inf}
  for f in "$GPDIR/${tag}_gridpack.tar.gz" "$GPDIR/${tag}_pythia8.cmd"; do
    [ -f "$f" ] || { echo "missing $f (run make_wlnu_gridpacks.sh $SAMPLE first)"; exit 1; }
  done
  for i in $(seq 0 $((NJOB - 1))); do
    echo "$GPDIR/${tag}_gridpack.tar.gz $GPDIR/${tag}_pythia8.cmd $((SEED0 + i))" >> "$LIST"
  done
done

cat > "$HERE/wlnu_${SAMPLE}.sub" <<EOS
executable            = $HERE/run_wlnu_job.sh
arguments             = \$(gridpack) \$(cmd) $HERE/delphes_card.dat $NEV \$(seed) $OUT $PROCDIR
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
echo "$(wc -l < "$LIST") jobs: $NJOB per slice x $NEV events, output to $OUT/$PROCDIR/Events/"
if [ "${DRYRUN:-0}" = 1 ]; then echo "DRYRUN: wrote $HERE/wlnu_${SAMPLE}.sub and $LIST, not submitted"; exit 0; fi
condor_submit "$HERE/wlnu_${SAMPLE}.sub"
