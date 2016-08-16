#!/bin/bash

EXPECTED_ARGS=2

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./`basename $0` Nevents Njobs"
    echo "Example: ./`basename $0` 100 50" 
    exit 1
fi

mv stepLHE.py stepLHE_original.py
mv crabConfig_stepLHE.py crabConfig_stepLHE_original.py
nevents=$1
njobs=$2

sed -e 's/.*NEventsPerJob=.*/NEventsPerJob='$nevents'/' stepLHE_original.py > stepLHE.py

sed -e 's/.*config\.Data\.unitsPerJob =.*/config\.Data\.unitsPerJob = '$nevents'/' -e 's/.*NJOBS =.*/NJOBS = '$njobs'/'  crabConfig_stepLHE_original.py > crabConfig_stepLHE.py

echo "Number of events per job is now "$nevents
echo "Number of jobs is now "$njobs

