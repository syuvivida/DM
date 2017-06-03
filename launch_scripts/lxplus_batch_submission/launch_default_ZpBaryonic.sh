#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=2
gzzhFactor=1

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname mmed mchi"
    echo "Example: ./$scriptname 1000 10"
    exit 1
fi

export PRODHOME=`pwd`
CARDSDIR=${PRODHOME}
name=default_ZpBaryonic
########################
#Locating the proc card#
########################
if [ ! -e $CARDSDIR/${name}_proc_card.dat ]; then
    echo $CARDSDIR/${name}_proc_card.dat " does not exist!"
    exit 1;
fi

mmed=$1
mchi=$2

echo ""
echo "Producing cards for mediator mass = "$mmed" GeV"
echo "Producing cards for DM mass = "$mchi" GeV "
echo ""

newname=${name}\_MSC${mmed}\_MDM${mchi}
sed -e 's/FOLDERNAME/'$newname'/g' -e 's/MMED/'$mmed'/g' -e 's/MCHI/'$mchi'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
bsub -q2nw -R "rusage[mem=12000]" $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat
#$PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat



