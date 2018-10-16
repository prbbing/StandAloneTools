#!/usr/bin/env python
import sys
import os
import re
import time
from math import *
from ROOT import *

inputFile = TFile("path_to_file")
outputFile = TFile("path_to_output", "RECREATE")
HistogramObj = inputFile.Get("path_to_2D_hist")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)
HistogramNew = TH1F("semi-cal","semi-cal",30,0,60)
HistogramNew.SetDirectory(0)

for i in range(0,30):
    ProjectionSlice = Histogram.ProjectionY("semi-cal",i,i+1,"e");
    if(ProjectionSlice.GetEntries() > 0):
        ProjectionSlice.Fit("gaus")
        HistogramNew.SetBinContent(i+1,float(ProjectionSlice.GetFunction('gaus').GetParameter(2)))
        HistogramNew.SetBinError(i+1,float(ProjectionSlice.GetFunction('gaus').GetParError(2)))

outputFile.cd()
HistogramNew.Write()
outputFile.Close()
inputFile.Close()
