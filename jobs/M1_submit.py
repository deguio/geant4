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
#    setup = "singleFibre"
    print 'submitting jobs for '+setup

#sub parameters
 
    subcmd = 'qsub'
    queue  = 'all.q'
#    queue = 'long.q'


    print os.environ["HOSTNAME"]
    if  'lxpl' in os.environ["HOSTNAME"]: 
        subcmd = 'bsub'
        queue = '8nh'   


    # Get number of events per run
    runJobStr='runJob_'+energy+'_'+setup+'.mac'

    with open( runJobStr, 'r' ) as macfile:
        mac = macfile.read()
        n_events = re.search( r'/run/beamOn\s+([0-9]+)', mac ).group(1)



    if  'lxpl' not in os.environ["HOSTNAME"]:
    # Copy the per-job Geant4 macro to /cmake
        shutil.copyfile( runJobStr, '/shome/micheli/geant4/EEShashlikSimulation/H4OpticalSmall_'+setup+'/cmake/runJob_'+energy+'_'+setup+'.mac' )
    # Set the directories for stdout and for the out.root files
        stddir = '/shome/micheli/geant4/jobs/stdout/'+energy+'_'+setup+"/"
        outdir = '/shome/micheli/geant4/jobs/output/'+energy+'_'+setup+"/"

    else:
        shutil.copyfile( runJobStr, '/afs/cern.ch/work/m/micheli/geant4_new/EEShashlikSimulation/H4OpticalSmall_'+setup+'/cmake/runJob_'+energy+'_'+setup+'.mac' )
        stddir = '/afs/cern.ch/work/m/micheli/geant4_new/jobs/stdout/'+energy+'_'+setup+"/"
        outdir = '/afs/cern.ch/work/m/micheli/geant4_new/jobs/output/'+energy+'_'+setup+"/"


    print stddir


    exit

    # Submit jobs
    for i in range(n_jobs):
        JOBNAME = 'job' + str(i)

        Recreate_dir( stddir + JOBNAME )

        if  'lxpl' not in os.environ["HOSTNAME"]:
            cmd = [
                subcmd,
                '-q', queue,
                #            '-q', 'long.q',
                '-o', stddir + JOBNAME,
                '-e', stddir + JOBNAME,
                '-N', JOBNAME,
                '-v', 'ENERGY='+energy,
                '-v', 'SETUP='+setup,
                'jobscript.sh',
                ]

        else:

            cmd = [
                subcmd,
                '-q', queue,
                #            '-q', 'long.q',
                '-o', stddir + JOBNAME,
                '-e', stddir + JOBNAME,
                '-J', JOBNAME,
#                '-v', 'ENERGY='+energy,
#                '-v', 'SETUP='+setup,
                'jobscript_lxplus.sh '+energy+" "+setup
                ]


        print cmd
        
        #subprocess.call( cmd , stdout=open(os.devnull, 'wb') )
        subprocess.call(cmd)
        time.sleep(2)


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
