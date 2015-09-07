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

if [ ! -e $dirname/step1.py ]; then
 echo $dirname/"step1.py does not exist!"
exit 1
fi


cd $dirname


export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`
filein=file_$index.lhe
echo "input file is" $filein
if [ ! -e $dirname/$filein ]; then
 echo $dirname/$filein " does not exist!"
exit 1
fi
fileout=step1_$index.root
echo "output file is" $fileout
rm -rf $fileout
cmsRun step1.py inputFiles=file:$filein outputFile=$fileout maxEvents=$maxevent
