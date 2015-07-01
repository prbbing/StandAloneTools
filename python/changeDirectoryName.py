#!/usr/bin/env python
import sys
import os
import re
import time  
from math import *
import FWCore.ParameterSet.Config as cms
from ROOT import *

inputFile = TFile("./condor/DummyCondorDir/Stop200_1000mm_BaseLine_DQM_750.root")
outputFile = TFile("Stop200_1000mm_BaseLine_DQM_750_new.root", "RECREATE")
HistogramObj = inputFile.Get("DQMData/Run 1/Muons/Run summary/RecoMuonV/MultiTrack/displacedGlobalMuons/effic_vs_vertpos")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)

outputFile.mkdir("MuonValAnalyzer")
outputFile.cd("MuonValAnalyzer")
Histogram.Write()

outputFile.Close()
