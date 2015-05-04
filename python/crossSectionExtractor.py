#!/usr/bin/env python
#it can be used within the OSUT3Analysis framework. 
import sys
import os
import re
from math import *
from array import *
from ROOT import *
from DataFormats.FWLite import Events, Handle, Runs
from OSUT3Analysis.Configuration.configurationOptions import *
from OSUT3Analysis.Configuration.processingUtilities import *
from OSUT3Analysis.Configuration.formattingUtilities import *

datasets = [
     #'DYJetsToLL_50_MiniAOD',
     #'DYToMuMu_50_MiniAOD',
     #'DYToEE_50_MiniAOD',
     #'DYToMuMu_MiniAOD',
     #'DYToEE_MiniAOD',
     #'WJetsToLNu_MiniAOD',
     #'WToENu_MiniAOD',
     #'WToMuNu_MiniAOD',
     #'Top_MiniAOD',
     #'QCD_MiniAOD',
     'QCD_MuEnriched_MiniAOD',
     'QCD_bcToE_MiniAOD',
     'QCD_MuEnriched_20toInf_MiniAOD',
]
split_datasets = split_composite_datasets(datasets, composite_dataset_definitions)

for dataset in split_datasets:
    datasetName = dataset_names[dataset]
    theFirstFile = os.popen('das_client.py --query="file dataset=' + datasetName + '" --limit 0').read().split('\n')
    xrootdPath = 'root://cmsxrootd.fnal.gov/' + str(theFirstFile[0])
    runs = Runs (xrootdPath)
    handle  = Handle ('GenRunInfoProduct')
    label = ("generator",'','SIM')
    for run in runs:
        run.getByLabel (label, handle)
        runGenInfo = handle.product()
        print "crossSection of " + str(dataset) + " is: " + str(runGenInfo.crossSection())
        print "filterEfficiency of " + str(dataset) + " is: " + str(runGenInfo.filterEfficiency())
    del datasetName
    del theFirstFile
    del xrootdPath
