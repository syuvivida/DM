#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=2
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname mA ma"
    echo "Example: ./$scriptname 500 100"
    exit 1
fi

export PRODHOME=`pwd`
name=2HDMa
mmed=$1
ma0=$2

echo ""
echo "Producing input text file for A mass = "$mmed" GeV"
echo "Producing input text file for a mass = "$ma0" GeV "
echo ""
header=MA${mmed}\_Ma${ma0}

## Now produce input text file for comparison

TANBARRAY=(0p5 1p0 2p0 4p0 6p0 8p0 10p0)
inputtextfile=${name}\_${header}.txt
rm -rf $inputtextfile

for ((i=0; i < ${#TANBARRAY[@]}; i++))
do
  tanb=${TANBARRAY[$i]}
  filename=/afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_26_patch1/src/scan_tb_ma_rootfiles/output2/combinedHistos/combined_histo_2HDMa_tb_${tanb}_MH3_${mmed}_MH4_${ma0}_MH2_${mmed}_MHC_${mmed}.root
  if [ -f $filename ]; 
  then  
      tbvalue=${tanb/p/.}
      echo $tbvalue
      legend="tan(#it{#beta})="$tbvalue
      echo $filename $legend >> $inputtextfile
  fi
done

cd /afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_26_patch1/src
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cd $PRODHOME

root -q -b dumpCompareMultiplePDF.C++\(\"$inputtextfile\",$mmed,$ma0\)

