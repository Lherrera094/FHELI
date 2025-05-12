#include "continuation_module.h"

 void init_continuity(   gridConfiguration *gridCfg,
                            saveData *saveDCfg,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ] ){
    
    printf("----------------Continue Simulations---------------\n");
    check_hdf5( gridCfg, saveDCfg, EB_WAVE, J_B0 );

}

void check_hdf5(    gridConfiguration *gridCfg,
                    saveData *saveDCfg,
                    double EB_WAVE[NX][NY][NZ],
                    double J_B0[NX][NY][NZ] ){

    //Function checks if continuation is possible and proceed to save the fields for continuation
    char fullDir[PATH_MAX];
    struct stat buffer;

    sprintf(fullDir,"%s/%s/%s", projectPath, foldername, file_hdf5);

    if ( stat( fullDir , &buffer) == 0) {
        printf("HDF5 file exists! Checking for continuation conditions.\n");

        readMyHDFInt( fullDir, "last_t_fields", &last_t_fields  );                      //Stores last_t_fields

        if( last_t_fields == T_END ){
            printf("Last saved time of fiels is equal to T_END. Simulation will be terminated.\n");
            exit(-1);

        } if ( last_t_fields < T_END ) {
            printf("Conditions met. Initializing saved fields.\n");
            save_fields_toArray( gridCfg, fullDir, EB_WAVE, J_B0 );
            last_t_fields += 1;
            printf("Simulation will start from t_int=%d.\n", last_t_fields);
        } else {
            printf("Simulation have been done. Check path: %s \n", fullDir);
        }

    } else {
        printf("HDF5 file does not exist or cannot be accessed. Simulation will start from t_int=0.\n");
        last_t_fields = 0;
    }

}

int save_fields_toArray(    gridConfiguration *gridCfg,
                            char filename[],
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ] ){
    
    double (*Array_tmp)[NY/2][NZ/2]  = calloc(NX/2, sizeof *Array_tmp);
    size_t ii, jj, kk;
    
    //E_x
    readMyHDF( NX/2, NY/2, NZ/2, filename, "E_x", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii+1][jj  ][kk  ]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //E_y
    readMyHDF( NX/2, NY/2, NZ/2, filename, "E_y", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii  ][jj+1][kk  ]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //E_z
    readMyHDF( NX/2, NY/2, NZ/2, filename, "E_z", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii  ][jj  ][kk+1]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //B_x
    readMyHDF( NX/2, NY/2, NZ/2, filename, "B_x", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii  ][jj+1][kk+1]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //B_y
    readMyHDF( NX/2, NY/2, NZ/2, filename, "B_y", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii+1][jj  ][kk+1]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //B_z
    readMyHDF( NX/2, NY/2, NZ/2, filename, "B_z", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                EB_WAVE[ii+1][jj+1][kk  ]   =   Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //J_x
    readMyHDF( NX/2, NY/2, NZ/2, filename, "J_x", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                J_B0[ii+1][jj  ][kk  ]   =      Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //J_y
    readMyHDF( NX/2, NY/2, NZ/2, filename, "J_y", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                J_B0[ii  ][jj+1][kk  ]   =      Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    //J_z
    readMyHDF( NX/2, NY/2, NZ/2, filename, "J_z", Array_tmp );
#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {
                J_B0[ii  ][jj  ][kk+1]   =      Array_tmp[ii/2][jj/2][kk/2];
                Array_tmp[ii/2][jj/2][kk/2] =   .0;
            }
        }
    }

    free( Array_tmp );
    return EXIT_SUCCESS;
}