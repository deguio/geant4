#!/usr/bin/env python
"""
Thomas:

"""

########################################
# Imports
########################################

import ROOT
import os
import subprocess
import re
from copy import deepcopy
from shutil import copyfile

from optparse import OptionParser


########################################
# Main
########################################

# Global root counter for unique hist names
rootc = 100

# Standard ROOT label
tl = ROOT.TLatex()
tl.SetNDC()
tl.SetTextSize(0.035)
tl.SetTextColor(1)

def main():

    parser = OptionParser()
    parser.add_option('-c', default=False, action='store_true', help='if given, copies the directory to lxplus:www/plots/')
    (options, args) = parser.parse_args()

    ########################################
    # ROOT stuff
    ########################################

    ROOT.gROOT.SetBatch(True)
    ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = 1001;")

    ROOT.gStyle.SetOptFit(1011)
    #ROOT.gStyle.SetOptStat(0)

    c1 = ROOT.TCanvas("c1","c1",500,400)
    c1.SetGrid()


    ########################################
    # I/O stuff
    ########################################

    sample_base = '/shome/tklijnsm/Samples/EEShashlik/'

    #sample_fn = '/e_50GeV_1L_0.5M/Combined.root'
    #sample_fn = '/e_50GeV_1L_5k/Combined.root'
    #sample_fn = '/e_50GeV_1L_50k_smeared/Combined.root'
    sample_fn = '/e_50GeV_1L_50k_unsmeared/Combined.root'

    assert os.path.isfile( sample_base + sample_fn ), \
        '{0}{1} does not seem to be a file'.format(sample_base,sample_fn)

    input_root_file = ROOT.TFile( sample_base + sample_fn )
    input_tree = input_root_file.Get('EEShash')
    
    plotdir = sample_fn.split('/')[-2] + '_specials/'
    if not os.path.isdir(plotdir): os.makedirs(plotdir)


    ########################################
    # Branches to plot
    ########################################

    branches = [
        'EOpt_0',
        #'EOpt_1',
        #'EOpt_2',
        #'EOpt_3',
        ]


    ########################################
    # Actual work
    ########################################

    for branch in branches:

        print 'Drawing ' + branch

        # ======================================
        # Retrieve full histogram for EOpt

        print '    Retrieving full histogram'

        # Arbitrary histogram name, defined to be unique
        Hname = Get_Hname()

        n_entries = input_tree.Draw( branch + '>>' + Hname , '' )

        print '      Found {0} entries in full histogram'.format(n_entries)

        hist = getattr(ROOT, Hname ).Clone()

        x_max = hist.GetXaxis().GetXmax()
        print '      Maximum bin at ' + str(x_max)


        # ======================================
        # Make high binning, low cut plot

        # Cut off value on the x-axis
        x_lim = 15.0

        # Number of bins to be used
        bins_lim = 200

        print '    Retrieving highly binned, low cut histogram'
        print '      bins_lim = ' + str(bins_lim)
        print '      x_lim =    ' + str(x_lim)

        # Arbitrary histogram name, defined to be unique
        Hname = Get_Hname()


        n_entries = input_tree.Draw(
            # Draw string:
            '{0}>>{1}({2},0.0,{3})'.format( branch, Hname, bins_lim, x_lim ),
            # Selection string:
            '{0}<{1}'.format( branch, x_lim )
            )

        print '      Found {0} entries'.format(n_entries)

        hist = getattr(ROOT, Hname ).Clone()

        hist.SetTitle( 'StrangePeaks ' + branch )
        hist.GetXaxis().SetTitle( 'Energy [eV]' )
        hist.Draw()

        Print_c1( c1, plotdir + 'StrangePeaks_' + branch )


        # ======================================
        # Plot each peak separately

        cut_points = [
            1.0,
            3.0,
            5.5,
            8.0,
            10.0,
            12.0,
            15.0,
            ]

        n_peaks = len(cut_points)-1


        for i_peak in range(n_peaks):

            x_min = cut_points[i_peak]
            x_max = cut_points[i_peak+1]

            print '    Retrieving peak {0}: from {1} to {2}'.format(
                i_peak, x_min, x_max )


            # ======================================
            # Plot EOpt per peak

            # Arbitrary histogram name, defined to be unique
            Hname = Get_Hname()

            n_entries = input_tree.Draw(
                # Draw string:
                '{0}>>{1}({2},{3},{4})'.format( branch, Hname, 20, x_min, x_max ),
                # Selection string:
                '{0}>{1}&&{0}<{2}'.format( branch, x_min, x_max )
                )

            hist = getattr(ROOT, Hname ).Clone()

            hist.SetTitle( 'Peak {0} ({1} from {2} to {3})'.format(
                i_peak, branch, x_min, x_max ))
            hist.GetXaxis().SetTitle( 'Energy [eV]' )
            hist.Draw()
            
            Print_c1( c1, plotdir + 'Peak' + str(i_peak) + '_' + branch )


            # ======================================
            # Plot Fibre per peak

            i_fiber = branch[-1]  # a bit delicate
            fiberbranch = 'Fibre' + i_fiber

            # Arbitrary histogram name, defined to be unique
            Hname = Get_Hname()

            n_entries = input_tree.Draw(
                # Draw string:
                '{0}>>{1}'.format( fiberbranch, Hname ),
                # Selection string:
                '{0}>{1}&&{0}<{2}'.format( branch, x_min, x_max )
                )

            hist = getattr(ROOT, Hname ).Clone()

            hist.SetTitle( '# of photons in peak {0} ({1} from {2} to {3})'.format(
                i_peak, branch, x_min, x_max ))
            hist.GetXaxis().SetTitle( 'Energy [eV]' )
            hist.Draw()
            
            Print_c1( c1, plotdir + 'NoPhotons' + str(i_peak) + '_' + branch )




    # Avoid memory overload
    input_root_file.Close()



    ########################################
    # Non-essential html stuff
    ########################################

    # Open html file for overview
    fh = open( plotdir + 'overview.html', 'w' )
    fh.write( '<html><body>\n<h1>Additional plots for {0}\n</h1>\n<br>\n<hr />'.format(sample_fn) )

    for branch in branches:

        fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
            'StrangePeaks_' + branch ))
        fh.write( '<br>\n' )

        for i_peak in range(n_peaks):
            fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
                'Peak' + str(i_peak) + '_' + branch ))
            fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
                'NoPhotons' + str(i_peak) + '_' + branch ))
            fh.write( '<br>\n' )

        fh.write( '<hr />\n' )
        fh.write( '<br>\n' )

    fh.close()


    ########################################
    # Copy to lxplus
    ########################################

    if options.c:
        print 'Copying over to lxplus'
        
        lxplus_path_and_dir = 'tklijnsm@lxplus.cern.ch:~/www/plots/' + plotdir

        # Copy index.php in the plotdir
        copyfile( 'index.php', plotdir + '/index.php' )

        # Parse the command
        cmd = [ 'scp', '-r', plotdir, lxplus_path_and_dir ]

        # Execute
        subprocess.call( cmd )

        print 'Run the following command from directory ~/www on lxplus to allow the webserver to access the files:'
        print 'afind . -t d -e "fs setacl -dir {} -acl webserver:afs read"'



