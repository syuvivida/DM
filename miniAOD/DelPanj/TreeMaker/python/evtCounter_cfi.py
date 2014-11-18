import FWCore.ParameterSet.Config as cms
evtCounter = cms.EDAnalyzer('EventCounter',
instance = cms.int32(0)
)
