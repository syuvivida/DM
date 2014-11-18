import FWCore.ParameterSet.Config as cms

eleRegressionEnergy = cms.EDFilter("ElectronRegressionEnergyProducer",
                                   printDebug = cms.untracked.bool(False),
                                   electronTag = cms.InputTag('gsfElectrons'),
                                   regressionInputFile = cms.string("EGamma/EGammaAnalysisTools/data/eleEnergyReg.root"),
                                   energyRegressionType = cms.uint32(1),
                                   nameEnergyReg = cms.string("eneRegForGsfEle"),
                                   nameEnergyErrorReg = cms.string("eneErrorRegForGsfEle"),
                                   recHitCollectionEB = cms.InputTag('reducedEcalRecHitsEB'),
                                   recHitCollectionEE = cms.InputTag('reducedEcalRecHitsEE')
                                   )
