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
maxevent=$2
index=$3

if [ ! -e $dirname/step3.py ]; then
 echo $dirname/"step3.py does not exist!"
exit 1
fi


cd $dirname

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
filein=step2_$index.root
if [ ! -e $dirname/$filein ]; then
    echo $dirname/$filein " does not exist!"
    filein=step2_${index}_numEvent${maxevent}.root
    if [ ! -e $dirname/$filein ]; then
        echo $dirname/$filein " does not exist!"
        exit 1
    fi
fi

echo "input file is" $filein

fileout=step3_$index.root
echo "output RECO file is " $fileout
minifileout=step3_miniAOD_$index.root
echo "output miniAOD file is" $minifileout
rm -rf $fileout
rm -rf $minifileout
cmsRun step3.py inputFiles=file:$filein outputFile=$fileout myOutputFile=$minifileout maxEvents=$maxevent
