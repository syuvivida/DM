# Recipe to produce Moriond17 MC samples

## Check the status of step0 production and prepare input for step1

Go to your work directory for stepLHE and step0 (created following the instruction at https://github.com/syuvivida/DM/tree/2016ana/Moriond17MC_production_crab/stepLHE_step0)

Make sure your step0 jobs are 100% finished before you move on to step1

```
source /afs/cern.ch/cms/cmsset_default.csh (bash: source /afs/cern.ch/cms/cmsset_default.sh)
cd CMSSW_7_1_20/src
cmsenv
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
curl https://raw.githubusercontent.com/syuvivida/DM/2016ana/Moriond17MC_production_crab/step123/MultiCrab_step1.py -o MultiCrab_step1.py
voms-proxy-init --voms cms
python MultiCrab_step1.py prepare step0_inputdataset.txt
```
A text file "step1_inputdataset.txt" will be created.


## CMSSW setup for step1, step2, and step3

Note, you should run the following commands in a different directory.

```
setenv SCRAM_ARCH slc6_amd64_gcc530 (bash: export SCRAM_ARCH=slc6_amd64_gcc530)
source /afs/cern.ch/cms/cmsset_default.csh (bash: source /afs/cern.ch/cms/cmsset_default.sh)
cmsrel CMSSW_8_0_21
cd CMSSW_8_0_21/src
cmsenv
```


## Download codes from github
```
git clone git@github.com:syuvivida/DM.git DM
cd DM
git checkout 2016ana
cd -
mv DM/Moriond17MC_production_crab/step123/* .
rm -rf DM
```

## Prepare the input files for step1
copy the file step1_inputdataset.txt to the directory CMSSW_8_0_21/src/

## Submit the jobs of step1 
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step1.py submit
```

## If you are running step1 on an official GEN-SIM sample

Prepare your own "step1_inputdataset.txt". The first column should be the dataset name from https://cmsweb.cern.ch/das/ and the second column is the corresponding name of the crab workDirectory.

```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step1.py submit official
```


## Check the status of step1 jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step1.py status crab_projects_MonoHStep1
```

## Prepare the input files for step2
Make sure your step1 jobs finish OK before you move to step2 (check the status).

The "step1_inputdataset.txt" below is the text file from the "prepare" step of step1:
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step2.py prepare step1_inputdataset.txt
```
A text file "step2_inputdataset.txt" will be created. 
The first column lists the input dataset names and the second column lists the crab directory to be used when submitting jobs.

## Submit the jobs of step2
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step2.py submit
``` 

## Check the status of step2 jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step2.py status crab_projects_MonoHStep2
```

## Prepare the input files for step3
Make sure your step2 jobs finish OK before you move to step4 (check the status).

The "step2_inputdataset.txt" below is the text file from the "prepare" step of step2:
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step3.py prepare step2_inputdataset.txt
```
A text file "step3_inputdataset.txt" will be created. 
The first column lists the input dataset names and the second column lists the crab directory to be used when submitting jobs.

## Submit the jobs of step3
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step3.py submit
``` 

## Check the status of step3 jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
cmsenv
python MultiCrab_step3.py status crab_projects_MonoHStep3
```
