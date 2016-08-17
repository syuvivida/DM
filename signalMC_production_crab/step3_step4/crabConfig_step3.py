""" In CRAB3 the configuration file is in Python language. It consists of creating a Configuration object imported from the WMCore library: """
from WMCore.Configuration import Configuration
config = Configuration()

"""  Once the Configuration object is created, it is possible to add new sections into it with corresponding parameters."""

config.section_("General")
config.General.requestName = 'MonoHStep3'
config.General.workArea = 'crab_projects_MonoHStep3'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'step3.py'

config.section_("Data")
config.Data.inputDataset = '/MonoH_2HDM_Zprime_A0hbb_MZp1000_MA0300/syu-RAWAODSIM-34254296de3eedb8bd57a822b932fc08/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits  = -1
config.Data.publication = True
config.Data.outputDatasetTag  = 'AODSIM'



config.section_("Site")
config.Site.storageSite = "T3_TW_NCU"
