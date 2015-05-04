#!/usr/bin/env python
#it will collect the valid files from a list of file and generate a new list.  
import sys
import os
import re
from math import *
from array import *
from ROOT import *

fileList = sys.argv[1]

inputTmp = open(fileList,'r')
os.system('touch ValidFilesFrom' + fileList)
outputTmp = open('ValidFilesFrom' + fileList,'r+w')
for line in inputTmp:
    tmpFile = TFile(str(line).strip('\n'))
    if tmpFile.IsZombie() or not tmpFile.GetNkeys():
        continue
    outputTmp.write(line)
