import FWCore.ParameterSet.Config as cms

from CMGTools.Common.Tools.cmsswRelease import isNewerThan

sep_line = '-'*67
print sep_line
print 'CMG PAT-tuplizer, contact Colin before any modification'
print sep_line

process = cms.Process("PAT")



########################################################
## User options
########################################################

## Basic options
createPATtuple = False
createCMGtuple = False
createFlatTuple = True
skimMuon = False
skimElec = False
runOnMC    = True
runOnFastSim = False

## Z-->ee/mumu reco filter
EE = ("patElectronsWithTrigger patElectronsWithTrigger")
MUMU = ("patMuonsWithTrigger patMuonsWithTrigger")
DILEPTON_KINCUT = ("60.0 < mass < 120.0 && pt > 60.0")


## MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

## Input files
#from CMGTools.Production.datasetToSource import *

process.source = cms.Source("PoolSource",
                            fileNames =cms.untracked.vstring(
        'file:/afs/cern.ch/work/s/syu/public/forStudents/GluGluToHToZZTo2L2Q_M-900_8TeV-powheg-pythia6_PU_S10_START53_V7A-v1.root'
        )
                            )


## Maximal Number of Events

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

print sep_line
print process.source.fileNames
print sep_line 


########################################################
## Setup PAT Sequence
########################################################

print 'loading the main CMG sequence'

process.load('CMGTools.Common.PAT.PATCMG_cff')
from CMGTools.Common.eventContent.patEventContentCMG_cff import patEventContentCMG

#### Adding AK7 jets

#process.load("ExoDiBosonResonances.PATtupleProduction.PAT_ak7jets_cff")
#process.PATCMGSequence += process.PATCMGJetSequenceAK7CHS

#### Adding AK7 pruned jets

#process.load("CMGTools.Common.PAT.jetSubstructure_cff")
#process.PATCMGSequence.remove(process.PATCMGJetSequenceCHSpruned) # don't produce the AK5 pruned collections
#process.jetMCSequenceAK7CHSpruned.remove(process.ak7GenJetsNoNu) # don't cluster the ak7GenJetsNoNu twice
#process.selectedPatJetsAK7CHSpruned.cut = 'pt()>20'
#process.PATCMGSequence += process.PATCMGJetSequenceAK7CHSpruned
#patEventContentCMG+=['keep *_ak7PFJetsCHSpruned_SubJets_*']
#patEventContentCMG+=['keep *_ak7GenJetsNoNu_*_*']


#### Adding CA8 jets and CA8 pruned jets

process.load("ExoDiBosonResonances.PATtupleProduction.PAT_ca8jets_cff")
process.PATCMGSequence += process.PATCMGJetSequenceCA8CHS
process.PATCMGSequence += process.PATCMGJetSequenceCA8CHSpruned
patEventContentCMG+=['keep *_patJetsCA8CHSprunedSubjets_*_*']
patEventContentCMG+=['drop patJets_selectedPatJetsCA8CHSprunedPre_*_*']
patEventContentCMG+=['keep *_ca8GenJetsNoNu_*_*']


#### Adding Nsubjetiness

#process.selectedPatJetsAK7CHSwithNsub = cms.EDProducer("NjettinessAdder",
#                              src=cms.InputTag("selectedPatJetsAK7CHS"),
#                              cone=cms.double(0.7)
#                              )
#process.PATCMGSequence += process.selectedPatJetsAK7CHSwithNsub
#patEventContentCMG+=['drop patJets_selectedPatJetsAK7CHS_*_*']
process.PATCMGSequence += process.selectedPatJetsCA8CHSwithNsub
patEventContentCMG+=['drop patJets_selectedPatJetsCA8CHS_*_*']

#### Adding QJets

#process.selectedPatJetsAK7CHSwithQjets = cms.EDProducer("QjetsAdder",
#			   src=cms.InputTag("selectedPatJetsAK7CHSwithNsub"),
#			   zcut=cms.double(0.1),
#			   dcutfctr=cms.double(0.5),
#			   expmin=cms.double(0.0),
#			   expmax=cms.double(0.0),
#			   rigidity=cms.double(0.1),
#			   ntrial = cms.int32(50),
#			   cutoff=cms.double(100.0),
#			   jetRad= cms.double(0.7),
#			   jetAlgo=cms.string("AK"),
#			   preclustering = cms.int32(30),
#			  )
#if not runQJets:
#    process.selectedPatJetsAK7CHSwithQjets.cutoff=100000.0
#process.PATCMGSequence += process.selectedPatJetsAK7CHSwithQjets
#patEventContentCMG+=['drop patJets_selectedPatJetsAK7CHSwithNsub_*_*']
process.PATCMGSequence += process.selectedPatJetsCA8CHSwithQjets
patEventContentCMG+=['drop patJets_selectedPatJetsCA8CHSwithNsub_*_*']

#### Adding Variables For Boosted Tau Analysis - Fully Hadronic channel
process.load("ExoDiBosonResonances.PATtupleProduction.PAT_boostedtaus_cff")
process.PATCMGSequence += process.selectedPatJetsCA8CHSprunedForBoostedTaus
process.PATCMGSequence += process.selectedPatJetsCA8CHSwithQJetsForBoostedTaus
patEventContentCMG+=['drop patJets_selectedPatJetsCA8CHSpruned_*_*']
patEventContentCMG+=['drop patJets_selectedPatJetsCA8CHSwithQjets_*_*']

