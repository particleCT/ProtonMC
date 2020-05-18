import os, sys
A = sys.argv[1]
for energy in range(120,400,10):
    command = "/home/cacof1/software/ProtonMC/bin/Linux-g++/ProtonMC 100000 "+str(energy)+" WaterBox 0 20 0 "+str(A)
    print command
    os.system(command)
