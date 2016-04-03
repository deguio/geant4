echo "Trying to run the EEShashlik code"

export ENERGY=$1
export SETUP=$2
export JOBNAME=$3

echo "energy:"
echo $ENERGY
echo "setup:"
echo $SETUP


export SHOME_OUTDIR=/afs/cern.ch/work/m/micheli/geant4_new/jobs/output/$1/$2/$3/
echo $SHOME_OUTDIR

#export SCRAM_ARCH=slc6_amd64_gcc481
#source /swshare/psit3/etc/profile.d/cms_ui_env.sh
#source $VO_CMS_SW_DIR/cmsset_default.sh
#cd /shome/tklijnsm/CMSSWs/CMSSW_7_2_2_patch2/src
#eval `scramv1 runtime -sh`

# Geant4 specific
#export G4LEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
#export G4LEVELGAMMADATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/PhotonEvaporation3.1
#export G4EMLOW=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
#export G4REALSURFACEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/RealSurface1.0


# Job specific
export JOB_OUTDIR=$TMPDIR

source /afs/cern.ch/sw/lcg/external/gcc/4.7/x86_64-slc6-gcc47-opt/setup.sh

# Attempt to run
cd /afs/cern.ch/work/m/micheli/geant4_new/EEShashlikSimulation/H4OpticalSmall_$SETUP/cmake
source ../setup_g4.sh
./runEEShashlik -m runJob_$ENERGY"_"$SETUP.mac 

# Copy out.root to shome

echo $SHOME_OUTDIR
ls -l
mkdir -p $SHOME_OUTDIR
cp -v $JOB_OUTDIR/out.root $SHOME_OUTDIR/out.root

