import FWCore.ParameterSet.Config as cms


###########################################################
#identification stuff                                     # 
###########################################################
eIdBase = cms.PSet(
    #Need to put in the variable values.
    detain=cms.double(99.),
    delphi= cms.double(99.),
    sieie= cms.double(99.),
    hoe=cms.double(99.),
    d0vtx=cms.double(-1.),
    dzvtx=cms.double(-1.),
    ooemoop=cms.double(99.),    
    passConv=cms.double(-1.),
    nmisHit=cms.double(99.),
    isoRel=cms.double(9999.0)
    )


#######################################
#Base Electron Selector Configuration #
#######################################
eSelBase = cms.PSet(
    ptx= cms.double(20),
    etax= cms.double(2.4),
    ##Identification
    idBrl = eIdBase,
    idEcp = eIdBase
    )


















