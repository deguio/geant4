echo "Trying to run the EEShashlik code"

#export ENERGY=$1
#export SETUP=$2

echo "energy:"
echo $ENERGY
echo "setup:"
echo $SETUP


export SCRAM_ARCH=slc6_amd64_gcc481
source /swshare/psit3/etc/profile.d/cms_ui_env.sh
source $VO_CMS_SW_DIR/cmsset_default.sh
#cd /shome/tklijnsm/CMSSWs/CMSSW_7_2_2_patch2/src
#eval `scramv1 runtime -sh`

# Geant4 specific
export G4LEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
export G4LEVELGAMMADATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/PhotonEvaporation3.1
export G4EMLOW=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/G4EMLOW6.40
export G4REALSURFACEDATA=/shome/tklijnsm/geant4/geant4.10.01.p02-install/share/Geant4-10.1.2/data/RealSurface1.0


# Job specific
export JOB_OUTDIR=$TMPDIR/

# Attempt to run
cd /mnt/t3nfs01/data01/shome/micheli/geant4/EEShashlikSimulation/H4OpticalSmall_$SETUP/cmake
source ../myroot.sh; source ../setup_g4.sh
./runEEShashlik -m runJob_$ENERGY"_"$SETUP.mac 

# Copy out.root to shome
export SHOME_OUTDIR=/mnt/t3nfs01/data01/shome/micheli/geant4/jobs/output/$ENERGY/$SETUP/$JOB_NAME/
echo $HOME_OUTDIR
ls
mkdir -p $SHOME_OUTDIR
cp -v $JOB_OUTDIR/out.root $SHOME_OUTDIR/out.root

