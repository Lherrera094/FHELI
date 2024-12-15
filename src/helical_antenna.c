#include "helical_antenna.h"

static int Z_0;             //initial Z coordinate for the antenna
static int Z_1;             //end Z coordinate for the antenna  
static int J_Amp;
static int u;               //Displacement for reference antenna

//Integers that save the lenght of the antenna arrays for implementation
static int lenght1;
static int lenght2;
static int lenght3;
static int lenght4;

//Each one saves the coordinates for the antenna sections
static double **S1 = NULL;
static double **S2 = NULL;
static double **S3 = NULL;
static double **S4 = NULL;

void init_helicalAntenna(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg ){

    char fullDir1[PATH_MAX],fullDir2[PATH_MAX],fullDir3[PATH_MAX],
         fullDir4[PATH_MAX], directory[1024];
    char dir[] = "tools";
    
    /*Compute antenna initial and end point*/
    //The wavelenght is defined by the antenna leght
    Z_0 = (int)( 0.5 * (NZ - ant_lenght) );
    Z_1 = (int)( 0.5 * (NZ + ant_lenght) );

    if ((Z_0 % 2) != 0)  ++Z_0;
    if ((Z_1 % 2) != 0)  ++Z_1;
    
    //save antenna curret amplitud
    J_Amp = J_amp;

    //Construct directory path to the .txt coordinate files
    if( ant_type == 1){
        sprintf(directory, "%s/%s", dir, "Nagoya_antenna");
        printf("Nagoya Antenna initialized. \n");
    } else if(ant_type == 2){
        sprintf(directory, "%s/%s", dir, "Helical_antenna");
        printf("Half helical Antenna initialized. \n");
    }
    
    sprintf(fullDir1,"%s/Section_%s.txt", directory, "1" );
    sprintf(fullDir2,"%s/Section_%s.txt", directory, "2" );
    sprintf(fullDir3,"%s/Section_%s.txt", directory, "3" );
    sprintf(fullDir4,"%s/Section_%s.txt", directory, "4" );

    //Initialize the antenna section arrays
    lenght1 = get_lenght(fullDir1);
    S1 = allocate2DArray( lenght1, 3 );

    lenght2 = get_lenght(fullDir2);
    S2 = allocate2DArray( lenght2, 3 );

    lenght3 = get_lenght(fullDir3);
    S3 = allocate2DArray( lenght3, 3 );

    lenght4 = get_lenght(fullDir4);
    S4 = allocate2DArray( lenght4, 3 );

    //Save the coordinates in the arrays
    read_file(fullDir1, S1);
    read_file(fullDir2, S2);
    read_file(fullDir3, S3);
    read_file(fullDir4, S4);

    //Compute z displacement for reference antenna
    u = Z_0 - D_ABSORB - 2;
    //u = 0;

}

void read_file(char *filename, double **Section_coord ){

    FILE *file;
    int row = 0; 
    int value;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    }

    int col=0;
    while (fscanf(file, "%d", &value) == 1) { 

        Section_coord[row][col] = value;
        // Skip over non-digit characters (e.g., commas, spaces, or newlines)
        while (fgetc(file) == ','); // Move past commas
        fseek(file, -1, SEEK_CUR); // Adjust the file pointer if necessary

        col++;

        if(col == 3){
            row++;
            col = 0;
        }
    }

    fclose(file);
}

int get_lenght(char *filename){

    FILE *file;
    int lenght = 0;
    char ch;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    }

    // Count lines by checking for newline characters
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lenght++;
        }
    }

    fclose(file);
    return lenght;
}

