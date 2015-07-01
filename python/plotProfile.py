#!/usr/bin/env python
import sys
import os
import re
import time  
from math import *
import FWCore.ParameterSet.Config as cms
from ROOT import *

inputFile = TFile("./condor/DummyCondorDir/muonValAnalyzer_June30th_NoPU_DeltaR0p3.root")
outputFile = TFile("profile.root", "RECREATE")
HistogramObj = inputFile.Get("MuonValAnalyzer/deltaRVsLxy")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)
Profile = Histogram.ProfileX();

outputFile.mkdir('MuonValAnalyzer')
outputFile.cd('MuonValAnalyzer')
Profile.Write()
outputFile.Close()
inputFile.Close()

