#!/usr/bin/env python
import sys
import os
import re
import time
from math import *
import FWCore.ParameterSet.Config as cms
from ROOT import *

inputFile = TFile("./condor/DummyCondorDir/muonValAnalyzer_July15th_PU40STA.root")
outputFile = TFile("projection0to60PU40STA.root", "RECREATE")
HistogramObj = inputFile.Get("MuonValAnalyzer/deltaRVsLxy")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)
Projection = Histogram.ProjectionY("deltaR",0,60,"e");
Projection.Scale(1/Projection.Integral())
Projection.Rebin(5)
outputFile.mkdir('MuonValAnalyzer')
outputFile.cd('MuonValAnalyzer')
Projection.Write()
outputFile.Close()
inputFile.Close()
