#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
gzzhFactor=1

if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "gzzhFactor is set to "$gzzhFactor
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname processName mmed mchi gzzhFactor"
    echo "Example: ./$scriptname Zprime_Zh_Zlephbb 1000 10 1"
    exit 1
else
    gzzhFactor=$4
fi
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
gzzh=`echo $mmed*$gzzhFactor | bc -l`

echo ""
echo "Producing cards for mediator mass = "$mmed" GeV"
echo "Producing cards for DM mass = "$mchi" GeV "
echo "Producing cards for gzzH = " $gzzh" GeV"
echo ""

newname=${name}\_MSC${mmed}\_MDM${mchi}
sed -e 's/FOLDERNAME/'$newname'/g' -e 's/MMED/'$mmed'/g' -e 's/MCHI/'$mchi'/g' -e 's/gZZH/'$gzzh'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
bsub -q2nw -R "rusage[mem=12000]" $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat
#$PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat



