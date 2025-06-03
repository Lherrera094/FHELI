#include "energy_calc_module.h"

static double   *energy_average         = NULL;
static double   *total_energy_average   = NULL;
static double   *power_absorbed         = NULL;
static double   *impedance              = NULL;

int         t_count                 = 0;       //index saves the index to store the values
int         counter                 = 0;       
int         energy_storage          = 0;
int         total_energy_storage    = 0;  
int         Power_save              = 0;
int         J_average               = 0;    

/*Initialize energy and power storage arrays*/
void init_energyCalculations(   gridConfiguration *gridCfg,
                                saveData *saveDCfg ){

    //Allocate memory for energy_wave and power_absorbed
    // Energy wave: stores time evolution the energy of the EM wave inside plasma region
    energy_average          =    allocate1DArray( (int)(T_END / PERIOD) );
    total_energy_average    =    allocate1DArray( (int)(T_END / PERIOD) );

    // Power absorbed: stores time evolution of wavepower obsorbed by the plasma
    power_absorbed  =    allocate1DArray( (int)(T_END / PERIOD) );
    impedance       =    allocate1DArray( (int)(T_END / PERIOD) );

    if ( last_t_fields != 0 ){
        /*Stored saved values for continuation*/
        stored_saved_dataset( gridCfg, saveDCfg );
        t_count = last_t_fields + 1;
    }

    printf("Energy and Power arrays initialized. \n");
}

void free_EnergyArray_memory(){

    free1DArray(energy_average);
    free1DArray(total_energy_average);
    free1DArray(power_absorbed);
    free1DArray(impedance);

}

void control_energy_calc(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            int t_int,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ],
                            double n_e[NX/2][NY/2][NZ/2] ){
    
    compute_energy_values( gridCfg, beamCfg, EB_WAVE, J_B0, n_e );

}

int compute_energy_values(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ],
                            double n_e[NX/2][NY/2][NZ/2] ){

    /*Functions computes power deposition and wave energy in the plasma region
      This values are saved in the power_absorbed and energy_wave arrays.*/
    double E2 = 0, B2 = 0, J2=0, P2=0;
    double radius_check;
    size_t
        ii, jj, kk;

#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk) reduction(+:E2,B2,J2,P2)
    for (ii=D_ABSORB ; ii<=NX-2-D_ABSORB ; ii+=2) {
        for (jj=D_ABSORB ; jj<=NY-2-D_ABSORB ; jj+=2) {
            for (kk=D_ABSORB ; kk<=NZ-2-D_ABSORB ; kk+=2) {

                radius_check = sqrt( pow((float)(ii/2) - (float)(ANT_X/2), 2) + pow((float)(jj/2) - (float)(ANT_Y/2), 2) );
                if( pow(radius_check,2) < pow(ant_radius/2,2) ) {
                    //E2 = Ex^2 + Ey^2 + Ez^2
                    E2 += pow(EB_WAVE[ii+1][jj  ][kk  ],2) + pow(EB_WAVE[ii  ][jj+1][kk  ],2) + pow(EB_WAVE[ii  ][jj  ][kk+1],2);
                    //B2 = Bx^2 + By^2 + Bz^2
                    B2 += pow(EB_WAVE[ii  ][jj+1][kk+1],2) + pow(EB_WAVE[ii+1][jj   ][kk+1],2) + pow(EB_WAVE[ii+1][jj+1][kk  ],2); 
                    //Power = Ex*Jx + Ey*Jy + Ez*Jz
                    J2 += ( pow(J_B0[ii+1][jj  ][kk  ],2) + pow(J_B0[ii  ][jj+1][kk  ],2) + pow(J_B0[ii  ][jj  ][kk+1],2) ) ;

                    P2 += EB_WAVE[ii+1][jj  ][kk  ]*J_B0[ii+1][jj  ][kk  ] 
                        + EB_WAVE[ii  ][jj+1][kk  ]*J_B0[ii  ][jj+1][kk  ]
                        + EB_WAVE[ii  ][jj  ][kk+1]*J_B0[ii  ][jj  ][kk+1] ;

                }

            }
        }
    }

    energy_storage          +=  (E2 + B2) * pow(DX,2) / 2 ;
    total_energy_storage    +=  (E2 + B2 + J2) * pow(DX,2) / 2 ;
    Power_save              +=  pow(P2,2) * pow(DX,2) ;
    t_count += 1;

    if( t_count == PERIOD ){

        energy_average[counter]         = energy_storage / PERIOD;
        total_energy_average[counter]   = total_energy_storage / PERIOD;
        power_absorbed[counter]         = Power_save / PERIOD;
        impedance[counter]              = 2 * Power_save / pow(I0,2);

        counter                += 1;
        t_count                 = 0;
        energy_storage          = 0;
        total_energy_storage    = 0;
        Power_save              = 0;

    }

    return EXIT_SUCCESS;
}

void save_EnergyPower( gridConfiguration *gridCfg, saveData *saveDCfg){

    /*Char values as directions to the correct folder*/
    char filename_hdf5[PATH_MAX];

    sprintf(filename_hdf5,"%s/%s/%s", projectPath, foldername, file_hdf5);

    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( (int)(T_END / PERIOD), filename_hdf5, "Energy_wave(T)", energy_average ) ) ;
    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( (int)(T_END / PERIOD), filename_hdf5, "Total_Energy(T)", total_energy_average ) ) ;
    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( (int)(T_END / PERIOD), filename_hdf5, "Absorbed_power(T)", power_absorbed ) ) ;
    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( (int)(T_END / PERIOD), filename_hdf5, "Impedance(T)", impedance ) ) ;


}

int stored_saved_dataset(   gridConfiguration *gridCfg,
                            saveData *saveDCfg ){
    
    double (*Array_tmp)  = calloc( last_t_fields, sizeof *Array_tmp );
    char fullDir[PATH_MAX];

    sprintf(fullDir,"%s/%s/%s", projectPath, foldername, file_hdf5);

    readMyHDF_1D( last_t_fields, fullDir, "Energy_wave(t)", Array_tmp );
    for( int i = 0 ; i == last_t_fields ; i+=1 ){
        energy_average[i] = Array_tmp[i];
        Array_tmp[i] = .0;
    }

    readMyHDF_1D( last_t_fields, fullDir, "Absorbed_power(t)", Array_tmp );
    for( int i = 0 ; i == last_t_fields ; i+=1 ){
        power_absorbed[i] = Array_tmp[i];
        Array_tmp[i] = .0;
    }

    free( Array_tmp );
    return EXIT_SUCCESS;
}


