
import argparse
import h5py
import numpy as np
import os.path

from mayavi import mlab


def readhdf5( fname, dSet_name ):
    #;{{{
    """
    Open hdf5-file, extract one dataset, and return it.
    Copied from little_helper.py on 2024-03-12.

    Parameters
    ----------
    fname: str
        filename of hdf5-file including full path
    dSet_name: str
        name of dataset to be read from hdf5-file

    Returns
    -------
    numpy array
    """

    err_value = 1

    # if file exists, open it for reading
    if os.path.isfile( fname ):
        h5f = h5py.File( fname, 'r' )
    else:
        print( 'ERROR: cannot read following file: {0}'.format( fname ))
        return err_value

    # trying to read dataset
    if dSet_name in h5f:
        data_in = h5f[ dSet_name ][()]
        # note that the dataset.value attribute was deprecated, i.e. 
        # data_in = h5f.get(dSet_name).value is no longer working
    else:
        print( 'ERROR: dataset <{0}> does not exists in file <{1}>'.format( dSet_name, fname ) )
        return err_value

    h5f.close()

    return data_in
    #;}}}



def plot_simple( fname_in, dSet_name='',
                 N_contLevels=20, 
                 colScale='lin',
                 plotReductionLevel=4,
                 fname_out='',
                 silent=True, 
               ):
    #{{{

    funcName    = 'plot_simple'

    # check keywords
    if len(dSet_name) == 0:
        print( funcName )
        print( "  ERROR: you did not set parameter 'dSet_name' " )
        print( "         will exit now" )
        return

    data2plot   = readhdf5( fname_in, dSet_name )

    print("dataset-name = {0}, min = {1}, max = {2}".format(dSet_name, np.amin(data2plot), np.amax(data2plot)) )

    if colScale == 'lin':
        contLevels  = np.linspace( np.amin(data2plot[::plotReductionLevel,::plotReductionLevel,::plotReductionLevel]),  # NOT use simply data2plot, otherwise error
                                   np.amax(data2plot[::plotReductionLevel,::plotReductionLevel,::plotReductionLevel]),  # NOT use simply data2plot, otherwise error
                                   N_contLevels )[1:].tolist()
    elif colScale == 'log':
        contLevels  = np.logspace( np.log10(1e-2), 
                                   np.log10(np.amax(data2plot[::plotReductionLevel,::plotReductionLevel,::plotReductionLevel])), 
                                   N_contLevels)[3:].tolist()

    if not silent:
        print( funcName )
        print( "  status: contour levels = ", contLevels )

    fig1    = mlab.figure( bgcolor=(1,1,1),
                           fgcolor=(0,0,0), # color of axes, orientation axes, label
                           size=(800,600),
                         )

    cont_Eabs   = mlab.contour3d( data2plot[::plotReductionLevel,::plotReductionLevel,::plotReductionLevel], 
                                  contours=contLevels,
                                  transparent=True, opacity=.4,
                                  figure=fig1
                                )


    n_e         = readhdf5( fname_in, "n_e" )
    cont_dens   = mlab.contour3d( n_e[::plotReductionLevel,::plotReductionLevel,::plotReductionLevel], 
                                  contours=[1],
                                  transparent=True, opacity=.3,
                                  color=(1,0,0),
                                  figure=fig1
                                )

    # create an axes instance to modify some of its properties afterwards
    ax1 = mlab.axes( nb_labels=4,
                     extent=[1, data2plot.shape[0]/plotReductionLevel, 
                             1, data2plot.shape[1]/plotReductionLevel,
                             1, data2plot.shape[2]/plotReductionLevel ],
                   )
    mlab.outline(ax1)
    ax1.axes.label_format   = '%.0f'
    # labels can also be set via mlab.xlabel('x')
    ax1.axes.x_label    = 'x'
    ax1.axes.y_label    = 'y'
    ax1.axes.z_label    = 'z'

    # set initial viewing angle
    # azimuth:   angle subtended by position vector on a sphere projected onto x-y plane with the x-axis, 0...360
    # elevation: zenith angle, i.e. angle subtended by position vector and the z-axis, 0...180
    mlab.view( azimuth=290, elevation=80 )

    if len(fname_out) > 0:
        mlab.savefig( fname_out )
    else:
        mlab.show()

    #}}}


def main():
    #{{{

    # initialize parser for command line options
    parser  = argparse.ArgumentParser()
    # add optional arguments
    parser.add_argument( "-f", "--filename", type=str, default="fileout.h5",
                         help="Filename of hdf5 output file from FOCAL." )
    parser.add_argument( "-d", "--dSet_name", type=str, default="E_abs__tint02500",
                         help="Dataset name to be plotted." )
    parser.add_argument( "-c", "--contLevels", type=int, default=20,
                         help="Number of contour levels used." )
    parser.add_argument( "-r", "--plotReductionLevel", type=int, default=4,
                         help="Reduce resolution for 3D plot by this amount." )
    parser.add_argument( "-t", "--time", type=int, default=0,
                         help="Timestep." )
    parser.add_argument( "-s", "--colScale", type=str, default="lin",
                         help="Lin or log color scale for contour plot." )
    parser.add_argument( "-o", "--output_file", type=str, default="",
                         help="Filename for plot (no X-window will be opened)." )
    parser.add_argument( "-e", "--cutExtended", type=float, default=1.9,
                         help="Cut grid point from plot (larger factor => larger cut, starting from 1.)." )


    # read all argments from command line
    args                = parser.parse_args()
    fname               = args.filename
    dSet_name           = args.dSet_name
    contLevels          = args.contLevels
    plotReductionLevel  = args.plotReductionLevel
    t_int               = args.time
    colScale            = args.colScale
    fname_plot          = args.output_file
    cutExtended_fact    = args.cutExtended

    print( "  Following arguments are set via command line options (if not set explicitely, their default values are used): " )
    print( "    fname = {0}".format(fname) )
    print( "    dSet_name = {0}".format(dSet_name) )
    print( "    contLevels = {0}".format(contLevels) )
    print( "    plotReductionLevel = {0}".format(plotReductionLevel) )
    print( "    t_int = {0}".format(t_int) )
    print( "    colScale = {0}".format(colScale) )
    print( "    fname_plot = {0}".format(fname_plot) )
    print( "    cutExtended_fact = {0}".format(cutExtended_fact) )

    plot_simple(fname, dSet_name=dSet_name, 
                N_contLevels=contLevels, 
                colScale=colScale, 
                plotReductionLevel=plotReductionLevel, 
                silent=False)

    #}}}


if __name__ == '__main__':
    main()

