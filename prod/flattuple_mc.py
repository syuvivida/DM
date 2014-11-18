import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

##  ____        _                       __  __  ____
## |  _ \  __ _| |_ __ _    ___  _ __  |  \/  |/ ___|
## | | | |/ _` | __/ _` |  / _ \| '__| | |\/| | |
## | |_| | (_| | || (_| | | (_) | |    | |  | | |___
## |____/ \__,_|\__\__,_|  \___/|_|    |_|  |_|\____|

isMC = True

##   ____             __ _                       _     _
##  / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |__ | | ___  ___
## | |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | '_ \| |/ _ \/ __|
## | |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_) | |  __/\__ \
##  \____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|_.__/|_|\___||___/
##                         |___/


if isMC:
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



process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


## Input files
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring("file:/afs/cern.ch/work/h/htong/public/patTuple_1_1_BJ0.root")
   )
 

process.badEventFilter = cms.EDFilter("HLTHighLevel",
                                      TriggerResultsTag =
                                      cms.InputTag("TriggerResults","","PAT"),
                                      HLTPaths =
                                      cms.vstring('primaryVertexFilterPath',
                                                  'noscrapingFilterPath',
                                                  'hcalLaserEventFilterPath',
                                                  'HBHENoiseFilterPath',
                                                  'trackingFailureFilterPath',
                                                  'CSCTightHaloFilterPath',
                                                  'eeBadScFilterPath',
                                                  'EcalDeadCellTriggerPrimitiveFilterPath'
                                                  ),
                                      eventSetupPathsKey = cms.string(''),
                                      andOr = cms.bool(False), # how to deal with multiple triggers: True (OR) accept if ANY is true, False (AN\                                      D) accept if ALL are true
                                      throw = cms.bool(True)   # throw exception on unknown path names
                                      )



from DelPanj.TreeMaker.eSelXZh_cff import *
from DelPanj.TreeMaker.muSelXZh_global_cff import *
from DelPanj.TreeMaker.muSelXZh_tracker_cff import *



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



if not isMC:
      process.tree.fillGenInfo_   = cms.bool(False)
      process.tree.fillPUweightInfo_ = cms.bool(False)


process.TFileService = cms.Service("TFileService",
        fileName = cms.string('output.root')
)

process.p = cms.Path(process.tree)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
