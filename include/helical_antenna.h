#ifndef HELICAL_ANTENNA_H
#define HELICAL_ANTENNA_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "focal-struct.h"
#include "macros-grid.h"

void init_helicalAntenna(   gridConfiguration *gridCfg, 
                            helicalAntenna *helicAnt );

void control_HelicalAntenna(    gridConfiguration *gridCfg,
                                beamAntennaConfiguration *beamCfg,
                                helicalAntenna *helicAnt, 
                                int t_int,
                                double EB_WAVE[NX][NY][NZ] );

double sinusoidal_current(  beamAntennaConfiguration *beamCfg,
                            int t_int );

int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamAnt,  
                    int t_int, int J_dir,
                    int x_pos, int y_pos,
                    double EB_WAVE[NX][NY][NZ] );

int circular_antenna(   gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg,  
                        helicalAntenna *helicAnt,
                        int t_int, int z0, int dir,
                        double EB_WAVE[NX][NY][NZ] );

int helic_antenna(  gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg,
                    helicalAntenna *helicAnt, 
                    int chirality, int t_int, int J_dir,
                    double EB_WAVE[NX][NY][NZ] );

int half_circular_antenna(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            helicalAntenna *helicAnt, 
                            int t_int, int z0,
                            double EB_WAVE[NX][NY][NZ] );

int to_Int(double u);

#endif