#include "helical_antenna.h"

static int Z_0;             //initial Z coordinate for the antenna
static int Z_1;             //end Z coordinate for the antenna  
static int I_0;             //Antenna current value
static int u;               //Displacement for reference antenna

//Integers that save the lenght of the antenna arrays for implementation
static int lenght1;
static int lenght2;
static int lenght3;
static int lenght4;
static int lenght5;
static int lenght6;
static int lenght7;
static int lenght8;
static int lenght9;
static int lenght10;
static int lenght11;
static int lenght12;

//Each one saves the coordinates for the antenna sections
static double **S1 = NULL;
static double **S2 = NULL;
static double **S3 = NULL;
static double **S4 = NULL;
static double **S5 = NULL;
static double **S6 = NULL;
static double **S7 = NULL;
static double **S8 = NULL;
static double **S9 = NULL;
static double **S10 = NULL;
static double **S11 = NULL;
static double **S12 = NULL;

//Function in charge of controlling the initialization of the helical antenna 
void init_helicalAntenna(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg ){

    char fullDir1[PATH_MAX],fullDir2[PATH_MAX],fullDir3[PATH_MAX],
         fullDir4[PATH_MAX],fullDir5[PATH_MAX],fullDir6[PATH_MAX],
         fullDir7[PATH_MAX],fullDir8[PATH_MAX],fullDir9[PATH_MAX],
         fullDir10[PATH_MAX],fullDir11[PATH_MAX],fullDir12[PATH_MAX],
         directory[1024], input_par[PATH_MAX];
    char dir[] = "Antenna_Models";
    
    //Construct directory path to the .txt coordinate files
    sprintf(directory, "%s/%s", dir, ant_file);

    if ( access(directory, F_OK) == 0) {
        
        sprintf(input_par, "%s/%s", directory, "Input_Parameters.txt");
        if ( access(input_par, F_OK) == 0){

            FILE *file = fopen(input_par, "r");
            if (file == NULL) {
                perror("Error opening file");
                exit(-1);
            }

            char line[100]; // Buffer to store each line
            int new_radius = -1, new_length = -1, 
                x_center = -1,  y_center = -1,  z_center = -1, 
                turns=-1, arms=-1;

            // Read the file line by line
            while (fgets(line, sizeof(line), file)) {
                // Check if the line contains "Radius"
                if (strstr(line, "Radius") != NULL) {
                    sscanf(line, "Radius: %d",  &new_radius );
                }
                // Check if the line contains "Length"
                else if (strstr(line, "Lenght") != NULL) {
                    sscanf(line, "Lenght: %d",  &new_length );
                }
                // Check if the line contains "X_position"
                else if (strstr(line, "X_position") != NULL) {
                    sscanf(line, "X_position (center): %d",  &x_center );
                }
                // Check if the line contains "Y_position"
                else if (strstr(line, "Y_position") != NULL) {
                    sscanf(line, "Y_position (center): %d",  &y_center );
                }
                // Check if the line contains "Z_position"
                else if (strstr(line, "Z_position") != NULL) {
                    sscanf(line, "Z_position (center): %d",  &z_center );
                }
                // Check if the line contains "num_turns"
                else if (strstr(line, "Number of Turns") != NULL) {
                    sscanf(line, "Num_turns: %d",  &turns );
                    num_turns = (int) turns;
                }
                // Check if the line contains "num_turns"
                else if (strstr(line, "Number of Arms") != NULL) {
                    sscanf(line, "Number of Arms: %d",  &arms );
                    num_arms = (int) arms;
                }
                
            }

            //Save the inpt parameters into the beamConfiguration struct
            ant_radius = (int) new_radius*2;
            ant_lenght = (int) new_length*2;
            ANT_X = (int) x_center*2;
            ANT_Y = (int) y_center*2;
            ANT_Z = (int) z_center*2;

        } else {
            printf("Input file does not exist.\n");
            exit(-1);
        }

    } else{

        printf("Antenna folder does not exist.\n");
        exit(-1);

    }
    
    /*Compute antenna initial and end point*/
    //The wavelenght is defined by the antenna leght
    Z_0 = (int)( ANT_Z - 0.5*ant_lenght); 
    Z_1 = (int)( ANT_Z + 0.5*ant_lenght);

    if ((Z_0 % 2) != 0)  ++Z_0;
    if ((Z_1 % 2) != 0)  ++Z_1;
    
    //save antenna curret amplitud
    I_0 = I0;

    sprintf(fullDir1,"%s/Section_%s.txt", directory, "1" );
    sprintf(fullDir2,"%s/Section_%s.txt", directory, "2" );
    sprintf(fullDir3,"%s/Section_%s.txt", directory, "3" );
    sprintf(fullDir4,"%s/Section_%s.txt", directory, "4" );
    sprintf(fullDir5,"%s/Section_%s.txt", directory, "5" );
    sprintf(fullDir6,"%s/Section_%s.txt", directory, "6" );
    sprintf(fullDir7,"%s/Section_%s.txt", directory, "7" );
    sprintf(fullDir8,"%s/Section_%s.txt", directory, "8" );
    sprintf(fullDir9,"%s/Section_%s.txt", directory, "9" );
    sprintf(fullDir10,"%s/Section_%s.txt", directory, "10" );
    sprintf(fullDir11,"%s/Section_%s.txt", directory, "11" );
    sprintf(fullDir12,"%s/Section_%s.txt", directory, "12" );
    
    // Check if the file exists and initialize the antenna section arrays
    //Saves the coordinates of in the S arrays
    if ( access(fullDir1, F_OK) == 0) {
        lenght1 = get_lenght(fullDir1);
        S1 = allocate2DArray( lenght1, 3 );
        read_file(fullDir1, S1);
        printf("Antenna Section 1 saved.\n");

    } if ( access(fullDir2, F_OK) == 0) {
        lenght2 = get_lenght(fullDir2);
        S2 = allocate2DArray( lenght2, 3 );
        read_file(fullDir2, S2);
        printf("Antenna Section 2 saved.\n");
    
    } if ( access(fullDir3, F_OK) == 0) {
        lenght3 = get_lenght(fullDir3);
        S3 = allocate2DArray( lenght3, 3 );
        read_file(fullDir3, S3);
        printf("Antenna Section 3 saved.\n");

    } if ( access(fullDir4, F_OK) == 0) {
        lenght4 = get_lenght(fullDir4);
        S4 = allocate2DArray( lenght4, 3 );
        read_file(fullDir4, S4);
        printf("Antenna Section 4 saved.\n");

    } if ( access(fullDir5, F_OK) == 0) {
        lenght5 = get_lenght(fullDir5);
        S5 = allocate2DArray( lenght5, 3 );
        read_file(fullDir5, S5);
        printf("Antenna Section 5 saved.\n");

    } if ( access(fullDir6, F_OK) == 0) {
        lenght6 = get_lenght(fullDir6);
        S6 = allocate2DArray( lenght6, 3 );
        read_file(fullDir6, S6);
        printf("Antenna Section 6 saved.\n");

    } if ( access(fullDir7, F_OK) == 0) {
        lenght7 = get_lenght(fullDir7);
        S7 = allocate2DArray( lenght7, 3 );
        read_file(fullDir7, S7);
        printf("Antenna Section 7 saved.\n");

    } if ( access(fullDir8, F_OK) == 0) {
        lenght8 = get_lenght(fullDir8);
        S8 = allocate2DArray( lenght8, 3 );
        read_file(fullDir8, S8);
        printf("Antenna Section 8 saved.\n");

    } if ( access(fullDir9, F_OK) == 0) {
        lenght9 = get_lenght(fullDir9);
        S9 = allocate2DArray( lenght9, 3 );
        read_file(fullDir9, S9);
        printf("Antenna Section 9 saved.\n");

    } if ( access(fullDir10, F_OK) == 0) {
        lenght10 = get_lenght(fullDir10);
        S10 = allocate2DArray( lenght10, 3 );
        read_file(fullDir10, S10);
        printf("Antenna Section 10 saved.\n");

    } if ( access(fullDir11, F_OK) == 0) {
        lenght11 = get_lenght(fullDir11);
        S11 = allocate2DArray( lenght11, 3 );
        read_file(fullDir11, S11);
        printf("Antenna Section 11 saved.\n");
    
    } if ( access(fullDir12, F_OK) == 0) {
        lenght12 = get_lenght(fullDir12);
        S12 = allocate2DArray( lenght12, 3 );
        read_file(fullDir12, S12);
        printf("Antenna Section 12 saved.\n");

    }

    //Compute z displacement for reference antenna
    u = 0;

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

//Gets the lenght of the file counting the number of rows.
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

//Function to print in terminal parameters of the helical antenna
void print_helical( beamAntennaConfiguration *beamCfg ){

    char *antenna_type;
    if(ant_type == 1){
        antenna_type = "Single Loop";
    } else if(ant_type == 2){
        antenna_type = "Spiral";
    } else if(ant_type == 3){
        antenna_type = "Double Loop";
    } else if(ant_type == 4){
        antenna_type = "Nagoya Type-III";
    } else if(ant_type == 5){
        antenna_type = "Boswell";
    } else if(ant_type == 6){
        antenna_type = "RH Half-helical";
    } else if(ant_type == 7){
        antenna_type = "LH Half-helical";
    } else if(ant_type == 8){
        antenna_type = "Birdcage";
    } else {
        antenna_type = "Unknown";
    }

    printf("-------------------Helicon Antenna------------------\n");
    printf("Geometry selected: %s Antenna.\n", antenna_type);
    printf( "Antenna Radius = %d \n", (int) ant_radius/2 );
    printf( "Antenna Lenght = %d \n", ant_lenght/2 );

    if(ant_type == 2 || ant_type == 6 || ant_type == 7){
        printf( "Number of turns = %f \n", num_turns );
    } else if( ant_type == 8 ){
        printf( "Number of arms = %f \n", num_arms );
    }

    printf( "Antenna Center Coord. (x,y,z) = (%d, %d, %d)\n", ANT_X, ANT_Y, ANT_Z );
    printf( "Antenna Current = %d \n", I_0 );

}

//Functions for evolution of helical antenna 
void control_HelicalAntenna(    gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise,
                                double EB_WAVE[NX][NY][NZ] ){

    /*Apply helical antenna to grid*/
    if( ant_type == 1 ){                                //Single Loop antenna  

        circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );

    } else if( ant_type == 2 ){                         //Spiral antenna  

        half_circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, M_PI, lenght2, S2, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );

    } else if( ant_type == 3 ){                         //Double Loop antenna  

        circular_antenna( gridCfg, beamCfg, t_rise, 0,  lenght1, S1, EB_WAVE );
        circular_antenna( gridCfg, beamCfg, t_rise, 0,  lenght2, S2, EB_WAVE );

    } else if( ant_type == 4 ){                         //Nagoya typeIII helical antenna  

        half_circular_antenna( gridCfg, beamCfg, t_rise, 0,     lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, M_PI,  lenght2, S2, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );

    } else if( ant_type == 5 ){                         //Boswell antenna  

        half_circular_antenna( gridCfg, beamCfg, t_rise, 0,     lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, M_PI,  lenght2, S2, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0,    lenght5, S5, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, M_PI, lenght6, S6, EB_WAVE );

    } else if(ant_type == 6 || ant_type == 7){          //RH-LH Half helical type antenna

        half_circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght2, S2, EB_WAVE );

        helical_antenna( gridCfg, beamCfg, t_rise, 0,       lenght3, S3, EB_WAVE );
        helical_antenna( gridCfg, beamCfg, t_rise, M_PI,    lenght4, S4, EB_WAVE );

    } else if( ant_type == 8 ){                         //Birdcage antenna

        half_circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );
        half_circular_antenna( gridCfg, beamCfg, t_rise, 0, lenght2, S2, EB_WAVE );

        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght3, S3, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght4, S4, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght5, S5, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght6, S6, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght7, S7, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght8, S8, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght9, S9, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght10, S10, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght11, S11, EB_WAVE );
        linear_antenna( gridCfg, beamCfg, t_rise, 0, lenght12, S12, EB_WAVE );

    }

}

