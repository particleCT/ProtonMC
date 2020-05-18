import rootplot
import rootplot.root2matplotlib as r2m
from ROOT import *
from matplotlib import pyplot as plt
import sys
import numpy as np
from numpy import *
from scipy.signal import argrelextrema
f=TFile(sys.argv[1])
print f.ls()
A=f.Get(sys.argv[2])
print A
histA = r2m.Hist2D(A)
histA = np.array(histA.content)

histA = np.clip(histA,float(sys.argv[3]),float(sys.argv[4]))
histA =np.rot90(histA,2)
#plt.xlim([100,400])
#plt.ylim([120,450])
histA =np.rot90(histA,2)
#plt.imshow(histA,cmap="gray_r",extent=[-16.5,16.5,-16.5,8.75])
plt.imshow(histA,extent=[-16.5,16.5,-16.5,8.75])
plt.colorbar()
plt.show()

np.savetxt("histA.txt",histA)
