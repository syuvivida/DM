
## Check the status of step2 production and prepare input for step3

Go to your work directory for step1 and step2 (created following the instruction at https://github.com/syuvivida/DM/tree/v0.05/signalMC_production_crab/step1_step2)

Make sure your step2 jobs are 100% finished before you move on to step3

```
source /afs/cern.ch/cms/cmsset_default.csh (bash: source /afs/cern.ch/cms/cmsset_default.sh)
cd CMSSW_8_0_3_patch2/src
cmsenv
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/signalMC_production_crab/step3_step4/MultiCrab_step3.py -o MultiCrab_step3.py
voms-proxy-init --voms cms
python MultiCrab_step3.py prepare step2_inputdataset.txt
```
A text file "step3_inputdataset.txt" will be created.


## CMSSW setup for step3 and step4
```
setenv SCRAM_ARCH slc6_amd64_gcc530 (bash: export SCRAM_ARCH=slc6_amd64_gcc530)
source /afs/cern.ch/cms/cmsset_default.csh (bash: source /afs/cern.ch/cms/cmsset_default.sh)
cmsrel CMSSW_8_0_11
cd CMSSW_8_0_11/src
cmsenv
```


## Download codes from github
```
git clone git@github.com:syuvivida/DM.git DM
cd DM
git checkout v0.05
cd -
mv DM/signalMC_production_crab/step3_step4/* .
rm -rf DM
```

## Prepare the input files for step3
copy the file step2_inputdataset.txt to the directory CMSSW_8_0_11/src/

## Submit the jobs of step3
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
python MultiCrab_step3.py submit
```

## Check the status of step3 jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
python MultiCrab_step3.py status crab_projects_MonoHStep3
```

## Prepare the input files for step4
Make sure your step3 jobs finish OK before you move to step4 (check the status).

The "step3_inputdataset.txt" below is the text file from the "prepare" step of step3:
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
python MultiCrab_step4.py prepare step3_inputdataset.txt
```
A text file "step4_inputdataset.txt" will be created. 
The first column lists the input dataset names and the second column lists the crab directory to be used when submitting jobs.

## Submit the jobs of step4
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
python MultiCrab_step4.py submit
``` 

## Check the status of step4 jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
voms-proxy-init --voms cms
python MultiCrab_step4.py status crab_projects_MonoHStep4
```
