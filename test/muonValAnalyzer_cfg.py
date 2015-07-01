import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
process = cms.Process ('MuonValAnalyzer')
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring()
)

Tmp = open('Stop200_1000mm_PU.txt','r')
for line in Tmp:
    process.source.fileNames.append(str(line))
Tmp.close()

process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
process.options = cms.untracked.PSet(
)


process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('muonValAnalyzer.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.MuonValAnalyzer = cms.EDAnalyzer ('MuonValAnalyzer', 
     genParticle = cms.InputTag("genParticles","","HLT"),
     recoTracks = cms.InputTag ('displacedGlobalMuons','','RECO'),
     primaryVertex = cms.InputTag ('offlinePrimaryVertices','','RECO'),
     beamSpot = cms.InputTag ("offlineBeamSpot","","RECO"),
     matchDelRCut = cms.double (1.0),
     matchRelPtCut = cms.double (0.5),
     LxyCut = cms.double (65),
     PtResBins = cms.vdouble (200,-1,1),
     LxyBins = cms.vdouble (100,0,100),
)



process.myPath = cms.Path (process.MuonValAnalyzer)