void control_HelicalAntenna_REF(    gridConfiguration *gridCfg, 
                                    beamAntennaConfiguration *beamCfg, 
                                    double t_rise,
                                    double EB_WAVE[NX][NY][NZ_REF] ){
    
    /*Apply helical antenna to grid*/
    if( ant_type == 1 ){                                //Single Loop antenna  

        circular_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );

    } else if( ant_type == 2 ){                         //Spiral antenna  

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0,     lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, M_PI,  lenght2, S2, EB_WAVE );

        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );

    } else if( ant_type == 3 ){                         //Double Loop antenna  

        circular_antenna_ref( gridCfg, beamCfg, t_rise, 0,  lenght1, S1, EB_WAVE );
        circular_antenna_ref( gridCfg, beamCfg, t_rise, 0,  lenght2, S2, EB_WAVE );

    } else if(ant_type == 4){                           //Nagoya typeIII helical antenna  

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0,     lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, M_PI,  lenght2, S2, EB_WAVE );

        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );

    } else if(ant_type == 5){                           //Boswell antenna  

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0,     lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, M_PI,  lenght2, S2, EB_WAVE );

        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0,    lenght3, S3, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, M_PI, lenght4, S4, EB_WAVE );

    } else if(ant_type == 6 || ant_type == 7){          //Half helical type antenna

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght2, S2, EB_WAVE );

        helical_antenna_ref( gridCfg, beamCfg, t_rise, 0,       lenght3, S3, EB_WAVE );
        helical_antenna_ref( gridCfg, beamCfg, t_rise, M_PI,    lenght4, S4, EB_WAVE );

    } else if( ant_type == 8 ){                         //Birdcage type antenna

        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght1, S1, EB_WAVE );
        half_circular_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght2, S2, EB_WAVE );

        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght3, S3, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght4, S4, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght5, S5, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght6, S6, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght7, S7, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght8, S8, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght9, S9, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght10, S10, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght11, S11, EB_WAVE );
        linear_antenna_ref( gridCfg, beamCfg, t_rise, 0, lenght12, S12, EB_WAVE );

    }

}