######ADD PU JET ID

from  CMGTools.External.pujetidsequence_cff import puJetId, puJetMva
#process.puJetIdAK7CHS = puJetId.clone(
#    jets ='selectedPatJetsAK7CHSwithQjets',
#    jec = 'AK7chs'
#    )
#process.PATCMGSequence += process.puJetIdAK7CHS
#patEventContentCMG+=['keep *_puJetIdAK7CHS_*_*']
process.puJetIdCA8CHS = puJetId.clone(
    jets ='selectedPatJetsCA8CHSwithQjets',
    jec = 'AK7chs'
    )

process.puJetIdCA8CHSwithQJetsForBoostedTaus = puJetId.clone(
    jets ='selectedPatJetsCA8CHSwithQJetsForBoostedTaus',
    jec = 'AK7chs'
    )

from CMGTools.External.pujetidproducer_cfi import  stdalgos_4x, stdalgos_5x, stdalgos, cutbased, chsalgos_4x, chsalgos_5x, chsalgos

process.puJetMvaAK5CHS= puJetMva.clone(
    jetids = cms.InputTag("puJetIdCHS"),
####    jets ='selectedPatJetsCHS',
    jets ='patJetsWithVarCHS',
    algos =  chsalgos
    )

#process.puJetMvaAK7CHS= puJetMva.clone(
#    jetids = cms.InputTag("puJetIdAK7CHS"),
#    jets ='selectedPatJetsAK7CHSwithQjets',
#    algos =  chsalgos
#    )

process.puJetMvaCA8CHS= puJetMva.clone(
    jetids = cms.InputTag("puJetIdCA8CHS"),
    jets ='selectedPatJetsCA8CHSwithQjets',
    algos =  chsalgos
    )

process.puJetMvaCA8CHSwithQJetsForBoostedTaus = puJetMva.clone(
    jetids = cms.InputTag("puJetIdCA8CHSwithQJetsForBoostedTaus"),
    jets ='selectedPatJetsCA8CHSwithQJetsForBoostedTaus',
    algos =  chsalgos
    )

process.puJetIdAK5Sequence = cms.Sequence(                      process.puJetMvaAK5CHS)
#process.puJetIdAK7Sequence = cms.Sequence(process.puJetIdAK7CHS+process.puJetMvaAK7CHS)
#process.puJetIdCA8Sequence = cms.Sequence(process.puJetIdCA8CHS+process.puJetMvaCA8CHS)
process.puJetIdCA8Sequence = cms.Sequence(process.puJetIdCA8CHS+process.puJetMvaCA8CHS+process.puJetIdCA8CHSwithQJetsForBoostedTaus+process.puJetMvaCA8CHSwithQJetsForBoostedTaus)
#### these are moved down below this same cfg, after having built AK5 CHS jets
#process.PATCMGSequence += process.puJetIdAK5Sequence
#process.PATCMGSequence += process.puJetIdAK7Sequence
#process.PATCMGSequence += process.puJetIdCA8Sequence
#patEventContentCMG+=['keep *_puJetIdAK7CHS_*_*']
patEventContentCMG+=['keep *_puJetIdCA8CHS_*_*']
patEventContentCMG+=['keep *_puJetMvaAK5CHS_*_*']
#patEventContentCMG+=['keep *_puJetMvaAK7CHS_*_*']
patEventContentCMG+=['keep *_puJetMvaCA8CHS_*_*']
patEventContentCMG+=['keep *_puJetIdCA8CHSwithQJetsForBoostedTaus_*_*']
patEventContentCMG+=['keep *_puJetMvaCA8CHSwithQJetsForBoostedTaus_*_*']

