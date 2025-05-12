#ifndef CONTINUATION_MODULE_H
#define CONTINUATION_MODULE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "focal-struct.h"
#include "hdf5.h"
#include "grid_io.h"

void init_continuity(   gridConfiguration *gridCfg,
                        saveData *saveDCfg,
                        double EB_WAVE[NX][NY][NZ],
                        double J_B0[NX][NY][NZ] );

void check_hdf5(    gridConfiguration *gridCfg,
                    saveData *saveDCfg,
                    double EB_WAVE[NX][NY][NZ],
                    double J_B0[NX][NY][NZ] );

int save_fields_toArray(    gridConfiguration *gridCfg,
                            char filename[],
                            double EB_WAVE[NX][NY][NZ],
                            double J_B0[NX][NY][NZ] );

#endif  // CONTINUATION_MODULE_H