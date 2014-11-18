import FWCore.ParameterSet.Config as cms

phoPFIso = cms.EDFilter("PhotonIsoProducer",
                        verbose = cms.untracked.bool(False),
                        vertexTag = cms.InputTag('offlinePrimaryVertices'),
                        photonTag = cms.InputTag('photons'),
                        particleFlowTag = cms.InputTag('particleFlow'),
                        nameValueMapIsoCh = cms.string("chIsoForGsfEle"),
                        nameValueMapIsoPh = cms.string("phIsoForGsfEle"),
                        nameValueMapIsoNh = cms.string("nhIsoForGsfEle")
)
