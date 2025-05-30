#ifndef ENERGY_CALC_MODULE_H
#define ENERGY_CALC_MODULE_H

#include <math.h>

#include "focal-struct.h"
#include "macros-grid.h"
#include "auxiliar_module.h"
#include "grid_io.h"
#include "continuation_module.h"

/*Initialize energy and power storage arrays*/
void init_energyCalculations(   gridConfiguration *gridCfg,
                                saveData *saveDCfg );

void free_EnergyArray_memory();

void control_energy_calc(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            int t_int,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ],
                            double n_e[NX/2][NY/2][NZ/2] );

int compute_energy_values(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ],
                            double n_e[NX/2][NY/2][NZ/2] );

void save_EnergyPower( gridConfiguration *gridCfg, saveData *saveDCfg);

int stored_saved_dataset(   gridConfiguration *gridCfg,
                            saveData *saveDCfg );

#endif
