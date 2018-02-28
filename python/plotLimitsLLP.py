#!/usr/bin/python

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
import math

from ROOT import *
from array import array
import os, sys, glob

from optparse import OptionParser, OptionGroup

doVerbose = False

ROOT.gStyle.SetPalette(1)
ROOT.gROOT.SetStyle('ATLAS')
ROOT.gROOT.ForceStyle()

ROOT.gStyle.SetPaintTextFormat('1.3f')
ROOT.gStyle.SetLineStyleString(11,"27 15")

ROOT.gStyle.SetOptStat(0)

luminosity = 36074.56

dictModels = {}
dictModels[2]  = ["Type I",                                  "fig6" ]  # a->SM, Type 1
dictModels[3]  = ["Type II, tan#beta = 0.5",                 "fig7a"]  # a->SM, Type 2, tanb 0.5
dictModels[4]  = ["Type II, tan#beta = 5",                   "fig7b"]  # a->SM, Type 2, tanb 5
dictModels[5]  = ["Type III, tan#beta = 0.5",                "fig8a"]  # a->SM, Type 3, tanb 0.5
dictModels[6]  = ["Type III, tan#beta = 5",                  "fig8b"]  # a->SM, Type 3, tanb 5
dictModels[7]  = ["Type IV, tan#beta = 0.5",                 "fig9a"]  # a->SM, Type 4, tanb 0.5
dictModels[8]  = ["Type IV, tan#beta = 5",                   "fig9b"]  # a->SM, Type 4, tanb 5
dictModels[9]  = ["Type II, tan#beta = 0.5, #alpha = 0.1",   "fig10a"] # s->SM, Type 2, tanb 0.5, alpha = 0.1
dictModels[10] = ["Type III, tan#beta = 0.5, #alpha = -1.4", "fig10b"] # s->SM, Type 3, tanb 0.5, alpha = -1.4
dictModels[11] = ["Type IV, tan#beta = 0.5, #alpha = 0.1",   "fig10c"] # s->SM, Type 4, tanb 0.5, alpha = 0.1

def getGraphAsymmErrors2D(central, up, down, color, fillStyle):
  x = []
  y = []
  eXLow = []
  eXHigh = []
  eYLow = []
  eYHigh = []
 
  for i in range (0, central.GetN ()):
    xPoint = Double (0.0)
    yPoint = Double (0.0)
    upXPoint = Double (0.0)
    upYPoint = Double (0.0)
    downXPoint = Double (0.0)
    downYPoint = Double (0.0)
    central.GetPoint (i, xPoint, yPoint)
    up.GetPoint (i, upXPoint, upYPoint)
    down.GetPoint (i, downXPoint, downYPoint)
    x.append (xPoint)
    y.append (yPoint)
    eXHigh.append (0.0)
    eXLow.append (0.0)
    eYHigh.append (upYPoint)
    eYLow.append (downYPoint)
  for i in range (0, len (x)):
    eYHigh[i] -= y[i]
    eYLow[i] = y[i] - eYLow[i]
  
  #Create TGraphAsymmErrors
  newGraph = TGraphAsymmErrors (len (x), array ('d', x), array ('d', y), array ('d', eXLow), array ('d', eXHigh), array ('d', eYLow), array ('d', eYHigh))
  
  newGraph.SetMarkerColor(color)
  newGraph.SetFillColor(color)
  newGraph.SetLineColor(color)
  newGraph.SetFillStyle(fillStyle)
  return newGraph


def applyBRsToSM(graph, mode, directory, dictToUpdate):

    numPoints = graph.GetN()

    f = open("%s/TRexFitterUtils/branchingRatios/%s/bb.dat"%(directory, dictModels[mode][1]))
    lines = f.readlines()
    BRvalues = {}
    for l in lines:
        mass, BR = float(l.split()[0]), float(l.split()[1])
        BRvalues[mass] = BR
    f.close()

    for point in range(1, numPoints+1):
        x, y = ROOT.Double(-1), ROOT.Double(-1)
        graph.GetPoint(point, x, y)
        if x in BRvalues.keys():
            graph.SetPoint(point, x, y/(BRvalues[x]**2))
            dictToUpdate[int(x)] = y/(BRvalues[x]**2)

    return graph, dictToUpdate


