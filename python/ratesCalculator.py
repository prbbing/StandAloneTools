#for HLT studies. It will calculate the rates for each path in the log files.

import math,ROOT, glob, re, os
from ROOT import gROOT, TFile, TChain, TTree, TH1F, TF1

gROOT.Reset()

def Rate(count,xsec,nevts):
    rate = xsec*ilumi*(count/nevts)
    return rate

def RateErr(count,xsec,nevts):
    rateerr = ((xsec * ilumi)/nevts) * math.sqrt(count)
    return rateerr

#Recommended samples to use: QCD with Pt 30-1800, DYToMuMu, DYToEE, WToMuNu, WToENu

samples = ['WToMuNu','WToENu','DYToMuMu','QCD80120','DYToEE','QCD3050','QCD5080','QCD120170','QCD170300','QCD300470','QCD470600','QCD600800','QCD8001000','QCD10001400','QCD14001800']
#samples = ['MuEnr20to30', 'MuEnr30to50', 'MuEnr50to80', 'MuEnr80to120', 'MuEnr120to170', 'MuEnr170to300', 'DYToMuMu', 'WToMuNu']
datasetDic = {}
pathList = []
for sample in samples:
    Tmp = open(sample + '.log','r')
    datasetDic[sample] = []
    for line in Tmp:
        if "End-Path" not in str(line):
               if "v1" in str(line):
                   datasetDic[sample].append(line)
                   if sample == samples[0]:
                       pathList.append(line.split()[7])
        else:
            break

crossSectionDic = {
    #General QCD
    'QCD3050'    :(161500000.),
    'QCD5080'    :(22110000.),
    'QCD80120'   :(3000114.3),
    'QCD120170'  :(493200.),
    'QCD170300'  :(120300.),
    'QCD300470'  :(7475.),
    'QCD470600'  :(587.1),
    'QCD600800'  :(167.),
    'QCD8001000' :(28.25),
    'QCD10001400':(8.195),
    'QCD14001800':(0.7346),
    'QCD1800'    :(0.1091),
    #Mu enriched QCD
    'MuEnr20to30'    : 675300000.*0.0065,
    'MuEnr30to50'    : 161500000.*0.0122,
    'MuEnr50to80'    : 22110000.*0.0218,
    'MuEnr80to120'   : 3000000.*0.0395,
    'MuEnr120to170'  : 493200.*0.0473,
    'MuEnr170to300'  : 120300.*0.0676,
     #W and Z cross sections
    'WToENu'         : 16000.,
    'WToMuNu'        : 16100.,
    'DYToMuMu'       : 6870.,
    'DYToEE'         : 6960.
}

for path in pathList:
    crossSections13TeV = {}
    for dic in datasetDic:
        crossSectionEntry = ''
        crossSections13TeV[dic] = []
        crossSections13TeV[dic].append(crossSectionDic[dic])
        for pathInfo in datasetDic[dic]:
            if path in pathInfo.split()[7]:
                totalNumber = float(pathInfo.split()[3])
                crossSections13TeV[dic].append(totalNumber)
                passNumber = float(pathInfo.split()[4])
                crossSections13TeV[dic].append(passNumber)

    ilumi = 14e33
    rates = []
    err = []

    for sample in samples:

        xsec = crossSections13TeV[sample][0]*1e-36
        nevts = crossSections13TeV[sample][1]
        count = crossSections13TeV[sample][2]

        rate = Rate(count,xsec,nevts)
        rateerr = RateErr(count,xsec,nevts)
        rateerrsq = pow(rateerr,2)

        rates.append(rate)
        err.append(rateerrsq)


    Totrate = sum(rates)

    Toterr = math.sqrt(sum(err))

    print path + ': ', Totrate, " +- ", Toterr

