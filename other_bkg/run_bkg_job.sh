#!/bin/bash
# One batch job: gridpack -> LHE -> Pythia8 (MadGraph's own MG5aMC_PY8_interface, MLM matching) -> Delphes -> EOS.
#
# usage: bash run_bkg_job.sh <gridpack.tar.gz> <pythia8.cmd> <delphes_card> <nevents> <seed> <outdir> <procdir>
#   outdir   final location, e.g. root://eosuser.cern.ch//eos/user/s/syu/other_bkg  (or a local path for tests)
#   procdir  folder name used in the output tree, e.g. ST_tW_MET (the DIR column of samples.sh)
# The output follows the MadGraph layout that hhmet_sensitivity.py reads:
#   <outdir>/<procdir>/Events/<sample>_MET<lo>to<hi>_r<seed>/tag_1_delphes_events.root
#   <outdir>/<procdir>/Events/<sample>_MET<lo>to<hi>_r<seed>/<run>_tag_1_banner.txt
# The banner holds the MadGraph cross section of the LHE file ("Integrated weight") and the cross section to use
# for the normalisation ("Matched Integrated weight", the line hhmet_sensitivity.py reads):
#   sigma_LHE x (events after matching) / (LHE events) x BR
# BR = 0.5824 for the samples in which Pythia8 forces H -> b b~ (the command file contains "25:onMode = off"),
# because Pythia8 does not rescale the cross section for a forced decay; BR = 1 otherwise. Override with BRFACTOR.
#
# environment: MG (MadGraph directory with HEPTools/MG5aMC_PY8_interface), DELPHES (Delphes directory),
#              LCGVIEW (same view as for building the gridpack and Delphes), BRFACTOR (optional, see above)
set -e
# Run in a clean environment if called from a shell with a CMSSW environment (cmsenv): CMSSW's ROOT libraries
# and dictionaries would otherwise be mixed with the LCG view's ROOT (Delphes "symbol lookup error").
# Batch jobs (getenv = False) never have it; this only matters for interactive tests.
if [ -n "$CMSSW_BASE" ] && [ -z "$_CLEAN_ENV" ]; then
  keep=(HOME="$HOME" USER="$USER" PATH=/usr/bin:/bin _CLEAN_ENV=1)
  for v in MG DELPHES LCGVIEW PY8_TIMEOUT KRB5CCNAME X509_USER_PROXY _CONDOR_SCRATCH_DIR TMPDIR; do
    [ -n "${!v}" ] && keep+=("$v=${!v}")
  done
  exec env -i "${keep[@]}" bash "$0" "$@"
fi
GP=$(readlink -f "$1"); CMD=$(readlink -f "$2"); CARD=$(readlink -f "$3")
NEV=$4; SEED=$5; OUT=$6; PROCDIR=$7
LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}
# shellcheck disable=SC1090
source "$LCGVIEW"
MG=${MG:?set MG}; DELPHES=${DELPHES:-$MG/Delphes}
PY8=$MG/HEPTools/MG5aMC_PY8_interface/MG5aMC_PY8_interface
[ -x "$PY8" ] || { echo "ERROR: $PY8 not found (install pythia8 in MadGraph on lxplus)"; exit 1; }
[ -x "$DELPHES/DelphesHepMC2" ] || { echo "ERROR: $DELPHES/DelphesHepMC2 not found"; exit 1; }

TAG=$(basename "$GP" _gridpack.tar.gz)            # e.g. st_tw_MET300toinf
RUN=${TAG}_r${SEED}
WORK=${_CONDOR_SCRATCH_DIR:-$(mktemp -d)}/job_$RUN
mkdir -p "$WORK" && cd "$WORK"
echo "=== $RUN on $(hostname) in $WORK"

# 1) LHE events from the gridpack
mkdir gp && tar -xzf "$GP" -C gp
( cd gp && ./run.sh "$NEV" "$SEED" ) > gridpack.log 2>&1 || { tail -30 gridpack.log; exit 1; }
LHE=$(ls gp/events.lhe.gz gp/Events/*/events.lhe.gz gp/Events/*/unweighted_events.lhe.gz 2>/dev/null | head -1)
[ -n "$LHE" ] || { echo "ERROR: no LHE file from the gridpack"; tail -30 gridpack.log; exit 1; }
gunzip -c "$LHE" > events.lhe
NLHE=$(grep -c "<event" events.lhe)
XSEC=$(grep -m1 "Integrated weight (pb)" events.lhe | awk -F: '{print $2}' | tr -d ' ')
[ -n "$XSEC" ] || XSEC=$(awk '/<init>/{getline; getline; print $1; exit}' events.lhe)
echo "LHE: $NLHE events, sigma = $XSEC pb"