#____________________________________________________________________________
def HaaExclusion(ctauRange=[60], rootFileNames=['path/to/file.root'], strategy="default", mode=0, doLogY=False, blinded=False, crossSection=None, crossSection_err=None, model="WH", saveAs=None, statOnlyGraph=None, SLOnlyGraph=None, DLOnlyGraph=None, otherGraph=None, mass = 20):

    if saveAs is not None:
        gROOT.SetBatch(1)

    outputFile = TFile(model + str(mass) + ".root","RECREATE")
    canvas = TCanvas("canvas", "A scan of #mu_{sig} versus c#tau_{a}", 0, 0, 600, 500)
    canvas.cd()

    dictObsLimit   = {}
    dictExpLimit   = {}
    dictExp1pLimit = {}
    dictExp1mLimit = {}
    dictExp2pLimit = {}
    dictExp2mLimit = {}

    maximum = 0.
    for i in range(len(ctauRange)):
        inFile = TFile(rootFileNames[i])
        histo = inFile.Get("limit").Clone()
        dictObsLimit[ctauRange[i]]   = histo.GetBinContent(1)
        dictExpLimit[ctauRange[i]]   = histo.GetBinContent(2)
        dictExp2pLimit[ctauRange[i]] = histo.GetBinContent(3)
        dictExp1pLimit[ctauRange[i]] = histo.GetBinContent(4)
        dictExp1mLimit[ctauRange[i]] = histo.GetBinContent(5)
        dictExp2mLimit[ctauRange[i]] = histo.GetBinContent(6)

    #--- Multiply by the cross section if necessary
    if crossSection is not None:
        crossSection = float(crossSection)
        for i in range(len(ctauRange)):
            dictObsLimit[ctauRange[i]]   *= crossSection
            dictExpLimit[ctauRange[i]]   *= crossSection
            dictExp2pLimit[ctauRange[i]] *= crossSection
            dictExp1pLimit[ctauRange[i]] *= crossSection
            dictExp1mLimit[ctauRange[i]] *= crossSection
            dictExp2mLimit[ctauRange[i]] *= crossSection

    #--- Build graphs
    hTemplate   = TH1F("hTemplate",   "hTemplate",   int(ctauRange[-1]), ctauRange[0], ctauRange[-1])
    gObsLimit   = TGraph(len(ctauRange)+2)
    gExpLimit   = TGraph(len(ctauRange)+2)
    gExp1SUp    = TGraph(len(ctauRange)+2)
    gExp1SDw    = TGraph(len(ctauRange)+2)
    gExp2SUp    = TGraph(len(ctauRange)+2)
    gExp2SDw    = TGraph(len(ctauRange)+2)

    gObsLimit.SetName("gObsLimit")
    gExpLimit.SetName("gExpLimit")
    gExp1SUp.SetName("gExp1SUp")
    gExp1SDw.SetName("gExp1SDw")
    gExp2SUp.SetName("gExp2SUp")
    gExp2SDw.SetName("gExp2SDw")
   
    #Set limit to a large number if nan  or a very large number is found 
    for p, ctau in enumerate(ctauRange):
        if not math.isnan(dictObsLimit[ctau]) and dictObsLimit[ctau] < 1000:
          gObsLimit.SetPoint(p+1, ctau, dictObsLimit[ctau])
        else:
          dictObsLimit[ctau] = 999
          gObsLimit.SetPoint(p+1, ctau, 999)
        if not math.isnan(dictExpLimit[ctau]) and dictExpLimit[ctau] < 1000:
          gExpLimit.SetPoint(p+1, ctau, dictExpLimit[ctau])
        else:      
          dictExpLimit[ctau] = 999
          gExpLimit.SetPoint(p+1, ctau, 999)
        if not math.isnan(dictExp1pLimit[ctau]) and dictExp1pLimit[ctau] < 1000:
           gExp1SUp.SetPoint(p+1, ctau, dictExp1pLimit[ctau])
        else:
          dictExp1pLimit[ctau] = 999
          gExp1SUp.SetPoint(p+1, ctau, 999)
        if not math.isnan(dictExp1mLimit[ctau]) and dictExp1mLimit[ctau] < 1000:
          gExp1SDw.SetPoint(p+1, ctau, dictExp1mLimit[ctau])
        else:
          dictExp1mLimit[ctau] = 999
          gExp1SDw.SetPoint(p+1, ctau, 999)
        if not math.isnan(dictExp2pLimit[ctau]) and dictExp2pLimit[ctau] < 1000:
          gExp2SUp.SetPoint(p+1, ctau, dictExp2pLimit[ctau])
        else:
          dictExp2mLimit[ctau] = 999
          gExp2SUp.SetPoint(p+1, ctau, 999)
        if not math.isnan(dictExp2mLimit[ctau]) and dictExp2mLimit[ctau] < 1000:
          gExp2SDw.SetPoint(p+1, ctau, dictExp2mLimit[ctau])
        else:
          dictExp2mLimit[ctau] = 999
          gExp2SDW.SetPoint(p+1, ctau, 999)
 
    #--- Divide by BR(a->SM) if necessary
    if mode>1:
        gExp2SUp, dictExp2pLimit  = applyBRsToSM(graph=gExp2SUp , mode=mode, directory=directory, dictToUpdate=dictExp2pLimit)
        gExp1SUp, dictExp1pLimit  = applyBRsToSM(graph=gExp1SUp , mode=mode, directory=directory, dictToUpdate=dictExp1pLimit)
        gExpLimit, dictExpLimit   = applyBRsToSM(graph=gExpLimit, mode=mode, directory=directory, dictToUpdate=dictExpLimit)
        gExp1SDw, dictExp1mLimit  = applyBRsToSM(graph=gExp1SDw , mode=mode, directory=directory, dictToUpdate=dictExp1mLimit)
        gExp2SDw, dictExp2mLimit  = applyBRsToSM(graph=gExp2SDw , mode=mode, directory=directory, dictToUpdate=dictExp2mLimit)
        gObsLimit, dictObsLimit   = applyBRsToSM(graph=gObsLimit, mode=mode, directory=directory, dictToUpdate=dictObsLimit)

    #--- Find maximum for the range of the plot
    maximum = 0.
    for ctau in ctauRange:
        maximum = max(max(max(max(max(max(dictObsLimit[ctau], dictExpLimit[ctau]), dictExp2pLimit[ctau]), dictExp1pLimit[ctau]), dictExp1mLimit[ctau]), dictExp2mLimit[ctau]), maximum)

    maximum *= 1.3
    if model=="VH":
        maximum *= 1.2

    #--- Print numbers
    for ctau in ctauRange:
        if not blinded:
            print "ctau point: %.2f - Observed: %.2f - Expected: %.2f  %+.2f  %+.2f %+.2f %+.2f" % (ctau, dictObsLimit[ctau], dictExpLimit[ctau], dictExp2pLimit[ctau]-dictExpLimit[ctau], dictExp1pLimit[ctau]-dictExpLimit[ctau], dictExp1mLimit[ctau]-dictExpLimit[ctau], dictExp2mLimit[ctau]-dictExpLimit[ctau])
        else:
            print "ctau point: %.2f - Expected: %.2f %+.2f %+.2f %+.2f %+.2f" % (ctau, dictExpLimit[ctau], dictExp2pLimit[ctau]-dictExpLimit[ctau], dictExp1pLimit[ctau]-dictExpLimit[ctau], dictExp1mLimit[ctau]-dictExpLimit[ctau], dictExp2mLimit[ctau]-dictExpLimit[ctau])

    gExp2SUp.SetPoint( len(ctauRange)+1, 10.1, 0)
    gExp1SUp.SetPoint( len(ctauRange)+1, 10.1, 0)
    gExpLimit.SetPoint(len(ctauRange)+1, 10.1, 0)
    gObsLimit.SetPoint(len(ctauRange)+1, 10.1, 0)
    gExp1SDw.SetPoint( len(ctauRange)+1, 10.1, 0)
    gExp2SDw.SetPoint( len(ctauRange)+1, 10.1, 0)  

    gObsLimit.SetLineStyle(1)
    gObsLimit.SetLineWidth(2)
    gObsLimit.SetFillColor(0)
    gObsLimit.SetLineColor(1)

    gExpLimit.SetLineStyle(2)
    gExpLimit.SetLineWidth(2)
    gExpLimit.SetFillColor(0)
    gExpLimit.SetLineColor(1)

    gExp1SUp.SetLineStyle(2)
    gExp1SUp.SetLineWidth(0)
    gExp1SUp.SetFillColor(3)
    gExp1SUp.SetLineColor(1)

    gExp1SDw.SetLineStyle(2)
    gExp1SDw.SetLineWidth(0)
    gExp1SDw.SetFillColor(5)
    gExp1SDw.SetLineColor(1)

    gExp2SUp.SetLineStyle(2)
    gExp2SUp.SetLineWidth(0)
    gExp2SUp.SetFillColor(5)
    gExp2SUp.SetLineColor(1)

    gExp2SDw.SetLineStyle(2)
    gExp2SDw.SetLineWidth(0)
    gExp2SDw.SetFillColor(10)
    gExp2SDw.SetLineColor(10)

    if not doLogY:
        hTemplate.SetAxisRange(0,maximum,"y")
        hTemplate.GetYaxis().SetNdivisions(505)
    else:
        hTemplate.SetAxisRange(1e-1,maximum*1e1,"y")
        ROOT.gPad.SetLogy()
    
    twoSigmaGraph = getGraphAsymmErrors2D(gExpLimit, gExp2SUp, gExp2SDw, 5, 1001)
    oneSigmaGraph = getGraphAsymmErrors2D(gExpLimit, gExp1SUp, gExp1SDw, 3, 1001)

    if saveAs is not None:
        saveFile = TFile(saveAs.replace(".png", "").replace(".eps", "")+"_TGraphs.root", "RECREATE")
        gObsLimit.Write()
        gExpLimit.Write()
        gExp1SUp.Write()
        gExp1SDw.Write()
        gExp2SUp.Write()
        gExp2SDw.Write()
        twoSigmaGraph.Write()
        oneSigmaGraph.Write()
        saveFile.Close()


    hTemplate.Draw()
    twoSigmaGraph.Draw("3")
    oneSigmaGraph.Draw("3")
    gExpLimit.Draw("L")

    line = TLine(hTemplate.GetXaxis().GetXmin(), crossSection, hTemplate.GetXaxis().GetXmax(), crossSection)
    line_err = TH1F("line_err", "line_err", 1, hTemplate.GetXaxis().GetXmin(), hTemplate.GetXaxis().GetXmax())
    line_err.SetBinContent(1, crossSection)
    line_err.SetBinError(1, crossSection_err)
    if mode==0:
        line.SetLineColor(ROOT.kRed)
        line.SetLineWidth(2)

        line_err.SetLineColor(ROOT.kRed)
        line_err.SetFillColor(ROOT.kRed)
        line_err.SetFillStyle(3001)

