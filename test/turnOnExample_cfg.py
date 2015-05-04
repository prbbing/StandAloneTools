import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
process = cms.Process ('TurnOnAnalyzer')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
    'file:/data/users/bing/Stop800_1000mm_2015HLTV21RECO.root',
    )
)

process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
    #  SkipEvent = cms.untracked.vstring('ProductNotFound')
)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('recoAnalyzer.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (100000)
)
process.TurnOnAnalyzer = cms.EDProducer ('TurnOnAnalyzer', 
     triggerSummaryLabel = cms.InputTag("hltTriggerSummaryAOD", "", "HLT2"),
     triggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT2"),
     genParticle = cms.InputTag("genParticles","","HLT"),
     recoMuons = cms.InputTag ('muons','','RECO'),
     primaryVertex = cms.InputTag ('offlinePrimaryVertices','','RECO'),
     l3Muons = cms.InputTag ("hltL3NoFiltersMuonCandidates","","HLT2"),
     l2Muons = cms.InputTag ("hltL2MuonCandidates","","HLT2"),
     d0cut = cms.double(999),
     L2Eff = cms.bool(False),
     L3Eff = cms.bool(True),
     FilterEff = cms.bool(False),
     filterTag = cms.string("HLT2"),
     beamSpot = cms.InputTag ("hltOnlineBeamSpot","","HLT2"),
     histogramSets = cms.VPSet(
                   cms.PSet (
                     setName = cms.string("Muon"),
                     #filters = cms.vstring("hltDimuonL3Filtered33","hltDimuonL2Filtered10"),#"hltDimuonL2Filtered10"),
                     #filters = cms.vstring("hltDimuonL3Filtered33","hltDimuonL2Filtered10","hltDimuonL1Filtered0"),#"hltDimuonL2Filtered10"),
                     filters = cms.vstring("hltDimuonL3Filtered33"),
                     #filters = cms.vstring("hltL3pfL1DoubleMu10MuOpenOR3p5L1f0L2pf0L3PreFiltered25"),
                     hists = cms.VPSet (
                       cms.PSet (
                           name = cms.string("Pt"),
                           binning = cms.vdouble(20,0,100),
                     ),
                       cms.PSet (
                           name = cms.string("Pt_D0"),
			   binning = cms.vdouble(10,0,1000,250,0,1000),
                     ),
                       cms.PSet (
                           name = cms.string("D0"),
                           binning = cms.vdouble(100,0,100),
                     ),
                 ),
              ),
    ),
)



process.myPath = cms.Path (process.TurnOnAnalyzer)

#out = cms.OutputModule ("PoolOutputModule",
#          fileName = cms.untracked.string ('/data/users/bing/SignalSkim800.root'),
#          SelectEvents = cms.untracked.PSet (
#            SelectEvents = cms.vstring ('myFilterPath')
#          ),
#          outputCommands = cms.untracked.vstring (
#            'keep *',
#          ),
#        )
#myFilter = cms.EDFilter ('SimpleFilter', src = cms.InputTag ('TriggerEfficiencyAnalyzer','Pass'))
          

#myFilterPath = cms.Path (myFilter)
#myEndPath = cms.EndPath (out)
#process.__setattr__ ("out", out)
#process.__setattr__ ("myFilter", myFilter)
#process.__setattr__ ("myFilterPath", myFilterPath)
#process.__setattr__ ("myEndPath", myEndPath)