//Antenna field injection functions
int linear_antenna( gridConfiguration *gridCfg, 
                    beamAntennaConfiguration *beamCfg, 
                    double t_rise, double Phase, 
                    int lenght, double **S_coord,
                    double EB_WAVE[NX][NY][NZ] ){

    int ii, jj ,kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){ 
        
        ii = 2 * (int)S_coord[ll][0];       //Multply by two for the leapfrog grid
        jj = 2 * (int)S_coord[ll][1];       //Multply by two for the leapfrog grid
        kk = 2 * (int)S_coord[ll][2];       //Multply by two for the leapfrog grid

        //Current goes in the Z-direction
        EB_WAVE[ii  ][jj  ][kk+1] = - I_0 * sin( OMEGA_T + Phase) * t_rise * DT ;

    }

    return EXIT_SUCCESS;
}

int helical_antenna(    gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, double Phase,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ] ){

    size_t ii, jj, kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){

        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        EB_WAVE[ii  ][jj  ][kk+1] = - I_0 * sin( OMEGA_T + Phase) * t_rise * DT ;
    }

    return EXIT_SUCCESS;
}


int half_circular_antenna(  gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg, 
                            double t_rise, double Phase,
                            int lenght, double **S_coord,
                            double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, kk, ll;
    double J_x, J_y, x, y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        x = (ii - ANT_X)/ant_radius;
        y = (jj - ANT_Y)/ant_radius;

        if( ( ii > ANT_X ) && ( ii <= (ANT_X + ant_radius) ) &&              // theta = [0, PI/2]
            ( jj > ANT_Y ) && ( jj < (ANT_Y + ant_radius) ) ) {                 //counterclockwise

            J_x = -(I_0/2) * y * cos( Phase ) ;
            J_y =  (I_0/2) * x * cos( Phase ) ;
        
        } else if( ( ii < ANT_X ) && ( ii >= (ANT_X - ant_radius) ) &&       // theta = [PI, PI/2]  
                   ( jj > ANT_Y ) && ( jj < (ANT_Y + ant_radius) ) ){           //clockwise

            J_x =  (I_0/2) * y * cos( Phase ) ;
            J_y = -(I_0/2) * x * cos( Phase ) ;

        } else if( ( ii < ANT_X ) && ( ii >= (ANT_X - ant_radius) ) &&       // theta = [PI, 3PI/2] 
                   ( jj < ANT_Y ) && ( jj > (ANT_Y - ant_radius) ) ){           //clockwise

            J_x =  (I_0/2) * y * cos( Phase ) ;
            J_y = -(I_0/2) * x * cos( Phase ) ;

        } else if( ( ii > ANT_X ) && ( ii <= (ANT_X + ant_radius) ) &&      // theta = [3PI/2, 0]
                   ( jj < ANT_Y ) && ( jj > (ANT_Y - ant_radius) ) ){           //counterclockwise

            J_x = -(I_0/2) * y * cos( Phase ) ;
            J_y =  (I_0/2) * x * cos( Phase ) ;

        } 

        EB_WAVE[ii+1][jj  ][kk  ]  = - J_x * sin( OMEGA_T ) * t_rise * DT * 0.5;
        EB_WAVE[ii  ][jj+1][kk  ]  = - J_y * sin( OMEGA_T ) * t_rise * DT * 0.5;
    }

    return EXIT_SUCCESS;

}