#        line_err.Draw("E2same")
        line.Draw()
    

    if not blinded:
        gObsLimit.Draw("L")

    #--- Extra curves
    gstatOnly = None
    if statOnlyGraph is not None:
        fstat = TFile(statOnlyGraph)
        if not blinded:
            gstatOnly = fstat.Get("gObsLimit")
        else:
            gstatOnly = fstat.Get("gExpLimit")
        gstatOnly.SetLineColor(ROOT.kBlue)
        gstatOnly.SetLineStyle(3)
        gstatOnly.Draw("Csame")

    gSLOnlyObs = None
    gSLOnlyExp = None
    if SLOnlyGraph is not None:
        fSL = TFile(SLOnlyGraph)
        if not blinded:
            gSLOnlyObs = fSL.Get("gObsLimit")
            gSLOnlyExp = fSL.Get("gExpLimit")
        else:
            gSLOnlyExp = fSL.Get("gExpLimit")
        gSLOnlyObs.SetLineColor(ROOT.kRed)
        gSLOnlyObs.SetLineStyle(1)
        gSLOnlyObs.SetMarkerColor(ROOT.kRed)
        gSLOnlyExp.SetLineColor(ROOT.kRed)
        gSLOnlyExp.SetLineStyle(2)
        h_gSLOnlyObs = None
        if not blinded:
            h_gSLOnlyObs = TGraphToTH1(gSLOnlyObs)
