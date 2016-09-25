### NOTE, this python can not run on miniAOD, object names are for GEN-SIM, AODSIM

import FWCore.ParameterSet.Config as cms
## removed cleaning from Exo VV package 
## 

process = cms.Process('GENONLY')
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')
options.parseArguments()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


process.source = cms.Source("PoolSource",
                            secondaryFileNames = cms.untracked.vstring(),
                            fileNames = cms.untracked.vstring(options.inputFiles)
                            )


process.load('DelPanj.TreeMaker.TreeMaker_cfi')
process.tree.saveGenJets      = cms.bool(True)
process.tree.fillGenInfo      = cms.bool(True)
process.tree.fillPUweightInfo = cms.bool(False)
process.tree.fillEventInfo    = cms.bool(False)
process.tree.fillMetInfo      = cms.bool(False)
process.tree.fillTrigInfo     = cms.bool(False)
process.tree.fillFilterInfo   = cms.bool(False)
process.tree.fillElecInfo     = cms.bool(False)
process.tree.fillMuonInfo     = cms.bool(False)
process.tree.fillTauInfo      = cms.bool(False)
process.tree.fillPhotInfo     = cms.bool(False)
process.tree.fillJetInfo      = cms.bool(False) 
process.tree.fillFATJetInfo   = cms.bool(False) 
process.tree.fillAddJetInfo   = cms.bool(False)
process.tree.genPartLabel     = cms.InputTag("genParticles")

process.TFileService = cms.Service("TFileService",
				   fileName = cms.string(options.outputFile)          
				   )

process.analysis = cms.Path(process.tree)
