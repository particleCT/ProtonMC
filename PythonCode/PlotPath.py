
import matplotlib.pyplot as plt
import numpy as np
import sys
from ROOT import *
f=TFile(sys.argv[1],"update")
A=f.Get("phase")
for eventnb,event in enumerate(A):
    print eventnb

    fig, ax = plt.subplots()
    tracks_X = np.array(phase.tracks_X)
    tracks_Y = np.array(phase.tracks_Y)
    tracks_Z = np.array(phase.tracks_Z)

    print tracks_Z
    plt.plot(tracks_X,tracks_Z,"o-",label="Proton Path")

    m0 = np.array([event.x0,event.y0,event.z0])
    m1 = np.array([event.x1,event.y1,event.z1])
    p0 = np.array([event.px0,event.py0,event.pz0])
    p1 = np.array([event.px1,event.py1,event.pz1])

    
    p0/=np.linalg.norm(p0)
    p1/=np.linalg.norm(p1)
    alpha1=112
    alpha2=120
    L1 = float(alpha1)/100
    L2 = float(alpha2)/100
    Length = np.linalg.norm(m1-m0)
            
    p0*=Length*L1    
    p1*=Length*L2
    A = 2*m0  -2*m1 +p0+p1;
    B = -3*m0 +3*m1 -2*p0-p1;
    C = p0;
    D = m0;
    points_X = []
    points_Y = []
    points_Z = []
    
    for i in range(0,501):
        t=float(i)/500;
        points = A*(t*t*t) + B*(t*t) + C*t + D

        points_X.append(points[0])
        points_Y.append(points[1])
        points_Z.append(points[2])
    plt.plot(points_X,points_Z,'k-o',label="",linewidth=1.0,mfc='none')
    
    plt.show()
