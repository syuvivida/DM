#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname MH3 MH4 MH2 MHC"
    echo "Example: ./$scriptname 600 400 600 600"
    exit 1
fi

export PRODHOME=`pwd`
CARDSDIR=${PRODHOME}
name=2HDMPZ2II
########################
#Locating the proc card#
########################
if [ ! -e $CARDSDIR/${name}_proc_card.dat ]; then
    echo $CARDSDIR/${name}_proc_card.dat " does not exist!"
    exit 1;
fi

mh3=$1
mh4=$2
mh2=$3
mhc=$4

echo ""
echo "Producing cards for A mass = "$mh3" GeV "
echo "Producing cards for a mass = "$mh4" GeV "
echo "Producing cards for h2 mass = "$mh2" GeV "
echo "Producing cards for hc mass = "$mhc" GeV "
echo ""

newname=${name}\_MH3\_${mh3}\_MH4\_${mh4}\_MH2\_${mh2}\_MHC\_${mhc}
echo $newname
sed -e 's/MH3/'$mh3'/g' -e 's/MH4/'$mh4'/g' -e 's/MH2/'$mh2'/g' -e 's/MHC/'$mhc'/g' -e 's/FOLDERNAME/'$newname'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
bsub -q2nw -R "rusage[mem=12000]" $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat
#$PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat