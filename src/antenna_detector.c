/*Antenna detector module contains functions to initialize,
store and save the number of antennas in the system.*/

#include "antenna_detector.h"

static double ***detAnt_01_fields = NULL;
static double ***detAnt_02_fields = NULL;
static double ***detAnt_03_fields = NULL;
static double ***detAnt_04_fields = NULL;
static double ***detAnt_05_fields = NULL;
static double ***detAnt_06_fields = NULL;

static double **detAnt_1D_01 = NULL;
static double **detAnt_1D_02 = NULL;
static double **detAnt_1D_03 = NULL;
static double **detAnt_1D_04 = NULL;
static double **detAnt_1D_05 = NULL;
static double **detAnt_1D_06 = NULL;

/*Initialize the arrays and values for detector antenna*/
int init_antennaDetect( gridConfiguration *gridCfg,
                        beamAntennaConfiguration *beamCfg,
                        antennaDetector *antDetect ){

    if( activate_antDetect1D == 1 ){

        detAnt_01_ypos  = ANT_Y;
        detAnt_01_zpos  = ANT_Z;
        detAnt_02_zpos  = round( ANT_Z - (ant_lenght/4) ); 
        detAnt_03_zpos  = round( ANT_Z + (ant_lenght/4) );
        detAnt_04_zpos  = round( ANT_Z + (ant_lenght/2) );
        detAnt_05_zpos  = round( ANT_Z +  ant_lenght    );
        detAnt_06_zpos  = round( ANT_Z + (3*ant_lenght/2) );

        // positions have to be even numbers, to ensure fields are accessed correctly
        if ((detAnt_01_ypos % 2) != 0)  ++detAnt_01_ypos;
        if ((detAnt_01_zpos % 2) != 0)  ++detAnt_01_zpos;
        if ((detAnt_02_zpos % 2) != 0)  ++detAnt_02_zpos;
        if ((detAnt_03_zpos % 2) != 0)  ++detAnt_03_zpos;
        if ((detAnt_04_zpos % 2) != 0)  ++detAnt_04_zpos;
        if ((detAnt_05_zpos % 2) != 0)  ++detAnt_05_zpos;
        if ((detAnt_06_zpos % 2) != 0)  ++detAnt_06_zpos;
        // issue a warning when detector antenna position is beyond NZ
        if (detAnt_06_zpos > (NZ - D_ABSORB)) {
            printf( "ERROR: check the detector antenna positions into z direction\n" );
            printf( "       NZ-D_ABSORB = %d, detAnt_06_zpos = %d\n", 
                    NZ - D_ABSORB, detAnt_06_zpos );
        }

        /*Initialize antennas*/
        // array for detector antennas
        // sum_t(Ex*Ex) | sum_t(Ey*Ey) | sum_t(Ez*Ez) | sum_t(E*E) | rms(E)
        // TODO: change into 3D array, such that each detector antenna corresponds
        //       to one 2D array; that way it can be written much more failsafe...
        //       requires some changes in procedures for storing and saving
        if (detAnt_01_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_01 = allocate2DArray( NX, 5 );
            detAnt_01_fields = allocate3DArray( NX, NY, 8 );
        }
        if (detAnt_02_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_02 = allocate2DArray( NX, 5 );
            detAnt_02_fields = allocate3DArray( NX, NY, 8 );
        }
        if (detAnt_03_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_03 = allocate2DArray( NX, 5 );
            detAnt_03_fields = allocate3DArray( NX, NY, 8 );
        }
        if (detAnt_04_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_04 = allocate2DArray( NX, 5 );
            detAnt_04_fields = allocate3DArray( NX, NY, 8 );
        }
        if (detAnt_05_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_05 = allocate2DArray( NX, 5 );
            detAnt_05_fields = allocate3DArray( NX, NY, 8 );
        }
        if (detAnt_06_zpos < ( NZ - D_ABSORB)) {
            detAnt_1D_06 = allocate2DArray( NX, 5 );
            detAnt_06_fields = allocate3DArray( NX, NY, 8 );
        }
        
    }

    return EXIT_SUCCESS;
}

/*Liberates the allocated memory for the antenna*/
int free_antDetect( gridConfiguration *gridCfg,
                    antennaDetector *antDetect ){

    if (detAnt_01_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_01, NX);
        free3DArray(detAnt_01_fields, NX, NY);            
    }
    if (detAnt_02_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_02, NX);
        free3DArray(detAnt_02_fields, NX, NY);
    }
    if (detAnt_03_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_03, NX);
        free3DArray(detAnt_03_fields, NX, NY);
    }
    if (detAnt_04_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_04, NX);
        free3DArray(detAnt_04_fields, NX, NY);
    }
    if (detAnt_05_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_05, NX);
        free3DArray(detAnt_05_fields, NX, NY);
    }
    if (detAnt_06_zpos < ( NZ - D_ABSORB)) {
        free2DArray(detAnt_1D_06, NX);
        free3DArray(detAnt_06_fields, NX, NY);
    }

    printf("Freed Antenna detector allocated memory. \n");

    return EXIT_SUCCESS;
}

