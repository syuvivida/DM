1) Prepare small LHE file, each LHE file has at most 1000 events

Therefore, if you need 50K events in total, you need to generate 50 LHE files.

Use runcmsgrid.sh with different seeds when you generate these files 
(so to ensure that there are no duplicates)

Give each LHE file a name like: ZpAbb_1.lhe, ZpAbb_2.lhe, ZpAbb_3.lhe ...

2) Once you have the LHE files, you could submit jobs to lxplus 
 system via loop.bash and runJob.sh

For step1 and 50 LHE files, there is no need for modification.


But before running, you need to save your grid certificate somewhere.

setenv X509_USER_PROXY $HOME/private/grid.proxy

voms-proxy-init --voms cms

Check if you do have a grid.proxy at $HOME/private


3) Submit step1 jobs

 ./loop.bash ZppAbb


4) Once step1 is finished, you could modify runJob.sh to run step2, and step3.

5) If you only want to try on one LHE file, modify lastfile in loop.bash