#!/usr/bin/env python
import sys
import os
import re
import time
from math import *
import FWCore.ParameterSet.Config as cms
from ROOT import *

inputFile = TFile("/home/bing/DisplacedMuon/CMSSW_7_5_0_pre5/src/OSUT3Analysis/MuonValTool/test/muonValAnalyzer_July18th_STA_PU.root")
outputFile = TFile("ptResSTA_PU_Lxy.root", "RECREATE")
HistogramObj = inputFile.Get("MuonValAnalyzer/ptRes_Lxy")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)
HistogramNew = TH1F("ptRes","ptRes",30,0,60)
HistogramNew.SetDirectory(0)

Projection = Histogram.ProjectionY("ptRes_overall",0,60,"e");
Projection.Fit("gaus")

for i in range(0,30):
    ProjectionSlice = Histogram.ProjectionY("ptRes",i,i+1,"e");
    if(ProjectionSlice.GetEntries() > 0):
        ProjectionSlice.Fit("gaus")
        HistogramNew.SetBinContent(i+1,float(ProjectionSlice.GetFunction('gaus').GetParameter(2)))
        HistogramNew.SetBinError(i+1,float(ProjectionSlice.GetFunction('gaus').GetParError(2)))

outputFile.mkdir('MuonValAnalyzer')
outputFile.cd('MuonValAnalyzer')
HistogramNew.Write()
Projection.Write()
outputFile.Close()
inputFile.Close()
