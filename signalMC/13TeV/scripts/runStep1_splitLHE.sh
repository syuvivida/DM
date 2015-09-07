#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname workdirectory nEventsPerJob rootFileIndex"
    echo "Example: ./$scriptname $PWD 1000 1"
    exit 1
fi

dirname=$1
maxevent=$(( $2 ))
index=$3

if test $maxevent -lt 0
then
    echo "maxEvent must be greater than zero when you are running in splitting LHE mode"
    exit 1
fi


if [ ! -e $dirname/step1.py ]; then
 echo $dirname/"step1.py does not exist!"
exit 1
fi


cd $dirname

export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`
filein=Higgs_hzpzp_100GeV.lhe
echo "input file is" $filein
if [ ! -e $dirname/$filein ]; then
 echo $dirname/$filein " does not exist!"
exit 1
fi
fileout=step1_$index.root
skipevent=$(( $maxevent * ($index - 1) ))
echo "output file is" $fileout
echo "skiping " $skipevent " events"
rm -rf $fileout
cmsRun step1_splitLHE_higgsDecay.py inputFiles=file:$filein outputFile=$fileout skipEvents=$skipevent maxEvents=$maxevent
