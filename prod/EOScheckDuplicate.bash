#!/bin/bash


scriptname=`basename $0`
EXPECTED_ARGS=5
userid="syu"
if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "user ID is set to "$userid
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname remoteDirectory string logdirectory numberofjobs userid"
    echo "Example: ./$scriptname TTT flattuple TTTo2L2Nu2B_8TeV-powheg 2160 syu"
    exit 1
else 
    userid=$5
fi
fi

echo "user id is "$userid

string=$2
if [[ -e filelist ]]; then
    echo "removing filelist"
    rm -rf filelist
fi

if [[ $1 == */* ]];
then
    cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk '{print $5}' >& filelist
else
    cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk '{print $5}' >& filelist
fi

iteration=0
lastfile=$4
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  number=`grep -a $string"_"$iteration"_" filelist | wc -l`
  rm -rf temp
  if [ $((number)) -gt 1 ]; then 
      echo $iteration "has problem"; 
      grep -a $string"_"$iteration"_" filelist > temp
      echo $"Here"
      tempiteration=0
      lasttemp=$((number))
#      echo $lasttemp
      while [ $tempiteration -lt $lasttemp ]; 
 	do
    	  tempiteration=$(( tempiteration + 1 ))
	  infilename=(`head -n $tempiteration temp  | tail -1`)
	  echo $infilename
	  numbertemp=`grep -a $infilename $3/res/CMSSW_$iteration.stdout | wc -l`
	  if [ $((numbertemp)) -gt 0 ]; then 
	      echo $infilename " is really produced"
	  else
	      echo $infilename " should be removed" 
	      cmsLs ${infilename}
# when you are sure which files to remove, uncomment the following line
#	      cmsRm ${infilename}
	  fi
      done
  else if [ $((number)) -lt 1 ]; then
      echo "Missing " $iteration ;
  fi

  fi

done
