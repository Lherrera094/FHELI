/**
 * Author:      Luis Herrera Quesada/Alf Köhn-Seemann
 * Email:       luis.herreraquesada@/koehn@igvp.uni-stuttgart.de
 * Copyright:   University of Stuttgart
 * 
 * This is a 3D FDTD code for simulating electromagnetic waves in cold 
 * magnetized plasmas. The code is called FOCAL.
 *
 * NOTE: FOCAL is still in development (but physicswise delivering 
 *       correct, i.e. benchmarked in some cases, results).
 *
 * Initial release on github: 2022-03-31
 *
 **/

#include "main_header.h"

int main( int argc, char *argv[] ) {
//{{{
    //Start clock for CPU running time measuring
    clock_t start_CPU = clock();
    
    struct timespec start_Ftime, end_Ftime;
    double time_elapsed;                                        //Computes running time for functions/CPU run time

    //Call structures
    gridConfiguration            *gridCfg;
    beamAntennaConfiguration     *beamCfg;
    saveData                     *saveDCfg;  
    antennaDetector              *antDetect; 
    boundaryVariables            *boundaryV;
    powerValues                  *powerVal;

    /*Alloc structs in memory*/
    ALLOC_1D( gridCfg, 1, gridConfiguration);
    ALLOC_1D( beamCfg, 1, beamAntennaConfiguration);
    ALLOC_1D( saveDCfg, 1, saveData);
    ALLOC_1D( antDetect, 1, antennaDetector );
    ALLOC_1D( boundaryV, 1, boundaryVariables);
    ALLOC_1D( powerVal, 1, powerValues);

    int  
#ifdef _OPENMP
        n_threads,                          // number of threads that will be used (OpenMP)
#endif
        t_int;

    // set-up grid (read values from JSON)
    control_init(  gridCfg, beamCfg, saveDCfg, antDetect );                 //function in INIT_MODULE.C
    create_folder( gridCfg, saveDCfg );                                     //function in SAVE_DATA.C

    printf("----------------Initializing Profiles---------------\n");
    init_boundary( gridCfg, boundaryV);                                     //function in BOUNDARY_MODULE.C
    init_antennaInjection( gridCfg, beamCfg );                              //function in ANTENNA.C
    init_antennaDetect( gridCfg, beamCfg, antDetect );                      //function in ANTENNA_DETECTOR.C
    init_powerValues( gridCfg, powerVal );                                  //function in POWER_CALC.C  

    // arrays realized as variable-length array (VLA)
    double (*EB_WAVE)[NY][NZ]           = calloc(NX, sizeof *EB_WAVE);      // E- and B-wavefield
    double (*EB_WAVE_ref)[NY][NZ_REF]   = calloc(NX, sizeof *EB_WAVE_ref);
    double (*J_B0)[NY][NZ]              = calloc(NX, sizeof *J_B0);         // J-(in plasma) and background magnetic field
    double (*n_e)[NY/2][NZ/2]           = calloc(NX/2, sizeof *n_e);        // background electron plasma density

    clock_gettime(CLOCK_MONOTONIC, &start_Ftime);
    init_background_profiles( gridCfg, beamCfg, n_e, J_B0 );                //function in BACKGROUND_PROFILES.C
    clock_gettime(CLOCK_MONOTONIC, &end_Ftime);
    time_elapsed =  (end_Ftime.tv_sec - start_Ftime.tv_sec) + 
                    (end_Ftime.tv_nsec - start_Ftime.tv_nsec) / 1e9;
    printf("Background profiles RUN_TIME: %f seconds\n", time_elapsed);

    init_continuity( gridCfg, saveDCfg, EB_WAVE, J_B0 );                    //function in CONTINUITY_MODULE.C
    init_energyCalculations( gridCfg, saveDCfg );                           //function in ENERGY_CALC_MODULE.C

    //Simulation values print to terminal
    print_systemConfiguration( gridCfg, beamCfg );                          //function in INIT_MODULE.C
    print_antennaDetec( gridCfg, antDetect );                               //function in ANTENNA_DETECTOR.C
    print_helical( beamCfg );                                               //function in HELICAL_ANTENNA.C

    printf("--------------------Simulation--------------------\n");

#ifdef _OPENMP
#pragma omp parallel private(n_threads)
    {
    n_threads = omp_get_num_threads();
    #pragma omp single
    printf( "number of threads that will be used (OpenMP) = %d\n", n_threads );
    }
#endif

    //System's time evolution
    clock_gettime(CLOCK_MONOTONIC, &start_Ftime);                                           //Counts time for 1 iteration
    for ( t_int = last_t_fields ; t_int <= T_END ; ++t_int ) {

        //Beam injection to grid
        control_antennaInjection(  gridCfg, beamCfg, t_int, EB_WAVE, EB_WAVE_ref );         //function in ANTENNA.C
        advance_fields( gridCfg, EB_WAVE, EB_WAVE_ref, J_B0, n_e );                         //function in FOCAL.C

        //optionally, apply numerical viscosity
        //apply_numerical_viscosity( &gridCfg, EB_WAVE );
        control_bc_helicon(  gridCfg, beamCfg, EB_WAVE, J_B0 );                             //function in BOUNDARY_MODULE.C
        advance_boundary(  gridCfg, boundaryV, EB_WAVE, EB_WAVE_ref );                      //function in BOUNDARY_MODULE.C
        control_antennaDetect(  gridCfg, antDetect, t_int, EB_WAVE, J_B0 );                 //function in ANTENNA_DETECTOR.C
        compute_power( gridCfg, beamCfg, powerVal, t_int, EB_WAVE, EB_WAVE_ref );           //function in POWER_CALC.C
        control_energy_calc( gridCfg, beamCfg, t_int, EB_WAVE, J_B0 );                      //function in ENERGY_CALC_MODULE.C
        //stores abs(E) into HDF5 file
        save_field_toHDF5( gridCfg, saveDCfg, t_int, EB_WAVE, J_B0 );                       //function in SAVE_DATA.C

        if(t_int == 0){
            clock_gettime(CLOCK_MONOTONIC, &end_Ftime);
            time_elapsed =  (end_Ftime.tv_sec - start_Ftime.tv_sec) + 
                            (end_Ftime.tv_nsec - start_Ftime.tv_nsec) / 1e9;
            printf("ITERATION RUN_TIME: %f seconds\n", time_elapsed);
        } //end iteration running time calculation

        if( t_int%5000 == 0 ){
            printf("Status update: On interation T_INT = %d \n", t_int );
        } //Print status when t_int is large.

    } // end of time loop
    
    printf("--------------------Finished!-------------------\n");
    clock_gettime(CLOCK_MONOTONIC, &end_Ftime);
    time_elapsed =  (end_Ftime.tv_sec - start_Ftime.tv_sec) + 
                    (end_Ftime.tv_nsec - start_Ftime.tv_nsec) / 1e9;
    printf("%d ITERATIONS RUN_TIME: %f seconds\n", T_END, time_elapsed);
    
    // write timetrace data into file
    save_SimData( gridCfg, beamCfg ,saveDCfg, n_e, J_B0 );                                  //function in SAVE_DATA.C
    write_timetraces( gridCfg, saveDCfg );                                                  //function in POWER_CALC.C
    save_EnergyPower( gridCfg, saveDCfg);                                                   //function in ENERGY_CALC_MODULE.C
    save_AntDetect( gridCfg, saveDCfg, antDetect );                                         //function in ANTENNA_DETECTOR.C
    clock_t end_CPU = clock();
    run_time = ((double) (end_CPU - start_CPU)) / CLOCKS_PER_SEC;      

    //free allocated arrays
    free_boundary( gridCfg );
    free_EnergyArray_memory();
    free_antDetect( gridCfg, antDetect );                                                   //function in
    free( EB_WAVE );
    printf( "freed EB_WAVE\n" );
    free( J_B0 );
    printf( "freed J_B0\n" );
    free( n_e );
    printf( "freed n_e\n" );

    end_CPU = clock();
    printf("CPU running time: %f seconds. \n", (((double) (end_CPU - start_CPU)) / CLOCKS_PER_SEC) );
    
    return EXIT_SUCCESS;
}//}}}
