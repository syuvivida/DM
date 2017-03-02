#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname sinp MH3 MH4 MDM"
    echo "Example: ./$scriptname \"1/sqrt(2)\" 500 200 1"
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

sinp=`bc -l <<< $1`
mh3=$2
mh4=$3
mchi=$4

echo ""
echo "sinp = "$sinp
echo "Producing cards for A mass = "$mh3" GeV "
echo "Producing cards for a mass = "$mh4" GeV "
echo "Producing cards for DM mass = "$mchi" GeV "
echo ""

sinpname_temp2=${sinp/./0p}
sinpname_temp=${sinpname_temp2/10p/1p}
sinpname=${sinpname_temp:0:6}
echo $sinpname

newname=${name}\_${sinpname}\_MA${mh3}\_Ma${mh4}\_MDM${mchi}
echo $newname
sed -e 's/FOLDERNAME/'$newname'/g' -e 's/SINTHETA/'$sinp'/g' -e 's/MH3/'$mh3'/g' -e 's/MH4/'$mh4'/g' -e 's/MCHI/'$mchi'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
#bsub -q2nw -R "rusage[mem=12000]" $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat
#$PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat