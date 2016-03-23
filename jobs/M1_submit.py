#!/usr/bin/env python
"""
Thomas:

"""

########################################
# Imports
########################################

import os
import shutil
import subprocess
import datetime
import re
import time

########################################
# Main
########################################

def main():

    n_jobs = 100

    # Energy string
#    energy = "200GeV"
#    energy = "100GeV"
#    energy = "50GeV"
    energy = "20GeV"

    # Setup string
    setup = "simpler"
    print 'submitting jobs for '+setup


    # Get number of events per run
    runJobStr='runJob_'+energy+'_'+setup+'.mac'

    with open( runJobStr, 'r' ) as macfile:
        mac = macfile.read()
        n_events = re.search( r'/run/beamOn\s+([0-9]+)', mac ).group(1)


    # Copy the per-job Geant4 macro to /cmake
    shutil.copyfile( runJobStr, '/shome/micheli/geant4/EEShashlikSimulation/H4OpticalSmall_'+setup+'/cmake/runJob_'+energy+'_'+setup+'.mac' )
    

    # Set the directories for stdout and for the out.root files
    stddir = '/shome/micheli/geant4/jobs/stdout/'+energy+'_'+setup+"/"
    outdir = '/shome/micheli/geant4/jobs/output/'+energy+'_'+setup+"/"

    print stddir

    # Submit jobs
    for i in range(n_jobs):
        JOBNAME = 'job' + str(i)

        Recreate_dir( stddir + JOBNAME )

        cmd = [
            'qsub',
            '-q', 'all.q',
#            '-q', 'long.q',
            '-o', stddir + JOBNAME,
            '-e', stddir + JOBNAME,
            '-N', JOBNAME,
            '-v', 'ENERGY='+energy,
            '-v', 'SETUP='+setup,
            'jobscript.sh',
            ]

        print cmd

        #subprocess.call( cmd , stdout=open(os.devnull, 'wb') )
        subprocess.call(cmd)
        time.sleep(5)


    with open( 'timelog.txt', 'a' ) as timelog:
        timelog.write( 'Submitted {0} jobs, {1} events per job at {2}\n'.format(
            n_jobs,
            n_events,
            datetime.datetime.now()
            ))





########################################
# Functions
########################################

def Recreate_dir( dirname ):
    if os.path.isdir(dirname):
        shutil.rmtree(dirname)
    os.makedirs(dirname)




########################################
# End of Main
########################################
if __name__ == "__main__":
    main()
