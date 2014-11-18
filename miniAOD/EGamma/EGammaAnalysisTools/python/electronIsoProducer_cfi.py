import FWCore.ParameterSet.Config as cms

elePFIso = cms.EDFilter("ElectronIsoProducer",
                        verbose = cms.untracked.bool(False),
                        vertexTag = cms.InputTag('offlinePrimaryVertices'),
                        electronTag = cms.InputTag('gsfElectrons'),
                        particleFlowTag = cms.InputTag('particleFlow'),
                        nameValueMapIsoCh = cms.string("chIsoForGsfEle"),
                        nameValueMapIsoPh = cms.string("phIsoForGsfEle"),
                        nameValueMapIsoNh = cms.string("nhIsoForGsfEle")
)
