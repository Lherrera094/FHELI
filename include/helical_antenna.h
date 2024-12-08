#ifndef HELICAL_ANTENNA_H
#define HELICAL_ANTENNA_H

#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "focal-struct.h"
#include "macros-grid.h"
#include "auxiliar_module.h"

void init_helicalAntenna(   gridConfiguration *gridCfg, 
                            helicalAntenna *helicAnt );

void control_HelicalAntenna(    gridConfiguration *gridCfg,
                                beamAntennaConfiguration *beamCfg,
                                helicalAntenna *helicAnt, 
                                int t_int,
                                double EB_WAVE[NX][NY][NZ] );

void read_file(char *filename, double **S_coord);
int get_lenght(char *filename); 

double sinusoidal_current(  beamAntennaConfiguration *beamCfg,
                            int t_int );

int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamAnt,  
                    helicalAntenna *helicAnt,
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
                            int t_int, int z0, int I_dir,
                            double EB_WAVE[NX][NY][NZ] );

int to_Int(double u);

#endif