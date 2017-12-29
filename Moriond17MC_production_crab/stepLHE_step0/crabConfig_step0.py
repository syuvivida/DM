""" In CRAB3 the configuration file is in Python language. It consists of creating a Configuration object imported from the WMCore library: """
from WMCore.Configuration import Configuration
config = Configuration()

"""  Once the Configuration object is created, it is possible to add new sections into it with corresponding parameters."""

config.section_("General")
config.General.requestName = 'MonoHStep0'
config.General.workArea = 'crab_projects_MonoHStep0'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'step0.py'

config.section_("Data")
config.Data.inputDataset = '/MonoH_2HDM_Zprime_A0hbb_MZp1000_MA0300/syu-LHE-51574c3714843ccfb0452d1664999d8e/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits  = -1
config.Data.publication = True
config.Data.outputDatasetTag  = 'cernT2-GEN-SIM'



config.section_("Site")
# stepLHE to step1 must be run at T2_CH_CERN
config.Site.storageSite = "T2_CH_CERN"
