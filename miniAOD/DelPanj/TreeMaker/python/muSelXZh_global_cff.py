import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.muSelBase_cff import *

###########################################################
#identification stuff                                     # 
###########################################################
muIdBase2012 = muIdBase.clone()
muIdBase2012.normalizedChi2= cms.double(9999.)
muIdBase2012.muonHits=cms.double(0.)
muIdBase2012.nMatches=cms.double(1)
muIdBase2012.dxy = cms.double(0.2)
muIdBase2012.dz = cms.double(0.5)
muIdBase2012.pixelHits=cms.double(0.)
muIdBase2012.trackerHits=cms.double(5.)
muIdBase2012.isoRel = cms.double(999999.) #0.1 for the default



muSelXZh_global = muSelBase.clone()
muSelXZh_global.pt = cms.double(10)
muSelXZh_global.ptErr= cms.double(0.3)
muSelXZh_global.eta= cms.double(2.4)
muSelXZh_global.phi= cms.double(3.2)
muSelXZh_global.requireTrigMatch= cms.bool(False)
muSelXZh_global.highPtID= cms.bool(True)
muSelXZh_global.trackerMuID= cms.bool(False)
muSelXZh_global.idPar  = muIdBase2012