/*Print in console the antennad detect information*/
int print_antennaDetec( antennaDetector *antDetect ){

    if( activate_antDetect1D == 1 ){

        printf("------------Detector Antenna Positions------------\n");
        printf( "detector antenna y: y1 = %d\n", detAnt_01_ypos );
        printf( "detector antenna 01: z1 = %d\n", detAnt_01_zpos );
        printf( "detector antenna 02: z2 = %d\n", detAnt_02_zpos );
        printf( "detector antenna 03: z3 = %d\n", detAnt_03_zpos );
        printf( "detector antenna 04: z4 = %d\n", detAnt_04_zpos );
        printf( "detector antenna 05: z5 = %d\n", detAnt_05_zpos );
        printf( "detector antenna 06: z6 = %d\n", detAnt_06_zpos );

    } else {
        printf("No detector antenna initialized. \n");
    }

    return EXIT_SUCCESS;
}

/*During time evolution, control the storing of values in the arrays*/
int control_antennaDetect(  gridConfiguration *gridCfg,
                            antennaDetector *antDetect,
                            int t_int,
                            double EB_WAVE[NX][NY][NZ] ){

    if( activate_antDetect1D == 1 ){ 
        // store wavefields for detector antennas over the final 10 
        // oscillation periods, it was found previously that only one period
        // does not result in a too nice average
        if ( t_int >= ( T_END - 10*PERIOD ) ) {
                if (detAnt_01_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_01_zpos,
                                            t_int, EB_WAVE, detAnt_1D_01 );
                    detAnt2D_storeValues(   gridCfg, detAnt_01_zpos, t_int,  
                                            EB_WAVE, detAnt_01_fields );
                }
                if (detAnt_02_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_02_zpos,
                                            t_int, EB_WAVE, detAnt_1D_02 );
                    detAnt2D_storeValues(   gridCfg, detAnt_02_zpos, t_int, 
                                            EB_WAVE, detAnt_02_fields );
                }
                if (detAnt_03_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_03_zpos,
                                            t_int, EB_WAVE, detAnt_1D_03 );
                    detAnt2D_storeValues(   gridCfg, detAnt_03_zpos, t_int,
                                            EB_WAVE, detAnt_03_fields );
                }
                if (detAnt_04_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_04_zpos,
                                            t_int, EB_WAVE, detAnt_1D_04 );
                    detAnt2D_storeValues(       gridCfg, detAnt_04_zpos, t_int,
                                        EB_WAVE, detAnt_04_fields );
                }
                if (detAnt_05_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_05_zpos,
                                            t_int, EB_WAVE, detAnt_1D_05 );
                    detAnt2D_storeValues(   gridCfg, detAnt_05_zpos, t_int,
                                            EB_WAVE, detAnt_05_fields );
                }
                if (detAnt_06_zpos < ( NZ - D_ABSORB)) {
                    detAnt1D_storeValues(   gridCfg, detAnt_01_ypos, detAnt_06_zpos,
                                            t_int, EB_WAVE, detAnt_1D_06 );
                    detAnt2D_storeValues(   gridCfg, detAnt_06_zpos, t_int,
                                            EB_WAVE, detAnt_06_fields );
                }
            }

    }

    return EXIT_SUCCESS;
}

//Save the field values in the antenna detector arrays
int detAnt1D_storeValues(   gridConfiguration *gridCfg, 
                            size_t detAnt_ypos, size_t detAnt_zpos,
                            int tt, 
                            double EB_WAVE[NX][NY][NZ], 
                            double **detAnt_fields ) { 
    //{{{
    size_t
        ii;

    double
        foo;

    // Ex: odd-even-even
    // Ey: even-odd-even
    // Ez: even-even-odd

#pragma omp parallel default(shared) private(ii,foo)
#pragma omp for
    for ( ii=2 ; ii <= NX-2 ; ii+=2 ) {
        // calculate abs(E)
        foo = sqrt(  pow(EB_WAVE[ii+1][detAnt_ypos  ][detAnt_zpos  ],2)
                    +pow(EB_WAVE[ii  ][detAnt_ypos+1][detAnt_zpos  ],2)
                    +pow(EB_WAVE[ii  ][detAnt_ypos  ][detAnt_zpos+1],2) );

        // sum of E over time
        // Ex*Ex
        detAnt_fields[ii/2][0]  += pow( EB_WAVE[ii+1][detAnt_ypos  ][detAnt_zpos  ], 2 );
        // Ey*Ey
        detAnt_fields[ii/2][1]  += pow( EB_WAVE[ii  ][detAnt_ypos+1][detAnt_zpos  ], 2 );
        // Ez*Ez
        detAnt_fields[ii/2][2]  += pow( EB_WAVE[ii  ][detAnt_ypos  ][detAnt_zpos+1], 2 );
        // E*E
        detAnt_fields[ii/2][3]  += foo*foo;

        // corresponding to an rms(E)-like quantity
        detAnt_fields[ii/2][4]  += ( foo * sqrt(1./( (double)(tt)/(double)(PERIOD) + 1e-6 )) );

        //printf( "tt = %d, ii = %d, sum_t(E*E) = %13.5e\n",
        //        tt, ii, detAnt_fields[ii/2][3] );
    }

    return EXIT_SUCCESS;

}//}}}

