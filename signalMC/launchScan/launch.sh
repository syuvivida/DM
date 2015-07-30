#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
MIN_ARGS=2
factor=-1
widthName="narrow"
if [ $# -eq $MIN_ARGS ]
then
    echo "Use default width for the signal 0.001 GeV or natural width"
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname processName massInputFile factor cardString"
    echo "Example: ./$scriptname Zprime_Zh_Zlephbb mass_points 0.05 medium"
    exit 1
else
    factor=$3
    widthName=$4
fi
fi

# name of the run
name=$1

# mass input files
massInput=$2


export PRODHOME=`pwd`
CARDSDIR=${PRODHOME}

########################
#Locating the proc card#
########################
if [ ! -e $CARDSDIR/${name}_proc_card.dat ]; then
    echo $CARDSDIR/${name}_proc_card.dat " does not exist!"
    exit 1;
fi


iteration=0
lastfile=`cat $massInput | wc -l`
echo "There are "$lastfile" mass points"

while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  mass=(`head -n $iteration $massInput  | tail -1`)
  comp=`bc <<< $factor"<=0"`
  if [ $comp -eq 1 ]
  then
      width=0.001
  else
      width=`echo "$factor*$mass" | bc -ql | head -c 9`
  fi
  echo ""
  echo "Producing cards for X mass = "$mass" GeV"
  echo "Producing signal width = "$width" GeV "
  echo "NOTE: If default HVT model is used, natural width is generated."
  echo ""
  newname=${name}_${widthName}_M${mass}
  sed -e 's/FOLDERNAME/'$newname'/g' -e 's/MASS/'$mass'/g' -e 's/WIDTH/'$width'/g' $CARDSDIR/${name}_proc_card.dat > $CARDSDIR/${newname}_proc_card.dat
  bsub -q8nh $PWD/runLaunch.sh $PWD $CARDSDIR/${newname}_proc_card.dat
done



