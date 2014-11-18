#!/bin/bash


scriptname=`basename $0`
EXPECTED_ARGS=3

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname string logdirectory numberofjobs"
    echo "Example: ./$scriptname flattuple TTTo2L2Nu2B_8TeV-powheg 2160"
    exit 1
fi

string=$1
dir=$2
if [[ -e filelist ]]; then
    echo "removing filelist"
    rm -rf filelist
fi

ls -l $dir/res/$string_*root | awk '{print $9}' | awk -F "${dir}/res/" '{print $2}' >& filelist

iteration=0
lastfile=$3
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  number=`grep -a $string"_"$iteration"_" filelist | wc -l`
  rm -rf temp
  if [ $((number)) -gt 1 ]; then 
      echo $iteration "has problem"; 
      grep -a $string"_"$iteration"_" filelist > temp
      tempiteration=0
      lasttemp=$((number))
#      echo $lasttemp
      while [ $tempiteration -lt $lasttemp ]; 
 	do
    	  tempiteration=$(( tempiteration + 1 ))
	  infilename=(`head -n $tempiteration temp  | tail -1`)
#	  echo $infilename
	  numbertemp=`grep -a $infilename $dir/res/CMSSW_$iteration.stdout | wc -l`
	  if [ $((numbertemp)) -gt 0 ]; then 
	      echo $infilename " is really produced"
	  else
	      echo $infilename " should be removed" 
	      ls $dir/res/${infilename}
# when you are sure which files to remove, uncomment the following line
#	      rm -rf $dir/res/${infilename}
	  fi
      done
  else if [ $((number)) -lt 1 ]; then
      echo "Missing " $iteration ;
  fi
  fi

done
