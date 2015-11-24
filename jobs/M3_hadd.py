#!/usr/bin/env python
"""
Thomas:

"""

########################################
# Imports
########################################

import os
import sys
import ROOT
import subprocess
import datetime


########################################
# Main
########################################

def main():

    outdir_list = os.listdir('output')

    if len( outdir_list ) == 0:
        print 'No output found'
        return 0

    
    check_if_outroot_exists( outdir_list )



    # Filename for combined root file
    out_root_file = 'Combined.root'
    if not os.path.isfile(out_root_file):
        ROOT.TFile( out_root_file, "recreate")

    # Parse the command
    arg_list = [ out_root_file ]
    for outdir in outdir_list:
        arg_list.append( 'output/' + outdir + '/out.root' )


    print 'Passing the following arguments to hadd:'
    for arg in arg_list: print arg

    cmd = ' '.join( [ 'python ParHadd.py' ] + arg_list )
    print '\nGiven command:'
    print cmd

    print '\nExecuting:'
    subprocess.call( [cmd], shell=True )


    # Write attempt to timelog.txt
    with open( 'timelog.txt', 'a' ) as timelog:
        timelog.write( 'Attempted hadd on {0} files at {1}\n'.format(
            len(outdir_list),
            datetime.datetime.now()
            ))







########################################
# Functions
########################################

def check_if_outroot_exists( dirlist ):

    not_files = []
    for odir in dirlist:
        if not os.path.isfile( 'output/' + odir + '/out.root' ):
            not_files.append( 'output/' + odir + '/out.root' )

    if len(not_files)>1:
        print 'The following out.roots were not (yet) created:'
        for i in not_files: print i
        print 'Try again in a minute.'
        sys.exit()







########################################
# End of Main
########################################
if __name__ == "__main__":
    main()
