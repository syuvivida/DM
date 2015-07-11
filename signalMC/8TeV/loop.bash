#!/bin/bash


iteration=0
lastfile=10
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  echo $iteration
  bsub -q 1nw $PWD/runJob.sh $PWD $iteration $1

done
