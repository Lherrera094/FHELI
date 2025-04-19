#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

//Include C libraries
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

// check if compiler understands OMP, if not, this file does probably not exist
#ifdef _OPENMP
    #include <omp.h>  
#endif

#define HDF5
#ifdef HDF5
    #include "hdf5.h"
#endif
//PI values is defined in the math.h
#ifndef M_PI    
  #define M_PI 3.14159265358979323846   
#endif

//Include FOCAL modules headers
#include "focal-struct.h"
#include "macros-grid.h"
#include "alloc-memory.h"
#include "init_module.h"
#include "focal.h"
#include "antenna.h"
#include "grid_io.h"
#include "background_profiles.h"
#include "power_calc.h"
#include "energy_calc_module.h"
#include "save_data.h"
#include "boundary_module.h"
#include "antenna_detector.h"

#endif