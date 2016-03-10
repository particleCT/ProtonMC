from ROOT import *
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import rc,rcParams
import sys
#f=TFile("200MeVSlab.root")
f=TFile(sys.argv[1])
from matplotlib.patches import FancyArrowPatch


phase = f.Get("phase")
for event in phase:
    fig, ax = plt.subplots()
    tracks_Z = phase.tracks_Z
    tracks_X = phase.tracks_X
    tracks_Y = phase.tracks_Y
    print np.array(tracks_X)
    plt.plot(tracks_X,tracks_Z,"o--",label="Proton Path")
    plt.show()
    

