#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname massFileName sinpFileName stringTobeReplaced"
    echo "Example: ./$scriptname scan_mass scan_sinp ggProduction"
    exit 1
fi

massfile=$1
sinpfile=$2
string=$3
echo $string

while read line
do
    sinpname=${line/0./0p}
    # first make a copy of folder from the template
    awk -v dir=$string -v sinp=$sinpname '{print "cp -pr "dir" sinp_"sinp"_MH3_"$1"_MH4_"$2"_MH2_"$3"_MHC_"$4}' $massfile | bash

    # rename the cards and the output folder
    for file in $(ls -d sinp_$sinpname*)
    do 
	echo $file
	cd $file
	sed -i 's/'$string'/'$file'/g' *_proc_card.dat
	../repfil $string $file spacing
	cd -
    done

    # replace the parameter values in the customize cards with the proper numbers
    awk -v sinp=$line -v name=$sinpname '{print "sed -i -e \x27s/SINP/"sinp"/g\x27 -e \x27s/MA/"$1"/g\x27 -e \x27s/Ma/"$2"/g\x27 sinp_"name"_MH3_"$1"_MH4_"$2"_MH2_"$3"_MHC_"$4"/*_customizecards.dat"}' $massfile | bash

done < $sinpfile

