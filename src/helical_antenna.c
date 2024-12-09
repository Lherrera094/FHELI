#include "helical_antenna.h"

static int Z_0;             //initial Z coordinate for the antenna
static int Z_1;             //end Z coordinate for the antenna  

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
                            helicalAntenna *helicAnt ){

    char fullDir1[PATH_MAX],fullDir2[PATH_MAX],fullDir3[PATH_MAX],
         fullDir4[PATH_MAX], directory[1024];
    char dir[] = "tools";
    
    /*Compute antenna initial and end point*/
    //The wavelenght is defined by the antenna leght
    Z_0 = (int)( 0.5 * (NZ - ant_lenght) );
    Z_1 = (int)( 0.5 * (NZ + ant_lenght) );

    if ((Z_0 % 2) != 0)  ++Z_0;
    if ((Z_1 % 2) != 0)  ++Z_1;

    //Construct directory path to the .txt coordinate files
    if( ant_type == 1){
        sprintf(directory, "%s/%s", dir, "Nagoya_antenna");
    } else if(ant_type == 2){
        sprintf(directory, "%s/%s", dir, "Helical_antenna");
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

void control_HelicalAntenna(    gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg,
                                helicalAntenna *helicAnt, 
                                int t_int,
                                double EB_WAVE[NX][NY][NZ] ){
    
    /*Apply helical antenna to grid*/
    if(ant_type == 1){ //Nagoya typeIII helical antenna  

        //half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_0, 1, EB_WAVE );
        //half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_1,-1, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, helicAnt, t_int, 1, lenght3, S3, EB_WAVE );
        //linear_antenna( gridCfg, beamCfg, helicAnt, t_int, -1, x_pos, y_pos, EB_WAVE );

    }else if(ant_type == 2){ //Half helical type antenna

        //Helic antenna( gridCfg, beamCfg, helicalAnt, chirality, t_int, J_dir, EB)
        //helic_antenna(  gridCfg, beamCfg, helicAnt, 1, t_int, 1, EB_WAVE );
        //helic_antenna(  gridCfg, beamCfg, helicAnt, -1, t_int, -1, EB_WAVE );
        //half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_0, 1, EB_WAVE );
        //half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_1, 1, EB_WAVE );

    }

}

/*void control_HelicalAntenna(    gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg,
                                helicalAntenna *helicAnt, 
                                int t_int,
                                double EB_WAVE[NX][NY][NZ] ){
    
    int x_pos, y_pos;
    //Apply helical antenna to grid
    if(ant_type == 1){ //Nagoya typeIII helical antenna

        x_pos = to_Int( ant_x + ant_radius * cos(0) );
        if ((x_pos % 2) != 0)  ++x_pos;
        y_pos = to_Int( ant_y + ant_radius * sin(0) );
        if ((y_pos % 2) != 0)  ++y_pos;
        linear_antenna( gridCfg, beamCfg, helicAnt, t_int, 1, x_pos, y_pos, EB_WAVE );

        x_pos = to_Int( ant_x + ant_radius * cos(M_PI) );
        if ((x_pos % 2) != 0)  ++x_pos;
        y_pos = to_Int( ant_y + ant_radius * sin(M_PI) );
        if ((y_pos % 2) != 0)  ++y_pos;
        linear_antenna( gridCfg, beamCfg, helicAnt, t_int, -1, x_pos, y_pos, EB_WAVE );   

        half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_0, 1, EB_WAVE );
        half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_1,-1, EB_WAVE );

    }else if(ant_type == 2){ //Half helical type antenna

        //Helic antenna( gridCfg, beamCfg, helicalAnt, chirality, t_int, J_dir, EB)
        helic_antenna(  gridCfg, beamCfg, helicAnt, 1, t_int, 1, EB_WAVE );
        helic_antenna(  gridCfg, beamCfg, helicAnt, -1, t_int, -1, EB_WAVE );
        half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_0, 1, EB_WAVE );
        half_circular_antenna(   gridCfg, beamCfg, helicAnt, t_int, Z_1, 1, EB_WAVE );

    }

}*/

double sinusoidal_current(  beamAntennaConfiguration *beamCfg,
                            int t_int ){

    return sin( omega_t ); //argument * dt
}

int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg, 
                    helicalAntenna *helicAnt, 
                    int t_int, int J_dir, 
                    int lenght, double **S_coord,
                    double EB_WAVE[NX][NY][NZ] ){

    size_t ii, jj ,kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){ 
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        //Current goes in the Z-direction
        EB_WAVE[ii  ][jj  ][kk+1] += - 2 * J_dir * J_amp * sinusoidal_current( beamCfg, t_int ) * DT;
    }

    return EXIT_SUCCESS;
}

