#!/usr/bin/env python
#a small script to calculate ratio and errors
import math
import sys

num = float(sys.argv[1])
den = float(sys.argv[2])
denerror = math.sqrt(den)
numerror = math.sqrt(num)

print 'ratio = ' + str(num/den)
print '\n'
print 'error = ' + str(math.pow(math.pow(numerror,2)/math.pow(den,2) + math.pow(denerror,2)*math.pow(num,2)/math.pow(den,4),0.5))
