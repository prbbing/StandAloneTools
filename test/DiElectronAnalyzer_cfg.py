import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
import PhysicsTools.PythonAnalysis.LumiList as LumiList
myLumis = LumiList.LumiList(filename = '2015BGood.json').getCMSSWString().split(',')
process = cms.Process ('DiElectronAnalyzer')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(),
)

Tmp = open('DoubleEG2015','r+w')
for line in Tmp:
    process.source.fileNames.append(str(line))



process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
)
process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('diElectronAnalyzer.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.DiElectronAnalyzer = cms.EDAnalyzer ('DiElectronAnalyzer', 
     triggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT"),
     recoElectrons = cms.InputTag ('gedGsfElectrons','','RECO'),
     beamSpot = cms.InputTag ("offlineBeamSpot","","RECO"),
     basicElectronSelections = cms.string("((isEB && pt > 25 && gsfTrack.numberOfLostHits <= 1 && abs(deltaEtaSuperClusterTrackAtVtx) < 0.006046 && abs(deltaPhiSuperClusterTrackAtVtx) < 0.028092 && full5x5_sigmaIetaIeta < 0.009947 && hadronicOverEm < 0.045772 && abs(1/et - 1/pt) < 0.020118 && !isGap) || (isEE && pt > 25 && gsfTrack.numberOfLostHits <= 1 && abs(eta) < 2.1 && abs(deltaEtaSuperClusterTrackAtVtx) < 0.007057 && abs(deltaPhiSuperClusterTrackAtVtx) < 0.030159 && full5x5_sigmaIetaIeta < 0.028237  && hadronicOverEm < 0.067778 && abs(1/et - 1/pt) < 0.098919 && !isGap)) && abs(pfIsolationVariables.sumChargedHadronPt + max(0.0,pfIsolationVariables.sumNeutralHadronEt + pfIsolationVariables.sumPhotonEt - 0.50 * pfIsolationVariables.sumPUPt))/pt < 0.10"),
     triggerNames = cms.vstring("HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_","HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95_","HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_","HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55_","HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_","HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_","HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60_"),
#     triggerNames = cms.vstring(),
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



process.myPath = cms.Path (process.DiElectronAnalyzer)

