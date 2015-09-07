
## To run on normal small LHE file
## There is no need to specify the number of events per job

./loop.sh runStep1.sh 2nd 

./loop.sh runStep2_pileup.sh 2nd 

./loop.sh runStep3.sh 2nd

If you wish to specify the number of events per job, you need to do that for all 3 steps, for example

./loop.sh runStep1.sh 2nd 1000

./loop.sh runStep2_pileup.sh 2nd 1000

./loop.sh runStep3.sh 2nd 1000
 

### To run on a large LHE file, you MUST specify the number of events per job

./loop.sh runStep1_splitLHE.sh 2nd 1000

./loop.sh runStep2_pileup.sh 2nd 1000

./loop.sh runStep3.sh 2nd 1000

