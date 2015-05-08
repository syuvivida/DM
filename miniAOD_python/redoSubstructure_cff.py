
import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import ak8PFJetsCHS, ak8PFJetsCHSPruned, ak8PFJetsCHSSoftDrop, ak8PFJetsCHSPrunedMass, ak8PFJetsCHSSoftDropMass

chs = cms.EDFilter("CandPtrSelector",
  src = cms.InputTag('packedPFCandidates'),
  cut = cms.string('fromPV')
)

ak8PFJetsCHS.src = cms.InputTag('chs')
ak8PFJetsCHSPruned.src = cms.InputTag('chs')
ak8PFJetsCHSSoftDrop.src = cms.InputTag('chs')

NjettinessAK8 = cms.EDProducer("NjettinessAdder",
                               src = cms.InputTag("ak8PFJetsCHS"),
                               Njets = cms.vuint32(1, 2, 3, 4),
                               # variables for measure definition : 
                               measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                               beta = cms.double(1.0),              # CMS default is 1
                               R0 = cms.double( 0.8 ),              # CMS default is jet cone size
                               Rcutoff = cms.double( -999.0),       # not used by default
                               # variables for axes definition :
                               axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                               nPass = cms.int32(-999),             # not used by default
                               akAxesR0 = cms.double(-999.0)        # not used by default
                               )

substructureSequence = cms.Sequence(chs + 
                                    ak8PFJetsCHS + 
                                    ak8PFJetsCHSPruned + 
                                    ak8PFJetsCHSSoftDrop + 
                                    NjettinessAK8 + 
                                    ak8PFJetsCHSPrunedMass + 
                                    ak8PFJetsCHSSoftDropMass
                                    )
