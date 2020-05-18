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
energymid = []
for event in phase:
    if(event.EnergyMid>0):
        #print event.part_name, event.EnergyMid
        energymid.append(event.EnergyMid)

print np.mean(energymid), np.sum(energymid)/10000
#array = tree2array(t, branches=['EnergyMid'])

"""
fig, ax  = plt.subplots(figsize=[9,9])
#n, bins = np.histogram(Elist,bins=100)


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
"""
