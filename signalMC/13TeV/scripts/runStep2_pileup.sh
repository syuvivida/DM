#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname workdirectory nEventPerJob rootFileIndex"
echo "Example: ./$scriptname $PWD 1000 1"
exit 1
fi

dirname=$1
maxevent=$2
index=$3

if [ ! -e $dirname/step2_pileup25ns.py ]; then
 echo $dirname/"step2_pileup25ns.py does not exist!"
exit 1
fi


cd $dirname

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
export X509_USER_PROXY=$HOME/private/grid.proxy
filein=root://eoscms//eos/cms/store/user/khurana/MonoHStep1/step1_M600_$index.root
echo "input file is" $filein
fileout=step2_$index.root
echo "output file is" $fileout
rm -rf $fileout
cmsRun step2_pileup25ns.py inputFiles=$filein outputFile=$fileout mixHistFile=histProbFunction_$index.root maxEvents=$maxevent
