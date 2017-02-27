#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname processName mA ma mDM"
    echo "Example: ./$scriptname 2HDMPZ2 500 100 1"
    exit 1
fi

export PRODHOME=`pwd`
name=$1
mmed=$2
ma0=$3
mdm=$4

echo ""
echo "Producing input text file for A mass = "$mmed" GeV"
echo "Producing input text file for a mass = "$ma0" GeV "
echo "Producing input text file for DM mass = "$mdm" GeV "
echo ""
header=MA${mmed}\_Ma${ma0}\_MDM${mdm}

## Now produce input text file for comparison

SINPARRAY=(0p5000 0p7071 0p8660)
inputtextfile=${name}\_${header}.txt
rm -rf $inputtextfile

for ((i=0; i < ${#SINPARRAY[@]}; i++))
do
  sinp=${SINPARRAY[$i]}
  filename=/data7/monoH/monoH_genOnly/2HDMPZ2/MA500_MDM1/output/histo\_2HDMPZ2II\_${sinp}\_MA${mmed}\_Ma${ma0}\_MDM${mdm}.root
  sinvalue=${sinp/p/.}
  echo $sinvalue
  legend="sin#theta="$sinvalue
  echo $filename $legend >> $inputtextfile
done

cd /afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_20/src
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cd $PRODHOME

root -q -b /afs/cern.ch/user/s/syu/scripts/dumpCompareMultiplePDF.C++\(\"$inputtextfile\",\"$header\"\)