//Functions for implementation of helical antenna 
void control_HelicalAntenna(    gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise,
                                double EB_WAVE[NX][NY][NZ] ){

    /*Apply helical antenna to grid*/
    if(ant_type == 1){ //Nagoya typeIII helical antenna  

        half_circular_antenna( gridCfg, beamCfg, t_rise, 1, lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, -1, lenght2, S2, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, t_rise, 1, lenght3, S3, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, -1, lenght4, S4, EB_WAVE );

    }else if(ant_type == 2){ //Half helical type antenna

        half_circular_antenna( gridCfg, beamCfg, t_rise, 1, lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, 1, lenght2, S2, EB_WAVE );

        helical_antenna( gridCfg, beamCfg, t_rise, 1, lenght3, S3, EB_WAVE );
        helical_antenna( gridCfg, beamCfg, t_rise, -1, lenght4, S4, EB_WAVE );

    }

}

void control_HelicalAntenna_REF(    gridConfiguration *gridCfg, 
                                    beamAntennaConfiguration *beamCfg, 
                                    double t_rise,
                                    double EB_WAVE[NX][NY][NZ_REF] ){
    
    /*Apply helical antenna to grid*/
    if(ant_type == 1){ //Nagoya typeIII helical antenna  

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 1, lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, -1, lenght2, S2, EB_WAVE );

        linear_antenna_ref( gridCfg, beamCfg, t_rise, 1, lenght3, S3, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, -1, lenght4, S4, EB_WAVE );

    }else if(ant_type == 2){ //Half helical type antenna

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 1, lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 1, lenght2, S2, EB_WAVE );

        helical_antenna_ref( gridCfg, beamCfg, t_rise, 1, lenght3, S3, EB_WAVE );
        helical_antenna_ref( gridCfg, beamCfg, t_rise, -1, lenght4, S4, EB_WAVE );

    }

}

//Antenna field injection functions
int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg, 
                    double t_rise, int I_dir, 
                    int lenght, double **S_coord,
                    double EB_WAVE[NX][NY][NZ] ){

    size_t ii, jj ,kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){ 
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        //Current goes in the Z-direction
        EB_WAVE[ii  ][jj  ][kk+1] += - 2 * I_dir * J_amp * sin( OMEGA_T ) * t_rise * DT;
    }

    return EXIT_SUCCESS;
}

int helical_antenna(    gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int I_dir,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ] ){

    size_t ii, jj, kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){

        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        EB_WAVE[ii  ][jj  ][kk+1] += - I_dir * J_amp * sin( OMEGA_T ) * t_rise * DT;
    }

    return EXIT_SUCCESS;
}


int half_circular_antenna(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg, 
                            double t_rise, int I_dir,
                            int lenght, double **S_coord,
                            double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, kk, ll;
    double J_x, J_y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        if( ii <= (ANT_X + ant_radius) && ii > ANT_X &&
            jj <= (ANT_Y + ant_radius) && jj > ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;
        
        } else if( ii  > (ANT_X - ant_radius) && ii <= ANT_X &&
                   jj < (ANT_Y + ant_radius) && jj >=  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;

        } else if( ii > (ANT_X - ant_radius) && ii <= ANT_X &&
                   jj >= (ANT_Y - ant_radius) && jj <  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;

        } else if( ii < (ANT_X + ant_radius) && ii >= ANT_X &&
                   jj >= (ANT_Y - ant_radius) && jj <  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;

        }

        EB_WAVE[ii+1][jj  ][kk  ]  += - 0.5 * J_x * sin( OMEGA_T ) * t_rise * DT;
        EB_WAVE[ii  ][jj+1][kk  ]  += - 0.5 * J_y * sin( OMEGA_T ) * t_rise * DT;
    }

    return EXIT_SUCCESS;

}

//Reference field functions
int linear_antenna_ref( gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg,  
                        double t_rise, int I_dir, 
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] ){

    size_t ii, jj ,kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){ 
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = (2 * (int)S_coord[ll][2]) - u;
        
        //Current goes in the Z-direction
        EB_WAVE[ii  ][jj  ][kk+1] += - 2 * I_dir * J_amp * sin( OMEGA_T ) * t_rise * DT;
    }

    return EXIT_SUCCESS;
}