int circular_antenna(   gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, double Phase,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ] ){

    int ii, jj, kk, ll;
    double J_x, J_y, x, y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        x = (ii - ANT_X)/ant_radius;
        y = (jj - ANT_Y)/ant_radius;
        
        J_x =  I_0 * y * cos( Phase ) ;
        J_y = -I_0 * x * cos( Phase ) ;

        EB_WAVE[ii+1][jj  ][kk  ]  = - J_x * sin( OMEGA_T ) * t_rise * DT ;
        EB_WAVE[ii  ][jj+1][kk  ]  = - J_y * sin( OMEGA_T ) * t_rise * DT ;
    }

    return EXIT_SUCCESS;

}

//Reference field functions
int linear_antenna_ref( gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg,  
                        double t_rise, double Phase,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] ){

    size_t ii, jj ,kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){ 
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = (2 * (int)S_coord[ll][2]) - u;
        
        //Current goes in the Z-direction
        EB_WAVE[ii  ][jj  ][kk+1] = - I_0 * sin( OMEGA_T + Phase) * t_rise * DT ;
    }

    return EXIT_SUCCESS;
}

int helical_antenna_ref(gridConfiguration *gridCfg, 
                        beamAntennaConfiguration *beamCfg, 
                        double t_rise, double Phase,
                        int lenght, double **S_coord,
                        double EB_WAVE[NX][NY][NZ_REF] ){

    size_t ii, jj, kk, ll;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){

        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = (2 * (int)S_coord[ll][2]) - u;

        EB_WAVE[ii  ][jj  ][kk+1] = - I_0 * sin( OMEGA_T + Phase) * t_rise * DT ;
    }

    return EXIT_SUCCESS;
}

