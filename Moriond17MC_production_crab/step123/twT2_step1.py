# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step1 --filein dbs:/MonoHaa_ZpBaryonic_MZp-500_MChi-1_13TeV-madgraph/RunIISummer15wmLHEGS-MCRUN2_71_V1-v1/GEN-SIM --fileout file:EXO-RunIISummer16DR80Premix-01710_step1.root --pileup_input /store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v2-v2/100000/007E6D91-3C81-E611-9AAA-782B --mc --eventcontent RAWSIM --pileup 2016_25ns_Moriond17MC_PoissonOOTPU --datatier GEN-SIM-RAW --conditions 80X_mcRun2_asymptotic_2016_TrancheIV_v6 --step DIGI,L1,DIGI2RAW,HLT:@frozen2016 --era Run2_2016 --python_filename EXO-RunIISummer16DR80Premix-01710_1_cfg.py --no_exec --customise Configuration/DataProcessing/Utils.addMonitoring -n 82
import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process('HLT',eras.Run2_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mix_2016_25ns_Moriond17MC_PoissonOOTPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('HLTrigger.Configuration.HLT_25ns15e33_v4_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    fileNames = cms.untracked.vstring('/store/mc/RunIISummer15wmLHEGS/MonoHaa_ZpBaryonic_MZp-500_MChi-1_13TeV-madgraph/GEN-SIM/MCRUN2_71_V1-v1/00000/20EC0E09-6C50-E611-B004-0025905B858C.root'),
    inputCommands = cms.untracked.vstring('keep *', 
        'drop *_genParticles_*_*', 
        'drop *_genParticlesForJets_*_*', 
        'drop *_kt4GenJets_*_*', 
        'drop *_kt6GenJets_*_*', 
        'drop *_iterativeCone5GenJets_*_*', 
        'drop *_ak4GenJets_*_*', 
        'drop *_ak7GenJets_*_*', 
        'drop *_ak8GenJets_*_*', 
        'drop *_ak4GenJetsNoNu_*_*', 
        'drop *_ak8GenJetsNoNu_*_*', 
        'drop *_genCandidatesForMET_*_*', 
        'drop *_genParticlesForMETAllVisible_*_*', 
        'drop *_genMetCalo_*_*', 
        'drop *_genMetCaloAndNonPrompt_*_*', 
        'drop *_genMetTrue_*_*', 
        'drop *_genMetIC5GenJs_*_*'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step1 nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RAW'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('file:monoHiggs_step1.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.input.fileNames = cms.untracked.vstring(['root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/0229AA65-58D0-E511-8561-02163E00B82F.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/060814BA-57D0-E511-8C42-02163E00C04A.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/1647C518-C1D0-E511-AABB-02163E00C6BB.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/2205EE58-C5D0-E511-ABCB-02163E00F496.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/2245C8B7-C9D0-E511-91A0-2C600CAFEE80.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/3AACDBDE-C9D0-E511-B0A6-0025904B26B4.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/462E2931-BAD0-E511-B5FC-02163E0165AF.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/4ED96624-68D0-E511-AF68-02163E015D9C.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/56A5723B-5DD0-E511-823F-02163E012320.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/589433E9-C0D0-E511-BC38-02163E01462F.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/82980C14-C1D0-E511-968F-02163E00F456.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/8626A1B8-BED0-E511-9A99-02163E00E845.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/92313280-B8D0-E511-9637-02163E00F283.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/AA0BBB25-CFD0-E511-BD24-0025901D6246.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/CE884008-D0D0-E511-8F87-02163E00B493.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/EAF9A778-C5D0-E511-91BA-02163E00E856.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/F4C9D24F-C1D0-E511-8DB2-02163E00B756.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/FAE23052-60D0-E511-B9AB-02163E016440.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/FC38E426-5DD0-E511-8D98-02163E00BF32.root', 'root://se01.grid.nchc.org.tw//dpm/grid.nchc.org.tw/home/cms/store/user/syu/RunIISummer15GS/MinBias_TuneCUETP8M1_13TeV-pythia8/GEN-SIM/MCRUN2_71_V1_ext1-v1/10000/FCEDB8CB-C9D0-E511-AFD9-02163E0165C2.root' ])

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v6', '')

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.L1simulation_step,process.digi2raw_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.endjob_step,process.RAWSIMoutput_step])

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