########################################
# Functions
########################################

# ======================================
def Make_draw_str( branch, n_bins=100, x_l=0.0, x_r=1.0 ):

    global rootc
    rootc += 1

    draw_str = "{0}>>h{1}({2},{3},{4})".format(
        branch,
        rootc,
        n_bins,
        x_l,
        x_r,
        )

    return ( draw_str, 'h' + str(rootc) )

# ======================================
# Creates arbitrary histogram name by increasing the counter
def Get_Hname():
    global rootc
    Hname = 'H' + str(rootc)
    rootc += 1
    return Hname

# ======================================
# Saves canvas c1 to file
def Print_c1( c1, out_name ):

    # Pdf
    c1.Print( out_name + '.pdf' , 'pdf' )

    # Png
    img = ROOT.TImage.Create()
    img.FromPad(c1)
    img.WriteImage( out_name + '.png' )

# ======================================
# Scales a histogram based on the maximum value *after* the first bin
def Scale_hist_without_first_bin( h ):

    # Copy histogram
    h_copy = deepcopy(h)

    # 'Delete' the first bin in the copy
    h_copy.SetBinContent( 1, 0.0 )

    # Get maximum, and set it in the original histogram
    y_max = h_copy.GetMaximum()

    h.SetMaximum( 1.2*y_max )

    # Return the value of the first bin
    return h.GetBinContent(1)

# ======================================
# Takes a branch, and checks if an element of string_list is in the branch name
def BranchContains( branch, string_list ):
    for string in string_list:
        if re.search( string, branch ): return True
    return False





########################################
# End of Main
########################################
if __name__ == "__main__":
    main()
