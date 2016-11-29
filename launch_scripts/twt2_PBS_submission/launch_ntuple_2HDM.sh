#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname MZp MA0 mchi"
    echo "Example: ./$scriptname 1000 300 100"
    exit 1
fi

export PRODHOME=`pwd`
name=2HDM
mmed=$1
mmed2=$2
mchi=$3

echo "Producing cards for Z' mass = "$mmed" GeV "
echo "Producing cards for A0 mass = "$mmed2" GeV "
echo "Producing cards for DM mass = "$mchi" GeV "
echo ""
newname=${name}\_MZp${mmed}\_MA0${mmed2}\_MDM${mchi}
inputfile=step0_${newname}.root
outputfile=gentuple_${newname}.root
qsub -v PRODHOME=$PWD,INPUTFILE=$inputfile,OUTPUTFILE=$outputfile $PWD/runNtuple.sh



