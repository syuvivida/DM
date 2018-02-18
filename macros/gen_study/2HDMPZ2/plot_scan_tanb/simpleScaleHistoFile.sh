#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=5
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname string mh3 mh4 tb xsec"
    echo "Example: ./$scriptname gg 600 200 10 0.005"
    exit 1
fi

## produce cross section in the right format
## define a function

proc=$1
mh3=$2
mh4=$3
tbtemp=$4
tb=${tbtemp/./p}
xsec=$5

file=histo_gentuple_2HDMa_${proc}_tb_${tb}_MH3_${mh3}_MH4_${mh4}_MH2_${mh3}_MHC_${mh3}.root

if [ -f $file ]; then
    echo "File $file exists."
else
    echo "File $file does not exist."
    return 0
fi

echo "xsec = $xsec pb."
cd /afs/cern.ch/work/s/syu/CMSSW_7_1_23/src
export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`
cd -
root -q -b scaleHistos.C++\(\"$file\",$xsec\)
