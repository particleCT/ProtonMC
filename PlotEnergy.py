from root_numpy import tree2array
from ROOT import *
import sys
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import rc
import pylandau

from scipy.optimize import curve_fit

import matplotlib
rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 20})

f=TFile(sys.argv[1])

t = f.Get("phase")


array = tree2array(t, branches=['Estop','Id'])
print array['Id']
idlist = np.where(array['Id']==1)

Elist = 200-array['Estop'][idlist]

fig, ax  = plt.subplots(figsize=[9,9])
#n, bins = np.histogram(Elist,bins=100)
n, bins,patches = plt.hist(Elist,bins=100,histtype=u'step',edgecolor='k', label=r"200 MeV/20 cm of water")

mean = np.average(bins[:-1],weights=n)
coeff = [mean, 0.05, 0.00001, np.max(n)]
#ax.plot(bins[:-1], pylandau.langau(bins[:-1],*coeff), "-")
ax.set_yscale('log')
ax.minorticks_on()
ax.yaxis.set_ticks_position('both')
ax.xaxis.set_ticks_position('both')

ax.tick_params('both', direction='in', length=4, width=1, which='minor')
ax.tick_params('both', direction='in', length=10, width=1, which='major')
ax.grid(alpha=0.5)
ax.set_ylabel(r"count/bin [-]")
ax.set_xlabel(r"Energy loss [MeV]")
plt.legend(frameon=False)
plt.tight_layout()
plt.show()



plt.show()
