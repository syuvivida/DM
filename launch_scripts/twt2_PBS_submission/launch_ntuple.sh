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
mchi=$3

echo ""
echo "Producing ntuples for mediator mass = "$mmed" GeV"
echo "Producing ntuples for DM mass = "$mchi" GeV "
echo ""
newname=${name}\_MSC${mmed}\_MDM${mchi}
inputfile=step0_${newname}.root
outputfile=gentuple_${newname}.root
qsub -v PRODHOME=$PWD,INPUTFILE=$inputfile,OUTPUTFILE=$outputfile $PWD/runNtuple.sh



