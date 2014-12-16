
import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.eSelBase_cff import *

##The isolation: non-combined, relative, detector based.

#Barrel
eIdBase2012Brl = eIdBase.clone()
eIdBase2012Brl.detain = cms.double(0.004)
eIdBase2012Brl.delphi= cms.double(0.030)
eIdBase2012Brl.sieie = cms.double(0.010)
eIdBase2012Brl.hoe =  cms.double(0.120)
eIdBase2012Brl.d0vtx = cms.double(0.020)
eIdBase2012Brl.dzvtx = cms.double(0.100)
eIdBase2012Brl.ooemoop = cms.double(0.050)
eIdBase2012Brl.passConv = cms.double(1)
eIdBase2012Brl.nmisHit=cms.double(0.)
#eIdBase2012Brl.isoRel=cms.double(0.1)
eIdBase2012Brl.isoRel=cms.double(999999.)



#Endcap
eIdBase2012Ecp = eIdBase.clone()
eIdBase2012Ecp.detain = cms.double(0.005)
eIdBase2012Ecp.delphi= cms.double(0.020)
eIdBase2012Ecp.sieie = cms.double(0.030)
eIdBase2012Ecp.hoe =  cms.double(0.100)
eIdBase2012Ecp.d0vtx = cms.double(0.020)
eIdBase2012Ecp.dzvtx = cms.double(0.100)
eIdBase2012Ecp.ooemoop = cms.double(0.050)
eIdBase2012Ecp.passConv = cms.double(1)
eIdBase2012Ecp.nmisHit=cms.double(0.)
#eIdBase2012Ecp.isoRel=cms.double(0.1)
eIdBase2012Ecp.isoRel=cms.double(999999.)



#Total 2012 Tight ID
eSelXZh  = eSelBase.clone()
eSelXZh.ptx = 10.
eSelXZh.etax = 2.5 
eSelXZh.idBrl = eIdBase2012Brl
eSelXZh.idEcp = eIdBase2012Ecp
