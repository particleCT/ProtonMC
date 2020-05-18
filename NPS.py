import os,sys
import numpy as np
import matplotlib.pyplot as plt
import scipy as sp
from matplotlib import rc

from matplotlib.colors import LogNorm
import matplotlib
from ROOT import *
from matplotlib import rc
import matplotlib
from scipy import signal

rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 14})

def ROI(img,i,j, NX, NY):
	region = img[i*100:i*100+NX,j*100:j*100+NY] ## shift from half the size of the image
	region = region-np.mean(img)
	return region

def Transform2nparray(hprof):
    h = hprof.ProjectionXY("")
    n = [h.GetNbinsX(),h.GetNbinsY()]
    n2 = (n[1]+2)*(n[0]+2)
    d = h.GetArray()
    d.SetSize(n2)
    h = np.array(d,np.float64).reshape(n[1]+2,n[0]+2)
    return h

# main
def SPB(filename,argument,plot_bool):
    f=TFile(filename)
    print f.ls()
    hprof = f.Get(argument)
    imgcalcul = Transform2nparray(hprof)
    ## remove border
    img_resized = imgcalcul

    ##TF2D matrices
    NX,NY    = 200,200
    somme_TF = np.zeros((NX,NY))

    ## 2D Fourier transform and norm
    for j in range(0,4):
	for i in range(0,4):
                region = ROI(img_resized,i,j, NX, NY)
                a = np.fft.fft2(region)
                b = np.fft.fftshift(a)
                TF = np.absolute(b)**2                                
                somme_TF = np.add(somme_TF,TF)

    delta_x,delta_y = hprof.GetXaxis().GetBinWidth(0),hprof.GetYaxis().GetBinWidth(0)    

    #Divide by the mean value of the noise allow to have a normalized NPS (NNPS)
    W_out = (somme_TF*delta_y*delta_x)/(i*j*NY*NX)#*np.mean(img_resized))

    ## Frequency associated with the images, X and Y have same dimensions
    freq = np.fft.fftfreq(NX, d=delta_x*0.1)

    ## Radially averaged power spectrum -- first collect a dict of 
    Dict = {}
    for idy in range(0,NX): 
	for idx in range(0,NY):
                if (idx!=NX/2 and idy!=NY/2): 
                        rho = np.sqrt(freq[idx]**2+freq[idy]**2)
                        try:
                                Dict[rho] += W_out[idx,idy]/30.0
                        except:
                                Dict[rho] = W_out[idx,idy]/30.0


    X = sorted([key for key in Dict])
    Y = [Dict[key] for key in X]
    hist, bin_edges = np.histogram(X, bins=100,weights=Y)
    return bin_edges[:-1], hist
if __name__ == "__main__":
	X,Y = SPB(sys.argv[1],sys.argv[2],False)
        X2,Y2 = SPB(sys.argv[3],sys.argv[4],False)
        

        plt.plot(X,Y,label=sys.argv[2].replace("_"," "))
        plt.plot(X2,Y2,label=sys.argv[4].replace("_"," "))

	

	plt.xlabel(r"Frequency [lp/cm]")
	plt.yscale("log")
	plt.ylabel(r"Normalized Noise Power Spectrum [-]")
	plt.legend(frameon=False,loc=7)
	plt.show()