int detAnt2D_storeValues(   gridConfiguration *gridCfg, 
                            size_t detAnt_zpos, int tt, 
                            double EB_WAVE[NX][NY][NZ], 
                            double ***detAnt_fields ) { 
    //{{{
    size_t
        ii, jj;

    double
        foo_e, foo_b;

    // Ex: odd-even-even        Bx: even-odd-odd
    // Ey: even-odd-even        Ey: odd-even-odd
    // Ez: even-even-odd
    
    // Save the Electric and Magnetic field components of the last -1 time step
    if( tt == T_END - 1) {

#pragma omp parallel default(shared) private(ii,jj,foo_e,foo_b)
#pragma omp for
        for ( ii=2 ; ii <= NX-2 ; ii+=2 ) {
            for ( jj=2 ; jj <= NY-2 ; jj+=2 ) {
            
                // calculate abs(E)
                foo_e = sqrt(   pow(EB_WAVE[ii+1][jj  ][detAnt_zpos  ],2)
                               +pow(EB_WAVE[ii  ][jj+1][detAnt_zpos  ],2)
                               +pow(EB_WAVE[ii  ][jj  ][detAnt_zpos+1],2) );

                // Store Electric field components
                // Ex component
                detAnt_fields[ii/2][jj/2][0]  = EB_WAVE[ii+1][jj  ][detAnt_zpos  ];
                // Ey component
                detAnt_fields[ii/2][jj/2][1]  = EB_WAVE[ii  ][jj+1][detAnt_zpos  ];
                // Ez Component
                detAnt_fields[ii/2][jj/2][2]  = EB_WAVE[ii  ][jj  ][detAnt_zpos+1];
                // E*E abslute value
                detAnt_fields[ii/2][jj/2][3]  = foo_e*foo_e;
                
                // calculate abs(B)
                foo_b = sqrt(   pow(EB_WAVE[ii  ][jj+1][detAnt_zpos+1],2)
                               +pow(EB_WAVE[ii+1][jj  ][detAnt_zpos+1],2)
                               +pow(EB_WAVE[ii+1][jj+1][detAnt_zpos  ],2) );
                    
                //Store Magnetic field components
                //Bx component
                detAnt_fields[ii/2][jj/2][4]  = EB_WAVE[ii  ][jj+1][detAnt_zpos+1];
                //By component
                detAnt_fields[ii/2][jj/2][5]  = EB_WAVE[ii+1][jj  ][detAnt_zpos+1];
                //Bz component
                detAnt_fields[ii/2][jj/2][6]  = EB_WAVE[ii+1][jj+1][detAnt_zpos  ];
                //B*B absolute value
                detAnt_fields[ii/2][jj/2][7]  = foo_b*foo_b;
                
            }
        }
    }

    return EXIT_SUCCESS;

}//}}}

//Send the antenna detect arrays to the grid_io module for storage in the HDF5 output file.
void save_AntDetect(    gridConfiguration *gridCfg, saveData *saveDCfg,
                        antennaDetector *antDetect ){

    if( activate_antDetect1D == 1 ){

        /*Char values as directions to the correct folder*/
        char filename_hdf5[PATH_MAX];

        sprintf(filename_hdf5,"%s/%s/%s", projectPath, foldername, file_hdf5);

        if (detAnt_01_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_01_1D" , 
                                    detAnt_01_ypos, detAnt_01_zpos, detAnt_1D_01 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_01" , 
                                 detAnt_01_zpos, detAnt_01_fields );
        }
        if (detAnt_02_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_02_1D" , 
                                    detAnt_01_ypos, detAnt_02_zpos, detAnt_1D_02 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_02" , 
                                 detAnt_02_zpos, detAnt_02_fields );
        }
        if (detAnt_03_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_03_1D" , 
                                    detAnt_01_ypos, detAnt_03_zpos, detAnt_1D_03 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_03" , 
                                 detAnt_03_zpos, detAnt_03_fields );
        }
        if (detAnt_04_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_04_1D" , 
                                    detAnt_01_ypos, detAnt_04_zpos, detAnt_1D_04 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_04" , 
                                 detAnt_04_zpos, detAnt_04_fields );
        }
        if (detAnt_05_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_05_1D" , 
                                    detAnt_01_ypos, detAnt_05_zpos, detAnt_1D_05 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_05" , 
                                 detAnt_05_zpos, detAnt_05_fields );
        }
        if (detAnt_06_zpos < ( NZ - D_ABSORB)) {
            detAnt1D_write2hdf5(    NX, filename_hdf5, "/detAnt_06_1D" , 
                                    detAnt_01_ypos, detAnt_06_zpos, detAnt_1D_06 );
            detAnt2D_write2hdf5( NX, NY, filename_hdf5, "/detAnt_06" , 
                                 detAnt_06_zpos, detAnt_06_fields );
        }

    }

}
