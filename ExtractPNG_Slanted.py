from ROOT import *
import numpy as np
import matplotlib.pyplot as plt
import sys
from scipy import ndimage
from scipy.ndimage.filters import gaussian_filter
import cv2

def LSF(edgespreadfunction): return np.diff(edgespreadfunction)
def GetTheEdge(img):
    angle      = 2.5*np.pi/180.
    esf_res    = int(round(1/np.tan(angle)))
    print esf_res
    Xdata_res  = 75
    print Xdata_res*esf_res
    Ydata      = np.zeros((Xdata_res*esf_res))
    Count      = np.ones((Xdata_res*esf_res))
    start      = 102
    for i in range(4): ## Four sides
        for id_ymin in range(start, start+3*esf_res):# Step of 23 along the line
            for n in range(0, esf_res): ## We loop on each slices
                data = img[n+id_ymin,70:145]
                for idx,value in enumerate(data): ## Place at the appropriate value
                    idbin        =  (esf_res*idx+n)
                    if(value>0): 
                        Ydata[idbin]+=value
                        Count[idbin]+=1
        img = np.rot90(img)
    Ydata/=Count
    return Ydata

def MTF(lsf):
    #lsf = lsf*np.hanning(len(lsf))
    MTF_v = np.abs(np.fft.fft(lsf))
    MTF_v /=MTF_v[0]
    #MTF_v = np.fft.fftshift(MTF_v)
    return MTF_v
                    
def GetHistArray(hist):
    hist    = hist.ProjectionXY("","")
    bins    = [hist.GetNbinsY()+2,hist.GetNbinsX()+2]
    bins2   = (bins[0])*(bins[1])    
    d       = hist.GetArray()
    d.SetSize(bins2)
    D = np.array(d, np.float64)
    D = np.reshape(D,bins)[1:-1,1:-1]
    return D

def doExtraction(nbStack, histname, blur_bool, kernel):
    laps     = []
    histlist = []
    for i in range(0,nbStack):
        hist = f.Get(histname+str(i))
        hist = GetHistArray(hist)
        kernel_size = kernel        
        blur_size   = 15
        if(blur_bool): blurred = cv2.GaussianBlur(hist, (blur_size,blur_size), 0)
        else: blurred = hist
        laplace = cv2.Laplacian(blurred, cv2.CV_64F, ksize=kernel_size)
        
        laps.append(laplace)
        histlist.append(hist)
    laps = np.asarray(laps)
    abs_laps = np.absolute(laps)
    print laps.shape
    """
    for i in range(95,105,1):
        plt.plot(abs_laps[:,145,i],label=str(i))
    plt.legend()
    plt.show()
    """
    maxima = abs_laps.max(axis=0)
    bool_mask = abs_laps == maxima
    mask = bool_mask.astype(np.uint8)
    output = np.zeros(shape=laps[0].shape, dtype=np.float64)
    for i, hist in enumerate(histlist): output[bool_mask[i]] = hist[bool_mask[i]]
    return output


def doTheMTF(img):

    img   =  np.fliplr(img)
    Ydata_1 = GetTheEdge(img) 
    Ydata_1-=np.mean(Ydata_1[0:25])
    Ydata_1/=np.max(Ydata_1)
    LSF_1 = LSF(Ydata_1)
    MTF_1 = MTF(LSF_1)[:60]
    return MTF_1

f=TFile(sys.argv[1])
hist = f.Get("MaxLikelihood_profile")
hist = GetHistArray(hist)
output = doExtraction(80,"Stack_100/MaxLikelihood_profile_stack_", True,15)
output2 = doExtraction(80,"Stack_100/MaxLikelihood_profile_stack_", False,15)

output3 = doExtraction(80,"Stack_100/MaxLikelihood_profile_stack_", True,3)
output4 = doExtraction(80,"Stack_100/MaxLikelihood_profile_stack_", False,3)

dx = 1
NX = 300
Xdata = 4*np.fft.fftfreq(300,d=np.tan(2.5*np.pi/180))[:60]
                                    
## Do the MTF    
plt.plot(Xdata,doTheMTF(hist),label='MaxLike')
plt.plot(Xdata,doTheMTF(output),label='blur-15')
plt.plot(Xdata,doTheMTF(output2),label='No blur-15')
plt.plot(Xdata,doTheMTF(output3),label='blur-3')
plt.plot(Xdata,doTheMTF(output4),label='No blur-3')
plt.xlim([0,10])
plt.ylim([0,1])
plt.legend()         
plt.show()


