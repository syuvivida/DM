#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=2
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname workdirectory rootFileIndex"
echo "Example: ./$scriptname $PWD 1"
exit 1
fi

if [ ! -e $1/step3.py ]; then
 echo $1/"step3.py does not exist!"
exit 1
fi


cd $1

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
filein=step2_$2.root
echo "input file is" $filein
if [ ! -e $1/$filein ]; then
 echo $1/$filein " does not exist!"
exit 1
fi
fileout=step3_$2.root
echo "output RECO file is " $fileout
minifileout=step3_miniAOD_$2.root
echo "output miniAOD file is" $minifileout
rm -rf $fileout
rm -rf $minifileout
cmsRun step3.py inputFiles=file:$filein outputFile=$fileout myOutputFile=$minifileout
