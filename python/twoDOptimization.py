import os,sys,math
from ROOT import *

def sOverSqrtB(s, b):
  return s/math.sqrt(b)

def keywithmaxval(d):
     v = list(d.values())
     k = list(d.keys())
     return k[v.index(max(v))]

def findMassBins(mass, hist):
  bins = []
  norm = hist.Integral()
  massBin = hist.GetXaxis().FindBin(mass)
  startingBin = 0
  for i in range(1, massBin):
    integral = hist.Integral(massBin - i, massBin)
    ratio = integral/norm
    startingBin = massBin - i
    if ratio > 0.67:
      break
  if startingBin >= 20:
    bins.append(startingBin)
  else:
    bins.append(20)
  bins.append(massBin)
  return bins

def makeTable(dict):
  os.system("touch table.tex")
  fout = open("table.tex", "w")
  fout.write ("\\documentclass[a4paper,12pt]{article}\n")
  fout.write ("\\usepackage[landscape,margin=0.15cm]{geometry}\n")
  fout.write ("\\usepackage{multirow}\n")
  fout.write ("\\begin{document}\n")
  fout.write ("\\pagestyle{empty}\n")
  fout.write("\\begin{table}[ht]\n")
  fout.write("\\begin{center}\n")
  line = "\\begin{tabular}{|c|"
  for mass in masses:
    line = line + "c|"
  line = line + "}\n"
  fout.write(line)
  fout.write("\\hline\n")
  line = "$r_{inv}$"
  for mass in masses:
    line = line + " & " + str(mass)
  line = line + "\\\\\n"
  fout.write(line)
  fout.write("\\hline\n")
  for rinv in rinvs:
    line = str(rinv)
    for mass in masses:
      line = line + " & "  + str(dict[mass][rinv])
    line = line + "\\\\\n"
    fout.write(line)
  fout.write("\\hline\n")
  fout.write("\\end{tabular}\n")
  fout.write("\\end{center}\n")
  fout.write("\\end{table}\n")
  fout.write("\\end{document}\n")
  fout.close()

step = 0.1
ystar_cuts = []
for i in range(1, int(2.5/step)):
  ystar_cuts.append(round(step*i,1))

masses = [1000,1250,1500,2000,2500,3000,4000]
rinvs = [2,4,6,8]

input = TFile("MultiJet.root")

hist_qcd  = input.Get("Inclusive/mt_12_vs_ystar_12").Clone()

signal_yields = {}
qcd_yields = {}

for mass in masses:
  signal_yields[mass] = {}
  qcd_yields[mass] = {}
  for rinv in rinvs:
    signal_yields[mass][rinv] = {}
    qcd_yields[mass][rinv] = {}
    signal = TFile("SVJSChan2j_" + str(mass).split(".")[0] + "_" + str(rinv).split(".")[0] + "_0.root")
    for i in ystar_cuts:
      hist = signal.Get("Inclusive/mt_12_vs_ystar_12").Clone()
      bin1 = hist.GetXaxis().FindBin(-1*i)
      bin2 = hist.GetXaxis().FindBin(i)
      ystar_sel = hist.ProjectionY("mt_inclusive",bin1,bin2)
      bins = findMassBins(mass, ystar_sel)
      signal_yields[mass][rinv][i] = ystar_sel.Integral(bins[0],bins[1])
      ystar_sel_qcd = hist_qcd.ProjectionY("mt_inclusive",bin1,bin2)
      qcd_yields[mass][rinv][i] = ystar_sel_qcd.Integral(bins[0],bins[1])

metric = {}
optmimal = {}

for mass in masses:
  metric[mass] = {}
  optmimal[mass] = {}
  for rinv in rinvs:
    metric[mass][rinv] = {}
    optmimal[mass][rinv] = {}
    for i in ystar_cuts:
      metric[mass][rinv][i] = sOverSqrtB(signal_yields[mass][rinv][i], qcd_yields[mass][rinv][i])
    optmimal[mass][rinv] = keywithmaxval(metric[mass][rinv])

makeTable(optmimal)

input.Close()