# 2) Pythia8 with the command file MadGraph wrote for the same cards (only file names, event count, seed change)
sed -e "s#^ *Beams:LHEF *=.*#Beams:LHEF = $WORK/events.lhe#" \
    -e "s#^ *HEPMCoutput:file *=.*#HEPMCoutput:file = $WORK/events.hepmc#" \
    -e "s#^ *Main:numberOfEvents *=.*#Main:numberOfEvents = $NLHE#" \
    -e "/^ *Random:seed *=/d" -e "/^ *Random:setSeed *=/d" "$CMD" > pythia8.cmd
printf "Random:setSeed = on\nRandom:seed = %d\n" $((SEED % 900000000)) >> pythia8.cmd
grep -q "^ *Beams:LHEF" pythia8.cmd || echo "Beams:LHEF = $WORK/events.lhe" >> pythia8.cmd
grep -q "^ *HEPMCoutput:file" pythia8.cmd || echo "HEPMCoutput:file = $WORK/events.hepmc" >> pythia8.cmd
grep -q "^ *Main:numberOfEvents" pythia8.cmd || echo "Main:numberOfEvents = $NLHE" >> pythia8.cmd
# Pythia8 through MadGraph's interface, with the same library path MadGraph itself sets (HEPTools/*/lib).
# The interface ignores a failed pythia.init() and then loops forever on "not properly initialized":
# watch the log and stop at once in that case; PY8_TIMEOUT (default 20 h) limits the total time.
HEPLIB=$(ls -d $MG/HEPTools/lib $MG/HEPTools/*/lib 2>/dev/null | tr '\n' ':')
( export LD_LIBRARY_PATH=$HEPLIB$LD_LIBRARY_PATH; exec "$PY8" pythia8.cmd ) > pythia8.log 2>&1 &
PYPID=$!; T0=$(date +%s)
while kill -0 $PYPID 2>/dev/null; do
  sleep 20
  if grep -q -m1 -E "not properly initialized|Abort from Pythia::init|Error in Pythia::init" pythia8.log; then
    kill $PYPID 2>/dev/null; echo "ERROR: Pythia8 initialisation failed:"
    grep -m15 -i -E "error|abort|warning" pythia8.log; exit 1
  fi
  if [ $(( $(date +%s) - T0 )) -gt ${PY8_TIMEOUT:-72000} ]; then
    kill $PYPID 2>/dev/null; echo "ERROR: Pythia8 timeout"; tail -40 pythia8.log; exit 1
  fi
done
wait $PYPID || { tail -40 pythia8.log; exit 1; }
echo "Pythia8: $(( $(date +%s) - T0 )) s"
[ -s events.hepmc ] || { echo "ERROR: Pythia8 wrote no HepMC file"; tail -40 pythia8.log; exit 1; }

# 3) Delphes
"$DELPHES/DelphesHepMC2" "$CARD" tag_1_delphes_events.root events.hepmc > delphes.log 2>&1 || { tail -30 delphes.log; exit 1; }
NOUT=$(root -l -b -q -e 'TFile f("tag_1_delphes_events.root"); TTree* t=(TTree*)f.Get("Delphes"); printf("NEVT %lld\n", t ? t->GetEntries() : -1LL);' 2>/dev/null | awk '/NEVT/{print $2}')
if [ -z "$BRFACTOR" ]; then
  if grep -qiE "^ *25:onMode *= *off" "$CMD"; then BRFACTOR=0.5824; else BRFACTOR=1; fi
fi
MATCHED=$(awk -v x="$XSEC" -v a="$NOUT" -v b="$NLHE" -v br="$BRFACTOR" 'BEGIN{printf "%.6g", x*a/b*br}')
echo "after matching: $NOUT events, BR factor $BRFACTOR, sigma for normalisation = $MATCHED pb"

# 4) banner (read by hhmet_sensitivity.py) and copy-out
cat > ${RUN}_tag_1_banner.txt <<EOB
# gridpack production: $(basename "$GP"), seed $SEED, $(date)
#  Number of Events        :       $NLHE
#  Integrated weight (pb)  :       $XSEC
#  Matched Integrated weight (pb)  :       $MATCHED
#  (= Integrated weight x events after matching / number of events x BR factor)
#  BR factor (forced decays)  :       $BRFACTOR
#  Events after matching   :       $NOUT
EOB
cat pythia8.cmd >> ${RUN}_tag_1_banner.txt
DEST=$OUT/$PROCDIR/Events/$RUN
if [[ "$OUT" == root://* ]]; then
  for f in tag_1_delphes_events.root ${RUN}_tag_1_banner.txt pythia8.log; do xrdcp -f -p "$f" "$DEST/$f"; done
else
  mkdir -p "$DEST" && cp tag_1_delphes_events.root ${RUN}_tag_1_banner.txt pythia8.log "$DEST/"
fi
echo "=== done: $DEST"
rm -rf "$WORK"
