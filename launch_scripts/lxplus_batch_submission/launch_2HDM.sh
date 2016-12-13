#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname MZp MA0 MDM"
    echo "Example: ./$scriptname 1000 300 100"
    exit 1
fi

export PRODHOME=`pwd`
CARDSDIR=${PRODHOME}
name=2HDM
########################
#Locating the proc card#
########################
if [ ! -e $CARDSDIR/${name}_proc_card.dat ]; then
    echo $CARDSDIR/${name}_proc_card.dat " does not exist!"
    exit 1;
fi

mmed=$1
mmed2=$2
mchi=$3

echo ""
echo "Producing cards for Z' mass = "$mmed" GeV "
echo "Producing cards for A0 mass = "$mmed2" GeV "
echo "Producing cards for DM mass = "$mchi" GeV "
echo ""
newname=${name}\_MZp${mmed}\_MA0${mmed2}\_MDM${mchi}
sed -e 's/FOLDERNAME/'$newname'/g' -e 's/MMED2/'$mmed2'/g' -e 's/MMED/'$mmed'/g' -e 's/MCHI/'$mchi'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
bsub -q2nw -R "rusage[mem=12000]" $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat