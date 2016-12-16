""" In CRAB3 the configuration file is in Python language. It consists of creating a Configuration object imported from the WMCore library: """
from WMCore.Configuration import Configuration
config = Configuration()

"""  Once the Configuration object is created, it is possible to add new sections into it with corresponding parameters."""

config.section_("General")
config.General.requestName = 'MonoHStep1'
config.General.workArea = 'crab_projects_MonoHStep1'
config.General.transferOutputs = True
config.General.transferLogs = True
config.JobType.inputFiles = ['pu_files.py']
config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'step1.py'

config.section_("Data")
config.Data.inputDataset = '/MonoH_2HDM_Zprime_A0hbb_MZp1000_MA0300/syu-GEN-SIM-f1e2824a9e11b7b9d6a221661f489786/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits  = -1
config.Data.publication = True
config.Data.outputDatasetTag  = 'RAWSIM'



config.section_("Site")
#config.Site.storageSite = "T3_TW_NCU"
config.Site.storageSite = "T2_TW_NCHC"