#            h_gSLOnlyObs.Draw("PLsame")
        h_gSLOnlyExp = TGraphToTH1(gSLOnlyExp)
        h_gSLOnlyExp.Draw("Csame")

    gDLOnlyObs = None
    gDLOnlyExp = None
    if DLOnlyGraph is not None:
        fDL = TFile(DLOnlyGraph)
        if not blinded:
            gDLOnlyObs = fDL.Get("gObsLimit")
            gDLOnlyExp = fDL.Get("gExpLimit")
        else:
            gDLOnlyExp = fDL.Get("gExpLimit")
        gDLOnlyObs.SetLineColor(ROOT.kBlue)
        gDLOnlyObs.SetLineStyle(1)
        gDLOnlyObs.SetMarkerColor(ROOT.kBlue)
        gDLOnlyExp.SetLineColor(ROOT.kBlue)
        gDLOnlyExp.SetLineStyle(2)
        h_gDLOnlyObs = None
        if not blinded:
            h_gDLOnlyObs = TGraphToTH1(gDLOnlyObs)
#            h_gDLOnlyObs.Draw("PLsame")
        h_gDLOnlyExp = TGraphToTH1(gDLOnlyExp)
        h_gDLOnlyExp.Draw("Csame")

    gOtherObs = None
    gOtherExp = None
    if otherGraph is not None:
        fDL = TFile(otherGraph.split(",")[0])
        if not blinded:
            gOtherObs = fDL.Get("gObsLimit")
            gOtherExp = fDL.Get("gExpLimit")
        else:
            gOtherExp = fDL.Get("gExpLimit")
        gOtherObs.SetLineColor(ROOT.kMagenta)
        gOtherObs.SetLineStyle(1)
        gOtherObs.SetMarkerColor(ROOT.kMagenta)
        gOtherExp.SetLineColor(ROOT.kMagenta)
        gOtherExp.SetLineStyle(2)
        h_gOtherObs = None
        if not blinded:
            h_gOtherObs = TGraphToTH1(gOtherObs)
            h_gOtherObs.Draw("PLsame")
        h_gOtherExp = TGraphToTH1(gOtherExp)
        h_gOtherExp.Draw("Csame")

    #--- Back to the template
    hTemplate.GetXaxis().SetTitle("c#tau_{a} [MM]")
    hTemplate.GetXaxis().SetTitleSize(0.045)


    if mode==0:
        hTemplate.GetYaxis().SetTitle("95%% C.L. upper limits on #sigma(%s)#timesBR [pb]" % (model))
    elif mode==1:
        hTemplate.SetMaximum(2.5)