if runOnMC is False:
    # removing MC stuff
    print 'removing MC stuff, as we are running on Data'

    process.patElectrons.addGenMatch = False
    process.makePatElectrons.remove( process.electronMatch )
    
    process.patMuons.addGenMatch = False
    process.makePatMuons.remove( process.muonMatch )
    
    process.PATCMGSequence.remove( process.PATCMGGenSequence )
    process.PATCMGJetSequence.remove( process.jetMCSequence )
    process.PATCMGJetSequence.remove( process.patJetFlavourId )
    process.patJets.addGenJetMatch = False
    process.patJets.addGenPartonMatch = False

   #### if isNewerThan('CMSSW_5_2_0'):
    process.PATCMGJetSequenceCHSpruned.remove( process.jetMCSequenceCHSpruned )
    process.patJetsCHSpruned.addGenJetMatch = False
    process.patJetsCHSpruned.addGenPartonMatch = False
    #process.PATCMGJetSequenceAK7CHS.remove( process.jetMCSequenceAK7CHS )
    #process.patJetsAK7CHS.addGenJetMatch = False
    #process.patJetsAK7CHS.addGenPartonMatch = False
    #process.PATCMGJetSequenceAK7CHSpruned.remove( process.jetMCSequenceAK7CHSpruned )
    #process.patJetsAK7CHSpruned.addGenJetMatch = False
    #process.patJetsAK7CHSpruned.addGenPartonMatch = False
    process.PATCMGJetSequenceCA8CHS.remove( process.jetMCSequenceCA8CHS )
    process.patJetsCA8CHS.addGenJetMatch = False
    process.patJetsCA8CHS.addGenPartonMatch = False
    process.PATCMGJetSequenceCA8CHSpruned.remove( process.jetMCSequenceCA8CHSpruned )
    process.patJetsCA8CHSpruned.addGenJetMatch = False
    process.patJetsCA8CHSpruned.addGenPartonMatch = False
    
    process.PATCMGTauSequence.remove( process.tauGenJets )
    process.PATCMGTauSequence.remove( process.tauGenJetsSelectorAllHadrons )
    process.PATCMGTauSequence.remove( process.tauGenJetMatch )
    process.PATCMGTauSequence.remove( process.tauMatch )
    process.patTaus.addGenJetMatch = False
    process.patTaus.addGenMatch = False

    process.patMETs.addGenMET = False 
    process.patMETsRaw.addGenMET = False 

    # setting up JSON file
    # json = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/DCSOnly/json_DCSONLY.txt'
    # print 'using json file: ', json
    # from CMGTools.Common.Tools.applyJSON_cff import *
    # applyJSON(process, json )

    # adding L2L3Residual corrections
    process.patJetCorrFactors.levels.append('L2L3Residual')

     ###   if isNewerThan('CMSSW_5_2_0'):
    process.patJetCorrFactorsCHSpruned.levels.append('L2L3Residual')
    #process.patJetCorrFactorsAK7CHS.levels.append('L2L3Residual')
    #process.patJetCorrFactorsAK7CHSpruned.levels.append('L2L3Residual')
    process.patJetCorrFactorsCA8CHS.levels.append('L2L3Residual')
    process.patJetCorrFactorsCA8CHSpruned.levels.append('L2L3Residual')
    

#### Adding HEEP and modified isolation
### Boosted electrons isolation
### Remake the HEEP ID with no isolation cuts
print "Adding HEEP and modified isolation..."

process.load("RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi")
from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

process.heepIdNoIso = cms.EDProducer("HEEPIdValueMapProducer",
                                     eleLabel = cms.InputTag("gsfElectrons"),
                                     barrelCuts = cms.PSet(heepBarrelCuts),
                                     endcapCuts = cms.PSet(heepEndcapCuts),
                                     eleIsolEffectiveAreas = cms.PSet(heepEffectiveAreas),
                                     eleRhoCorrLabel = cms.InputTag("kt6PFJets", "rho"),
                                     verticesLabel = cms.InputTag("offlinePrimaryVertices"),
                                     applyRhoCorrToEleIsol = cms.bool(True),
                                     writeIdAsInt = cms.bool(True)
                                     )

process.heepIdNoIso.barrelCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:nrMissHits:dxy")
process.heepIdNoIso.endcapCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:nrMissHits:dxy")

process.heepIdNoIsoEles = cms.EDProducer("tsw::HEEPGsfProducer",
                                         cutValueMap = cms.InputTag("heepIdNoIso"),
                                         inputGsfEles = cms.InputTag("gsfElectrons")
                                         )

from TSWilliams.BstdZeeTools.bstdzeemodisolproducer_cff import *

process.modElectronIso = cms.EDProducer("BstdZeeModIsolProducer",
                                        bstdZeeModIsolParams,
                                        vetoGsfEles = cms.InputTag("heepIdNoIsoEles")
                                        )

# Adding these new value maps to the patElectrons
process.patElectrons.userIsolation.user = cms.VPSet(
    cms.PSet(src = cms.InputTag("modElectronIso","track")),
    cms.PSet(src = cms.InputTag("modElectronIso","ecal")),
    cms.PSet(src = cms.InputTag("modElectronIso","hcalDepth1")),
    )

# And now redo the PAT electrons with Mathias' module
process.heepPatElectrons = cms.EDProducer("HEEPAttStatusToPAT",
                                          eleLabel = cms.InputTag("patElectrons"),
                                          barrelCuts = cms.PSet(heepBarrelCuts),
                                          endcapCuts = cms.PSet(heepEndcapCuts),
                                          applyRhoCorrToEleIsol = cms.bool(True),
                                          eleIsolEffectiveAreas = cms.PSet (heepEffectiveAreas),
                                          eleRhoCorrLabel = cms.InputTag("kt6PFJets","rho"),
                                          verticesLabel = cms.InputTag("offlinePrimaryVerticesWithBS"),
                                          )

# With no isolation cuts. In this way, the newly created PAT electrons have a complete HEEP ID, minus isolation cuts,
# while they ALSO have three userIsolations corresponding to the modified isolations.
process.heepPatElectrons.barrelCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:nrMissHits:dxy")
process.heepPatElectrons.endcapCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:nrMissHits:dxy")

process.PATCMGSequence.replace( process.patElectrons,
                                process.heepIdNoIso +
                                process.heepIdNoIsoEles +
                                process.modElectronIso +
                                process.patElectrons +
                                process.heepPatElectrons )

# Finally, change the source of the selectedPatElectrons.
# The changes trickle down from here.
process.selectedPatElectrons.src = cms.InputTag("heepPatElectrons")

#### Adding new TuneP muons and new Track Errors
# Load the tune P muons
print "Adding new TuneP muons and Track Errors..."

# This adds process.tunePmuons and process.muonTrackError
process.load("ExoDiBosonResonances.EDBRMuon.newTuneP_cff")

