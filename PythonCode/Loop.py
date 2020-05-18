import sys,os
import numpy as np

AngleList = np.arange(0,360,2)
for angle in AngleList:
    command = "./bin/Linux-g++/ProtonMC 2000000 250 Catphan "+str(angle)+" 30 0 1"
    print command
    os.system(command)
