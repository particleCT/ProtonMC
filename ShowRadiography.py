
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
    lsf = lsf*np.hanning(len(lsf))
    MTF_v = np.abs(np.fft.fft(lsf))
    MTF_v /=MTF_v[0]
    #MTF_v = np.fft.fftshift(MTF_v)
    return MTF_v
                    
def LSF(edgespreadfunction): return np.diff(edgespreadfunction)
                    
def NormalizeAndGet9010(profile,profile_err,i):
    Xaxis    = np.linspace(0,300,600)
    ## Normalize
    profile = (profile - np.mean(profile[0:10]))
    profile = profile/np.max(profile)
    PointMiddle = i+100 ## of the block
    ## In the first half only
    Point90  = np.argmin( np.abs(profile[:PointMiddle] - 0.9))
    Point10  = np.argmin( np.abs(profile[:PointMiddle] - 0.1))
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
print f.ls()
histA, errA = GetHistArray(f.Get(sys.argv[2]))
histB, errB = GetHistArray(f.Get(sys.argv[3]))
histC, errC = GetHistArray(f.Get(sys.argv[4]))
histD, errD = GetHistArray(f.Get(sys.argv[5]))
histE, errE = GetHistArray(f.Get(sys.argv[6]))
histF, errF = GetHistArray(f.Get(sys.argv[7]))
histG, errG = GetHistArray(f.Get(sys.argv[8]))
histH, errH = GetHistArray(f.Get(sys.argv[9]))

DistA = np.zeros(5)
DistB = np.zeros(5)
DistC = np.zeros(5)
DistD = np.zeros(5)
DistE = np.zeros(5)
DistF = np.zeros(5)
DistG = np.zeros(5)
DistH = np.zeros(5)
DisterrA = np.zeros(5)
DisterrB = np.zeros(5)
DisterrC = np.zeros(5)
DisterrD = np.zeros(5)
DisterrE = np.zeros(5)
DisterrF = np.zeros(5)
DisterrG = np.zeros(5)
DisterrH = np.zeros(5)
Xaxis = [-70, -35, 0 ,35, 70]

for nb,i in enumerate(range(0,500,100)):

    profileA = np.average(histA[i+70:i+130],axis=0,weights=histA[i+70:i+130])
    profileB = np.average(histB[i+70:i+130],axis=0,weights=histB[i+70:i+130])
    profileC = np.average(histC[i+70:i+130],axis=0,weights=histC[i+70:i+130])    
    profileD = np.average(histD[i+70:i+130],axis=0,weights=histD[i+70:i+130])
    profileE = np.average(histE[i+70:i+130],axis=0,weights=histE[i+70:i+130])
    profileF = np.average(histF[i+70:i+130],axis=0,weights=histF[i+70:i+130])
    profileG = np.average(histG[i+70:i+130],axis=0,weights=histG[i+70:i+130])
    profileH = np.average(histH[i+70:i+130],axis=0,weights=histH[i+70:i+130])

    err_profileA = np.average(errA[i+70:i+130],axis=0,weights=errA[i+70:i+130])
    err_profileB = np.average(errB[i+70:i+130],axis=0,weights=errB[i+70:i+130])
    err_profileC = np.average(errC[i+70:i+130],axis=0,weights=errC[i+70:i+130])    
    err_profileD = np.average(errD[i+70:i+130],axis=0,weights=errD[i+70:i+130])
    err_profileE = np.average(errE[i+70:i+130],axis=0,weights=errE[i+70:i+130])
    err_profileF = np.average(errF[i+70:i+130],axis=0,weights=errF[i+70:i+130])
    err_profileG = np.average(errG[i+70:i+130],axis=0,weights=errG[i+70:i+130])
    err_profileH = np.average(errH[i+70:i+130],axis=0,weights=errH[i+70:i+130])

    DistA[nb], DisterrA[nb]  = NormalizeAndGet9010(profileA, err_profileA,i)
    DistB[nb], DisterrB[nb]  = NormalizeAndGet9010(profileB, err_profileB,i)
    DistC[nb], DisterrC[nb]  = NormalizeAndGet9010(profileC, err_profileC,i)
    DistD[nb], DisterrD[nb]  = NormalizeAndGet9010(profileD, err_profileD,i)
    DistE[nb], DisterrE[nb]  = NormalizeAndGet9010(profileE, err_profileE,i)
    DistF[nb], DisterrF[nb]  = NormalizeAndGet9010(profileF, err_profileF,i)
    DistG[nb], DisterrG[nb]  = NormalizeAndGet9010(profileG, err_profileG,i)
    DistH[nb], DisterrH[nb]  = NormalizeAndGet9010(profileH, err_profileH,i)

    """
    LSFA = LSF(profileA[i:i+125])
    MTFA = MTF(LSFA)
    LSFB = LSF(profileB[i:i+125])
    MTFB = MTF(LSFB)

    LSFC = LSF(profileC[i:i+125])
    MTFC = MTF(LSFC)
    LSFD = LSF(profileD[i:i+125])
    MTFD = MTF(LSFD)

    plt.plot(MTFA,'r-',label=sys.argv[2])
    plt.plot(MTFB,'b-',label=sys.argv[3])
    plt.plot(MTFC,'g-',label=sys.argv[4])
    plt.plot(MTFD,'y-',label=sys.argv[5])
    plt.legend()
    plt.show()
    """

print sys.argv, sys.argv[8]
fig, ax  = plt.subplots(figsize=[12,8])
ax.errorbar(Xaxis, DistA,yerr=DisterrA ,fmt='bo-',label="MLP Reconstruction")
ax.errorbar(Xaxis, DistB,yerr=DisterrB,fmt='ro-',label="Front binning")
ax.errorbar(Xaxis, DistC,yerr=DisterrC,fmt='ko-',label="Rear binning")
DistMix    = [DistD[0], DistE[1], DistF[2], DistG[3], DistH[4]]
DisterrMix = [DisterrD[0], DisterrE[1], DisterrF[2], DisterrG[3], DisterrH[4]]

ax.errorbar(Xaxis, DistMix,yerr=DisterrMix, fmt='gs-', label="Binning at depth")

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

