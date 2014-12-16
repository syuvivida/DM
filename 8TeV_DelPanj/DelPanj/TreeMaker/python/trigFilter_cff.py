import FWCore.ParameterSet.Config as cms
trigFilter = cms.EDFilter('TrigFilter',
   TrigTag = cms.InputTag("TriggerResults::HLT"),
   TrigPaths = cms.vstring("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v2") 
)

allPathsData =  (
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v1",
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v2",
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v3",
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v4",
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v5",
"HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v6",
"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v6",
"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v7",
"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v8",
"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v9",
"HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v10"
     )

allPathsMuEGData =  (
	"HLT_Mu17_Ele8_CaloIdL_v1",
        "HLT_Mu17_Ele8_CaloIdL_v2",
        "HLT_Mu17_Ele8_CaloIdL_v3",
        "HLT_Mu17_Ele8_CaloIdL_v4",
        "HLT_Mu17_Ele8_CaloIdL_v5",
        "HLT_Mu17_Ele8_CaloIdL_v6",
        "HLT_Mu17_Ele8_CaloIdL_v7",
        "HLT_Mu17_Ele8_CaloIdL_v8",
        "HLT_Mu17_Ele8_CaloIdL_v9",
	"HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v1",
        "HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v2",
        "HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v3",
        "HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v4",
	"HLT_Mu8_Ele17_CaloIdL_v1",
       "HLT_Mu8_Ele17_CaloIdL_v2",
        "HLT_Mu8_Ele17_CaloIdL_v3",
        "HLT_Mu8_Ele17_CaloIdL_v4",
        "HLT_Mu8_Ele17_CaloIdL_v5",
        "HLT_Mu8_Ele17_CaloIdL_v6",
        "HLT_Mu8_Ele17_CaloIdL_v7",
        "HLT_Mu8_Ele17_CaloIdL_v8",
        "HLT_Mu8_Ele17_CaloIdL_v9",
	"HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v1",
        "HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v2",
        "HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v3",
        "HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v4"

)


trigFilterData  =trigFilter.clone()
trigFilterData.TrigPaths = allPathsData

trigFilterMuEGData  =trigFilter.clone()
trigFilterMuEGData.TrigPaths = allPathsMuEGData

