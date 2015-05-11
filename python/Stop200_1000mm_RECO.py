# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --conditions auto:run2_mc -s RAW2DIGI,L1Reco,RECO,EI,VALIDATION,DQM --datatier GEN-SIM-RECO,DQMIO -n 10 --magField 38T_PostLS1 --eventcontent RECOSIM,DQM --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('CommonTools.ParticleFlow.EITopPAG_cff')
process.load('Configuration.StandardSequences.Validation_cff')
process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:Stop200_1000mm_HLT.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step3 nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('Stop200_1000mm_RECO.root'),
    outputCommands = process.RECOSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DQMIO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('Stop200_1000mm_DQM.root'),
    outputCommands = process.DQMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.playback = True
process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.eventinterpretaion_step = cms.Path(process.EIsequence)
process.prevalidation_step = cms.Path(process.prevalidation)
process.dqmoffline_step = cms.Path(process.DQMOffline)
process.validation_step = cms.EndPath(process.validation)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)
process.DQMoutput_step = cms.EndPath(process.DQMoutput)
process.MuonTrackValidation = cms.Sequence(cms.SequencePlaceholder("mix")+process.recoMuonValidation)
process.MYvalidation_step = cms.EndPath(process.MuonTrackValidation)


process.displacedGlobalMuons.TrackerCollectionLabel = cms.InputTag("displacedTracks")
process.displacedGlobalMuons.MuonCollectionLabel = cms.InputTag("displacedStandAloneMuons","")
process.muonSeededSeedsOutInDisplaced.fromVertex = cms.bool(False)
process.muonSeededSeedsOutInDisplaced.src = cms.InputTag("earlyDisplacedMuons")
process.earlyDisplacedMuons.inputCollectionLabels = cms.VInputTag(cms.InputTag("earlyGeneralTracks"),cms.InputTag("displacedStandAloneMuons",""))
process.muonSeededTrackCandidatesOutInDisplaced.reverseTrajectories = cms.bool( True )
process.displacedGlbMuonTrackVMuonAssoc.maxVertpos = cms.double(100)
process.displacedGlbMuonTrackVMuonAssoc.nintVertpos = cms.int32(100)
process.displacedGlbMuonTrackVMuonAssoc.maxDxy = cms.double(100)
process.displacedGlbMuonTrackVMuonAssoc.minDxy = cms.double(-100)
process.displacedGlbMuonTrackVMuonAssoc.nintDxy = cms.int32(200)
process.displacedGlbMuonTrackVMuonAssoc.maxpT = cms.double(200)
process.displacedGlbMuonTrackVMuonAssoc.minpT = cms.double(0)
process.displacedGlbMuonTrackVMuonAssoc.nintpT = cms.int32(40)
process.displacedGlbMuonTrackVMuonAssoc.ptMinTP = cms.double(10)
process.displacedTrackVMuonAssoc.maxVertpos = cms.double(100)
process.displacedTrackVMuonAssoc.nintVertpos = cms.int32(100)
process.displacedTrackVMuonAssoc.maxDxy = cms.double(100)
process.displacedTrackVMuonAssoc.minDxy = cms.double(-100)
process.displacedTrackVMuonAssoc.nintDxy = cms.int32(200)
process.displacedTrackVMuonAssoc.maxpT = cms.double(200)
process.displacedTrackVMuonAssoc.minpT = cms.double(0)
process.displacedTrackVMuonAssoc.nintpT = cms.int32(40)
process.displacedTrackVMuonAssoc.ptMinTP = cms.double(10)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.MYvalidation_step,process.DQMoutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.postLS1Customs
from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1 

#call to customisation function customisePostLS1 imported from SLHCUpgradeSimulations.Configuration.postLS1Customs
process = customisePostLS1(process)

# Automatic addition of the customisation function from SimGeneral.MixingModule.fullMixCustomize_cff
from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn 

#call to customisation function setCrossingFrameOn imported from SimGeneral.MixingModule.fullMixCustomize_cff
process = setCrossingFrameOn(process)

# End of customisation functions

