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
    parser.add_option('--sample', type='string', help='filename of the sample (takes a regex)')
    (options, args) = parser.parse_args()

    ########################################
    # ROOT stuff
    ########################################

    ROOT.gROOT.SetBatch(True)
    ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = 1001;")

    ROOT.gStyle.SetOptFit(1011)
    #ROOT.gStyle.SetOptStat(0)

    c1 = ROOT.TCanvas("c1","c1",1000,800)
    c1.SetGrid()


    ########################################
    # I/O stuff
    ########################################

    sample_base = '/shome/tklijnsm/Samples/EEShashlik/'

    #sample_fn = '/e_50GeV_1L_0.5M/Combined.root'
    #sample_fn = '/e_50GeV_1L_5k/Combined.root'
    #sample_fn = '/e_50GeV_1L_50k_smeared/Combined.root'
    #sample_fn = '/e_50GeV_1L_50k_unsmeared/Combined.root'
    #sample_fn = '/e_50GeV_1L_250k_unsmeared_fibre0/Combined.root'
    #sample_fn = '/e_50GeV_1L_250k_unsmeared_fibre1/Combined.root'
    #sample_fn = '/e_50GeV_1L_250k_unsmeared_fibre2/Combined.root'
    #sample_fn = '/e_50GeV_1L_250k_unsmeared_fibre3/Combined.root'
    #sample_fn = '/e_50GeV_1L_50k_unsmeared_center_wrongABSspec/Combined.root'
    #sample_fn = '/e_50GeV_15L_INCOMPLETE_smeared_center/Combined.root'
    #sample_fn = '/e_50GeV_15L_10k_smeared_center/Combined.root'

    sample_fn = '/pi_50GeV_1L_250k_unsmeared_fibre1/Combined.root'

    if options.sample != None:
        all_dirs = os.listdir(sample_base)
        for a_dir in all_dirs:
            if re.search( options.sample, a_dir ):
                sample_fn = a_dir + '/Combined.root'

    print 'Using ' + sample_fn


    assert os.path.isfile( sample_base + sample_fn ), \
        '{0}{1} does not seem to be a file'.format(sample_base,sample_fn)

    input_root_file = ROOT.TFile( sample_base + sample_fn )
    input_tree = input_root_file.Get('EEShash')
    
    plotdir = sample_fn.split('/')[-2] + '/'
    if not os.path.isdir(plotdir): os.makedirs(plotdir)


    ########################################
    # Branches to plot
    ########################################

    branches = [
        'Eabs',
        'Eact',
        'Ebgo',
        'EfibrCore',
        'EfibrClad',
        'nLayers',
        'Eact_0',
        #'Eact_1', # Only works for >1 layer runs
        'nBGOs',
        'Ebgo_0',
        'Ebgo_1',
        'nFibers',
        'EFiber_0',
        'EFiber_1',
        'EFiber_2',
        'EFiber_3',
        'Fibre0',
        'Fibre1',
        'Fibre2',
        'Fibre3',
        'xPosition',
        'yPosition',
        'EOpt_0',
        'EOpt_1',
        'EOpt_2',
        'EOpt_3',
        ]

    # The program usually tries to zoom in on the histogram
    # This list defines for which branches the program should skip that step
    skip_zoom_list = [
        'nLayers',
        'nBGOs',
        'nFibers',
        'xPosition',
        'yPosition',
        ]


    ########################################
    # Actual work
    ########################################

    for branch in branches:

        # Determine the x-axis label
        if re.search( r'E.+', branch ): x_label = 'Energy [MeV]'
        elif re.search( r'Fibre[0-9]', branch ): x_label = 'Number of hits'
        elif re.search( r'\wPosition', branch ): x_label = 'Distance from (0.0,0.0) [mm]'
        else: x_label = ''
        if re.search( r'EOpt_[0-9]', branch ): x_label = 'Energy [eV]' # Exception to E.+ rule

        print 'Drawing ' + branch


        ########################################
        # Retrieve the full histogram
        #  - This is done for every branch
        ########################################

        print '    Full histogram'

        # Arbitrary histogram name, defined to be unique
        Hname = Get_Hname()

        #n_entries = input_tree.Draw( branch + '>>' + Hname , '' )

        if re.search( r'EOpt_[0-9]', branch ):
            n_entries = input_tree.Draw( branch + '>>' + Hname + '(100,0.0,800.0)' , '' )
        else:
            n_entries = input_tree.Draw( branch + '>>' + Hname , '' )


        print '      Found {0} entries'.format(n_entries)

        hist = getattr(ROOT, Hname ).Clone()
        hist.SetTitle( 'Full ' + branch )
        hist.GetXaxis().SetTitle(x_label)
        hist.Draw()
        Print_c1( c1, plotdir + 'full_' + branch )

        n_bins_used = hist.GetSize() - 2
        print '      Using {0} bins'.format(n_bins_used)

        x_max = hist.GetXaxis().GetXmax()
        print '      Maximum bin at ' + str(x_max)

        # Don't do the zooming for some branches
        if branch in skip_zoom_list: continue


        ########################################
        # Zoomed histograms
        ########################################

        # Cut off value on the x-axis
        #  - 0.15 is usually okay; could maybe make this value change per branch
        x_lim = 0.15*x_max

        # Number of bins to be used
        bins_lim = 100

        # --------------------------------------
        # Special stuff for some branches
        
        # Do binning differently for Fibre*, since integer entries make histograms with holes
        if re.search( r'Fibre[0-9]', branch ): bins_lim = min( 100, int(x_lim) )

        # Scale some branches to eV's
        if BranchContains( branch, [ 'EFiber', 'EfibrCore', 'EfibrClad' ] ):
            print '    * Scaling from MeV to eV'
            scale = 1000000.0
            x_label = 'Energy [eV]'
        else:
            scale = 1.0
        # --------------------------------------


        # ======================================
        # Retrieve histogram from x = 0 to x_lim

        print '    Zoomed histogram'

        # Arbitrary histogram name, defined to be unique
        Hname = Get_Hname()


        print '      Making zoomed histogram (reduce x-axis range to 15%)'
        n_entries = input_tree.Draw(
            # Draw string:
            '{4}*{0}>>{1}({2},0.0,{3})'.format( branch, Hname, bins_lim, scale*x_lim, scale ),
            # Selection string:
            '{0}<{1}'.format( branch, x_lim )
            )

        print '      Found {0} entries'.format(n_entries)

        hist = getattr(ROOT, Hname ).Clone()

        hist.SetTitle( 'Zoomed ' + branch + ' (x-axis range reduced to 15%)' )
        hist.GetXaxis().SetTitle(x_label)
        hist.Draw()

        # Do some fancy zooming for some branches:
        #  - Looks for the next maximum other than the first bin, and sets the range
        #    to 1.2 times that second maximum
        #  - Useful when the first bin is so large the rest of the histogram is barely visible
        if BranchContains( branch, [ 'EFiber', 'EfibrCore', 'EfibrClad' ] ):
            print '    * Ignoring first bin when setting the maximum for the plot'
            peak_value = Scale_hist_without_first_bin( hist )
            tl.DrawLatex( 0.13, 0.85, '#uparrow First bin: ' + str(peak_value) )

        Print_c1( c1, plotdir + 'zoomed_' + branch )


        # ======================================
        # Retrieve histogram from x = 0 to 0.15*x_max, and entries are non-zero

        print '    Zoomed non-zero histogram'

        # Arbitrary histogram name, defined to be unique
        Hname = Get_Hname()

        if re.search( r'EOpt_[1-9]', branch ):
            x_lim = 8.0 # Set manually to 8.0


        print '      Making zoomed non-zero histogram (reduce x-axis range to 15%)'
        n_entries = input_tree.Draw(
            # Draw string
            '{4}*{0}>>{1}({2},0.0,{3})'.format( branch, Hname, bins_lim, scale*x_lim, scale ),
            # Selection string
            '{0}<{1}&&{0}!=0.0'.format( branch, x_lim )
            )

        print '      Found {0} entries'.format(n_entries)

        hist = getattr(ROOT, Hname ).Clone()
        hist.SetTitle( 'Zoomed non-zero ' + branch + ' (x-axis range reduced to 15%)' )
        hist.GetXaxis().SetTitle(x_label)
        hist.Draw()

        # Do some fancy zooming for some branches:
        #  - Looks for the next maximum other than the first bin, and sets the range
        #    to 1.2 times that second maximum
        #  - Useful when the first bin is so large the rest of the histogram is barely visible
        if BranchContains( branch, [ 'EFiber', 'EfibrCore', 'EfibrClad' ] ):
            peak_value = Scale_hist_without_first_bin( hist )
            tl.DrawLatex( 0.13, 0.85, '#uparrow First bin: ' + str(peak_value) )

        Print_c1( c1, plotdir + 'zoomednonzero_' + branch )


    # Avoid memory overload
    input_root_file.Close()


    ########################################
    # Non-essential html stuff
    ########################################

    # Open html file for overview
    fh = open( plotdir + 'overview.html', 'w' )
    fh.write( '<html><body>\n<h1>Plots for {0}\n</h1>\n<br>\n<hr />'.format(sample_fn) )

    for branch in branches:
        fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
            'full_' + branch ))

        if not branch in skip_zoom_list:
            fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
                'zoomed_' + branch ))
            fh.write( '<a href="{0}.pdf"><img width="450" src="{0}.png"></a>\n'.format(
                'zoomednonzero_' + branch ))

        fh.write( '<hr />\n' )
        fh.write( '<br>\n' )

    fh.close()


    ########################################
    # Copy to lxplus
    ########################################

    if options.c:
        print 'Copying over to lxplus'
        
        plotdir = sample_fn.split('/')[-2]
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
