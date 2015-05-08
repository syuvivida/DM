import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

process.GlobalTag.globaltag = cms.string("PHYS14_25_V1::All")

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

process.jetSequence = cms.Sequence(process.fatJetsSequence + process.ak4JetsSequence)




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
    fillTauInfo_   = cms.bool(True),
    genPartLabel=cms.InputTag("prunedGenParticles"),
    genJetLabel=cms.InputTag("slimmedGenJets"),
    maxNumGenPar  =  cms.uint32(30),
    patMuons=cms.InputTag("slimmedMuons"),
    patElectrons = cms.InputTag("slimmedElectrons"),
    PrunedJets=cms.InputTag("ak8PFJetsCHSPrunedLinks"),
    pvSrc  = cms.InputTag('offlineSlimmedPrimaryVertices'),
    tauLabel_ = cms.untracked.InputTag("slimmedTaus"),
    rhoSrc = cms.InputTag('kt6PFJets','rho'),
    ### CA8Jet
    CA8Jets=cms.InputTag("cleanJets"),
    CA8jecPayloadNames = cms.vstring( CA8jecLevels ),
    CA8jecUncName = cms.string(CA8jecUnc),    
    ### AK5Jet
    AK5Jets=cms.InputTag("cleanAK4Jets"),
    AK5jecPayloadNames = cms.vstring( AK5jecLevels ),
    AK5jecUncName = cms.string(AK5jecUnc),    
    patMetRaw=cms.InputTag("slimmedMETs"),
    patMet = cms.InputTag("slimmedMETs"),
    outFileName=cms.string('outputFileName.root')
    )


process.analysis = cms.Path(process.leptonSequence +
                            process.jetSequence +
                            process.tree)

#process.printing = cms.Path(process.printTree)


### Source


process.source = cms.Source("PoolSource",
                            fileNames =cms.untracked.vstring(
        'file:/afs/cern.ch/work/s/syu/public/miniAOD/RSGravToZZ_kMpl01_M-4500_Tune4C_13TeV-pythia8_Phys14DR.root'
        )
                            )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.MessageLogger.cerr.FwkReport.limit = 99999999

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("my.root")
                                   )
