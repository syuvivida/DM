# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step1 --filein dbs:/LQLQToTopMu_M-400_TuneCUETP8M1_13TeV_pythia8/RunIISpring16DR80-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_v3-v1/RAWAODSIM --fileout file:B2G-RunIISpring16reHLT80-00025.root --mc --eventcontent AODSIM --datatier AODSIM --processName HLT2 --inputCommands keep *,drop *_TriggerResults_*_HLT,drop *_hltTriggerSummaryAOD_*_HLT,drop *_hltGtStage2ObjectMap_*_HLT,drop *_l1extraParticles_*_RECO,drop L1GlobalTriggerReadoutRecord_gtDigis_*_RECO,drop *_cscSegments_*_RECO,drop *_dt4DSegments_*_RECO,drop *_rpcRecHits_*_RECO --conditions 80X_mcRun2_asymptotic_v14 --customise_commands process.AODSIMoutput.outputCommands.append('drop L1GlobalTriggerReadoutRecord_gtDigis_*_HLT2') --step L1REPACK:FullMC,HLT:25ns10e33_v2,RAW2DIGI:L1TRawToDigi --era Run2_2016 --python_filename /afs/cern.ch/cms/PPD/PdmV/work/McM/submit/B2G-RunIISpring16reHLT80-00025/B2G-RunIISpring16reHLT80-00025_1_cfg.py --no_exec --customise Configuration/DataProcessing/Utils.addMonitoring -n 576
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('HLT2',eras.Run2_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1EmulatorRepack_FullMC_cff')
process.load('HLTrigger.Configuration.HLT_25ns10e33_v2_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(576)
)

# Input source
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    fileNames = cms.untracked.vstring('file:monoHiggs_step2.root'),
    inputCommands = cms.untracked.vstring('keep *', 
        'drop *_TriggerResults_*_HLT', 
        'drop *_hltTriggerSummaryAOD_*_HLT', 
        'drop *_hltGtStage2ObjectMap_*_HLT', 
        'drop *_l1extraParticles_*_RECO', 
        'drop L1GlobalTriggerReadoutRecord_gtDigis_*_RECO', 
        'drop *_cscSegments_*_RECO', 
        'drop *_dt4DSegments_*_RECO', 
        'drop *_rpcRecHits_*_RECO'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step1 nevts:576'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.AODSIMoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('AODSIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    fileName = cms.untracked.string('file:monoHiggs_step3.root'),
    outputCommands = process.AODSIMEventContent.outputCommands
)

# Additional output definition

# Other statements
from HLTrigger.Configuration.CustomConfigs import ProcessName
process = ProcessName(process)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_v14', '')

# Path and EndPath definitions
process.L1RePack_step = cms.Path(process.SimL1Emulator)
process.raw2digi_step = cms.Path(process.L1TRawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.AODSIMoutput_step = cms.EndPath(process.AODSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.L1RePack_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.raw2digi_step,process.endjob_step,process.AODSIMoutput_step])

# customisation of the process.

# Automatic addition of the customisation function from Configuration.DataProcessing.Utils
from Configuration.DataProcessing.Utils import addMonitoring 

#call to customisation function addMonitoring imported from Configuration.DataProcessing.Utils
process = addMonitoring(process)

# Automatic addition of the customisation function from HLTrigger.Configuration.customizeHLTforMC
from HLTrigger.Configuration.customizeHLTforMC import customizeHLTforFullSim 

#call to customisation function customizeHLTforFullSim imported from HLTrigger.Configuration.customizeHLTforMC
process = customizeHLTforFullSim(process)

# End of customisation functions

# Customisation from command line
process.AODSIMoutput.outputCommands.append('drop L1GlobalTriggerReadoutRecord_gtDigis_*_HLT2')

# Customisation from command line
process.AODSIMoutput.outputCommands.append('drop L1GlobalTriggerReadoutRecord_gtDigis_*_HLT2')
