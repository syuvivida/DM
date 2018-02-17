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

#SINPARRAY=(0p1 0p2 0p3 0p35 0p4 0p5 0p6 0p7 0p8 0p9)
SINPARRAY=(0p1 0p35 0p5 0p7 0p9)
#SINPARRAY=(0p1 0p2 0p35 0p4 0p5 0p6 0p7 0p8 0p9)
inputtextfile=${name}\_${header}.txt
rm -rf $inputtextfile

for ((i=0; i < ${#SINPARRAY[@]}; i++))
do
  sinp=${SINPARRAY[$i]}
  filename=/afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_26_patch1/src/scan_sinp_rootfiles/output2/histo_gentuple_2HDMa_gg_sinp_${sinp}_tanb_1_mXd_10_MH3_${mmed}_MH4_${ma0}_MH2_${mmed}_MHC_${mmed}.root
  if [ -f $filename ]; 
  then  
      sinvalue=${sinp/p/.}
      echo $sinvalue
      legend="sin(#it{#theta})="$sinvalue
      echo $filename $legend >> $inputtextfile
  fi
done

cd /afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_26_patch1/src
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cd $PRODHOME

root -q -b dumpCompareMultiplePDF.C++\(\"$inputtextfile\",$mmed,$ma0\)

