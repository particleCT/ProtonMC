
from ROOT import *
import numpy as np
import matplotlib.pyplot as plt
import sys
from scipy import ndimage
from scipy.ndimage.filters import gaussian_filter
import cv2

def GetHistArray(hist):
    
    err     = hist.ProjectionXY(hist.GetName()+"mean","C=E")
    hist    = hist.ProjectionXY(hist.GetName()+"err","")
    n     = [hist.GetNbinsY()+2,hist.GetNbinsX()+2]
    n2    = (n[0])*(n[1])
    
    
    d     = hist.GetArray()
    d.SetSize(n2)
    histA = np.array(d)
    histA = np.reshape(histA,n)
    
    d2     = err.GetArray()
    d2.SetSize(n2)
    histerr = np.array(d2)
    histerr = np.reshape(histerr,n)
    return histA, histerr
                                                
def rebin(a, shape):
    sh = shape[0],a.shape[0]//shape[0],shape[1],a.shape[1]//shape[1]
    return a.reshape(sh).mean(-1).mean(1)

def doExtraction(nbStack, histname, blur_bool, kernel):
    laps = []
    histlist = []
    shape =[ ]
    for i in range(0,nbStack):
        hist = f.Get(histname+str(i))
        hist, err = GetHistArray(hist)
        kernel_size = kernel        
        blur_size   = 5
        if(blur_bool):
            blurred = cv2.GaussianBlur(hist, (blur_size,blur_size), 0)
        else:
            blurred = cv2.GaussianBlur(err, (blur_size,blur_size), 0)

        laplace = cv2.Laplacian(blurred, cv2.CV_64F, ksize=kernel_size)
        laps.append(laplace)
        histlist.append(hist)
    laps = np.asarray(laps)
    abs_laps = np.absolute(laps)

    mean_laps = np.mean(abs_laps,axis=0)    
    plt.imshow(mean_laps)
    plt.plot([28],[50],'ro')
    plt.plot([78],[100],'bo')
    plt.plot([128],[150],'go')
    plt.plot([178],[200],'ko')
    plt.plot([228],[250],'yo')
    plt.show()
    
    plt.plot(abs_laps[:,28,50],'r-')
    plt.plot(abs_laps[:,78,100],'b-')
    plt.plot(abs_laps[:,128,150],'g-')
    plt.plot(abs_laps[:,178,200],'k-')
    plt.plot(abs_laps[:,228,250],'y-')
    plt.show()


    maxima = abs_laps.max(axis=0)
    bool_mask = abs_laps == maxima
    mask = bool_mask.astype(np.uint8)
    output = np.zeros(shape=laps[0].shape, dtype=np.float64)
    for i, hist in enumerate(histlist): output[bool_mask[i]] = hist[bool_mask[i]]
    return output

f=TFile(sys.argv[1])
print f.ls()
output = doExtraction(100,"Stack_100/MaxLikelihood_profile_stack_", True,3)
output = output[:,5:-5]
plt.plot(np.mean(output[20:700],axis=0), label="Laplace_20stack_mean")

#output = doExtraction(15,"Stack_20/MaxLikelihood_profile_stack_", False,5)
#output = output[:,5:-5]
#plt.plot(np.mean(output[20:700],axis=0), label="Laplace_20stack_err")

hist = f.Get("MaxLikelihood_profile")#Stack/MaxLikelihood_profile_stack_5")
hist,err = GetHistArray(hist)
hist = hist[:,5:-5]
plt.plot(np.mean(hist[20:200],axis=0),label="MaxLikelihood")

hist = f.Get("Stack_100/MaxLikelihood_profile_stack_10")
hist,err = GetHistArray(hist)
hist = hist[:,5:-5]
plt.plot(np.mean(hist[20:200],axis=0),label="Binning at depth middle")
plt.legend()
plt.show()
