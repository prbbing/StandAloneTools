#!/usr/bin/env python
import sys
import os
import re
import time
from array import array
import math
import FWCore.ParameterSet.Config as cms
from ROOT import *

def FittingFunction(x,par):
    return par[2] / (1 + math.exp(-par[1]*(x[0] - par[0])))

inputFile = TFile("/home/bing/DisplacedMuon/CMSSW_7_5_0_pre5/src/OSUT3Analysis/MuonValTool/test/dataAnalyzer_July14th_Mu38_FromSingleMuSeed.root")
outputFile = TFile("FittedEfficiency.root", "RECREATE")
HistogramObj = inputFile.Get("QuickTriggerAnalyzer/muonMatchedPt")
Histogram = HistogramObj.Clone()
Histogram.SetDirectory(0)

Fitting = TF1("Fitting",FittingFunction,0,500,3)
Fitting.SetParName(0, "midpoint")
Fitting.SetParName(1, "steepness")
Fitting.SetParName(2, "max")
Fitting.SetParameter(0, 120)
Fitting.SetParameter(1, 0.1)
Fitting.SetParameter(2, 1)
Histogram.Fit(Fitting)

outputFile.mkdir('FittedEfficiency')
outputFile.cd('FittedEfficiency')
Histogram.Write()
outputFile.Close()
inputFile.Close()
