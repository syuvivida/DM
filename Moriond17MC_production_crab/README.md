## Recipe to produce Moriond17 MC samples

### General things to remember 

Within the same terminal, the following commands only need to run once
```
voms-proxy-init --voms cms
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh (bash: source /cvmfs/cms.cern.ch/crab3/crab_standalone.sh)
```

### Data-teir output of every step

stepLHE: LHE

step0: GEN-SIM

step1: RAWSIM

step2: AODSIM

step3: MINIAODSIM
