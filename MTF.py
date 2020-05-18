import numpy as np
from scipy.optimize import curve_fit
import scipy,sys
import scipy.ndimage as ndimage
from scipy.signal import savgol_filter
from scipy.interpolate import interp1d
from scipy.interpolate import UnivariateSpline
import matplotlib.pylab as plt
import time
from ROOT import *
from matplotlib import rc
import matplotlib
rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 14})

def findMTF10(Xdata,Ydata):
    f    = interp1d(Xdata,Ydata)
    X    = np.linspace(Xdata[0],Xdata[22],1000)
    id   = np.argmin(np.abs(f(X)-0.1))
    return X[id]

def find1090(Xdata,Ydata):
    f    = interp1d(Xdata,Ydata)
    X    = np.linspace(Xdata[0],Xdata[-1],1000)
    return X[np.argmin(np.abs(f(X)-0.9))] - X[np.argmin(np.abs(f(X)-0.1))]
    
def gaus(x,A,sigma):
    return A*np.exp(-np.power(x,2)/(2.*sigma**2))

def MTF(lsf):
    lsf = lsf*np.hanning(len(lsf))
    MTF_v = np.abs(np.fft.fft(lsf))
    MTF_v /=MTF_v[0]
    #MTF_v = np.fft.fftshift(MTF_v)
    return MTF_v

def LSF(edgespreadfunction): return np.diff(edgespreadfunction)

def polynomialfit(data, order):
    x, y = range(len(data)), data
    coefficients = np.polyfit(x, y, order)
    return np.polyval(coefficients, x)


def Transform2nparray(hprof):
    h = hprof.ProjectionXY("")
    n = [h.GetNbinsX(),h.GetNbinsY()]
    n2 = (n[1]+2)*(n[0]+2)
    d = h.GetArray()
    d.SetSize(n2)
    h = np.array(d,np.float64).reshape(n[1]+2,n[0]+2)
    return h

def GetTheEdge(img):
    angle      = 2.5*np.pi/180. 
    esf_res    = int(round(1/np.tan(angle)))
    Xdata_res  = 150
    dx         = 1./esf_res
    Ydata      = np.zeros((Xdata_res*esf_res)/5)
    Count      = np.ones((Xdata_res*esf_res)/5)
    for i in range(4):
        for id_ymin in range(225,375,23):

            for n in range(0, esf_res): 
                data = img[n+id_ymin,140:290]  

                for idx,value in enumerate(data): ## Place at the appropriate value
                    idbin = (esf_res*idx+n)/5
                    if(value>0):
                        Ydata[idbin]+=value
                        Count[idbin]+=1
    
            img = np.rot90(img)
    Ydata/=Count
    plt.plot(Ydata)
    plt.show()
    return Ydata

def Plot(argument):
    f=TFile(argument)
    print f.ls()
    hprof = f.Get("WET_map_front")
    histA = Transform2nparray(hprof)[1:-1,1:-1]
    histA = np.fliplr(histA)
    
    #hprof2= f.Get("WET_map_rear")
    hprof2= f.Get("MaxLikelihood_profile")
    histB = Transform2nparray(hprof2)[1:-1,1:-1]
    print histB.shape
    histB = np.fliplr(histB)

    Ydata_1 = GetTheEdge(histA)
    Ydata_2 = GetTheEdge(histB)
    dx      = 300./600.#0.055 ## mm
    Xdata   = np.arange(0,(len(Ydata_1))*dx,dx)

    for x in range(len(Ydata_1)): 
        if(np.isinf(Ydata_1[x])): Ydata_1[x] = Ydata_1[x-1]
    for x in range(len(Ydata_2)): 
        if(np.isinf(Ydata_2[x])): Ydata_2[x] = Ydata_2[x-1]
        
    Ydata_1 = savgol_filter(Ydata_1, 31, 5)
    Ydata_2 = savgol_filter(Ydata_2, 31, 5)    

    Ydata_1-=np.min(Ydata_1)
    Ydata_1/=np.max(Ydata_1)
    
    Ydata_2-=np.min(Ydata_2)
    Ydata_2/=np.max(Ydata_2)

    Ydata_1 = ndimage.gaussian_filter1d(Ydata_1, 4.75)
    Ydata_2 = ndimage.gaussian_filter1d(Ydata_2, 4.75)

    LSF_1 = LSF(Ydata_1)
    LSF_2 = LSF(Ydata_2)
    Xdata = np.fft.fftfreq(len(Ydata_1),d=dx*np.tan(np.radians(2.5)))[:100]
    MTF_1 = MTF(LSF_1)[:100]
    MTF_2 = MTF(LSF_2)[:100]
    
    plt.plot(Xdata,MTF_1, label='Front')
    plt.plot(Xdata, MTF_2, label='MTF')
    plt.legend()
    plt.show()
    mtf1 = findMTF10(Xdata,MTF_1)
    mtf2 = findMTF10(Xdata,MTF_2)
    
    return Xdata,MTF_1,MTF_2,mtf1,mtf2


Xdata,MTF_1_1,MTF_2_1,mtf1_1,mtf2_1 = Plot(sys.argv[1])





