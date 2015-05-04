#Used for HLT studies. It will generate a central Trig report with the most useful information from multiple log files.
import math,ROOT, glob, re, os
from ROOT import gROOT, TFile, TChain, TTree, TH1F, TF1

gROOT.Reset()

samples = ['WToMuNu','WToENu','DYToMuMu','QCD80120','DYToEE','QCD3050','QCD5080','QCD120170','QCD170300','QCD300470','QCD470600','QCD600800','QCD8001000','QCD10001400','QCD14001800']

dirList = os.popen('ls').read().split('\n')
for sample in samples:
    for dir in dirList:
        if sample in dir and os.path.isdir(dir):
            logFileList = os.popen('ls ' + dir + '/*.log').read().split('\n')
            pathDic = {}
            for file in logFileList:
                if file:
                    Tmp = open(str(file),'r')
                    for line in Tmp:
                        if "End-Path" not in str(line):
                           if "v1" in str(line) and "TrigReport" in str(line):
                               pathName = line.split()[7]
                               if not pathDic.has_key(pathName):
                                   pathDic[pathName] = [float(line.split()[3]), float(line.split()[4])]
                               else:
                                   pathDic[pathName][0] = float(pathDic[pathName][0]) + float(line.split()[3])
                                   pathDic[pathName][1] = float(pathDic[pathName][1]) + float(line.split()[4])
                        else:
                            break
                    Tmp.close()

            os.system('touch ' + sample + '.log')
            Tmp = open(sample + '.log','r+w')
            for path in pathDic:
                string = 'TrigReport    Dummy1    Dummy2    ' + str(pathDic[path][0]) + '    ' + str(pathDic[path][1]) + '    ' + str(float(pathDic[path][0]) - float(pathDic[path][1])) + '   Dummy3    ' + str(path) + '\n'
                Tmp.write(string)
            Tmp.close()