#        hTemplate.SetMaximum(4)
        hTemplate.GetYaxis().SetTitle("95% C.L. upper limits on BR(H#rightarrow2a#rightarrow4b)")
    elif mode>=2 and mode <=8:
        hTemplate.GetYaxis().SetTitle("95% C.L. upper limits on BR(H#rightarrow2a)")
    elif mode>8:
        hTemplate.GetYaxis().SetTitle("95% C.L. upper limits on BR(H#rightarrow2s)")

    hTemplate.GetYaxis().SetTitleOffset(1.0)
    hTemplate.GetYaxis().SetTitleSize(0.045)

    hTemplate.GetXaxis().SetNdivisions(505)
    hTemplate.GetYaxis().SetNdivisions(510)

    gPad.RedrawAxis()

    gExp1SUpCopy = gExp1SUp.Clone()
    gExp1SUpCopy.SetLineWidth(2)
    gExp2SUpCopy = gExp2SUp.Clone()
    gExp2SUpCopy.SetLineWidth(2)

    # Draw legend
    legend = TLegend(0.53,0.5,0.95,0.75)
    legend.AddEntry(gObsLimit,"Observed 95% CLs","l")
    legend.AddEntry(gExp1SUpCopy,"Expected 95% CLs #pm 1#sigma","lf")
    legend.AddEntry(gExp2SUpCopy,"Expected 95% CLs #pm 2#sigma","lf")
    if statOnlyGraph is not None:
        legend.AddEntry(gstatOnly,"Stat. only","l")
    if SLOnlyGraph is not None:
