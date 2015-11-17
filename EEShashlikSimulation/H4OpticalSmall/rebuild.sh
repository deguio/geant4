cd /shome/tklijnsm/testbeam/geant4/EEShashlikSimulation/H4OpticalSmall/cmake
rm -rf *
cp ../rebuild.sh rebuild.sh
cp ../runThomas.mac runThomas.mac
export G4REALSURFACEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/RealSurface1.0
cmake -DGeant4_DIR=/shome/tklijnsm/geant4/geant4.10.01.p02-install/lib64/Geant4-10.1.2/ ..
make
