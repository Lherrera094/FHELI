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
                            beamAntennaConfiguration *beamCfg );

void control_HelicalAntenna(    gridConfiguration *gridCfg,
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise,
                                double EB_WAVE[NX][NY][NZ] );

void control_HelicalAntenna_REF(    gridConfiguration *gridCfg, 
                                    beamAntennaConfiguration *beamCfg, 
                                    double t_rise,
                                    double EB_WAVE[NX][NY][NZ_REF] );

void read_file(char *filename, double **Section_coord);
int get_lenght(char *filename);

int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg, 
                    double t_rise, int I_dir, 
                    int lenght, double **S_coord,
                    double EB_WAVE[NX][NY][NZ] );

int circular_antenna(   gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg,
                        double t_rise, int z0, int I_dir,
                        double EB_WAVE[NX][NY][NZ] );

int helical_antenna(    gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int I_dir,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ] );

int half_circular_antenna(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg, 
                            double t_rise, int I_dir,
                            int lenght, double **S_coord,
                            double EB_WAVE[NX][NY][NZ] );

//Antenna reference functions
int linear_antenna_ref( gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int I_dir, 
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] );

int helical_antenna_ref(gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int I_dir,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] );

int half_circular_antenna_ref(  gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise, int I_dir,
                                int lenght, double **S_coord,
                                double EB_WAVE[NX][NY][NZ_REF] );

//Delete anetnna values to keep only fields
void delete_ant2save( gridConfiguration *gridCfg, double array_3D[NX/2][NY/2][NZ/2] );
void delete_field(  gridConfiguration *gridCfg, double array_3D[NX/2][NY/2][NZ/2], 
                    int lenght, double **S_coord );

#endif