#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname massFileName mDMFileName stringTobeReplaced"
    echo "Example: ./$scriptname scan_mass scan_dm ggProduction"
    exit 1
fi

massfile=$1
mdmfile=$2
string=$3
echo $string

while read line
do
    mdmname=${line/./p}
    # first make a copy of folder from the template
    awk -v dir=$string -v mdm=$mdmname '{print "cp -pr "dir" gg_mdm_"mdm"_MH3_"$1"_MH4_"$2"_MH2_"$3"_MHC_"$4}' $massfile | bash

    # rename the cards and the output folder
    for file in $(ls -d gg_mdm_$mdmname*)
    do 
	echo $file
	cd $file
	sed -i 's/'$string'/'$file'/g' *_proc_card.dat
	../repfil $string $file spacing
	cd -
    done

    # replace the parameter values in the customize cards with the proper numbers
    awk -v mdm=$line -v name=$mdmname '{print "sed -i -e \x27s/MDM/"mdm"/g\x27 -e \x27s/MA/"$1"/g\x27 -e \x27s/Ma/"$2"/g\x27 gg_mdm_"name"_MH3_"$1"_MH4_"$2"_MH2_"$3"_MHC_"$4"/*_customizecards.dat"}' $massfile | bash

done < $mdmfile

