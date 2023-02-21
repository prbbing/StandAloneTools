import os,sys,re
import math
from array import array
import optparse
import numpy as np
from numpy import linalg

staDimX = 3
staDimY = 5

matrixArray = np.zeros((staDimY, staDimX))
for j in range(0,staDimY):
  for i in range(0,staDimX):
    if i == j and j != staDimX - 1:
      matrixArray[j][i] = 1
    elif j == staDimX - 1 and i == j:
      matrixArray[j][i] = 1.0/math.sqrt(staDimY - staDimX + 1)
    elif i == staDimX - 1 and j > staDimX - 1 :
      matrixArray[j][i] = 1.0/math.sqrt(staDimY - staDimX + 1)
    else:
      matrixArray[j][i] = 0

print("New cov matrix: ")
print(matrixArray)

cov = np.dot(matrixArray, matrixArray.transpose())
print(cov)

values, vectors = linalg.eig(cov)

eigVec = []
for i in range(1, len(vectors)):
  eigVec.append(vectors[:,i])

print("Eigen vectors: ")
print(eigVec)
print("Eigen values: ")
print(values)

print("Checking orthogonality: ")
for i in range(1, len(eigVec)):
  for j in range(1, len(eigVec)):
    if i != j:
      print(np.dot(eigVec[i], eigVec[j]))
