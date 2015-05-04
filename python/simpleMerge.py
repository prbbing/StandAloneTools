#!/usr/bin/env python
#it will quick ly merge all root files in a codor directory.
import sys
import os
import re
from math import *
from array import *
from ROOT import *

condorDir = sys.argv[1]
workingDir = '/data/users/bing/condor/' + str(condorDir) 
os.chdir(workingDir)
sourceFileList = os.popen('ls *.root').read().replace('\n',' ')
targetFileName = sys.argv[2]
os.system('hadd ' + str(targetFileName) + ' ' + sourceFileList)
