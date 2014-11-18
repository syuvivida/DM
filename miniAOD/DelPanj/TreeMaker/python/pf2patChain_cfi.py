import FWCore.ParameterSet.Config as cms

def pf2patChain(process, poolSource, runOnMC, postfix, GLOBAL_TAG):
    process.load("Configuration.StandardSequences.Geometry_cff")
    process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
    process.source = poolSource 
    process.GlobalTag.globaltag = GLOBAL_TAG
    process.load("Configuration.StandardSequences.MagneticField_cff")
    ## Standard PAT Configuration File
    process.load("PhysicsTools.PatAlgos.patSequences_cff")
    process.load("CommonTools.ParticleFlow.Isolation.pfIsolatedElectrons_cfi")
    process.pfIsolatedElectrons.isolationCut = cms.double(9999)


## Output Module Configuration (expects a path 'p')
    from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
    process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               # save only events passing the full path
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent )
                               )


    #Get a list of good primary vertices, in 42x, these are DAF vertices
    from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
    process.goodOfflinePrimaryVertices = cms.EDFilter(
        "PrimaryVertexObjectFilter",
        filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
        src=cms.InputTag('offlinePrimaryVertices')
        )
   # Configure PAT to use PF2PAT instead of AOD sources
   # this function will modify the PAT sequences. It is currently 
   # not possible to run PF2PAT+PAT and standart PAT at the same time
    from PhysicsTools.PatAlgos.tools.pfTools import *
   #runOnMC = False
    jetAlgo="AK5"
    if(runOnMC==False):
	usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=runOnMC, postfix = postfix, jetCorrections=('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute','L2L3Residual']) )
    else:
	usePF2PAT(process,runPF2PAT=True, jetAlgo=jetAlgo, runOnMC=True, postfix=postfix,jetCorrections=('AK5PFchs', ['L1FastJet','L2Relative','L3Absolute']))
    process.pfPileUpPFlow.Enable = True
    process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)
    process.pfPileUpPFlow.Vertices = cms.InputTag('goodOfflinePrimaryVertices')
    process.pfJetsPFlow.doAreaFastjet = True
    process.pfJetsPFlow.doRhoFastjet = False
    if runOnMC == False:
        # removing MC matching for standard PAT sequence
        # for the PF2PAT+PAT sequence, it is done in the usePF2PAT function
        removeMCMatchingPF2PAT( process, '' )
   #### jets ####
    process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
    process.load('RecoJets.Configuration.RecoPFJets_cff')
    ##-------- Turn-on the FastJet density calculation -----------------------
    from RecoJets.JetProducers.kt4PFJets_cfi import *
    process.kt6PFJets = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, voronoiRfact = 0.9)
    process.kt6PFJets.Rho_EtaMax = cms.double(4.5)
    process.patJetCorrFactors.rho = cms.InputTag("kt6PFJets","rho")
    #to compute FastJet rho to correct isolation (note: EtaMax restricted to 2.5)
    process.kt6PFJetsForIsolation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, voronoiRfact = 0.9 )
    process.kt6PFJetsForIsolation.Rho_EtaMax = cms.double(2.5)
    ##------- Turn-on the FastJet jet area calculation for your favorite algorithm ----
    from RecoJets.JetProducers.ak5PFJets_cfi import *
    ak5PFJets.Rho_EtaMax = cms.double(4.5)
    ak5PFJets.doAreaFastjet = True
    # Compute the mean pt per unit area (rho) from the
    # PFchs inputs
    process.kt6PFJetsPFlow = kt4PFJets.clone(
        rParam = cms.double(0.6),
        src = cms.InputTag('pfNoElectron'+postfix),
        doAreaFastjet = cms.bool(True),
        doRhoFastjet = cms.bool(True)
        )
    process.patJetCorrFactorsPFlow.rho = cms.InputTag("kt6PFJetsPFlow", "rho")
    
    
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                               vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                               minimumNDOF = cms.uint32(4) ,
                                               maxAbsZ = cms.double(15), 
                                               maxd0 = cms.double(2) 
                                               )

    process.noscraping = cms.EDFilter("FilterOutScraping",
                                      applyfilter = cms.untracked.bool(True),
                                      debugOn = cms.untracked.bool(True),
                                      numtrack = cms.untracked.uint32(10),
                                      thresh = cms.untracked.double(0.25)
                                      )
 
  # HBHENoiseFilter
    process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
    #process.HBHENoiseFilter.maxRBXEMF = cms.double(0.01)

  # Add the PV selector and KT6 producer to the sequence
    getattr(process,"patPF2PATSequence"+postfix).replace(
        getattr(process,"pfNoElectron"+postfix),
        getattr(process,"pfNoElectron"+postfix)*process.kt6PFJetsPFlow 
    )
    # top projections in PF2PAT:
    getattr(process,"pfNoPileUp"+postfix).enable = True
    getattr(process,"pfNoMuon"+postfix).enable = True
    getattr(process,"pfNoElectron"+postfix).enable = True
    getattr(process,"pfNoTau"+postfix).enable = False
    getattr(process,"pfNoJet"+postfix).enable = True
        
   # enable delta beta correction for muon selection in PF2PAT? 
    getattr(process,"pfIsolatedMuons"+postfix).doDeltaBetaCorrection = False
