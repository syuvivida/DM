import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.muSelBase_cff import *

###########################################################
#identification stuff                                     # 
###########################################################
muIdBase2012 = muIdBase.clone()
muIdBase2012.normalizedChi2= cms.double(10.)
muIdBase2012.muonHits=cms.double(0.)
muIdBase2012.nMatches=cms.double(1)
muIdBase2012.dxy = cms.double(0.2)
muIdBase2012.dz = cms.double(0.5)
muIdBase2012.pixelHits=cms.double(0.)
muIdBase2012.trackerHits=cms.double(5.)
muIdBase2012.isoRel = cms.double(0.12)


muSel2012HZZ = muSelBase.clone()
muSel2012HZZ.pt = cms.double(20)
muSel2012HZZ.eta= cms.double(2.4)
muSel2012HZZ.requireTrigMatch= cms.bool(False)
muSel2012HZZ.highPtID  = cms.bool(False)
muSel2012HZZ.trackerMuID  = cms.bool(False)
muSel2012HZZ.idPar  = muIdBase2012






