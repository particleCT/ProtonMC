
import rootplot
import rootplot.root2matplotlib as r2m
from ROOT import *
from matplotlib import pyplot as plt
import sys
import numpy as np
from numpy import *
from scipy.signal import argrelextrema
import scipy.ndimage as ndimage


from matplotlib import rc
import matplotlib
rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 26})

def findMTF10(Xdata,Ydata):
    f    = interp1d(Xdata,Ydata)
    X    = np.linspace(Xdata[0],Xdata[22],1000)
    id   = np.argmin(np.abs(f(X)-0.1))
    return X[id]
                
def MTF(lsf):

    #lsf = lsf*np.hanning(len(lsf))
    MTF_v = np.abs(np.fft.fft(lsf))
    #MTF_v /=MTF_v[0]
    MTF_v = np.fft.fftshift(MTF_v)

    return MTF_v
                    
def LSF(edgespreadfunction): return np.diff(edgespreadfunction)
                    
def NormalizeAndGet9010(profile,profile_err,i):
    Xaxis    = np.linspace(0,600,600)
    print Xaxis.shape, Xaxis
    ## Normalize
    profile = (profile - np.mean(profile[0:10]))
    profile = profile/np.max(profile)
    PointMiddle = i+100 ## of the block

    ## In the first half only
    Point90  = np.argmin( np.abs(profile[:PointMiddle] - 0.9))
    Point10  = np.argmin( np.abs(profile[:PointMiddle] - 0.1))
    print Xaxis[Point10], Xaxis[Point90], profile[Point90], profile[Point90-1]

    Dist     = Xaxis[Point90] - Xaxis[Point10]
    Err  = np.sqrt( np.power(profile_err[Point90],2) + np.power(profile_err[Point10],2))
    
    return Dist, Err


def GetHistArray(hist):

    err     = hist.ProjectionXY("","C=E")
    entries = hist.ProjectionXY("","B")
    hist    = hist.ProjectionXY("","")

    bins    = [hist.GetNbinsY()+2,hist.GetNbinsX()+2]
    bins2   = (bins[0])*(bins[1])
    
    d       = hist.GetArray()
    d2      = err.GetArray()
    n       = entries.GetArray()
    d.SetSize(bins2)
    d2.SetSize(bins2)
    
    D = np.array(d, np.float64)
    D = np.reshape(D,bins)[1:-1,1:-1]

    D2 = np.array(d2, np.float64)
    D2 = np.reshape(D2,bins)[1:-1,1:-1]
    n.SetSize(bins2)
    N  = np.array(n)
    N  = np.reshape(N,bins)[1:-1,1:-1]

    return D, D2/np.sqrt(N)
                                                            

f=TFile(sys.argv[1])
histB, errB = GetHistArray(f.Get(sys.argv[2]))

histA = np.load("output.py.npy")
histA[np.isnan(histA)] = 0

DistA = np.zeros(5)
DisterrA = np.zeros(5)

DistB = np.zeros(5)
DisterrB = np.zeros(5)

Xaxis = [-70, -35, 0 ,35, 70]

for nb,i in enumerate(range(0,500,100)):
    PointMiddle = i+100 ## of the block
    profileA = np.average(histA[i+70:i+130],axis=0,weights=histA[i+70:i+130])
    #MTFA = MTF(profileA[PointMiddle-75:PointMiddle])
    #profileA = (profileA - np.mean(profileA[0:10]))
    #profileA /= np.max(profileA)

    profileB = np.average(histB[i+70:i+130],axis=0,weights=histB[i+70:i+130])
    #MTFB = MTF(profileB[PointMiddle-75:PointMiddle])

    plt.plot(profileA)
    plt.plot(profileB)
    plt.show()
    #profileB = (profileB - np.mean(profileB[0:10]))
    #profileB /= np.max(profileB)

    print "A"
    DistA[nb], DisterrA[nb]  = NormalizeAndGet9010(profileA, profileA,i)
    print "B"
    DistB[nb], DisterrB[nb]  = NormalizeAndGet9010(profileB, profileB,i)


fig, ax  = plt.subplots(figsize=[12,8])
ax.plot(Xaxis, DistA,'bo-',label="Laplace Reconstruction")
ax.plot(Xaxis, DistB,'ro-',label="Front Tracker")

ax.minorticks_on()
ax.yaxis.set_ticks_position('both')
ax.xaxis.set_ticks_position('both')

ax.tick_params('both', direction='in', length=4, width=1, which='minor')
ax.tick_params('both', direction='in', length=10, width=1, which='major')
ax.grid(alpha=0.5)
ax.set_xlim([-100,100])
ax.set_ylabel(r"Distance 10$\%$-90$\%$ [mm]")
ax.set_xlabel(r"Depth [mm]")
plt.text(-97, 7.5,"Front tracker", rotation=90, fontsize=28)
plt.text(94, 7.5,"Rear tracker", rotation=90, fontsize=28)
plt.legend(frameon=False)
plt.tight_layout()   

plt.show()

