#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("LHE")

process.source = cms.Source("LHESource",
#	fileNames = cms.untracked.vstring('file:unweighted_events.lhe')
#        fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v2_9_22/Ulrich/Events/paper_fixDecayWidth2GeV/unweighted_events.lhe')
        fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v2_9_22/Ulrich/Events/paper2_computeDecayWidth/unweighted_events.lhe')
)

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.configurationMetadata = cms.untracked.PSet(
	version = cms.untracked.string('alpha'),
	name = cms.untracked.string('LHEF input'),
	annotation = cms.untracked.string('ttbar')
)

process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cout = cms.untracked.PSet( threshold = cms.untracked.string('INFO') )

process.LHE = cms.OutputModule("PoolOutputModule",
	dataset = cms.untracked.PSet(dataTier = cms.untracked.string('LHE')),
	fileName = cms.untracked.string('lhe.root')
)

process.lhedump = cms.EDAnalyzer("DummyLHEAnalyzer",
                                 src = cms.InputTag("source")
                                 )

process.TFileService = cms.Service("TFileService", fileName = cms.string('results_eiko.root'))

process.outpath = cms.EndPath(process.lhedump+process.LHE)