int helical_antenna_ref(gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int I_dir,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] ){

    size_t ii, jj, kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){

        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = (2 * (int)S_coord[ll][2]) - u;

        EB_WAVE[ii  ][jj  ][kk+1] += - I_dir * J_amp * sin( OMEGA_T ) * t_rise * DT;
    }

    return EXIT_SUCCESS;
}

int half_circular_antenna_ref(  gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise, int I_dir,
                                int lenght, double **S_coord,
                                double EB_WAVE[NX][NY][NZ_REF] ){

    int ii, jj, kk, ll;
    double J_x, J_y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = (2 * (int)S_coord[ll][2]) - u;

        if( ii <= (ANT_X + ant_radius) && ii > ANT_X &&
            jj <= (ANT_Y + ant_radius) && jj > ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;
        
        } else if( ii  > (ANT_X - ant_radius) && ii <= ANT_X &&
                   jj < (ANT_Y + ant_radius) && jj >=  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;

        } else if( ii > (ANT_X - ant_radius) && ii <= ANT_X &&
                   jj >= (ANT_Y - ant_radius) && jj <  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;

        } else if( ii < (ANT_X + ant_radius) && ii >= ANT_X &&
                   jj >= (ANT_Y - ant_radius) && jj <  ANT_Y ){

            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;

        }

        EB_WAVE[ii+1][jj  ][kk  ]  += - 0.5 * J_x * sin( OMEGA_T ) * t_rise * DT;
        EB_WAVE[ii  ][jj+1][kk  ]  += - 0.5 * J_y * sin( OMEGA_T ) * t_rise *  DT;
    }

    return EXIT_SUCCESS;

}

int circular_antenna(   gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, int z0, int I_dir,
                        double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, theta;

#pragma omp parallel for           
    for( theta = 0; theta < 360; theta++ ){

        ii = ANT_X + (int)( ant_radius * cos( theta * M_PI/180) );
        jj = ANT_Y + (int)( ant_radius * sin( theta * M_PI/180) );

        if ((ii % 2) != 0)  ++ii;
        if ((jj % 2) != 0)  ++jj;

        EB_WAVE[ii+1][jj  ][z0  ]  += - I_dir * sin( OMEGA_T ) * t_rise * DT;
        EB_WAVE[ii  ][jj+1][z0  ]  += - I_dir * sin( OMEGA_T ) * t_rise * DT;

    }

    return EXIT_SUCCESS;
}

//Delete anetnna values to keep only fields
void delete_ant2save( gridConfiguration *gridCfg, double array_3D[NX/2][NY/2][NZ/2] ){

    if( NZ_REF > ( 2*D_ABSORB + (int)PERIOD ) ){

        delete_field( gridCfg, array_3D, lenght1, S1 ); //Delete Section 1
        delete_field( gridCfg, array_3D, lenght2, S2 ); //Delete Section 2
        delete_field( gridCfg, array_3D, lenght3, S3 ); //Delete Section 3
        delete_field( gridCfg, array_3D, lenght4, S4 ); //Delete Section 4

    }
    
}

void delete_field(  gridConfiguration *gridCfg, double array_3D[NX/2][NY/2][NZ/2], 
                    int lenght, double **S_coord ){

    int ii, jj, kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){

        ii = (int)S_coord[ll][0];
        jj = (int)S_coord[ll][1];
        kk = (int)S_coord[ll][2];

        array_3D[ii  ][jj  ][kk  ] = 0.0;
        
    }

/*#pragma omp parallel for collapse(3) default(shared) private(ii,jj,kk)
    for (ii=0 ; ii<NX ; ii+=2) {
        for (jj=0 ; jj<NY ; jj+=2) {
            for (kk=0 ; kk<NZ ; kk+=2) {

                if( array_3D[(ii/2)][(jj/2)][(kk/2)] >= (J_Amp*J_Amp*DT*DT) ){
                    array_3D[(ii/2)][(jj/2)][(kk/2)] = 0.0;
                }
                    
            }
        }
    } */

}