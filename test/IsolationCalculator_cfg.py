import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
import PhysicsTools.PythonAnalysis.LumiList as LumiList
myLumis = LumiList.LumiList(filename = '2015DGood.txt').getCMSSWString().split(',')
process = cms.Process ('IsolationCalculator')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValZMM_13/MINIAODSIM/PU25ns_75X_mcRun2_asymptotic_v3-v1/00000/8C340B82-A63D-E511-8807-0025905A48BC.root',
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValZMM_13/MINIAODSIM/PU25ns_75X_mcRun2_asymptotic_v3-v1/00000/A0F3B284-A63D-E511-9D2F-0025905964A2.root',
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValDisplacedSUSY_stopToBottom_M_300_1000mm_13/MINIAODSIM/75X_mcRun2_asymptotic_v3-v1/00000/8E48A8DE-5A3D-E511-A02A-0025905A60BC.root',
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValDisplacedSUSY_stopToBottom_M_300_1000mm_13/MINIAODSIM/75X_mcRun2_asymptotic_v3-v1/00000/68FD7CDF-5A3D-E511-90DC-0025905A48D6.root',
    #'file:/data/users/bing/condor/ZControl2015_Aug27th/DoubleMu_2015B/zMuMu/skim_30.root',
    #'root://cmsxrootd.fnal.gov//store/data/Run2015C/SingleMuon/MINIAOD/PromptReco-v1/000/254/227/00000/82DAA750-C945-E511-9429-02163E014229.root',
    #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring15DR74/DisplacedSUSY_StopToBL_M-500_CTau-100_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/70000/9212879F-3B0D-E511-B6FD-0025907DCA7E.root',
    ),
)

Tmp = open('DoubleMu2015DMiniAOD','r+w')
for line in Tmp:
    process.source.fileNames.append(str(line))

process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
)
process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('IsolationCalculator.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.IsolationCalculator = cms.EDAnalyzer ('IsolationCalculator',
  New = cms.bool(False),
  match = cms.double(5),
  veto = cms.double(99999999),
  vertex = cms.InputTag("offlineSlimmedPrimaryVertices", "", "PAT"),
)



process.myPath = cms.Path (process.IsolationCalculator)

