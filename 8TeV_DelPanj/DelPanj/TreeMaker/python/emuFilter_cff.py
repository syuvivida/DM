import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.eSelLvdp2011_cff import eSelLvdp2011
from DelPanj.TreeMaker.muSelLvdp2011_cff import muSelLvdp2011

emuFilter = cms.EDFilter('EMuFilter',
  patMuons = cms.InputTag("selectedPatMuonsPFlow"),
  beamSpot=cms.InputTag("offlineBeamSpot"),
  patElectrons = cms.InputTag("selectedPatElectronsPFlow"), 
  elPset = eSelLvdp2011,
  muPset = muSelLvdp2011,
  zMassLowerLimit = cms.double(60.),
  zMassUpperLimit = cms.double(120.), 
  scoreHistos = cms.double(1)
)

