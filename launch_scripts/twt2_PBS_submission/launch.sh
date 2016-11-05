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
CARDSDIR=${PRODHOME}
name=$1
########################
#Locating the proc card#
########################
if [ ! -e $CARDSDIR/${name}_proc_card.dat ]; then
    echo $CARDSDIR/${name}_proc_card.dat " does not exist!"
    exit 1;
fi

mmed=$2
mchi=$3

echo ""
echo "Producing cards for mediator mass = "$mmed" GeV"
echo "Producing cards for DM mass = "$mchi" GeV "
echo ""
newname=${name}\_MSC${mmed}\_MDM${mchi}
sed -e 's/FOLDERNAME/'$newname'/g' -e 's/MMED/'$mmed'/g' -e 's/MCHI/'$mchi'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
qsub -v NUM=$mmed,PRODHOME=$PWD,CARDNAME=$CARDSDIR/${newname}_proc_card.dat $PWD/runLaunch.sh
