import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import math
import os
import sys
from Geometry.CSCGeometry.cscGeometry_cfi import *

process = cms.Process ('NoBPTXAnalyzer')
process.load("Geometry.MuonCommonData.muonIdealGeometryXML_cfi")
process.load ('FWCore.MessageService.MessageLogger_cfi')
process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.GlobalPositionRcd = cms.ESSource("PoolDBESSource",
        process.CondDBSetup,
        connect = cms.string("frontier://FrontierProd/CMS_COND_310X_ALIGN"),
        toGet = cms.VPSet(cms.PSet(record = cms.string("GlobalPositionRcd"), tag = cms.string("GlobalAlignment_v4_offline"))))
process.CSCAlignmentRcd  = cms.ESSource("PoolDBESSource",
        process.CondDBSetup,
        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ALIGNMENT"),
        toGet = cms.VPSet(cms.PSet(record = cms.string("CSCAlignmentRcd"), tag = cms.string("CSCAlignment_2009_v1_offline"))))

process.CSCAlignmentErrorRcd  = cms.ESSource("PoolDBESSource",
        process.CondDBSetup,
        connect = cms.string("frontier://FrontierProd/CMS_COND_31X_ALIGNMENT"),
        toGet = cms.VPSet(cms.PSet(record = cms.string("CSCAlignmentErrorRcd"), tag = cms.string("CSCIdealGeometryErrors310me42_mc"))))

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
    #'file:/data/users/bing/Stop200_1000mm_Reco.root',
    'root://cmsxrootd.fnal.gov//store/data/Run2012D/NoBPTX/RECO/22Jan2013-v1/20000/0023450E-EE89-E211-B6E3-003048673F3A.root',
    )
)

process.TFileService = cms.Service ('TFileService',
    fileName = cms.string ('NoBPTX_test.root')
)
process.maxEvents = cms.untracked.PSet (
    input = cms.untracked.int32 (-1)
)
process.NoBPTXAnalyzer = cms.EDAnalyzer ('NoBPTXAnalyzer', 
     triggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT"),
     recoMuons = cms.InputTag ('muons','','RECO'),
     cscSegments = cms.InputTag ('cscSegments','','RECO'),
    )
process.myPath = cms.Path (process.NoBPTXAnalyzer)