# Change the source of the patMuons
process.patMuons.muonSource = "tunePmuons"
# Add the user float
process.patMuons.userData.userFloats.src = ['muonTrackError']

# Since we don't use these isoDeposits, we might as well take them out.
process.patMuons.isoDeposits = cms.PSet()
process.patMuons.isolationValues = cms.PSet()
process.patMuons.embedCaloMETMuonCorrs = False # Don't use
process.patMuons.embedTcMETMuonCorrs = False # Don't use

# Put the new modules in the sequence
if runOnMC is False:
    process.PATCMGSequence.replace( process.patMuons,
                                    process.tunePmuons +
                                    process.muonTrackError +
                                    process.patMuons )

if runOnMC is True:
    process.PATCMGSequence.replace( process.muonMatch,
                                    process.tunePmuons +
                                    process.muonTrackError +
                                    process.muonMatch )
    process.muonMatch.src = "tunePmuons"

print 'cloning the jet sequence to build PU chs jets'

from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
process.PATCMGJetCHSSequence = cloneProcessingSnippet(process, process.PATCMGJetSequence, 'CHS')
process.PATCMGJetCHSSequence.insert( 0, process.ak5PFJetsCHS )
from CMGTools.Common.Tools.visitorUtils import replaceSrc
replaceSrc( process.PATCMGJetCHSSequence, 'ak5PFJets', 'ak5PFJetsCHS')
replaceSrc( process.PATCMGJetCHSSequence, 'particleFlow', 'pfNoPileUp')
jecPayload = 'AK5PFchs'
process.patJetsWithVarCHS.payload = jecPayload
process.patJetCorrFactorsCHS.payload = jecPayload
process.puJetIdCHS.jec = jecPayload
process.cmgPUJetMvaCHS.jec = jecPayload
process.selectedPatJetsCHS.cut = 'pt()>10'

# Change the soft muons? Change the MET?
###
### WW ANALYSIS - PAY ATTENTION TO THIS
###

#process.softMuonTagInfos.leptons    = "tunePmuons"
#process.softMuonTagInfosCHS.leptons = "tunePmuons"

###
### WW ANALYSIS - PAY ATTENTION TO THIS
###
#

##########################################################
######### Met Sequence: apply met phi correction #########
##########################################################
process.load("JetMETCorrections/Type1MET/pfMETsysShiftCorrections_cfi")
if runOnMC is False :
    process.pfMEtSysShiftCorr.parameter = cms.PSet(
        numJetsMin = cms.int32(-1),
        numJetsMax = cms.int32(-1),
        px = cms.string("+0.2661 + 0.3217*Nvtx"),
        py = cms.string("-0.2251 - 0.1747*Nvtx")
        )
else :
    process.pfMEtSysShiftCorr.parameter = cms.PSet(
        numJetsMin = cms.int32(-1),
        numJetsMax = cms.int32(-1),
        px = cms.string("+0.1166 + 0.0200*Nvtx"),
        py = cms.string("+0.2764 - 0.1280*Nvtx")
        )

process.patMetShiftCorrected = cms.EDProducer("CorrectedPATMETProducer",
                                              src = cms.InputTag('patMETs'),
                                              applyType1Corrections = cms.bool(True),
                                              srcType1Corrections = cms.VInputTag(
        cms.InputTag('pfMEtSysShiftCorr')),
                                              applyType2Corrections = cms.bool(False)
                                              )
process.metphiCorretionSequence = cms.Sequence(
    process.pfMEtSysShiftCorrSequence *
    process.patMetShiftCorrected
    )
process.PATCMGSequence += process.metphiCorretionSequence
patEventContentCMG+=['keep *_patMetShiftCorrected_*_*']

####################################################
######## CLEANED JET COLLECTION FOR MU-TAUH ########
####################################################
process.load('CMGTools.Common.PAT.addFilterPaths_cff')
print "Cleaning the jet collection for mu-tauh channel"
from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
process.load("ExoDiBosonResonances.PATtupleProduction.CleanJets_cff")
process.PATCMGSequence += process.primaryVertexFilter
process.PATCMGSequence += process.CleanJetsMuTauSequence
process.PATTauSequenceMuTau = cloneProcessingSnippet(process,process.PATTauSequence, "MuTau")
massSearchReplaceAnyInputTag(process.PATTauSequenceMuTau,cms.InputTag("pfJetsForHPSTauMuTau"),cms.InputTag("ak5PFJetsNoMu"))
process.PATTauSequenceMuTau.replace( process.pfJetsForHPSTauMuTau, process.primaryVertexFilter+process.CleanJetsMuTauSequence )
process.PATCMGSequence += process.PATTauSequenceMuTau
patEventContentCMG+=['keep *_*selectedPatTausMuTau*_*_*']

#####################################################
######### CLEANED JET COLLECTION FOR EL-TAUH ########
#####################################################
print "Cleaning the jet collection for ele-tauh channel"
process.PATCMGSequence += process.CleanJetsETauSequence
process.PATTauSequenceEleTau = cloneProcessingSnippet(process,process.PATTauSequence, "EleTau")
massSearchReplaceAnyInputTag(process.PATTauSequenceEleTau,cms.InputTag("pfJetsForHPSTauEleTau"),cms.InputTag("ak5PFJetsNoEle"))
process.PATTauSequenceEleTau.replace( process.pfJetsForHPSTauEleTau, process.primaryVertexFilter+process.CleanJetsETauSequence )
process.PATCMGSequence += process.PATTauSequenceEleTau
patEventContentCMG+=['keep *_*selectedPatTausEleTau*_*_*']

