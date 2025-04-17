#include "energy_calc_module.h"

//static double   *energy         = NULL;
static double   *energy_average = NULL;
//static double   *power          = NULL;
static double   *power_absorbed = NULL;
int              t_count        = 0;            //index saves the index to store the values until tuples of PERIOD/4
//int              EP_indx        = 0;                

/*Initialize energy and power storage arrays*/
void init_energyCalculations(   gridConfiguration *gridCfg ){

    //Allocate memory for energy_wave and power_absorbed
    // Energy wave: stores time evolution the energy of the EM wave inside plasma region
    energy_average =    allocate1DArray( T_END );
    //energy         =    allocate1DArray( PERIOD/4 );
    // Power absorbed: stores time evolution of wavepower obsorbed by the plasma
    power_absorbed =    allocate1DArray( T_END );
    //power          =    allocate1DArray( PERIOD/4 );

}

void free_EnergyArray_memory(){

    free1DArray(energy_average);
    //free1DArray(energy);
    free1DArray(power_absorbed);

}

void control_energy_calc(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            int t_int,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ] ){
    
    compute_energy_values( gridCfg, beamCfg, EB_WAVE, J_B0 );

}

int compute_energy_values(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ] ){

    /*Functions computes power deposition and wave energy in the plasma region
      This values are saved in the power_absorbed and energy_wave arrays.*/
    double E2 = 0, B2 = 0, P = 0;
    double radius_check;
    size_t
        ii, jj, kk;

#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk) 
    for (ii=D_ABSORB ; ii<=NX-2-D_ABSORB ; ii+=2) {
        for (jj=D_ABSORB ; jj<=NY-2-D_ABSORB ; jj+=2) {
            for (kk=D_ABSORB ; kk<=NZ-2-D_ABSORB ; kk+=2) {

                radius_check = sqrt( pow((float)(ii/2) - (float)(ANT_X/2), 2) + pow((float)(jj/2) - (float)(ANT_Y/2), 2) );
                if( pow(radius_check,2) < pow(ant_radius/2,2) ) {
                    
                    //E2 = Ex^2 + Ey^2 + Ez^2
                    E2 += pow(EB_WAVE[ii+1][jj  ][kk  ],2) + pow(EB_WAVE[ii  ][jj+1][kk  ],2) + pow(EB_WAVE[ii  ][jj  ][kk+1],2);
                    //B2 = Bx^2 + By^2 + Bz^2
                    B2 += pow(EB_WAVE[ii  ][jj+1][kk+1],2) + pow(EB_WAVE[ii  ][jj+1][kk  ],2) + pow(EB_WAVE[ii  ][jj  ][kk+1],2); 
                    // P = Jx*Ex + Jy*Ey + Jz*Ez
                    P +=    J_B0[ii+1][jj  ][kk  ]*EB_WAVE[ii+1][jj  ][kk  ] + 
                            J_B0[ii  ][jj+1][kk  ]*EB_WAVE[ii  ][jj+1][kk  ] + 
                            J_B0[ii  ][jj  ][kk+1]*EB_WAVE[ii  ][jj  ][kk+1];

                }

            }
        }
    }

    energy_average[t_count]     =   (E2 + B2) * pow(DX,3) / 2;
    power_absorbed[t_count]      =   P * pow(DX,3); 
    t_count += 1;

    //Computes averaged energy and power to save into hdf5 file
    /*if( t_count == PERIOD/4 ){

        for (int i=0 ; i<PERIOD/4 ; i+=1){
            energy_average[EP_indx] += energy[i] / (PERIOD/4);
            power_absorbed[EP_indx] += power[i] / (PERIOD/4);
        }
        
        EP_indx += 1;
        t_count = 0;
    }*/

    return EXIT_SUCCESS;
}

void save_EnergyPower( gridConfiguration *gridCfg, saveData *saveDCfg){

    /*Char values as directions to the correct folder*/
    char filename_hdf5[PATH_MAX];

    sprintf(filename_hdf5,"%s/%s/%s", projectPath, foldername, file_hdf5);

    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( T_END, filename_hdf5, "Energy_wave(t)", energy_average ) ) ;
    printf( "status of writeMyHDF_v4: %d\n", writeMyHDF_v4_1D( T_END, filename_hdf5, "Absorbed_power(t)", power_absorbed ) ) ;

}



