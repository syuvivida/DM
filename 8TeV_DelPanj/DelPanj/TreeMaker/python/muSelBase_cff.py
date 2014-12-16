import FWCore.ParameterSet.Config as cms


###########################################################
#identification stuff                                     # 
###########################################################
muIdBase = cms.PSet(
    #Need to put in the variable values.
    dxy= cms.double(99.),
    dz = cms.double(-1),
    normalizedChi2= cms.double(99.),
    trackerHits=cms.double(0.),
    pixelHits=cms.double(0.),
    muonHits=cms.double(0.),
    nMatches=cms.double(0.),
    applyGlbTrk=cms.double(1),
    isoRel = cms.double(0.12)
    )

#######################################
#Base Electron Selector Configuration #
#######################################
muSelBase = cms.PSet(
    pt= cms.double(20),
    ptErr = cms.double(0.3),
    eta= cms.double(2.4),
    phi= cms.double(3.2),
    requireTrigMatch= cms.bool(False),
    highPtID= cms.bool(False),
    trackerMuID = cms.bool(False),
    idPar = muIdBase
    )