######################################################
###################### TAUH-TAUH #####################
######################################################
print "Adding tau collection using subjet as seed"
process.load("RecoTauTag/Configuration/boostedHPSPFTaus_cff")
process.PATTauSequenceBoosted = cloneProcessingSnippet(process,process.PATTauSequence, "Boosted")
process.produceAndDiscriminateBoostedHPSPFTaus.replace( process.PFTau, process.PFTauBoosted )
process.PATTauSequenceBoosted.replace( process.PFTauBoosted, process.produceAndDiscriminateBoostedHPSPFTaus )
process.recoTauAK5PFJets08RegionBoosted.src = cms.InputTag('boostedTauSeeds')
process.recoTauAK5PFJets08RegionBoosted.pfCandSrc = cms.InputTag('pfNoPileUpForBoostedTaus')
process.recoTauAK5PFJets08RegionBoosted.pfCandAssocMapSrc = cms.InputTag('boostedTauSeeds', 'pfCandAssocMapForIsolation')
process.ak5PFJetsLegacyHPSPiZerosBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
process.ak5PFJetsRecoTauChargedHadronsBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
process.ak5PFJetsRecoTauChargedHadronsBoosted.builders[1].dRcone = cms.double(0.3)
process.ak5PFJetsRecoTauChargedHadronsBoosted.builders[1].dRconeLimitedToJetArea = cms.bool(True)
process.combinatoricRecoTausBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
process.combinatoricRecoTausBoosted.builders[0].pfCandSrc = cms.InputTag('pfNoPileUpForBoostedTaus')
process.combinatoricRecoTausBoosted.modifiers.remove(process.combinatoricRecoTausBoosted.modifiers[3])
process.selectedPatTausBoosted.cut = 'abs(pfJetRef().eta()) < 2.3 & pfJetRef().pt() > 10.'
process.PATCMGSequence += process.PATTauSequenceBoosted
patEventContentCMG+=['keep *_*selectedPatTausBoosted*_*_*']

if runOnMC is False:
    process.PATTauSequenceBoosted.remove( process.tauMatchBoosted )
    process.PATTauSequenceBoosted.remove( process.tauGenJetsBoosted )
    process.PATTauSequenceBoosted.remove( process.tauGenJetsSelectorAllHadronsBoosted )
    process.PATTauSequenceBoosted.remove( process.tauGenJetMatchBoosted )
    process.PATTauSequenceMuTau.remove( process.tauMatchMuTau )
    process.PATTauSequenceMuTau.remove( process.tauGenJetsMuTau )
    process.PATTauSequenceMuTau.remove( process.tauGenJetsSelectorAllHadronsMuTau )
    process.PATTauSequenceMuTau.remove( process.tauGenJetMatchMuTau )
    process.PATTauSequenceEleTau.remove( process.tauMatchEleTau )
    process.PATTauSequenceEleTau.remove( process.tauGenJetsEleTau )
    process.PATTauSequenceEleTau.remove( process.tauGenJetsSelectorAllHadronsEleTau )
    process.PATTauSequenceEleTau.remove( process.tauGenJetMatchEleTau )


#######################################################################
###################### Modified PF Muon Isolation #####################
#######################################################################
# compute muon isolation sums excluding particles contained in other subjet
process.load("PhysicsTools/IsolationAlgos/boostedMuonIsolation_cff")
process.PATCMGMuonSequenceBoosted = cloneProcessingSnippet(process,process.PATCMGMuonSequence, "Boosted")
process.PATCMGMuonSequenceBoosted.replace( process.pfMuonIsolationSequenceBoosted, process.boostedMuonPFIsolationSequence )
process.muPFIsoDepositChargedForBoostedMuons.src = cms.InputTag('tunePmuons')
process.muPFIsoDepositNeutralForBoostedMuons.src = cms.InputTag('tunePmuons')
process.muPFIsoDepositGammaForBoostedMuons.src = cms.InputTag('tunePmuons')
process.muPFIsoDepositChargedAllForBoostedMuons.src = cms.InputTag('tunePmuons')
process.muPFIsoDepositPUforBoostedTauStudy.src = cms.InputTag('tunePmuons')
process.PATCMGMuonSequenceBoosted.replace( process.patMuonsBoosted, process.patBoostedMuons )
process.selectedPatMuonsBoosted.src = cms.InputTag("patBoostedMuons")
process.PATCMGSequence += process.PATCMGMuonSequenceBoosted

#######################################################################
###################### Modified PF Electron Isolation #####################
#######################################################################
#--------------------------------------------------------------------------------
# compute electron isolation sums excluding particles contained in other subjet
process.load("PhysicsTools/IsolationAlgos/boostedElectronIsolation_cff")
process.PATCMGElectronSequenceBoosted = cloneProcessingSnippet(process,process.PATElectronSequence, "Boosted")
process.PATCMGElectronSequenceBoosted.replace( process.pfElectronIsolationSequenceBoosted, process.boostedElectronPFIsolationSequence )
process.PATCMGElectronSequenceBoosted.replace( process.patElectronsBoosted, process.patBoostedElectrons )
process.PATCMGSequence += process.PATCMGElectronSequenceBoosted

