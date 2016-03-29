#!/usr/bin/env python

import sys
import os
import re
from math import *
from array import *
from ROOT import TFile  
from DataFormats.FWLite import Events, Handle, Runs

for dataset in split_datasets:
    datasetName = dataset_names[dataset] 
    fileList = os.popen('das_client.py --query="file dataset=' + datasetName + '" --limit 0').read().split('\n') 
    xrootdPath = 'root://cmsxrootd.fnal.gov/' + str(fileList[0])
    f = TFile.Open(xrootdPath)  # Check that file is valid  
    if not f:
        print "ERROR:  Could not open file: ", xrootdPath  
        continue  
    runs = Runs (xrootdPath)
    handle  = Handle ('GenRunInfoProduct')
    label = ("generator",'','SIM')
    for run in runs:
        run.getByLabel (label, handle)
        runGenInfo = handle.product() 
        print "crossSection of      " + str(dataset) + " is: " + str(runGenInfo.crossSection())
        print "internal xsec of     " + str(dataset) + " is: " + str(runGenInfo.internalXSec().value())
        print "external LO xsec of  " + str(dataset) + " is: " + str(runGenInfo.externalXSecLO().value())
        print "external NLO xsec of " + str(dataset) + " is: " + str(runGenInfo.externalXSecNLO().value())
        print "filterEfficiency of  " + str(dataset) + " is: " + str(runGenInfo.filterEfficiency())
    content += "    " + ("'"+dataset+"'").ljust(35) 
    content += ": "   + (str(runGenInfo.crossSection()) + "*" + str(runGenInfo.filterEfficiency())).rjust(20)
    content += ", \n"
    del datasetName
    del fileList
    del xrootdPath
