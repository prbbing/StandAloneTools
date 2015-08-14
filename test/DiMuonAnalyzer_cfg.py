import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
import PhysicsTools.PythonAnalysis.LumiList as LumiList
myLumis = LumiList.LumiList(filename = '2015BGood.json').getCMSSWString().split(',')
process = cms.Process ('DiMuonAnalyzer')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(),
)

Tmp = open('DoubleMu2015','r+w')
for line in Tmp:
    process.source.fileNames.append(str(line))



process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
)
process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('diMuonAnalyzer.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.DiMuonAnalyzer = cms.EDAnalyzer ('DiMuonAnalyzer', 
     triggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT"),
     recoMuons = cms.InputTag ('muons','','RECO'),
     beamSpot = cms.InputTag ("offlineBeamSpot","","RECO"),
     basicMuonSelections = cms.string("pt > 25 && abs(eta) < 2.1 && isPFMuon && isGlobalMuon && numberOfMatchedStations > 1 && globalTrack.hitPattern.numberOfValidMuonHits > 0 && globalTrack.normalizedChi2 < 10 && abs(pfIsolationR04.sumChargedHadronPt + max(0.0,pfIsolationR04.sumNeutralHadronEt + pfIsolationR04.sumPhotonEt - 0.50 * pfIsolationR04.sumPUPt))/pt < 0.12"),
     #triggerNames = cms.vstring("HLT_DoubleMu23NoFiltersNoVtxDisplaced","HLT_DoubleMu18NoFiltersNoVtx","HLT_DoubleMu33NoFiltersNoVtx","HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10","HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10","HLT_L2DoubleMu23_NoVertex"),
     triggerNames = cms.vstring(),
     twoDHistograms = cms.VPSet(
             cms.PSet (
                     variable = cms.string("Phi_Dxy"),
                     nXBins = cms.int32(32),
                     lXBin = cms.double(-3.2),
                     hXBin = cms.double(3.2),
                     nYBins = cms.int32(1000),
                     lYBin = cms.double(-0.5),
                     hYBin = cms.double(0.5),
                      ),
     ),
     histograms = cms.VPSet(
             cms.PSet (
                     variable = cms.string("Pt"),
                     nBins = cms.int32(20),
                     lBin = cms.double(0),
                     hBin = cms.double(100)
                      ),
             cms.PSet (
                     variable = cms.string("Eta"),
                     nBins = cms.int32(25),
                     lBin = cms.double(-2.5),
                     hBin = cms.double(2.5)
                      ),
             cms.PSet (
                     variable = cms.string("Phi"),
                     nBins = cms.int32(32),
                     lBin = cms.double(-3.2),
                     hBin = cms.double(3.2)
                      ),
             cms.PSet (
                     variable = cms.string("Dxy"),
                     nBins = cms.int32(1000),
                     lBin = cms.double(-0.5),
                     hBin = cms.double(0.5)
                      ),
      ),
     pairHistograms = cms.VPSet(
             cms.PSet (
                     variable = cms.string("3DAngle"),
                     nBins = cms.int32(15),
                     lBin = cms.double(0),
                     hBin = cms.double(3)
                      ),
             cms.PSet (
                     variable = cms.string("deltaR"),
                     nBins = cms.int32(25),
                     lBin = cms.double(0),
                     hBin = cms.double(5)
                      ),
             cms.PSet (
                     variable = cms.string("chargedProduct"),
                     nBins = cms.int32(4),
                     lBin = cms.double(-2),
                     hBin = cms.double(2)
                      ),
             cms.PSet (
                     variable = cms.string("invMass"),
                     nBins = cms.int32(600),
                     lBin = cms.double(0),
                     hBin = cms.double(600)
                      ),
    ),
)



process.myPath = cms.Path (process.DiMuonAnalyzer)

