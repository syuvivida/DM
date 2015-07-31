#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=2
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname workdirectory rootFileIndex"
echo "Example: ./$scriptname $PWD 1"
exit 1
fi

if [ ! -e $1/step2.py ]; then
 echo $1/"step2.py does not exist!"
exit 1
fi


cd $1

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
export X509_USER_PROXY=$HOME/private/grid.proxy
filein=step1_$2.root
echo "input file is" $filein
if [ ! -e $1/$filein ]; then
 echo $1/$filein " does not exist!"
exit 1
fi
fileout=step2_$2.root
echo "output file is" $fileout
rm -rf $fileout
cmsRun step2.py inputFiles=file:$filein outputFile=$fileout
