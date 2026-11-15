#!/bin/bash
# Write the job list and the HTCondor submit file for gridpack production, then submit.
#
# usage: bash submit_ttbar.sh <sample> <n_jobs_per_slice> <events_per_job> [first_seed]
#   e.g. bash submit_ttbar.sh semilep 40 10000 1000      -> 40 jobs x 10k events in every MET slice
# environment: MG, DELPHES, LCGVIEW (as for run_ttbar_job.sh); OUT (default root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/ttbar),
#              SLICES (default "100:200 200:300 300:-1"), FLAVOUR (default workday), GPDIR (default ./gridpacks)
# Seeds must be unique per slice: use a new first_seed when you add jobs later (the run name is ..._r<seed>).
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
SAMPLE=${1:?semilep, dilep or ttbb}; NJOB=${2:?jobs per slice}; NEV=${3:?events per job}; SEED0=${4:-1000}
GPDIR=$(cd "${GPDIR:-$HERE/gridpacks}" && pwd)
OUT=${OUT:-root://eosuser.cern.ch//eos/user/${USER:0:1}/$USER/ttbar}
SLICES=${SLICES:-"100:200 200:300 300:-1"}
FLAVOUR=${FLAVOUR:-workday}
: "${MG:?set MG}"
case $SAMPLE in semilep) PROCDIR=TT_semilep_MET ;; dilep) PROCDIR=TT_dilep_MET ;; ttbb) PROCDIR=TTbb_lep_MET ;; *) exit 1 ;; esac

mkdir -p "$HERE/logs"
LIST=$HERE/jobs_${SAMPLE}_s${SEED0}.txt
: > "$LIST"
for s in $SLICES; do
  lo=${s%%:*}; hi=${s##*:}; tag=${SAMPLE}_MET${lo}to${hi/-1/inf}
  [ -f "$GPDIR/${tag}_gridpack.tar.gz" ] || { echo "missing $GPDIR/${tag}_gridpack.tar.gz"; exit 1; }
  for i in $(seq 0 $((NJOB - 1))); do
    echo "$GPDIR/${tag}_gridpack.tar.gz $GPDIR/${tag}_pythia8.cmd $((SEED0 + i))" >> "$LIST"
  done
done

cat > "$HERE/ttbar_${SAMPLE}.sub" <<EOF
executable            = $HERE/run_ttbar_job.sh
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
EOF
echo "$(wc -l < "$LIST") jobs: $NJOB per slice x $NEV events, output to $OUT/$PROCDIR/Events/"
condor_submit "$HERE/ttbar_${SAMPLE}.sub"
