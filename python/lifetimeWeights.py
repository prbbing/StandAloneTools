#!/usr/bin/python

from ROOT import *

sourceFunc = TF1("source","exp(-x)",0,10)

for lifetime in (0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0):
  cutoff = -log(0.2) * lifetime
  weights= []
  largeWeights = []
  for i in range(1,10000):
    sourceCtau1 = sourceFunc.GetRandom()
    sourceCtau2 = sourceFunc.GetRandom()
    if (sourceCtau1 < cutoff  and sourceCtau2 < cutoff):
      weight = (1/lifetime)*exp(sourceCtau1*((lifetime - 1)/lifetime)) * (1/lifetime)*exp(sourceCtau2*((lifetime - 1)/lifetime))
      weights.append(weight)
      if weight > 1:
        largeWeights.append(weight)
  weights.sort()
  print "To weight " + str(lifetime) + " sample, the fraction of > 1 weights is : " + str((len(largeWeights) * 1.0)/len(weights))
  print "To weight " + str(lifetime) + " sample, the median of the weights is : " + str(weights[len(weights)/2])
