
import numpy as np
from ROOT import *
import glob,sys
import matplotlib.pyplot as plt
import matplotlib
from matplotlib import rc
rc('font', **{'family': 'serif', 'serif': ['Computer Modern'],'monospace': ['Computer Modern Typewriter']})
rc('text', usetex=True)
matplotlib.rcParams.update({'font.size': 22})
#SPWater      = np.loadtxt("Water_Geant4.dat")
SPWater      = np.loadtxt("Water_Geant4_He.dat")
#SPWater      = np.loadtxt("Water_Geant4_Deuterium.dat")

def GetHistArray(hist):
        err     = hist.ProjectionXY("err","C=E")
        hist    = hist.ProjectionXY("hist","")
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

        histA       = np.mean(histA[100:300],axis=0)[1:-1]
        histerr     = np.mean(histerr[100:300],axis=0)[1:-1]
        return histA, histerr
                                                            
def FindEnergyAtDepth(SPWater,Einit,R):
    Eout = Einit
    dx   = 1
    depth = np.arange(0,R, dx) ## cm
    for d in depth:
        idE    = np.argmin(np.abs(Eout-SPWater[:,0]))
        Eout  -= (dx)*SPWater[idE,1] ## dx in cm      
    return Eout


############################################################################
## Comparison with photons
############################################################################
def Dose_SNR_photon(SNR):
    Ex      = 75./1000 ## keV -> MeV
    fc      = 1.
    QDE     = 0.8 # Quantum detection effiency
    pix     = 0.1 # cm
    rho     = 1 # g/cm3
    uav     = 0.18925 # cm^-1
    pi_fact = np.pi*np.pi
    num     = 2*pi_fact*np.power(SNR,2)*Ex*fc*np.exp(uav*10)
    den     = rho*uav*QDE*np.power(pix,4)
    Dose    = num/den # MeV/g    
    Dose   *= 1E3 # g/Kg
    Dose   *= 1.6022E-13 # J/MeV (J/Kg is Gy)
    Dose   *= 1E3 # mGy/Gy
    return Dose
                                    
############################################################################
## Most Likely Path fed with various data
############################################################################
def MLP(x0 ,x1 ,p0 ,p1 ,tracks, tracks_pv, tracks_X0):

    NStep   = len(tracks)
    Y0      = [x0[2],p0[2]]
    Y2      = [x1[2],p1[2]]
    y_MLP   = []
    y_ERR   = []
    E0      = 13.6
    for k in range(1,NStep-1): ## Because at zero the logarithm diverges
        x            = tracks[k]
        
        # SIGMA I
        us1          = tracks[:k+1]
        L_0          = tracks_X0[:k+1]
        y1           = tracks_pv[:k+1]/L_0
        
        E0s1         = (E0*(1+0.038*np.log(np.trapz(x=us1,y=(x-x0[0])/L_0))))**2
        sigma_t1     = np.trapz(x=us1,y=((x-us1)**2)*y1)        
        sigma_theta1 = np.trapz(x=us1,y=y1)
        sigma_tt1    = np.trapz(x=us1,y=(x-us1)*y1)
        
        SI1          = E0s1*np.array([[sigma_t1, sigma_tt1],[ sigma_tt1, sigma_theta1]])
        R0           = np.array([[1, x-x0[0]], [0, 1]])
        
        # SIGMA II
        us2          = tracks[k:]
        L_2          = tracks_X0[k:]
        y2           = tracks_pv[k:]/L_2

        E0s2         = (E0*(1+0.038*np.log(np.trapz(x=us2,y=(x1[0]-x)/L_2))))**2
        sigma_t2     = np.trapz(x=us2,y=((x-us2)**2)*y2)
        sigma_theta2 = np.trapz(x=us2,y=y2)
        sigma_tt2    = np.trapz(x=us2,y=(x-us2)*y2)
        
        SI2          = E0s2*np.array([[sigma_t2, sigma_tt2],[sigma_tt2, sigma_theta2]])
        R2           = np.array([[1, (x1[0]-x)], [0, 1]])
        
        # Mean MLP
        SI1          = np.linalg.inv(SI1)
        SI2          = np.linalg.inv(SI2)
        part1        = np.dot(SI1,np.dot(R0,Y0)) + np.dot(SI2,np.dot(R2,Y2))
        part2        = np.linalg.inv(SI1+SI2)
        y1_MLP       = np.dot(part1,part2)
        y_MLP.append(y1_MLP.tolist())
        
        # Error MLP
        sig          = np.sqrt(part2)*2
        y_ERR.append(sig[0,0]) ## Both measurement
        #y_ERR.append(np.sqrt(sigma_t2)*10) ## Exit distribution
        #y_ERR.append(np.sqrt(sigma_t1)*10) ## Entrance distribution

    tracks_Z    =  np.concatenate([ [x0[2]], np.array(y_MLP)[:,0], [x1[2]]])
    tracks_sig  =  np.concatenate([[0], np.array(y_ERR),[0]])
    return tracks_Z, tracks_sig
                                                                                                          
