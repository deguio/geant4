if [ $HOSTNAME = "t3ui17" ]; then
    # Set paths to the extra Geant4 data; works by default on lxplus
    export G4LEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
    export G4LEVELGAMMADATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/PhotonEvaporation3.1
    export G4EMLOW=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
    export G4REALSURFACEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/RealSurface1.0
    echo "Environment set up for PSI"
else
    source /afs/cern.ch/sw/lcg/contrib/gcc/4.7/x86_64-slc6/setup.sh
    source /afs/cern.ch/sw/lcg/external/geant4/10.0.p01/x86_64-slc6-gcc47-opt/CMake-setup.sh
    export CXX=/afs/cern.ch/sw/lcg/contrib/gcc/4.7/x86_64-slc6/bin/g++
    export CC=/afs/cern.ch/sw/lcg/contrib/gcc/4.7/x86_64-slc6/bin/gcc
    echo "Environment set up for lxplus"
fi
