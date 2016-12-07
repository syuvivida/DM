#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname processName mmed mchi"
    echo "Example: ./$scriptname Zprime_Zh_Zlephbb 1000 10"
    exit 1
fi

export PRODHOME=`pwd`
name=$1
mmed=$2
mdm=$3

echo ""
echo "Producing input text file for Z' mediator mass = "$mmed" GeV"
echo "Producing input text file for DM mass = "$mdm" GeV "
echo ""
header=${name}\_MSC${mmed}\_MDM${mdm}

## Now produce input text file for comparison

gzARRAY=(1 100 10000 0.1Mzp 1Mzp 10Mzp)
inputtextfile=${header}.txt
rm -rf $inputtextfile

for ((i=0; i < ${#gzARRAY[@]}; i++))
do
  gz=${gzARRAY[$i]}
  gzname=${gz/\./p} 
  filename=/data7/monoH/monoH_genOnly/${name}/gz_variation/gz$gzname/output/gz${gzname}_${header}.root
  legend="gz="$gz
  echo $filename $legend >> $inputtextfile
done

cd /afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_20/src
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cd $PRODHOME

root -q -b /afs/cern.ch/user/s/syu/scripts/dumpCompareMultiplePDF.C++\(\"$inputtextfile\",\"$header\"\)