int half_circular_antenna_ref(  gridConfiguration *gridCfg, 
                                beamAntennaConfiguration *beamCfg, 
                                double t_rise, double Phase,
                                int lenght, double **S_coord,
                                double EB_WAVE[NX][NY][NZ_REF] ){

    int ii, jj, kk, ll;
    double J_x, J_y, x, y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        x = (ii - ANT_X)/ant_radius;
        y = (jj - ANT_Y)/ant_radius;

        if( ( ii > ANT_X ) && ( ii <= (ANT_X + ant_radius) ) &&              // theta = [0, PI/2]
            ( jj > ANT_Y ) && ( jj < (ANT_Y + ant_radius) ) ) {                 //counterclockwise

            J_x = -(I_0/2) * y * cos( Phase ) ;
            J_y =  (I_0/2) * x * cos( Phase ) ;
        
        } else if( ( ii < ANT_X ) && ( ii >= (ANT_X - ant_radius) ) &&       // theta = [PI, PI/2]  
                   ( jj > ANT_Y ) && ( jj < (ANT_Y + ant_radius) ) ){           //clockwise

            J_x =  (I_0/2) * y * cos( Phase ) ;
            J_y = -(I_0/2) * x * cos( Phase ) ;

        } else if( ( ii < ANT_X ) && ( ii >= (ANT_X - ant_radius) ) &&       // theta = [PI, 3PI/2] 
                   ( jj < ANT_Y ) && ( jj > (ANT_Y - ant_radius) ) ){           //clockwise

            J_x =  (I_0/2) * y * cos( Phase ) ;
            J_y = -(I_0/2) * x * cos( Phase ) ;

        } else if( ( ii > ANT_X ) && ( ii <= (ANT_X + ant_radius) ) &&      // theta = [3PI/2, 0]
                   ( jj < ANT_Y ) && ( jj > (ANT_Y - ant_radius) ) ){           //counterclockwise

            J_x = -(I_0/2) * y * cos( Phase ) ;
            J_y =  (I_0/2) * x * cos( Phase ) ;

        } 

        EB_WAVE[ii+1][jj  ][kk  ]  = - J_x * sin( OMEGA_T ) * t_rise * DT * 0.5;
        EB_WAVE[ii  ][jj+1][kk  ]  = - J_y * sin( OMEGA_T ) * t_rise * DT * 0.5;
    }

    return EXIT_SUCCESS;

}

int circular_antenna_ref(   gridConfiguration *gridCfg, 
                            beamAntennaConfiguration *beamCfg, 
                            double t_rise, double Phase,
                            int lenght, double **S_coord,
                            double EB_WAVE[NX][NY][NZ_REF] ){

    int ii, jj, kk, ll;
    double J_x, J_y, x, y;

#pragma omp parallel for
    for( ll = 0 ; ll < lenght ; ll++ ){
        
        ii = 2 * (int)S_coord[ll][0];
        jj = 2 * (int)S_coord[ll][1];
        kk = 2 * (int)S_coord[ll][2];

        x = (ii - ANT_X)/ant_radius;
        y = (jj - ANT_Y)/ant_radius;
        
        J_x =  I_0 * y * cos( Phase ) ;
        J_y = -I_0 * x * cos( Phase ) ;

        EB_WAVE[ii+1][jj  ][kk  ]  = - J_x * sin( OMEGA_T ) * t_rise * DT ;
        EB_WAVE[ii  ][jj+1][kk  ]  = - J_y * sin( OMEGA_T ) * t_rise * DT ;
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