#        legend.AddEntry(gSLOnlyObs,"Single lepton limit","l")
        legend.AddEntry(gSLOnlyExp,"Single lepton exp. limit","l")
    if DLOnlyGraph is not None:
#        legend.AddEntry(gDLOnlyObs,"Dilepton limit","l")
        legend.AddEntry(gDLOnlyExp,"Dilepton exp. limit","l")
    if otherGraph is not None:
        legend.AddEntry(gOtherObs,otherGraph.split(",")[1],"l")
    if mode==0:
        legend.AddEntry(line,"#sigma_{SM}(%s)"%(model),"l")
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.Draw()

    tex = TLatex()
    tex.SetNDC(kTRUE)
    tex.SetTextSize(0.045)
    tex.SetTextFont(72)
    tex.DrawLatex(0.2, 0.8, "ATLAS Internal")

    tex2 = TLatex()
    tex2.SetNDC(kTRUE)
    tex2.SetTextSize(0.045)
    tex2.DrawLatex(0.5, 0.8, "#sqrt{s} = 13 TeV, %.1f fb^{-1}" % (luminosity/1000.))

    if mode>1:
        tex3 = TLatex()
        tex3.SetNDC(kTRUE)
        tex3.SetTextSize(0.045)
        tex3.DrawLatex(0.15, 0.7, dictModels[mode][0])

    gPad.RedrawAxis()
    outputFile.cd()
    canvas.Write()
    
    if saveAs is not None:
        canvas.Print("limits_" + str(mass) + "_" + model + ".pdf", "PDF")