# Modified Isolation from TSW
process.heepIdNoIsoBoosted = process.heepIdNoIso.clone()
process.heepIdNoIsoBoosted.barrelCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:nrMissHits:dxy")
process.heepIdNoIsoBoosted.endcapCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:nrMissHits:dxy")
process.heepIdNoIsoElesBoosted = process.heepIdNoIsoEles.clone(cutValueMap = cms.InputTag("heepIdNoIsoBoosted"),)
process.modElectronIsoBoosted = process.modElectronIso.clone(vetoGsfEles = cms.InputTag("heepIdNoIsoElesBoosted"))

process.patBoostedElectrons.userIsolation.user.append( cms.PSet(src = cms.InputTag("modElectronIso","track")) )
process.patBoostedElectrons.userIsolation.user.append( cms.PSet(src = cms.InputTag("modElectronIso","ecal")) )
process.patBoostedElectrons.userIsolation.user.append( cms.PSet(src = cms.InputTag("modElectronIso","hcalDepth1")) )

# And now redo the PAT electrons with Mathias' module
process.heepPatElectronsBoosted = process.heepPatElectrons.clone( eleLabel = cms.InputTag("patBoostedElectrons"), )
process.heepPatElectronsBoosted.barrelCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:e2x5Over5x5:nrMissHits:dxy")
process.heepPatElectronsBoosted.endcapCuts.cuts=cms.string("et:detEta:ecalDriven:dEtaIn:dPhiIn:hadem:sigmaIEtaIEta:nrMissHits:dxy")


process.PATCMGElectronSequenceBoosted.replace(process.patBoostedElectrons,
                                              process.heepIdNoIsoBoosted +
                                              process.heepIdNoIsoElesBoosted +
                                              process.modElectronIsoBoosted +
                                              process.patBoostedElectrons +
                                              process.heepPatElectronsBoosted
                                              )
# Finally, change the source of the selectedPatElectronsBoosted.
# The changes trickle down from here.
process.selectedPatElectronsBoosted.src = cms.InputTag("heepPatElectronsBoosted")
process.patBoostedElectrons.isolationValues = cms.PSet()
process.patBoostedElectrons.isolationValuesNoPFId = cms.PSet()
patEventContentCMG+=['keep *_boostedTauSeeds_*_*']
patEventContentCMG+=['keep *_ca8PFJetsCHSprunedForBoostedTaus_*_*']
patEventContentCMG+=['keep *_patMuonsWithTriggerBoosted_*_*']
patEventContentCMG+=['keep *_patElectronsWithTriggerBoosted_*_*']


########################################################
## Path definition
########################################################

process.dump = cms.EDAnalyzer('EventContentAnalyzer')

process.p = cms.Path(
    process.prePathCounter + 
    process.PATCMGSequence +
    process.PATCMGJetCHSSequence+
    process.puJetIdAK5Sequence+
    #process.puJetIdAK7Sequence+
    process.puJetIdCA8Sequence
    )

process.p += process.postPathCounter

# For testing, you can remove some of the objects:
# NOTE: there are a few dependencies between these sequences
# process.PATCMGSequence.remove(process.PATCMGPileUpSubtractionSequence)
# process.PATCMGSequence.remove(process.PATCMGRhoSequence)
# process.PATCMGSequence.remove(process.PATCMGMuonSequence)
# process.PATCMGSequence.remove(process.PATCMGElectronSequence)
# process.PATCMGSequence.remove(process.PATCMGGenSequence)
# process.PATCMGSequence.remove(process.PATCMGJetSequence)
# process.PATCMGSequence.remove(process.PATCMGTauSequence)
# process.PATCMGSequence.remove(process.PATCMGMetSequence)
# process.p.remove(process.PATCMGJetCHSSequence)
# process.p.remove(process.PATCMGTriggerSequence)
# process.p.remove(process.PATCMGPhotonSequence)
# process.p.remove(process.PATCMGVertexSequence)
# process.p.remove(process.PATCMGPhotonSequence)
# process.p.remove(process.MetSignificanceSequence)
# process.p.remove(process.PATCMGMetRegressionSequence)
# process.p.remove(process.PATCMGJetSequenceCHSpruned)

if runOnFastSim :
    process.vertexWeightSequence.remove(process.vertexWeight3DMay10ReReco)
    process.vertexWeightSequence.remove(process.vertexWeight3DMay10ReReco)
    process.vertexWeightSequence.remove(process.vertexWeight3DPromptRecov4)
    process.vertexWeightSequence.remove(process.vertexWeight3D05AugReReco)
    process.vertexWeightSequence.remove(process.vertexWeight3DPromptRecov6)
    process.vertexWeightSequence.remove(process.vertexWeight3D2invfb)
    process.vertexWeightSequence.remove(process.vertexWeight3D2011B)
    process.vertexWeightSequence.remove(process.vertexWeight3D2011AB)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall11May10ReReco)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall11PromptRecov4)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall1105AugReReco)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall11PromptRecov6)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall112invfb)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall112011B)
    process.vertexWeightSequence.remove(process.vertexWeight3DFall112011AB)


