
import FWCore.ParameterSet.Config as cms
from DelPanj.TreeMaker.eSelBase_cff import *

##The isolation: non-combined, relative, detector based.

#Barrel
eIdBase2012Brl = eIdBase.clone()
eIdBase2012Brl.detain = cms.double(0.007)
eIdBase2012Brl.delphi= cms.double(0.150)
eIdBase2012Brl.sieie = cms.double(0.010)
eIdBase2012Brl.hoe =  cms.double(0.120)
eIdBase2012Brl.ooemoop = cms.double(0.050)
eIdBase2012Brl.passConv = cms.double(1)
eIdBase2012Brl.nmisHit=cms.double(1.)
eIdBase2012Brl.isoRel =cms.double(0.15)

#Endcap
eIdBase2012Ecp = eIdBase.clone()
eIdBase2012Ecp.detain = cms.double(0.009)
eIdBase2012Ecp.delphi= cms.double(0.100)
eIdBase2012Ecp.sieie = cms.double(0.030)
eIdBase2012Ecp.hoe =  cms.double(0.100)
eIdBase2012Ecp.d0vtx = cms.double(0.020)
eIdBase2012Ecp.dzvtx = cms.double(0.200)
eIdBase2012Ecp.ooemoop = cms.double(0.050)
eIdBase2012Ecp.passConv = cms.double(1)
eIdBase2012Ecp.nmisHit=cms.double(1.)
eIdBase2012Ecp.isoRel=cms.double(0.15)


#Total Loose ID
eSel2012HZZ  = eSelBase.clone()
eSel2012HZZ.ptx = 20.
eSel2012HZZ.etax = 2.5
eSel2012HZZ.idBrl = eIdBase2012Brl
eSel2012HZZ.idEcp = eIdBase2012Ecp