if __name__=="__main__":

    usage = "usage: %prog [options]"
    parser = OptionParser(usage=usage)

    parser.add_option("-v","--verbose", action="store_true", dest="verbose", default=False, help="Set the program in verbose mode")
    parser.add_option("--directory", action="store", type="string", dest="directory", default=os.getenv("PWD")+"/", help="TRExFitter folder")
    parser.add_option("--mass", action="store", dest="massPoint", default=20, help="Specify the mass point to scan")
    parser.add_option("--strategy", action="store", type="string", dest="strategy", default='', help="Use all ROOT files in folder (default), use the best limit in case of repeated mass points (best), use mass associated to each BDT (bdt)")
    parser.add_option("--mode", action="store", type=int, dest="mode", default=0, help="0: xsec*BR, 1-8: BR(H->2a->4b) models, 9-11: BR(H->2a->4b) models")
    parser.add_option("--model", action="store", type="string", dest="model", default="WH", help="Specify which model to scan")
    parser.add_option("--excludeMassPoint", action="store", type="string", dest="excludeMassPoint", default='', help="Mass point to exclude, i.e. WHa50a50")
    parser.add_option("--blinded", action="store_true", dest="blinded", default=False, help="")
    parser.add_option("--doLogY", action="store_true", dest="doLogY", default=False, help="")
    parser.add_option("--statOnlyGraph", action="store", type="string", dest="statOnlyGraph", default=None, help="Extra curve to show (typically StatOnly)")
    parser.add_option("--SLOnlyGraph", action="store", type="string", dest="SLOnlyGraph", default=None, help="Extra curve to show (single-lepton when plotting combined)")
    parser.add_option("--DLOnlyGraph", action="store", type="string", dest="DLOnlyGraph", default=None, help="Extra curve to show (typically dilepton when plotting combined)")
    parser.add_option("--otherGraph", action="store", type="string", dest="otherGraph", default=None, help="Extra curve to show. Comma-separated with the title for the legend")
    parser.add_option("--saveAs", action="store", type="string", dest="saveAs", default=None, help="")

    (options, args) = parser.parse_args()

    verbose          = options.verbose
    directory        = options.directory
    massPoint        = options.massPoint
    excludeMassPoint = options.excludeMassPoint.split(",")
    mode             = options.mode
    model            = options.model
    strategy         = options.strategy
    blinded          = options.blinded
    doLogY           = options.doLogY
    saveAs           = options.saveAs
    statOnlyGraph    = options.statOnlyGraph
    SLOnlyGraph      = options.SLOnlyGraph
    DLOnlyGraph      = options.DLOnlyGraph
    otherGraph       = options.otherGraph

    #--- Get files
    rootFileNames = []
    rootFileNames = os.popen("ls " + directory + "VHSS_" + model + "125_S" + str(massPoint) + "S" + str(massPoint) + "*/fit*/Limits/*.root").readlines()
    rootFileNameDict = {}
    ctauRangeList = []
    for filePath in rootFileNames:
      ctauString = filePath.rstrip("\n").split("_")[-1].split(".")[0]
      ctauNumber = float(ctauString.split("p")[0].split("t")[1] + "." +  ctauString.split("p")[1])
      ctauRangeList.append(ctauNumber)
      rootFileNameDict[str(ctauNumber)] = filePath.rstrip("\n") 
    ctauRangeList.sort()
    rootFileNames = []

    for ctau in ctauRangeList:
      rootFileNames.append(rootFileNameDict[str(ctau)])

    #--- Get Cross section (https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt13TeV)
    crossSection = 1.
    crossSection_err = 0.
    if model == "WH":
        crossSection = 1.373
        crossSection_err = crossSection * TMath.Sqrt(0.007**2 + 0.019**2 + 0.017**2 + 0.009**2)
    elif model == "ZH":
        crossSection = 0.8839 # qqZH + ggZH
        crossSection_err = crossSection * TMath.Sqrt(0.038**2 + 0.016**2 + 0.013**2 + 0.009**2)
    elif model == "VH":
        crossSection = 1.373+0.8839 #WH + qqZH + ggZH
        crossSection_err = crossSection * TMath.Sqrt(0.007**2 + 0.019**2 + 0.017**2 + 0.009**2 + 0.038**2 + 0.016**2 + 0.013**2 + 0.009**2)

    if mode>0:
        crossSection = 1.
        crossSection_err = 0.

    print "\nC O N F I G U R A T I O N"
    print "Directory: %s" % directory
    print "strategy: %s" % strategy
    print "Model: %s" % model
    if mode==0:
        print "SM cross section: %f +/- %f" % (crossSection, crossSection_err)
    elif mode==1:
        print "Setting limits on BR(H->2a->4b)"
    elif mode==2:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 1)"
    elif mode==3:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 2, tanb = 0.5)"
    elif mode==4:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 2, tanb = 5)"
    elif mode==5:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 3, tanb = 0.5)"
    elif mode==6:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 3, tanb = 5)"
    elif mode==7:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 4, tanb = 0.5)"
    elif mode==8:
        print "Setting limits on BR(H->2a) for 2HDM+S (type 4, tanb = 5)"
    elif mode==9:
        print "Setting limits on BR(H->2s) for 2HDM+S (type 2, tanb = 0.5, alpha = 0.1)"
    elif mode==10:
        print "Setting limits on BR(H->2s) for 2HDM+S (type 3, tanb = 0.5, alpha = -1.4)"
    elif mode==11:
        print "Setting limits on BR(H->2s) for 2HDM+S (type 4, tanb = 0.5, alpha = 0.1)"

    print "Ctau points: %s" % str(ctauRangeList)

    canvas = HaaExclusion(ctauRangeList, rootFileNames, strategy=strategy, blinded=blinded, doLogY=doLogY, mode=mode, crossSection=crossSection, crossSection_err=crossSection_err, model=model, saveAs=saveAs, statOnlyGraph=statOnlyGraph, SLOnlyGraph=SLOnlyGraph, DLOnlyGraph=DLOnlyGraph, otherGraph=otherGraph, mass = massPoint)
