export G4WORKDIR=$PWD
mkdir build
cd build

rm -rf *

#cmake -DGeant4_DIR=/home/cacof1/software/geant4.10.03.p03/lib/Geant4-10.3.3 -DCMAKE_INSTALL_PREFIX=$G4WORKDIR ../
cmake -DCMAKE_INSTALL_PREFIX=$G4WORKDIR ../

make -j8

make install

cd ../


