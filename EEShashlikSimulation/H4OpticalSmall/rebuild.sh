cd /afs/cern.ch/user/t/tklijnsm/Work/geant4/EEShashlikSimulation/H4OpticalSmall
rm -rf cmake
mkdir cmake
cp rebuild.sh cmake
cp runThomas.mac cmake
cd cmake
cmake ..
make
