#include "helical_antenna.h"

static int Z_0;             //initial Z coordinate for the antenna
static int Z_1;             //end Z coordinate for the antenna  

static double **S1 = NULL;
static double **S2 = NULL;
static double **S3 = NULL;
static double **S4 = NULL;

void init_helicalAntenna(   gridConfiguration *gridCfg, 
                            helicalAntenna *helicAnt ){

    char fullDir[PATH_MAX], directory[1024];
    char dir[] = "tools";
    
    /*Compute antenna initial and end point*/
    //The wavelenght is defined by the antenna leght
    Z_0 = (int)( 0.5 * (NZ - ant_lenght) );
    Z_1 = (int)( 0.5 * (NZ + ant_lenght) );

    if ((Z_0 % 2) != 0)  ++Z_0;
    if ((Z_1 % 2) != 0)  ++Z_1;

    if( ant_type == 1){
        sprintf(directory, "%s/%s", dir, "Nagoya_antenna");
    } else if(ant_type == 2){
        sprintf(directory, "%s/%s", dir, "Helical_antenna");
    }

    sprintf(fullDir,"%s/Section_%s.txt", directory, "0" );

    read_file(fullDir, S1);

    exit(-1);
}

void read_file(char *filename, double **S_coord ){

    FILE *file;
    int row = 0; 
    char line[500];

    //int lenght = get_lenght(filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    }

    S_coord = allocate2DArray( lenght, 3 );
    while( fgets(line, sizeof(line), file ) ){
        char *token = strtok(line,",");
        int col=0; 

        while( token != NULL && col < 3){
            S_coord[row][col] = atoi(token);
            token = strtok(NULL, ",");
            col++;
            printf(" %.1f \n", S_coord[row][col] );
        }
        row++;
    }

    fclose(file);
}

int count_lines(FILE* file){

    char buf[BUF_SIZE];
    int counter = 0;
    for(;;){
        size_t res = fread(buf, 1, BUF_SIZE, file);
        if (ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                counter++;

        if (feof(file))
            break;
    }

    return counter;
}

int get_lenght(char *filename){

    FILE *file;
    int lenght = 0;
    char ch;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    }

    // Read the file character by character
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lenght++;  // Increment row count when newline character is found
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
    
    int x_pos, y_pos;
    /*Apply helical antenna to grid*/
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

}

double sinusoidal_current(  beamAntennaConfiguration *beamCfg,
                            int t_int ){

    return sin( omega_t ); //argument * dt
}

int linear_antenna( gridConfiguration *gridCfg, 
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
}

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

