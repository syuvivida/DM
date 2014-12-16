import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.eSelLvdp2011_cff import *

zeeFilter = cms.EDFilter('ZeeFilter',
  patElectrons = cms.InputTag("selectedPatElectronsPFlow"), 
  leadElecPset_ = eSelLvdp2011,
  subLeadElecPset_ = eSelLvdp2011,
  zMassLowerLimit = cms.double(50),
  zMassUpperLimit = cms.double(400), 
  scoreHistos = cms.double(0)
)




##Lovedeep's full 2011 selection
zeeFilterLvdp2011 = zeeFilter.clone()
zeeFilterLvdp2011.zMassLowerLimit = cms.double(71)
zeeFilterLvdp2011.zMassUpperLimit = cms.double(111)


##Lovedeep's full selection sans Mass Window.
zeeFilterLvdp2011NoMWin = zeeFilter.clone()

##Lovedeep's full selection sans isolation.
zeeFilterLvdp2011NoIsoNoMWin = zeeFilterLvdp2011NoMWin.clone()
zeeFilterLvdp2011NoIsoNoMWin.leadElecPset_ = eSelLvdp2011NoIso
zeeFilterLvdp2011NoIsoNoMWin.subLeadElecPset_ = eSelLvdp2011NoIso
#rint "=========================="
#rint eSelLvdp2011NoIso
##Lovedeep's full selection sans identification.
zeeFilterLvdp2011NoIdNoMWin = zeeFilterLvdp2011NoMWin.clone()
zeeFilterLvdp2011NoIdNoMWin.leadElecPset_ = eSelLvdp2011NoId
zeeFilterLvdp2011NoIdNoMWin.subLeadElecPset_ = eSelLvdp2011NoId

##Lovedeep's full selection sans id and sans iso
zeeFilterLvdp2011NoIdNoIsoNoMWin = zeeFilterLvdp2011NoIsoNoMWin.clone()
zeeFilterLvdp2011NoIdNoIsoNoMWin.leadElecPset_ = eSelLvdp2011NoIsoNoId
zeeFilterLvdp2011NoIdNoIsoNoMWin.subLeadElecPset_ = eSelLvdp2011NoIsoNoId

##Lovedeep's full selection sans id and sans iso
zeeFilterLvdp2011NoPtEtaNoIdNoIsoNoMWin = zeeFilterLvdp2011NoIdNoIsoNoMWin.clone()
zeeFilterLvdp2011NoPtEtaNoIdNoIsoNoMWin.leadElecPset_ = eSelLvdp2011NoPtEtaNoIsoNoId
zeeFilterLvdp2011NoPtEtaNoIdNoIsoNoMWin.subLeadElecPset_ = eSelLvdp2011NoPtEtaNoIsoNoId

#print zeeFilterLvdp2011NoPtEtaNoIdNoIsoNoMWin.leadElecPset_

#print zeeFilterLvdp2011NoPtEtaNoIdNoIsoNoMWin

###################FOR SYNC EFFORT
#zeeFilterSync = zeeFilter.clone()
#zeeFilterSync.leadElecPset_ = eSelBase,
#zeeFilterSync.subLeadElecPset_ = eSelBase,
#zeeFilterSync.zMassLowerLimit = cms.double(71)
#zeeFilterSync.zMassUpperLimit = cms.double(101)