############################################################################
## Create a pv map
############################################################################
def pv_map(Einit, Xmap, SPWater):
    dx    = (Xmap[-1] - Xmap[0])/len(Xmap)
    Estop = Einit
    tracks_E = []
    for X in Xmap:
        idE    = np.argmin(np.abs(Estop-SPWater[:,0]))
        Estop -= (dx/10)*SPWater[idE,1] ## dx in cm
        tracks_E.append(Estop)
    tracks_E   = np.array(tracks_E)
    tau        = tracks_E/938.272
    pv         = (tracks_E*(tau+2))/(tau+1)
    tracks_pv  = 1/np.power(pv,2)
    return np.array(tracks_pv)

############################################################################
## Calculate the Nomograms
############################################################################                                      
def DoNomograms(File1, Std, Energy):
        x0           = np.array([0,0,0])
        x1           = np.array([200,0,0]) ## mm
        p0           = np.array([1,0,0])
        p1           = np.array([1,0,0])
        tracks_X     = np.arange(0,200,0.5) ## mm
        tracks_X0    = np.array([361]*len(tracks_X)) ## mm   
        
        K            = 0.012 # CM -1
        R            = 100   # Radius CM
        rho          = 1.0 ## Density of water
        C            = 0.6 ## fraction of energy released to the medium
        SNR          = np.arange(0.1,50,0.1)
        pi_fact      = np.pi*np.pi
        signal       = 1.0 ## RSP
        var          = np.power(Std, 2)

        ## Get the MC factor
        f = TFile(File1)
        phase = f.Get("phase")
        energymid = []
        N         = 0
        N_0       = 100000. ## maybe change?
        for event in phase:
                energymid.append(event.EnergyMid)
                if(event.proc_name=="primary"): N +=1
        MC_factor = np.sum(energymid)/(N_0)
        f.Close()
        #tracks_pv = pv_map(Energy,tracks_X, SPWater)
        #tracks_Z, tracks_sig  =  MLP(x0, x1, p0, p1, tracks_X, tracks_pv, tracks_X0)
        #sig          = tracks_sig[len(tracks_sig)/2]/2 ## cm
        aout         = 1 #mm
        
        ## Get the energy at the exit
        #Eout    = FindEnergyAtDepth(SPWater, Energy, 2*R)
        #idEout  = np.argmin(np.abs(Eout-SPWater[:,0]))
        #SPEout  = SPWater[idEout,1]



        num     = 0.5*pi_fact*np.power(SNR,2)*var*(N_0/N)*MC_factor
        den     = 3*np.power(aout,4)*rho*np.power(signal,2)
        Dc      = num/den # MeV / g
        Dc     *= 1./1000 ## g/Kg
        Dc     *= 1E3#1./1000 ## mGy/Gy
        print Energy, N_0/N, MC_factor, var, Std
        return  Dc, SNR, Energy

############################################################################
## main
############################################################################


