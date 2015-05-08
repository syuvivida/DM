import FWCore.ParameterSet.Config as cms

process = cms.Process('MVAMET')
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')


process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = cms.string("PHYS14_25_V2")


option = 'RECO' # 'GEN' or 'RECO'

### GEN level studies
if option == 'GEN':
    process.load("ExoDiBosonResonances.EDBRGenStudies.genMuons_cff")
    process.load("ExoDiBosonResonances.EDBRGenStudies.genElectrons_cff")
    process.load("ExoDiBosonResonances.EDBRGenStudies.genFatJets_cff")

### RECO level studies
if option == 'RECO':
    process.load("ExoDiBosonResonances.EDBRCommon.goodMuons_cff")
    process.load("ExoDiBosonResonances.EDBRCommon.goodElectrons_cff")
    process.load("ExoDiBosonResonances.EDBRJets.redoSubstructure_cff")
    process.load("ExoDiBosonResonances.EDBRJets.redoPatJets_cff")
    process.load("ExoDiBosonResonances.EDBRCommon.goodJets_cff")
    process.load("ExoDiBosonResonances.EDBRCommon.goodAK4Jets_cff")


# Updates
if option == 'RECO':
    process.goodMuons.src = "slimmedMuons"
    process.goodElectrons.src = "isolatedElectrons"
    process.goodJets.src = "slimmedJetsAK8"
    process.goodAK4Jets.src = "slimmedJets"

process.leptonSequence = cms.Sequence(process.muSequence +
                                      process.eleSequence)



process.jetSequence = cms.Sequence(process.fatJetsSequence + process.substructureSequence + process.redoPatJets + process.ak4JetsSequence)




CA8jecUnc='jec/START53_V23_Uncertainty_AK7PFchs.txt'
CA8jecLevels = [
    'jec/START53_V23_L1FastJet_AK7PFchs.txt',
    'jec/START53_V23_L2Relative_AK7PFchs.txt',
    'jec/START53_V23_L3Absolute_AK7PFchs.txt'
    ]
AK5jecUnc='jec/START53_V23_Uncertainty_AK5PFchs.txt'
AK5jecLevels = [
    'jec/START53_V23_L1FastJet_AK5PFchs.txt',
    'jec/START53_V23_L2Relative_AK5PFchs.txt',
    'jec/START53_V23_L3Absolute_AK5PFchs.txt'
    ]


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)



# Input source
process.source = cms.Source("PoolSource",
                            secondaryFileNames = cms.untracked.vstring(),
                            fileNames = cms.untracked.vstring(
        #'file:ZPrimeToTTBarMiniAOD.root'
        'file:/afs/cern.ch/work/k/khurana/RunIIDelPanj/CMSSW_7_2_3_patch1/DYJetsMiniAOD/DYJets_100-200_1.root'
        #'/store/mc/Phys14DR/TTJets_MSDecaysCKM_central_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00C90EFC-3074-E411-A845-002590DB9262.root'
        ),
                            skipEvents = cms.untracked.uint32(0)         
                            )


process.options = cms.untracked.PSet(
    allowUnscheduled = cms.untracked.bool(True)
)



process.load("RecoJets.JetProducers.ak4PFJets_cfi")
process.ak4PFJets.src = cms.InputTag("packedPFCandidates")

from JetMETCorrections.Configuration.DefaultJEC_cff import ak4PFJetsL1FastL2L3

process.load("RecoMET.METPUSubtraction.mvaPFMET_cff")
#process.pfMVAMEt.srcLeptons = cms.VInputTag("slimmedElectrons")
process.pfMVAMEt.srcPFCandidates = cms.InputTag("packedPFCandidates")
process.pfMVAMEt.srcVertices = cms.InputTag("offlineSlimmedPrimaryVertices")

process.puJetIdForPFMVAMEt.jec =  cms.string('AK4PF')
#process.puJetIdForPFMVAMEt.jets = cms.InputTag("ak4PFJets")
process.puJetIdForPFMVAMEt.vertexes = cms.InputTag("offlineSlimmedPrimaryVertices")
process.puJetIdForPFMVAMEt.rho = cms.InputTag("fixedGridRhoFastjetAll")



# Other statements
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'PHYS14_25_V1', 'All')

from RecoMET.METProducers.PFMET_cfi import pfMet
process.pfMet = pfMet.clone(src = "packedPFCandidates")
process.pfMet.calculateSignificance = False # this can't be easily implemented on packed PF candidates at the moment


process.tree = cms.EDAnalyzer(
    'TreeMaker',
    fillPUweightInfo_ = cms.bool(True),
    fillEventInfo_ = cms.bool(True),
    fillGenInfo_   = cms.bool(True),
    fillMuonInfo_  = cms.bool(True),
    fillElecInfo_  = cms.bool(True),
    fillJetInfo_   = cms.bool(True), ## Raman switched it off
    fillMetInfo_   = cms.bool(True),
    fillTrigInfo_  = cms.bool(True),
    fillPhotInfo_  = cms.bool(False),
    fillTauInfo_   = cms.bool(True),
    genPartLabel=cms.InputTag("prunedGenParticles"),
    genJetLabel=cms.InputTag("slimmedGenJets"),
    maxNumGenPar  =  cms.uint32(30),
    patMuons=cms.InputTag("slimmedMuons"),
    patElectrons = cms.InputTag("slimmedElectrons"),
    
    #PrunedJets=cms.InputTag("ak8PFJetsCHSPrunedLinks"),
    PrunedJets=cms.InputTag("ak8PFJetsCHSPrunedMass"),
    
    pvSrc  = cms.InputTag('offlineSlimmedPrimaryVertices'),
    tauLabel_ = cms.untracked.InputTag("slimmedTaus"),
    rhoSrc = cms.InputTag('kt6PFJets','rho'),
    ### CA8Jet
    CA8Jets=cms.InputTag("cleanJets"),
    CA8jecPayloadNames = cms.vstring( CA8jecLevels ),
    CA8jecUncName = cms.string(CA8jecUnc),    
    ### AK5Jet
    #AK5Jets=cms.InputTag("ak8PFJetsCHS"),
    AK5Jets=cms.InputTag("cleanAK4Jets"),
    AK5jecPayloadNames = cms.vstring( AK5jecLevels ),
    AK5jecUncName = cms.string(AK5jecUnc),    
    
    patMetRaw=cms.InputTag("pfMet"),
    patMet = cms.InputTag("slimmedMETs"),
    #patMet = cms.InputTag("pfMet"),
    #patMetRaw=cms.InputTag("slimmedMETs"),
    outFileName=cms.string('outputFileName.root')
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("mvamet.root")
                                   )





process.analysis = cms.Path(process.leptonSequence+
                            process.jetSequence+                            
                            process.pfMVAMEtSequence+
                            process.pfMet+
                            process.tree
                            )


#print process.dumpPython()