########################################################
## Skim events
########################################################


## ZToEE
process.ZToEEcand = cms.EDProducer("CandViewShallowCloneCombiner",
                                   decay = cms.string(EE),
                                   checkCharge = cms.bool(False),
                                   cut = cms.string(DILEPTON_KINCUT)
                                   )

process.ZToEEfilter = cms.EDFilter("CandViewCountFilter",
                                   src = cms.InputTag("ZToEEcand"),
                                   minNumber = cms.uint32(1) )

process.ZToEEskimSequence = cms.Sequence( process.ZToEEcand * process.ZToEEfilter )
process.ZToEEskimPath = cms.Path( process.ZToEEskimSequence )


## ZToMUMU
process.ZToMUMUcand = cms.EDProducer("CandViewShallowCloneCombiner", 
                                     decay = cms.string(MUMU),
                                     checkCharge = cms.bool(False),
                                     cut = cms.string(DILEPTON_KINCUT)  
                                     )

process.ZToMUMUfilter = cms.EDFilter("CandViewCountFilter",
                                     src = cms.InputTag("ZToMUMUcand"),
                                     minNumber = cms.uint32(1) )

process.ZToMUMUskimSequence = cms.Sequence( process.ZToMUMUcand * process.ZToMUMUfilter )
process.ZToMUMUskimPath = cms.Path( process.ZToMUMUskimSequence )



########################################################
## PAT output definition
########################################################

## Define event selection

EventSelection = cms.vstring('p')

if createPATtuple and skimElec and skimMuon:
    EventSelection = cms.vstring('ZToEEskimPath','ZToMUMUskimPath')
if createPATtuple and skimElec and not skimMuon:
    EventSelection = cms.vstring('ZToEEskimPath')
if createPATtuple and skimMuon and not skimElec:
    EventSelection = cms.vstring('ZToMUMUskimPath')

## Output Module Configuration (expects a path 'p')
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = EventSelection ),
                               outputCommands = patEventContentCMG
                               )


# needed to override the CMG format, which drops the pat taus
process.out.outputCommands.append('keep patTaus_selectedPatTaus_*_*')

#### drop collections not used by EXO-VV analysis
process.out.outputCommands.append('drop *_cmg*_*_*')
process.out.outputCommands.append('drop *_particleFlow*_*_*')
process.out.outputCommands.append('drop *_pfNoPileUp_*_*')
process.out.outputCommands.append('drop *_pfSelectedPhotons_*_*')
process.out.outputCommands.append('drop *_phPFIsoDeposit*_*_*')

#FIXME now keeping the whole event content...
# process.out.outputCommands.append('keep *_*_*_*')

process.outpath = cms.EndPath()

if createPATtuple:
    process.outpath += process.out


########################################################
## CMG output definition
########################################################

from CMGTools.Common.eventContent.patEventContentCMG_cff import everything
process.outcmg = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('cmgTuple.root'),
    SelectEvents   = cms.untracked.PSet( SelectEvents = EventSelection ),
    outputCommands = everything,
    dropMetaData = cms.untracked.string('PRIOR')
    )

if createCMGtuple:
    process.outpath += process.outcmg


########################################################
## Flat ntuple output definition
########################################################

##  ____        _                       __  __  ____
## |  _ \  __ _| |_ __ _    ___  _ __  |  \/  |/ ___|
## | | | |/ _` | __/ _` |  / _ \| '__| | |\/| | |
## | |_| | (_| | || (_| | | (_) | |    | |  | | |___
## |____/ \__,_|\__\__,_|  \___/|_|    |_|  |_|\____|

##   ____             __ _                       _     _
##  / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |__ | | ___  ___
## | |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | '_ \| |/ _ \/ __|
## | |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_) | |  __/\__ \
##  \____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|_.__/|_|\___||___/
##                         |___/



from DelPanj.TreeMaker.eSelXZh_cff import *
from DelPanj.TreeMaker.muSelXZh_global_cff import *
from DelPanj.TreeMaker.muSelXZh_tracker_cff import *

if runOnMC:
    CA8jecUnc='START53_V23_Uncertainty_AK7PFchs.txt'
    CA8jecLevels = [
        'START53_V23_L1FastJet_AK7PFchs.txt',
        'START53_V23_L2Relative_AK7PFchs.txt',
        'START53_V23_L3Absolute_AK7PFchs.txt'
        ]
    AK5jecUnc='START53_V23_Uncertainty_AK5PFchs.txt'
    AK5jecLevels = [
        'START53_V23_L1FastJet_AK5PFchs.txt',
        'START53_V23_L2Relative_AK5PFchs.txt',
        'START53_V23_L3Absolute_AK5PFchs.txt'
        ]
else :
    CA8jecUnc='FT_53_V21_AN4_Uncertainty_AK7PFchs.txt'
    CA8jecLevels = [
        'FT_53_V21_AN4_L1FastJet_AK7PFchs.txt',
        'FT_53_V21_AN4_L2Relative_AK7PFchs.txt',
        'FT_53_V21_AN4_L3Absolute_AK7PFchs.txt',
        'FT_53_V21_AN4_L2L3Residual_AK7PFchs.txt'
        ]
    AK5jecUnc='FT_53_V21_AN4_Uncertainty_AK5PFchs.txt'
    AK5jecLevels = [
        'FT_53_V21_AN4_L1FastJet_AK5PFchs.txt',
        'FT_53_V21_AN4_L2Relative_AK5PFchs.txt',
        'FT_53_V21_AN4_L3Absolute_AK5PFchs.txt',
        'FT_53_V21_AN4_L2L3Residual_AK5PFchs.txt'
        ]

