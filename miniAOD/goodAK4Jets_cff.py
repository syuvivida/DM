import FWCore.ParameterSet.Config as cms
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector

goodAK4Jets = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                           filterParams = pfJetIDSelector.clone(),
                           src = cms.InputTag("slimmedJets")
                           )

### Cleaning
# We want to make sure that the jets are not the electrons or muons done previously

import PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi as jetCleaner_cfi

cleanAK4Jets = jetCleaner_cfi.cleanPatJets.clone()
cleanAK4Jets.src = "goodAK4Jets"
cleanAK4Jets.checkOverlaps.muons.src = "goodMuons"
cleanAK4Jets.checkOverlaps.muons.deltaR = 0.4
cleanAK4Jets.checkOverlaps.muons.requireNoOverlaps = True
cleanAK4Jets.checkOverlaps.electrons.src = "goodElectrons"
cleanAK4Jets.checkOverlaps.electrons.deltaR = 0.4
cleanAK4Jets.checkOverlaps.electrons.requireNoOverlaps = True
cleanAK4Jets.checkOverlaps.photons = cms.PSet()
cleanAK4Jets.checkOverlaps.taus = cms.PSet()
cleanAK4Jets.checkOverlaps.tkIsoElectrons = cms.PSet()
cleanAK4Jets.finalCut = "pt > 20 & abs(eta) < 2.4"


ak4JetsSequence = cms.Sequence(goodAK4Jets + cleanAK4Jets)