/*int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg, 
                    helicalAntenna *helicAnt, 
                    int t_int, int J_dir,
                    int x_pos, int y_pos,
                    double EB_WAVE[NX][NY][NZ] ){

    size_t kk;

#pragma omp parallel for
    for( kk = Z_0 ; kk <= Z_1; kk+=2 ){ 
        //Z position
        EB_WAVE[x_pos  ][y_pos  ][kk+1] += - 2 * J_dir * J_amp * sinusoidal_current( beamCfg, t_int ) * DT;
    }

    return EXIT_SUCCESS;
}*/

int helic_antenna(  gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg,
                    helicalAntenna *helicAnt, 
                    int chirality, int t_int, int J_dir,
                    double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, kk, theta;
    double pitch;

    pitch = ant_lenght/num_turns;

//#pragma omp parallel for
    for( theta = 0 ; theta < num_turns*360 ; theta++){

        if( chirality == 1 ){
            ii = to_Int( ant_x + ( ant_radius*cos( theta * M_PI/180 ) ) );
            jj = to_Int( ant_y + ( ant_radius*sin( theta * M_PI/180 ) ) );

            if ((ii % 2) != 0)  ++ii;
            if ((jj % 2) != 0)  ++jj;
        }else if( chirality == -1 ){
            ii = to_Int( ant_x + ( ant_radius*cos( theta * M_PI/180 + M_PI ) ) );
            jj = to_Int( ant_y + ( ant_radius*sin( theta * M_PI/180 + M_PI ) ) );

            if ((ii % 2) != 0)  ++ii;
            if ((jj % 2) != 0)  ++jj;
        }
        
        kk = to_Int( Z_0 + ( pitch * ( num_turns * theta/( num_turns * 360 ) ) ) );
        if ((kk % 2) != 0)  ++kk;

        EB_WAVE[ii  ][jj  ][kk+1] += - J_dir * J_amp * sinusoidal_current( beamCfg, t_int )*DT;
    }

    return EXIT_SUCCESS;
}

int half_circular_antenna(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg,
                            helicalAntenna *helicAnt, 
                            int t_int, int z0, int I_dir,
                            double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, theta;
    double J_x, J_y;

#pragma omp parallel for
    for( theta = 0 ; theta < 360 ; theta++){

        ii = to_Int( ant_x + ( ant_radius * cos( theta * M_PI/180 ) ) );
        jj = to_Int( ant_y + ( ant_radius * sin( theta * M_PI/180 ) ) );

        if ((ii % 2) != 0)  ++ii;
        if ((jj % 2) != 0)  ++jj;

        if(theta < 90){
            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;
        } else if( theta >= 90 && theta < 180){
            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;
        } else if( theta >= 180 && theta < 270){
            J_x = I_dir*J_amp;
            J_y = -I_dir*J_amp;
        } else if( theta >= 270 && theta < 360){
            J_x = I_dir*J_amp;
            J_y = I_dir*J_amp;
        }

        EB_WAVE[ii+1][jj  ][z0  ]  += - 0.5 * J_x * sinusoidal_current( beamCfg, t_int )*DT;
        EB_WAVE[ii  ][jj+1][z0  ]  += - 0.5 * J_y * sinusoidal_current( beamCfg, t_int )*DT;
    }

    return EXIT_SUCCESS;

}

int to_Int(double u){

    int new_u;
    new_u = (int)u;

    if( u - (int)u <= 0.5 ){
        new_u = (int)u;
    }else if( u - (int)u > 0.5 ){
        new_u = (int)u + 1;
    }

    return new_u;
}

int circular_antenna(   gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        helicalAntenna *helicAnt, 
                        int t_int, int z0, int J_dir,
                        double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, theta;

#pragma omp parallel for           
    for( theta = 0; theta < 360; theta++ ){

        ii = ant_x + (int)( ant_radius * cos( theta * M_PI/180) );
        jj = ant_y + (int)( ant_radius * sin( theta * M_PI/180) );

        if ((ii % 2) != 0)  ++ii;
        if ((jj % 2) != 0)  ++jj;

        EB_WAVE[ii+1][jj  ][z0  ]  += - J_dir * sinusoidal_current( beamCfg, t_int )*DT;
        EB_WAVE[ii  ][jj+1][z0  ]  += - J_dir * sinusoidal_current( beamCfg, t_int )*DT;

    }

    return EXIT_SUCCESS;
}

