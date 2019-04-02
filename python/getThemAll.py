#!/usr/bin/env python
import os,sys
import math
import sympy
import mpmath

def fdMoment(z,n):
  if n == 1:
    return sympy.sqrt(2)*sympy.exp(-z)
  elif n == 0:
    return 0
  else:
    return (2*n-1)*sympy.Pow(sympy.sqrt((2*n-1)*(2*n-1) - 1),-1)*(4*sympy.exp(-z)*fdMoment(z,n-1) -  8*(n-1)*(n-1)*sympy.Pow(4*(n-1)*(n-1)-1,-1)*fdMoment(z,n-1) -sympy.sqrt((2*n-3)*(2*n-3)-1)*sympy.Pow(2*n-3,-1)*fdMoment(z,n-2))

for n in range(0,11):
  print "E(z)" + str(n) + " is: "
  z = sympy.Symbol('z')
print sympy.simplify(fdMoment(z,n))
