import numpy as np
import matplotlib.pyplot as plt
import sys
import matplotlib.gridspec as gridspec
import matplotlib.ticker as mticker
from matplotlib import rc
import matplotlib
from scipy import interpolate
from scipy.signal import savgol_filter
from ROOT import *
rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 20})

def GetHistArray(hist):
    err     = hist.ProjectionXY("","C=E")
    hist    = hist.ProjectionXY("","")
    n     = [hist.GetNbinsY()+2,hist.GetNbinsX()+2]
    n2    = (n[0])*(n[1])

    
    d     = hist.GetArray()
    d.SetSize(n2)
    histA = np.array(d)
    histA = np.reshape(histA,n)
    
    d2     = err.GetArray()
    d2.SetSize(n2)
    histerr = np.array(d2)
    histerr     = np.reshape(histerr,n)

    histA       = np.mean(histA[50:250],axis=0)[1:-1]
    histerr     = np.mean(histerr[50:250],axis=0)[1:-1]
    return histA, histerr

MC       = TFile(sys.argv[1])
hist      = MC.Get("Primaries/WET_map_prim_front")
Xmin      = hist.GetXaxis().GetXmin()
Xmax      = hist.GetXaxis().GetXmax()
Xbins     = hist.GetNbinsX()
Xaxis     = np.linspace(Xmin,Xmax,Xbins)
hist, err = GetHistArray(hist)
hist2     = MC.Get("AllParticles/WET_map_front")
hist2, err2 = GetHistArray(hist2)
plt.plot(err)
plt.show()

plt.plot(hist)
plt.show()

print "sigma_WET: ", err[150]
header = MC.Get("header")
NPart            = 0
NPrimCylinderMiddle  = []
TotEnergyDeposit =   []
NPrimaryMiddle   =  []
NPrimaryEnd   =  []
NFinal = 0
for entries in header:
    NPart           += entries.NPart
    NPrimCylinderMiddle.append(entries.NPrimaryCylinderMiddle)
    TotEnergyDeposit.append(entries.TotEnergyDeposit)
    NPrimaryMiddle.append(entries.NPrimaryMiddle)
    NPrimaryEnd.append(entries.NPrimaryEnd)
NPrimCylinderMiddle = np.array(NPrimCylinderMiddle)
TotEnergyDeposit = np.array(TotEnergyDeposit)
NPrimaryMiddle =  np.array(NPrimaryMiddle)

try:    
    print "S_MC ", np.mean(TotEnergyDeposit/NPrimCylinderMiddle), "+/-", np.std(TotEnergyDeposit/NPrimCylinderMiddle)

except:
    print "No data for S_MC"

try:    
    print "g_MC ", np.mean(NPrimaryMiddle/NPrimaryEnd), "+/-", np.std(NPrimaryMiddle/NPrimaryEnd)
except:
    print "No data for g_MC"    