## Proton
EnergyList = [200]#,300]
Energies, SigmaList = np.loadtxt(sys.argv[1],unpack=True)
FileList = glob.glob(sys.argv[2]+"/*.root")
FileList = sorted(FileList)
fig, ax = plt.subplots(figsize=[9,9])
print "Energy", "Particle Loss", "Energy Deposited", "Variance"
print "Proton"
for ROOTFile in FileList:        
        Energy = int(ROOTFile.split("/")[-1].split("_")[1])
        if(Energy not in EnergyList): continue
        STD = SigmaList[np.where(Energies==Energy)][0]
        Dc1, SNR1, Energy1 = DoNomograms(ROOTFile, STD, Energy)
        ax.plot(Dc1,SNR1,'-.',label=r"Proton %.2f MeV/u"%tuple([Energy]), lw=1)

## Deuteron
EnergyList = [130]#, 200]
print "Deuteron"
Energies, SigmaList = np.loadtxt(sys.argv[3],unpack=True)
FileList = glob.glob(sys.argv[4]+"/*.root")
FileList = sorted(FileList)
for ROOTFile in FileList:
        Energy = int(ROOTFile.split("/")[-1].split("_")[1])
        if(Energy not in EnergyList): continue
        STD = SigmaList[np.where(Energies==Energy)][0]
        Dc1, SNR1, Energy1 = DoNomograms(ROOTFile, STD, Energy)
        ax.plot(Dc1,SNR1,'--',label=r"Deuteron %.2f MeV/u"%tuple([Energy]), markevery=25, mfc='white', lw=2)

## Helium
print "Helium"
EnergyList = [200]#,300]
Energies, SigmaList = np.loadtxt(sys.argv[5],unpack=True)
FileList = glob.glob(sys.argv[6]+"/*.root")
FileList = sorted(FileList)
for ROOTFile in FileList:
        Energy = int(ROOTFile.split("/")[-1].split("_")[1])
        if(Energy not in EnergyList): continue
        STD = SigmaList[np.where(Energies==Energy)][0]
        Dc1, SNR1, Energy1 = DoNomograms(ROOTFile, STD, Energy)
        ax.plot(Dc1,SNR1,':',label=r"Helium %.2f MeV/u"%tuple([Energy]), markevery=25, mfc='white', lw=2)

## Lithium
print "Lithium"
EnergyList = [250]
Energies, SigmaList = np.loadtxt(sys.argv[7],unpack=True)
FileList = glob.glob(sys.argv[8]+"/*.root")
FileList = sorted(FileList)
for ROOTFile in FileList:
        Energy = int(ROOTFile.split("/")[-1].split("_")[1])
        if(Energy not in EnergyList): continue
        STD = SigmaList[np.where(Energies==Energy)][0]
        Dc1, SNR1, Energy1 = DoNomograms(ROOTFile, STD, Energy)
        ax.plot(Dc1,SNR1,'-o',label=r"Lithium %.2f MeV/u"%tuple([Energy]), markevery=25, mfc='white', lw=2)

        
ax.annotate('R = 26 cm', xy=(0.86, 0.90), xytext=(0.86, 0.93), xycoords='axes fraction',
            ha='center', va='bottom',
            bbox=dict(boxstyle='square', fc='white'),
            arrowprops=dict(arrowstyle='-[, widthB=3.0, lengthB=0.5', lw=2.0))
"""
ax.annotate('R = 52cm', xy=(0.8, 0.70), xytext=(0.8, 0.60), xycoords='axes fraction',
            ha='center', va='bottom',
            bbox=dict(boxstyle='square', fc='white'),
            arrowprops=dict(arrowstyle='-[, widthB=0.8, lengthB=0.5', lw=2.0))
"""
ax.plot(Dose_SNR_photon(SNR1), SNR1, 'k-', lw =2 ,label = 'X-ray, 75 keV')    
ax.minorticks_on()
ax.yaxis.set_ticks_position('both')
ax.xaxis.set_ticks_position('both')
ax.tick_params('both', direction='in', length=4, width=1, which='minor')
ax.tick_params('both', direction='in', length=10, width=1, which='major')

ax.set_yscale('log')
ax.set_xscale('log')
ax.set_xlim([0.01,10])
ax.set_ylim([0.5,10])
ax.set_xlabel('Dose [mGy]')
ax.set_ylabel('SNR')
plt.grid(alpha=0.5, which='both')
plt.legend(frameon=False,loc=2)
plt.tight_layout()
plt.show()
