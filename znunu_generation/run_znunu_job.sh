#!/bin/bash
# One batch job: gridpack -> LHE -> Pythia8 (MadGraph's own MG5aMC_PY8_interface, MLM matching) -> Delphes -> EOS.
#
# usage: bash run_znunu_job.sh <gridpack.tar.gz> <pythia8.cmd> <delphes_card> <nevents> <seed> <outdir> <procdir>
#   outdir   final location, e.g. root://eosuser.cern.ch//eos/user/s/syu/znunu  (or a local path for tests)
#   procdir  folder name used in the output tree: ZNuNu_jets_MET or ZNuNu_bb4F_MET
# The output follows the MadGraph layout that hhmet_sensitivity.py reads:
#   <outdir>/<procdir>/Events/znunu_<sample>_MET<lo>to<hi>_r<seed>/tag_1_delphes_events.root
#   <outdir>/<procdir>/Events/znunu_<sample>_MET<lo>to<hi>_r<seed>/<run>_tag_1_banner.txt
# The banner holds the MadGraph cross section of the LHE file ("Integrated weight") and the matched cross
# section of this job = sigma_LHE x (events after matching) / (LHE events), as for the interactive samples.
#
# environment: MG (MadGraph directory with HEPTools/MG5aMC_PY8_interface), DELPHES (Delphes directory),
#              LCGVIEW (same view as for building the gridpack and Delphes)
set -e
GP=$(readlink -f "$1"); CMD=$(readlink -f "$2"); CARD=$(readlink -f "$3")
NEV=$4; SEED=$5; OUT=$6; PROCDIR=$7
LCGVIEW=${LCGVIEW:-/cvmfs/sft.cern.ch/lcg/views/LCG_110a/x86_64-el9-gcc14-opt/setup.sh}
# shellcheck disable=SC1090
source "$LCGVIEW"
MG=${MG:?set MG}; DELPHES=${DELPHES:-$MG/Delphes}
PY8=$MG/HEPTools/MG5aMC_PY8_interface/MG5aMC_PY8_interface
[ -x "$PY8" ] || { echo "ERROR: $PY8 not found (install pythia8 in MadGraph on lxplus)"; exit 1; }
[ -x "$DELPHES/DelphesHepMC2" ] || { echo "ERROR: $DELPHES/DelphesHepMC2 not found"; exit 1; }

TAG=$(basename "$GP" _gridpack.tar.gz)            # e.g. znunu_bb_MET300toinf
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
    -e "s#^ *Main:numberOfEvents *=.*#Main:numberOfEvents = -1#" \
    -e "/^ *Random:seed *=/d" -e "/^ *Random:setSeed *=/d" "$CMD" > pythia8.cmd
printf "Random:setSeed = on\nRandom:seed = %d\n" $((SEED % 900000000)) >> pythia8.cmd
grep -q "^ *Beams:LHEF" pythia8.cmd || echo "Beams:LHEF = $WORK/events.lhe" >> pythia8.cmd
grep -q "^ *HEPMCoutput:file" pythia8.cmd || echo "HEPMCoutput:file = $WORK/events.hepmc" >> pythia8.cmd
"$PY8" pythia8.cmd > pythia8.log 2>&1 || { tail -40 pythia8.log; exit 1; }
[ -s events.hepmc ] || { echo "ERROR: Pythia8 wrote no HepMC file"; tail -40 pythia8.log; exit 1; }

# 3) Delphes
"$DELPHES/DelphesHepMC2" "$CARD" tag_1_delphes_events.root events.hepmc > delphes.log 2>&1 || { tail -30 delphes.log; exit 1; }
NOUT=$(root -l -b -q -e 'TFile f("tag_1_delphes_events.root"); TTree* t=(TTree*)f.Get("Delphes"); printf("NEVT %lld\n", t ? t->GetEntries() : -1LL);' 2>/dev/null | awk '/NEVT/{print $2}')
MATCHED=$(awk -v x="$XSEC" -v a="$NOUT" -v b="$NLHE" 'BEGIN{printf "%.6g", x*a/b}')
echo "after matching: $NOUT events, matched sigma = $MATCHED pb"

# 4) banner (read by hhmet_sensitivity.py) and copy-out
cat > ${RUN}_tag_1_banner.txt <<EOB
# gridpack production: $(basename "$GP"), seed $SEED, $(date)
#  Number of Events        :       $NLHE
#  Integrated weight (pb)  :       $XSEC
#  Matched Integrated weight (pb)  :       $MATCHED
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
