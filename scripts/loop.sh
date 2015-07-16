#!/bin/bash



iteration=0
infiles=$1
lastfile=`cat $infiles | wc -l`
echo "There are "$lastfile" input files"

while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  name=(`head -n $iteration $infiles  | tail -1`)
  echo "Sending jobs for the input file "$name
  dir=$PWD/job_${iteration}
  mkdir $dir
  cp -p $PWD/xAna_rec.C $dir/.
  cp -p $PWD/untuplizer.h $dir/.
  bsub -q8nh $PWD/runJob.csh $dir $name
done
