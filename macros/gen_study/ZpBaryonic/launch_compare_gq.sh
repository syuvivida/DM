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

gqARRAY=(0.05 0.1 0.25 0.3333)
inputtextfile=${header}.txt
rm -rf $inputtextfile

for ((i=0; i < ${#gqARRAY[@]}; i++))
do
  gq=${gqARRAY[$i]}
  gqname=${gq/\./p} 
  filename=/data7/monoH/monoH_genOnly/${name}/gq_variation/gq$gqname/output/gq${gqname}_${header}.root
  legend="gq="$gq
  echo $filename $legend >> $inputtextfile
done

cd /afs/cern.ch/work/s/syu/13tev/80X/CMSSW_8_0_20/src
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cd $PRODHOME

root -q -b /afs/cern.ch/user/s/syu/scripts/dumpCompareMultiplePDF.C++\(\"$inputtextfile\",\"$header\"\)

