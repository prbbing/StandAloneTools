#!/usr/bin/env python
import os, sys, time, argparse, copy
from array import array
import math,random
import ROOT

def Global(x,par):
  return par[0]*math.pow((1-(x[0]/13000.0)),par[1])*math.pow(x[0]/13000.0,par[2])*math.pow(x[0]/13000.0, math.log(x[0]/13000.0)*par[3])*math.pow(x[0]/13000.0,math.pow(math.log(x[0]/13000.0),2)*par[4])

#def Global(x,par):
  #return par[0]*math.pow((1-(x[0]/13000.0)),par[1])*math.pow(x[0]/13000.0,par[2])*math.pow(x[0]/13000.0, math.log(x[0]/13000.0)*par[3])

Fitting = ROOT.TF1("Fitting",Global,1500,3000,5)
Fitting.SetParName(0, "norm")
Fitting.SetParName(1, "p1")
Fitting.SetParName(2, "p2")
Fitting.SetParName(3, "p3")
Fitting.SetParName(4, "p4")

initPars = [5e2, 10, -5, 0.5, -0.2]

def SetFunc(pars):
  Fitting.SetParameter(0, pars[0])
  Fitting.SetParameter(1, pars[1])
  Fitting.SetParameter(2, pars[2])
  Fitting.SetParameter(3, pars[3])
  Fitting.SetParameter(4, pars[4])

def fudge(pars, seed):
  random.seed(seed)
  for i in range(0, len(pars)):
    ran = random.uniform(-1, 1)
    pars[i] = pars[i]*(1 + ran)
  return pars

thisFitter = ROOT.Fit.Fitter()
thisFitter.Config().MinimizerOptions().SetMinimizerType("Minuit2")
thisFitter.Config().MinimizerOptions().SetMinimizerAlgorithm("Minimize")
thisFitter.Config().MinimizerOptions().SetMaxIterations(100000000)
thisFitter.Config().MinimizerOptions().SetMaxFunctionCalls(10000000)
thisFitter.Config().MinimizerOptions().SetDefaultMaxFunctionCalls(10000000)
thisFitter.Config().MinimizerOptions().SetPrintLevel(0)
thisFitter.Config().MinimizerOptions().SetTolerance(1) #minimize tolerance to reach EDM, 1 = 1e-3

output = ROOT.TFile("out_elena.root","RECREATE")
#inputFile = ROOT.TFile("/afs/cern.ch/user/b/biliu/SVJFit/FrequentistFramework/workspace/svj_s-channel/svj_s-channel_metTrigger.root")
#inputFile = ROOT.TFile("/afs/cern.ch/user/b/biliu/bh_inputs_1500.root")
inputFile = ROOT.TFile("~/public/v9p1_PFNv6_data16_CR_manualError.root")
hist = inputFile.Get("hist").Clone()
bkgHist = inputFile.Get("hist").Clone()
bkgHist.SetName("hist")

iteration = 1000

SetFunc(initPars)

for i in range(0, iteration):
  print("interation number " + str(i))
  try:
    status = hist.Fit("Fitting","IMSL")
  except TypeError or ValueError:
    print("Fit failed due to math value error.")
    continue
  print(status.Status())
  if status.Status() <= 1:
    print(initPars)
    break
  initPars = fudge(initPars, i)
  SetFunc(initPars)

output.cd()
#hist = inputFile.Get("mT_jj__QCDtestmc16e_PostSel__metTrigger").Clone()
#bkgHist = inputFile.Get("mT_jj__QCDtestmc16e_PostSel__metTrigger").Clone()
for binNum in range(bkgHist.GetXaxis().FindBin(1500), bkgHist.GetXaxis().FindBin(3000)):
  bkgHist.SetBinContent(binNum, Fitting.Integral(bkgHist.GetBinLowEdge(binNum), bkgHist.GetBinLowEdge(binNum) + bkgHist.GetBinWidth(binNum))/bkgHist.GetBinWidth(binNum))
hist.Write()
bkgHist.Write()
output.Close(