process.tree = cms.EDAnalyzer(
    'TreeMaker',
    fillPUweightInfo_ = cms.bool(True),
    fillEventInfo_ = cms.bool(True),
    fillGenInfo_   = cms.bool(True),
    fillMuonInfo_  = cms.bool(True),
    fillElecInfo_  = cms.bool(True),
    fillJetInfo_   = cms.bool(True),
    fillMetInfo_   = cms.bool(True),
    fillTrigInfo_  = cms.bool(True),
    fillPhotInfo_  = cms.bool(False),
    genPartLabel=cms.InputTag("genParticles"),
    patMuons=cms.InputTag("patMuonsWithTrigger"),
    patElectrons = cms.InputTag("patElectronsWithTrigger"),
    PrunedJets=cms.InputTag("selectedPatJetsCA8CHSprunedForBoostedTaus"),
    rhoSrc = cms.InputTag('kt6PFJets', 'rho'),
    pvSrc  = cms.InputTag('goodOfflinePrimaryVertices'),
    ### CA8Jet
    CA8Jets=cms.InputTag("selectedPatJetsCA8CHSwithQJetsForBoostedTaus"),
    CA8jecPayloadNames = cms.vstring( CA8jecLevels ),
    CA8jecUncName = cms.string(CA8jecUnc),    
    ### AK5Jet
    AK5Jets=cms.InputTag("patJetsWithVarCHS"),
    AK5jecPayloadNames = cms.vstring( AK5jecLevels ),
    AK5jecUncName = cms.string(AK5jecUnc),    
    e2012IDSet  =  eSelXZh,
    globalMuonIDSet = muSelXZh_global,
    trackerMuonIDSet = muSelXZh_tracker,
    eleRhoIso = cms.InputTag("kt6PFJets","rho"),  
    patMetRaw=cms.InputTag("patMETsRaw"),
    patMet = cms.InputTag("patMetShiftCorrected"),
    outFileName=cms.string('outputFileName.root')
    )


if not runOnMC:
    process.tree.fillGenInfo_   = cms.bool(False)
    process.tree.fillPUweightInfo_ = cms.bool(False)


if createFlatTuple: 
    process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('flattuple.root')
                                       )

process.eiko_e = cms.Path(process.ZToEEcand * process.ZToEEfilter*process.tree)
process.eiko_m = cms.Path(process.ZToMUMUcand * process.ZToMUMUfilter*process.tree)
process.eiko = cms.Path(process.tree)





########################################################
## Conditions 
########################################################

process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")

### Set the global tag from
### https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions#Winter13_2012_A_B_C_D_datasets_r


if runOnMC is True:
    process.GlobalTag.globaltag = cms.string("START53_V23::All")
if runOnMC is False:
    process.GlobalTag.globaltag = cms.string("FT_53_V21_AN4::All")


print 'Global tag       : ', process.GlobalTag.globaltag
###

########################################################
## Below, stuff that you probably don't want to modify
########################################################

## Geometry and Detector Conditions (needed for a few patTuple production steps)

## Get the schedule
from CMGTools.Common.PAT.patCMGSchedule_cff import getSchedule
process.schedule = getSchedule(process, runOnMC, runOnFastSim)
## Add filters to the schedule
if createPATtuple and skimElec and skimMuon:
    process.schedule.append( process.ZToEEskimPath )
    process.schedule.append( process.ZToMUMUskimPath )

if createPATtuple and skimElec and not skimMuon:
    process.schedule.append( process.ZToEEskimPath )

if createPATtuple and skimMuon and not skimElec:
    process.schedule.append( process.ZToMUMUskimPath )

## Also add the TOBTEC Fakes Filter
process.load("KStenson.TrackingFilters.tobtecfakesfilter_cfi")
print process.tobtecfakesfilter
process.tobtecfakesfilterPath = cms.Path(~process.tobtecfakesfilter)
process.schedule.append( process.tobtecfakesfilterPath )

## Close the schedule
process.schedule.append( process.outpath )

if createFlatTuple and not skimElec and not skimMuon:
    process.schedule.append(process.eiko )
if createFlatTuple and skimElec and skimMuon:    
    process.schedule.append(process.eiko_e )
    process.schedule.append(process.eiko_m )
if createFlatTuple and skimElec and not skimMuon:
    process.schedule.append(process.eiko_e )
if createFlatTuple and skimMuon and not skimElec:
    process.schedule.append(process.eiko_m )

## MessageLogger
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter')
## Options and Output Report
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )


## Print the schedule
print process.schedule

print sep_line

print 'Fastjet instances (dominating our processing time...):'
from CMGTools.Common.Tools.visitorUtils import SeqVisitor
v = SeqVisitor('FastjetJetProducer')
process.p.visit(v)

print sep_line

print 'starting CMSSW'

if not runOnMC and isNewerThan('CMSSW_5_2_0'):
    process.pfJetMETcorr.jetCorrLabel = cms.string("ak5PFL1FastL2L3Residual")
