#!/usr/bin/env python
"""
Thomas:

"""

########################################
# Imports
########################################

from math import *

import matplotlib.pyplot as plt


########################################
# Main
########################################

def main():

    data_source = 'Paper'
    #data_source = 'Website'

    filename = 'Data_from_{0}.csv'.format(data_source)

    # Read raw data file
    # Entries are [ wavelength [nm], loss [dB/km] ]
    data = Read_data_file(filename)

    # For converting wavelength [nm] to photon energy [eV]:
    eV_per_nm = 0.0012398
    

    # ======================================
    # Processing data

    if data_source == 'Website':

        print '{0:15s} | {1:15s} | {2:15s} | {3:15s}'.format(
            'Wavelength [nm]', 'E_Photon [eV]',
            'Loss [dB/km]', 'abs length [m]' )

        wls = []
        photon_energies = []
        abs_lengths = []

        for wl, dB_per_km in data:

            photon_energy = wl * eV_per_nm
            abs_length = dB_per_km_TO_abs_length( dB_per_km )
            
            print '{0:<15.10f} | {1:<15.10f} | {2:<15.10f} | {3:<15.10f}'.format(
                wl, photon_energy, dB_per_km, abs_length)

            photon_energies.append( photon_energy )
            wls.append( wl )
            abs_lengths.append( abs_length )


    elif data_source == 'Paper':

        print '{0:15s} | {1:15s} | {2:15s} | {3:15s}'.format(
            'Wavelength [nm]', 'E_Photon [eV]',
            'Transmision [%]', 'abs length [m]' )

        wls = []
        photon_energies = []
        abs_lengths = []

        for wl, transmission in data:

            photon_energy = wl * eV_per_nm
            abs_length = transmission_TO_abs_length( transmission )
            
            print '{0:<15.10f} | {1:<15.10f} | {2:<15.10f} | {3:<15.10f}'.format(
                wl, photon_energy, transmission, abs_length)

            photon_energies.append( photon_energy )
            wls.append( wl )
            abs_lengths.append( abs_length )


    # Write to .txt file
    txt = open( 'PMMA_abs_length_{0}.txt'.format(data_source), 'w' )
    for wl, abs_length in zip( wls, abs_lengths ):
        txt.write( '{0:<15.10f} {1:<15.10f}\n'.format(wl, abs_length) )
    txt.close()
    
    # Format C++ style
    print '\nCopyable into EEShashDetectorConstruction.cc:'
    max_vals_per_line = 5

    wl_cpp_str = 'G4double photonEnergy_PMMA_abs[] = {\n    '
    vals_in_line = 0
    
    for wl in wls:
        wl_cpp_str += '{0:<8.6f} *eV, '.format( wl * eV_per_nm )
        vals_in_line += 1
        if vals_in_line == max_vals_per_line:
            wl_cpp_str += '\n    '
            vals_in_line = 0
    wl_cpp_str += '};'

    rl_cpp_str = 'G4double absPMMA[] = {\n    '
    vals_in_line = 0
    for rl in abs_lengths:
        rl_cpp_str += '{0:<9.6f} *m, '.format( rl )
        vals_in_line += 1
        if vals_in_line == max_vals_per_line:
            rl_cpp_str += '\n    '
            vals_in_line = 0

    rl_cpp_str += '};'

    print wl_cpp_str
    print rl_cpp_str

    # Make a plot
    plt.plot( photon_energies, abs_lengths )
    plt.xlabel( 'Photon energy [eV]' )
    plt.ylabel( 'Absorption length [m]' )
    plt.savefig( 'PMMA_abs_length_{0}.png'.format(data_source) )



########################################
# Functions
########################################

# ======================================
# Reads a raw data file
def Read_data_file( filename = "Attenuation_data.csv" ):
    with open( filename, 'r' ) as data_fp:
        data = data_fp.readlines()

    data = map( lambda x: map( float, x.split() ), data )
    return data


# ======================================
# Conversion functions: dB/km to abs length (for data from website)

# Abs length: distance after which intensity I_in has dropped to 1/e * I_in,
# so I_in / I_out = 1/(1/e) = e

# In decibels, this becomes:
# 10*log_10( I_in / I_out ) = 10*log_10( e ) =~ 4.342944819032518 dB / abs_length

# Getting att_length from dB/km:
# [ dB/abs_length ] / [ db/km ] = [ km/abs_length ]
def dB_per_km_TO_abs_length( dB_per_km ):
    dB_loss_after_one_abs_length = 10*log10( e )
    abs_length = dB_loss_after_one_abs_length / dB_per_km
    return 1000 * abs_length  # Return in meters


# ======================================
# Conversion functions: transmission to abs length (for data from website)

# transmission_fraction = exp( - distance / transmission_coef )
# --> Get transmission_coef from data, then get abs_length by
#     evaluating distance at transmission_fraction == 1/e
def transmission_TO_abs_length( transmission ):
    # The thickness of the material over which a certain percentage of
    # the initial intensity was lost. 3.175 mm in meters equals:
    h = 3.175 * 0.001
    transmission /= 100 # Raw data is in percentages, convert to fraction
    # Calculate transmission coef from data
    transmission_coef = -h / log(transmission)

    # Calcalate abs length from transmission coef
    abs_length = -transmission_coef * log(1/e)

    if False: # Turn on to check if intensity really drops to 1/e
        print '    Found transmission_coef = {0};\n    Checking: exp(-rad_length/coef) = {1}'.format( transmission_coef, exp( -abs_length / transmission_coef ) )

    return abs_length


########################################
# End of Main
########################################
if __name__ == "__main__":
    main()
