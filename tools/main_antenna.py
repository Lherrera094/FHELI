# Developer: Luis Carlos Herrera Quesada
# Date: 05/12/2024
# Universität Stuttgart
#  
#Main file for antenna modeling, calls the other functions to construct, plot and save 
#the antenna values that wil be used in the FHELI code

import sys
sys.path.append("Antenna_geometryTK/")
from antenna_coordinates import *
from plot_geometry2D import *

def main():
    #{{{

    #Grid size values
    NX = 200
    NY = 200
    NZ = 400

    #Antenna center position
    ant_x = 100
    ant_y = 100
    ant_z = 200

    #Antenna characteristics
    ant_radius = 50
    antenna_lenght = 200

    x_coordinate, y_coordinate = construct_circular(ant_radius, ant_x, ant_y)
    plot_circular(x_coordinate, y_coordinate, NX, NY)

    # initialize parser for command line options
    #parser  = argparse.ArgumentParser()
    # add optional arguments
    #parser.add_argument( "-f", "--filename", type=str, default="antennaOut.csv",
    #                    help="Filename of hdf5 output file from FOCAL." )
    #parser.add_argument( "-d", "--dSet_name", type=str, default="",
    #                     help="Dataset name to be plotted." )
    #parser.add_argument( "-c", "--contLevels", type=int, default=20,
    #                     help="Number of contour levels used." )
    #parser.add_argument( "-r", "--plotReductionLevel", type=int, default=4,
    #                     help="Reduce resolution for 3D plot by this amount." )
    #parser.add_argument( "-p", "--plot_type", type=int, default=1,
    #                     help="Plot type." )
    #parser.add_argument( "-t", "--time", type=int, default=0,
    #                     help="Timestep." )
    #parser.add_argument( "-s", "--colScale", type=str, default="lin",
    #                     help="Lin or log color scale for contour plot." )

    # read all argments from command line
    #args                = parser.parse_args()
    #fname               = args.filename
    #dSet_name           = args.dSet_name
    #contLevels          = args.contLevels
    #plotReductionLevel  = args.plotReductionLevel
    #plot_type           = args.plot_type
    #t_int               = args.time
    #colScale            = args.colScale

    #print( "  Following arguments are set via command line options (if not set explicitely, their default values are used): " )
    #print( "    fname = {0}".format(fname) )
    #print( "    dSet_name = {0}".format(dSet_name) )
    #print( "    contLevels = {0}".format(contLevels) )
    #print( "    plotReductionLevel = {0}".format(plotReductionLevel) )
    #print( "    t_int = {0}".format(t_int) )
    #print( "    colScale = {0}".format(colScale) )

    #if plot_type == 1:
    #    plot_simple(fname, dSet_name=dSet_name, 
    #                contLevels=contLevels, plotReductionLevel=plotReductionLevel, 
    #                silent=False)
    #elif plot_type == 2:
    #    plot_fullwave( fname, t_int=t_int, 
    #                   include_absorbers=False, 
    #                   cutExtended_fact=1.,    # 1.5 might be useful value to crop density profile going to 0 from plot to not mislead user
    #                   oplot_dens_projection=True,
    #                   N_contLevels=contLevels, colScale=colScale, 
    #                   #oplot_Efieldcut='x1z1',
    #                 )

    #}}}


if __name__ == '__main__':
    main()
