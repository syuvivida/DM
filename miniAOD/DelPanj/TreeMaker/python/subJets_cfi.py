process.pfCHS = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
## Fat PFJets
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
process.ak8PFJetsCHS = ak4PFJets.clone(
    rParam = cms.double(0.8),
    src = cms.InputTag("pfCHS"),
    doAreaFastjet = cms.bool(True),
    jetPtMin = cms.double(50.)
)
## Pruned fat PFJets (two jet collections are produced, fat jets and subjets)
from RecoJets.JetProducers.ak5PFJetsPruned_cfi import ak5PFJetsPruned
process.ak8PFJetsCHSPruned = ak5PFJetsPruned.clone(
    rParam = cms.double(0.8),
    src = cms.InputTag("pfCHS"),
    doAreaFastjet = cms.bool(True),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets"),
    jetPtMin = cms.double(50.)
)


#################################################
## Make PAT jets
#################################################

## b-tag discriminators
bTagDiscriminators = [
    'pfTrackCountingHighEffBJetTags',
    'pfTrackCountingHighPurBJetTags',
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfSimpleSecondaryVertexHighEffBJetTags',
    'pfSimpleSecondaryVertexHighPurBJetTags',
    'pfCombinedSecondaryVertexBJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags'
]


from PhysicsTools.PatAlgos.tools.jetTools import *
## PATify fat jets
addJetCollection(
    process,
    labelName = 'AK8PFCHS',
    jetSource = cms.InputTag('ak8PFJetsCHS'),
    algo = 'ak',  # needed for jet flavor clustering
    rParam = 0.8, # needed for jet flavor clustering
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    pfCandidates = cms.InputTag('packedPFCandidates'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    btagDiscriminators = bTagDiscriminators,
    jetCorrections = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
    genJetCollection = cms.InputTag('ak8GenJetsNoNu')
)
getattr(process,'patJetPartons').particles = cms.InputTag('prunedGenParticles')
getattr(process,'patJetPartonMatchAK8PFCHS').matched = cms.InputTag('prunedGenParticles')
if hasattr(process,'pfInclusiveSecondaryVertexFinderTagInfosAK8PFCHS'):
    getattr(process,'pfInclusiveSecondaryVertexFinderTagInfosAK8PFCHS').extSVCollection = cms.InputTag('slimmedSecondaryVertices')
getattr(process,'patJetsAK8PFCHS').addAssociatedTracks = cms.bool(False) # needs to be disabled since there is no track collection present in MiniAOD
getattr(process,'patJetsAK8PFCHS').addJetCharge = cms.bool(False)        # needs to be disabled since there is no track collection present in MiniAOD
## PATify pruned fat jets
addJetCollection(
    process,
    labelName = 'AK8PFCHSPruned',
    jetSource = cms.InputTag('ak8PFJetsCHSPruned'),
    btagDiscriminators = ['None'],
    jetCorrections = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
    genJetCollection = cms.InputTag('ak8GenJetsNoNu'),
    getJetMCFlavour = False # jet flavor disabled
)
getattr(process,'patJetPartonMatchAK8PFCHSPruned').matched = cms.InputTag('prunedGenParticles')

## PATify pruned subjets
addJetCollection(
    process,
    labelName = 'AK8PFCHSPrunedSubjets',
    jetSource = cms.InputTag('ak8PFJetsCHSPruned','SubJets'),
    algo = 'ak',  # needed for subjet flavor clustering
    rParam = 0.8, # needed for subjet flavor clustering
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    pfCandidates = cms.InputTag('packedPFCandidates'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    btagDiscriminators = bTagDiscriminators,
    jetCorrections = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
    genJetCollection = cms.InputTag('ak8GenJetsNoNuPruned','SubJets'),
    explicitJTA = True,  # needed for subjet b tagging
    svClustering = True, # needed for subjet b tagging
    fatJets=cms.InputTag('ak8PFJetsCHS'),             # needed for subjet flavor clustering
    groomedFatJets=cms.InputTag('ak8PFJetsCHSPruned') # needed for subjet flavor clustering
)
getattr(process,'patJetPartonMatchAK8PFCHSPrunedSubjets').matched = cms.InputTag('prunedGenParticles')
if hasattr(process,'pfInclusiveSecondaryVertexFinderTagInfosAK8PFCHSPrunedSubjets'):
    getattr(process,'pfInclusiveSecondaryVertexFinderTagInfosAK8PFCHSPrunedSubjets').extSVCollection = cms.InputTag('slimmedSecondaryVertices')
getattr(process,'patJetsAK8PFCHSPrunedSubjets').addAssociatedTracks = cms.bool(False) # needs to be disabled since there is no track collection present in MiniAOD
getattr(process,'patJetsAK8PFCHSPrunedSubjets').addJetCharge = cms.bool(False)        # needs to be disabled since there is no track collection present in MiniAOD

## Establish references between PATified fat jets and subjets using the BoostedJetMerger
process.selectedPatJetsAK8PFCHSPrunedPacked = cms.EDProducer("BoostedJetMerger",
    jetSrc=cms.InputTag("selectedPatJetsAK8PFCHSPruned"),
    subjetSrc=cms.InputTag("selectedPatJetsAK8PFCHSPrunedSubjets")
)

from PhysicsTools.PatAlgos.tools.pfTools import *
## Adapt primary vertex collection
adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))

#################################################

## Let it run
process.subJets = cms.Sequence(process.selectedPatJetsAK8PFCHS + process.selectedPatJetsAK8PFCHSPrunedPacked)
