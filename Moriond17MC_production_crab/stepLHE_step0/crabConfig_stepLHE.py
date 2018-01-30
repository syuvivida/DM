""" In CRAB3 the configuration file is in Python language. It consists of creating a Configuration object imported from the WMCore library: """
from WMCore.Configuration import Configuration
config = Configuration()

"""  Once the Configuration object is created, it is possible to add new sections into it with corresponding parameters."""

config.section_("General")
config.General.requestName = 'MonoHStepLHE'
config.General.workArea = 'crab_projects_MonoHStepLHE'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'stepLHE.py'
config.JobType.pyCfgParams = ['tarball=/cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.0/monoHiggs/Zp2HDM/Zprime_A0h_A0chichi/v1/Zprime_A0h_A0chichi_MZp1000_MA0300_tarball.tar.xz']

config.section_("Data")
config.Data.outputPrimaryDataset = 'MonoH_2HDM_Zprime_A0hbb_MZp1000_MA0300'
config.Data.splitting = 'EventBased'
## Note, the number of events here need to match the variable NEventsPerJob in stepLHE.py
config.Data.unitsPerJob = 500
NJOBS = 20
config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.publication = True
config.Data.outputDatasetTag  = 'LHE'



config.section_("Site")
# stepLHE to step1 must be run at T2_CH_CERN
config.Site.storageSite = "T3_TW_NCU"
