import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
import PhysicsTools.PythonAnalysis.LumiList as LumiList
myLumis = LumiList.LumiList(filename = '2015BGood.json').getCMSSWString().split(',')
process = cms.Process ('IsolationCalculator')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
    'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValZMM_13/MINIAODSIM/PU25ns_75X_mcRun2_asymptotic_v3-v1/00000/8C340B82-A63D-E511-8807-0025905A48BC.root'
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValDisplacedSUSY_stopToBottom_M_300_1000mm_13/MINIAODSIM/75X_mcRun2_asymptotic_v3-v1/00000/8E48A8DE-5A3D-E511-A02A-0025905A60BC.root',
    #'root://cmsxrootd.fnal.gov//store/relval/CMSSW_7_5_1/RelValDisplacedSUSY_stopToBottom_M_300_1000mm_13/MINIAODSIM/75X_mcRun2_asymptotic_v3-v1/00000/68FD7CDF-5A3D-E511-90DC-0025905A48D6.root',
    #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring15DR74/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v3/10000/009D49A5-7314-E511-84EF-0025905A605E.root',
    #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring15DR74/DisplacedSUSY_StopToBL_M-300_CTau-1000_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v1/00000/06232938-BA10-E511-B05B-0025905A60A6.root',
    ),
)

process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
)
#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
#process.source.lumisToProcess.extend(myLumis)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('IsolationCalculator.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.IsolationCalculator = cms.EDAnalyzer ('IsolationCalculator',
  New = cms.bool(True),
  match = cms.double(23),
  veto = cms.double(99999999),
  vertices = cms.InputTag("offlineSlimmedPrimaryVertices","","PAT"),
)



process.myPath = cms.Path (process.IsolationCalculator)

