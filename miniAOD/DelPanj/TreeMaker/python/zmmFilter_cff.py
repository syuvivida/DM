import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.muSelLvdp2011_cff import *

zmmFilter = cms.EDFilter('ZmmFilter',
  patMuons = cms.InputTag("selectedPatMuonsPFlow"), 
  leadMuPset_ = muSelLvdp2011,
  subLeadMuPset_ = muSelLvdp2011,
  beamSpot = cms.InputTag("offlineBeamSpot"),
  zMassLowerLimit = cms.double(0),
  zMassUpperLimit = cms.double(2000), 
  scoreHistos = cms.double(0)
)




##Lovedeep's full 2011 selection
zmmFilterLvdp2011 = zmmFilter.clone()
zmmFilterLvdp2011.zMassLowerLimit = cms.double(60)
zmmFilterLvdp2011.zMassUpperLimit = cms.double(120)




