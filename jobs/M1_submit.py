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


########################################
# Main
########################################

def main():

    n_jobs = 500

    # Get number of events per run
    with open( 'runJob.mac', 'r' ) as macfile:
        mac = macfile.read()
        n_events = re.search( r'/run/beamOn\s+([0-9]+)', mac ).group(1)


    # Copy the per-job Geant4 macro to /cmake
    shutil.copyfile( 'runJob.mac', '/shome/tklijnsm/testbeam/geant4/EEShashlikSimulation/H4OpticalSmall/cmake/runJob.mac' )

    # Set the directories for stdout and for the out.root files
    stddir = '/shome/tklijnsm/testbeam/geant4/jobs/stdout/'
    outdir = '/shome/tklijnsm/testbeam/geant4/jobs/output/'

    # Submit jobs
    for i in range(n_jobs):
        JOBNAME = 'job' + str(i)

        Recreate_dir( stddir + JOBNAME )

        cmd = [
            'qsub',
            '-q', 'all.q',
            '-o', stddir + JOBNAME,
            '-e', stddir + JOBNAME,
            '-N', JOBNAME,
            'jobscript.sh'
            ]

        #subprocess.call( cmd , stdout=open(os.devnull, 'wb') )
        subprocess.call( cmd )

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
