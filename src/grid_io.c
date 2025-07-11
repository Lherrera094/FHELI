#include "grid_io.h"

//Write timetraces to file
int writeTimetraces2ascii( int T_end, double Period, 
                           char filename[], double **timetraces ) {
//{{{

    size_t
        ii;

    FILE
        *file_pntr;

    // open file in w(rite) mode; might consider using a+ instead
    file_pntr   = fopen( filename, "w" );
    if (file_pntr == NULL) {
        printf( "ERROR: Unable to create file for timetraces.\n" );
        return EXIT_FAILURE;
    } else {
        // NOTE: if return value of printf < 0, then writing failed.
        //       might be good idea to implicetely check this
        //       e.g. if ( (fprintf( file_pntr, "a b c" )) < 0 ) ....
        fprintf( file_pntr, "# T  poynt_z1  poynt_z2  poynt_x1  poynt_x2  poynt_y1  poynt_y2  P_out\n" ); 
        for ( ii=0 ; ii<(T_end/(int)Period) ; ++ii )
            fprintf( file_pntr, " %4d  %13.6e  %13.6e  %13.6e  %13.6e  %13.6e  %13.6e  %13.6e\n",
                    (int)timetraces[ii][1], 
                    timetraces[ii][2], timetraces[ii][3],
                    timetraces[ii][4], timetraces[ii][5],
                    timetraces[ii][6], timetraces[ii][7],
                    (timetraces[ii][2]+timetraces[ii][3] + timetraces[ii][4]+timetraces[ii][5] + timetraces[ii][6]+timetraces[ii][7])
                  );
        if ((fclose(file_pntr)) == EOF) {
            printf( "ERROR: could not close file for timetraces.\n" );
        }
    }
    printf( "successfully written timetraces into %s\n", filename );
    return EXIT_SUCCESS;

}//}}}


//#ifdef HDF5
int writeMyHDF_v4( int dim0, int dim1, int dim2, char filename[], char dataset[], double array_3D[dim0][dim1][dim2] ) {
    //#{{{

    // hdf related variables
    hid_t       file_id, dataset_id, dataspace_id;      // object identifiers
    hsize_t     dims[3];                                // size used for dimensions
    herr_t      status;                                 // function return value

    // hdf5 related variables used for applying shuffle and compression filter
    hid_t       dcpl;
    hsize_t     chunk[3];
    unsigned int    filter_info;
    int         filter_avail;

    // required for check if hdf5-file already exists
    struct      stat st;

    // assume as default setting, that filters are available
    filter_avail = 1;

    // check if specified hdf5 file already exists 
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)

        // hdf5 version 1.8.0 introduced H5Lexists to check if link (to group or dataset) exists in hdf5-file
#if H5_VERS_MAJOR>=1 && H5_VERS_MINOR>=8
        if ( H5_VERS_MINOR >= 10 ) {
            printf( "WARNING: hdf5 version 1.10 (or larger is used)\n" );
            printf( "         behavior of H5Lexists was slightly changed in this version\n" );
            printf( "         for details, see https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists\n" );
        }
        if ( H5Lexists( file_id,                // file or group identifier
                        dataset,                // name of link (to group or dataset) to check
                        H5P_DEFAULT )           // link access property list identifiert
                > 0 ) {                         // NOTE: for version 1.8.10, this might be slightly different
            printf( "ERROR: dataset named '%s' already exists in file '%s'\n", dataset, filename );
            printf( "       dataset WILL be overwrite. \n" );
            status = H5Ldelete( file_id, dataset, H5P_DEFAULT );
            //printf( "ERROR: dataset named '%s' already exists in file '%s'\n", dataset, filename );
            //printf( "       dataset will NOT be saved (no overwrite by default)\n" );
            //status = H5Fclose(file_id);
            if (status < 0) {
                printf( "ERROR: could not close file '%s'\n", filename );
                H5Fclose(file_id);
                return EXIT_FAILURE;
            }
        }
#endif
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }

    // create simple data space for the dataset
    // (simple = regular N-dimensional array, i.e. data on regular rectangular grid)
    // (complex = e.g.: irregular grids resulting from dynamic refinement of mesh)
    dims[0] = dim0;
    dims[1] = dim1;
    dims[2] = dim2;
    dataspace_id = H5Screate_simple( 3,     // number of dimensions of dataspace 
                                     dims,  // size of array in each dimension
                                     NULL); // allow stretching of data space (NULL=no)

    // check if filters for shuffle and gzip exists, if yes, apply them
    // (check is done, because the filters are optional parts of hdf5 library)
    // check if gzip-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_DEFLATE )) ) {
        printf( "WARNING: gzip filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_DEFLATE, &filter_info );
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: gzip filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
    }
    // check if shuffle-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_SHUFFLE )) ) {
        printf( "WARNING: shuffle filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_SHUFFLE, &filter_info );
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: shuffle filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
    }

    // apply shuffle and gzip filters, if available
    if (filter_avail) {
        // set chunk size to be same as dimension (might not be optimal, but seems same as h5repack)
        chunk[0] = dim0;
        chunk[1] = dim1;
        chunk[2] = dim2;
        // create dataset creation property list
        dcpl = H5Pcreate( H5P_DATASET_CREATE );
        // add shuffle filter and gzip compression filter
        // note that the order of filter is significant: first shuffle!
        // order of filters applied correspond to order in which they are invoked when writin gdata
        status = H5Pset_shuffle( dcpl );
        if (status < 0) printf( "ERROR: could not add shuffle filter\n" );
        status = H5Pset_deflate( dcpl, 9 );
        if (status < 0) printf( "ERROR: could not add gzip filter\n" );
        status = H5Pset_chunk(dcpl,             // dataset creation property list identifier
                              3,                // number of dimensions of each chunk
                              chunk );          // array defining size, in dataset elements, of each chunk
        if (status < 0) printf( "ERROR: could not set chunk size\n" );
        // create the dataset
        dataset_id = H5Dcreate( file_id,        // file identifier (or group identifier)
                                dataset,        // name of dataset (relative to group specified, if speficied)
                                H5T_NATIVE_DOUBLE, // datatype to use when creating dataset
                                dataspace_id,   // dataspace identifier
                                H5P_DEFAULT,    // link creation property list (was dataset creating property list <=v1.6)
                                dcpl,           // dataset creation property list (added in HDF5v1.8)
                                H5P_DEFAULT);   // dataset access property list (added in HDF5v1.8)
    } else {
        // create the dataset
        dataset_id = H5Dcreate( file_id,        // file identifier (or group identifier)
                                dataset,        // name of dataset (relative to group specified, if speficied)
                                H5T_NATIVE_DOUBLE, // datatype to use when creating dataset
                                dataspace_id,   // dataspace identifier
                                H5P_DEFAULT,    // link creation property list (was dataset creating property list <=v1.6)
                                H5P_DEFAULT,    // dataset creation property list (added in HDF5v1.8)
                                H5P_DEFAULT);   // dataset access property list (added in HDF5v1.8)
    }

    // write the dataset
    status = H5Dwrite( dataset_id,          // dataset identifier
                       H5T_NATIVE_DOUBLE,    // informs hdf about format of data in memory of computer
                       H5S_ALL,             // identifier of memory dataspace
                       H5S_ALL,             // file space identifier
                       H5P_DEFAULT,         // data transfer property list
//                       array_2D[0]);        // pointer to data array
                       array_3D);        // pointer to data array
    if (status < 0) 
        printf( "ERROR: could not write dataset '%s' to file '%s'\n", dataset, filename );

    // terminate access and free ressources/identifiers
    // dataset creation property list
    if (filter_avail) {
        status = H5Pclose(dcpl);
        if (status < 0)
            printf( "ERROR: could not close filter\n" );
    }
    // dataset
    status = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset '%s'\n", dataset );
    // data space
    status = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for dataset '%s'\n", dataset );
    // file 
    status = H5Fclose(file_id);
    if (status < 0) printf( "ERROR: could not close file '%s'\n", filename );
    
    return EXIT_SUCCESS;
}//#}}}
//#endif

//#ifdef HDF5
int writeMyHDF_v4_1D(int dim0, char filename[], char dataset[], double array_1D[dim0]) {
    // hdf related variables
    hid_t file_id, dataset_id, dataspace_id;
    hsize_t dims[1];
    herr_t status;

    // hdf5 related variables used for applying shuffle and compression filter
    hid_t dcpl;
    hsize_t chunk[1];
    unsigned int filter_info;
    int filter_avail = 1;

    // required for check if hdf5-file already exists
    struct stat st;

    // check if specified hdf5 file already exists 
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)

        // hdf5 version 1.8.0 introduced H5Lexists to check if link (to group or dataset) exists in hdf5-file
#if H5_VERS_MAJOR>=1 && H5_VERS_MINOR>=8
        if ( H5_VERS_MINOR >= 10 ) {
            printf( "WARNING: hdf5 version 1.10 (or larger is used)\n" );
            printf( "         behavior of H5Lexists was slightly changed in this version\n" );
            printf( "         for details, see https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists\n" );
        }
        if ( H5Lexists( file_id,                // file or group identifier
                        dataset,                // name of link (to group or dataset) to check
                        H5P_DEFAULT )           // link access property list identifiert
                > 0 ) {                         // NOTE: for version 1.8.10, this might be slightly different
            printf( "ERROR: dataset named '%s' already exists in file '%s'\n", dataset, filename );
            printf( "       dataset WILL be overwrite. \n" );
            status = H5Ldelete( file_id, dataset, H5P_DEFAULT );
            //printf( "ERROR: dataset named '%s' already exists in file '%s'\n", dataset, filename );
            //printf( "       dataset will NOT be saved (no overwrite by default)\n" );
            //status = H5Fclose(file_id);
            if (status < 0) {
                printf( "ERROR: could not close file '%s'\n", filename );
                H5Fclose(file_id);
                return EXIT_FAILURE;
            }
        }
#endif
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }

    // Create dataspace
    dims[0] = dim0;
    dataspace_id = H5Screate_simple(1, dims, NULL);

    // Check filters
    if (!(H5Zfilter_avail(H5Z_FILTER_DEFLATE))) {
        printf("WARNING: gzip filter not available (for hdf5)\n");
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info(H5Z_FILTER_DEFLATE, &filter_info);
        if (status < 0) printf("ERROR: could not get hdf5 filter info\n");
        if (!(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
            !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED)) {
            printf("WARNING: gzip filter not available for encoding and decoding (for hdf5)\n");
            filter_avail = 0;
        }
    }
    
    if (!(H5Zfilter_avail(H5Z_FILTER_SHUFFLE))) {
        printf("WARNING: shuffle filter not available (for hdf5)\n");
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info(H5Z_FILTER_SHUFFLE, &filter_info);
        if (status < 0) printf("ERROR: could not get hdf5 filter info\n");
        if (!(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
            !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED)) {
            printf("WARNING: shuffle filter not available for encoding and decoding (for hdf5)\n");
            filter_avail = 0;
        }
    }

    // Create dataset with or without filters
    if (filter_avail) {
        chunk[0] = dim0;
        dcpl = H5Pcreate(H5P_DATASET_CREATE);
        status = H5Pset_shuffle(dcpl);
        if (status < 0) printf("ERROR: could not add shuffle filter\n");
        status = H5Pset_deflate(dcpl, 9);
        if (status < 0) printf("ERROR: could not add gzip filter\n");
        status = H5Pset_chunk(dcpl, 1, chunk);
        if (status < 0) printf("ERROR: could not set chunk size\n");
        
        dataset_id = H5Dcreate(file_id, dataset, H5T_NATIVE_DOUBLE, dataspace_id,
                             H5P_DEFAULT, dcpl, H5P_DEFAULT);
    } else {
        dataset_id = H5Dcreate(file_id, dataset, H5T_NATIVE_DOUBLE, dataspace_id,
                             H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    // Write data
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, array_1D);
    if (status < 0)
        printf("ERROR: could not write dataset '%s' to file '%s'\n", dataset, filename);

    // Clean up
    if (filter_avail) {
        status = H5Pclose(dcpl);
        if (status < 0) printf("ERROR: could not close filter\n");
    }
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '%s'\n", dataset);
    status = H5Sclose(dataspace_id);
    if (status < 0) printf("ERROR: could not close dataspace for dataset '%s'\n", dataset);
    status = H5Fclose(file_id);
    if (status < 0) printf("ERROR: could not close file '%s'\n", filename);
    
    return EXIT_SUCCESS;
}
//#endif

//#ifdef HDF5
int writeMyHDF_v4_2D(int dim0, int dim1, char filename[], char dataset[], double array_2D[dim0][dim1]) {
    // hdf related variables
    hid_t file_id, dataset_id, dataspace_id;
    hsize_t dims[2];
    herr_t status;

    // hdf5 related variables used for applying shuffle and compression filter
    hid_t dcpl;
    hsize_t chunk[2];
    unsigned int filter_info;
    int filter_avail = 1;

    // required for check if hdf5-file already exists
    struct stat st;

    // Check if file exists
    if (stat(filename, &st) == 0) {
        file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);

#if H5_VERS_MAJOR>=1 && H5_VERS_MINOR>=8
        if (H5_VERS_MINOR >= 10) {
            printf("WARNING: hdf5 version 1.10 (or larger is used)\n");
            printf("         behavior of H5Lexists was slightly changed in this version\n");
        }
        if (H5Lexists(file_id, dataset, H5P_DEFAULT) > 0) {
            printf("ERROR: dataset named '%s' already exists in file '%s'\n", dataset, filename);
            status = H5Fclose(file_id);
            if (status < 0) printf("ERROR: could not close file '%s'\n", filename);
            return EXIT_FAILURE;
        }
#endif
    } else {
        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }

    // Create dataspace
    dims[0] = dim0;
    dims[1] = dim1;
    dataspace_id = H5Screate_simple(2, dims, NULL);

    // Check filters
    if (!(H5Zfilter_avail(H5Z_FILTER_DEFLATE))) {
        printf("WARNING: gzip filter not available (for hdf5)\n");
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info(H5Z_FILTER_DEFLATE, &filter_info);
        if (status < 0) printf("ERROR: could not get hdf5 filter info\n");
        if (!(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
            !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED)) {
            printf("WARNING: gzip filter not available for encoding and decoding (for hdf5)\n");
            filter_avail = 0;
        }
    }
    
    if (!(H5Zfilter_avail(H5Z_FILTER_SHUFFLE))) {
        printf("WARNING: shuffle filter not available (for hdf5)\n");
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info(H5Z_FILTER_SHUFFLE, &filter_info);
        if (status < 0) printf("ERROR: could not get hdf5 filter info\n");
        if (!(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
            !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED)) {
            printf("WARNING: shuffle filter not available for encoding and decoding (for hdf5)\n");
            filter_avail = 0;
        }
    }

    // Create dataset with or without filters
    if (filter_avail) {
        chunk[0] = dim0;
        chunk[1] = dim1;
        dcpl = H5Pcreate(H5P_DATASET_CREATE);
        status = H5Pset_shuffle(dcpl);
        if (status < 0) printf("ERROR: could not add shuffle filter\n");
        status = H5Pset_deflate(dcpl, 9);
        if (status < 0) printf("ERROR: could not add gzip filter\n");
        status = H5Pset_chunk(dcpl, 2, chunk);
        if (status < 0) printf("ERROR: could not set chunk size\n");
        
        dataset_id = H5Dcreate(file_id, dataset, H5T_NATIVE_DOUBLE, dataspace_id,
                             H5P_DEFAULT, dcpl, H5P_DEFAULT);
    } else {
        dataset_id = H5Dcreate(file_id, dataset, H5T_NATIVE_DOUBLE, dataspace_id,
                             H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    // Write data
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, array_2D);
    if (status < 0)
        printf("ERROR: could not write dataset '%s' to file '%s'\n", dataset, filename);

    // Clean up
    if (filter_avail) {
        status = H5Pclose(dcpl);
        if (status < 0) printf("ERROR: could not close filter\n");
    }
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '%s'\n", dataset);
    status = H5Sclose(dataspace_id);
    if (status < 0) printf("ERROR: could not close dataspace for dataset '%s'\n", dataset);
    status = H5Fclose(file_id);
    if (status < 0) printf("ERROR: could not close file '%s'\n", filename);
    
    return EXIT_SUCCESS;
}
//#endif

//#ifdef HDF5
int writeMyHDF_single_value( char filename[], int value ) {
    //#{{{

    long        data2write_long[1];

    // hdf related variables
    hid_t       file_id, dataset_id, dataspace_id;      // object identifiers
    hsize_t     dims[1];                                // size used for dimensions
    herr_t      status;                                 // function return value

    // note that shuffle and compression filter is not applied here, as only single values are saved

    // required for check if hdf5-file already exists
    struct      stat st;

    // check if specified hdf5 file already exists
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)

        // hdf5 version 1.8.0 introduced H5Lexists to check if link (to group or dataset) exists in hdf5-file
#if H5_VERS_MAJOR>=1 && H5_VERS_MINOR>=8
        if ( H5_VERS_MINOR >= 10 ) {
            printf( "WARNING: hdf5 version 1.10 (or larger is used)\n" );
            printf( "         behavior of H5Lexists was slightly changed in this version\n" );
            printf( "         for details, see https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists\n" );
        }
        if (H5Lexists(file_id, "/last_t_fields", H5P_DEFAULT) > 0) {
            status = H5Ldelete(file_id, "/last_t_fields", H5P_DEFAULT);
            if (status < 0) {
                printf("WARNING: could not delete existing dataset '/last_t_fields'\n");
            }
        }
#endif
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }
    
    // create simple data space for the dataset
    // (simple = regular N-dimensional array, i.e. data on regular rectangular grid)
    // (complex = e.g.: irregular grids resulting from dynamic refinement of mesh)
    dims[0] = 1;
    dataspace_id = H5Screate_simple( 1,     // number of dimensions of dataspace 
                                     dims,  // size of array in each dimension
                                     NULL); // allow stretching of data space (NULL=no)

    // Value
    // create the dataset
    dataset_id = H5Dcreate( file_id,                // file identifier (or group identifier)
                            "last_t_fields",        // name of dataset (relative to group specified, if speficied)
                            H5T_NATIVE_LONG,        // datatype to use when creating dataset
                            dataspace_id,           // dataspace identifier
                            H5P_DEFAULT,            // link creation property list (was dataset creating property list <=v1.6)
                            H5P_DEFAULT,            // dataset creation property list (added in HDF5v1.8)
                            H5P_DEFAULT);           // dataset access property list (added in HDF5v1.8)

    // write the dataset
    data2write_long[0]  = (long)value;
    status = H5Dwrite( dataset_id,                  // dataset identifier
                       H5T_NATIVE_LONG,             // informs hdf about format of data in memory of computer
                       H5S_ALL,                     // identifier of memory dataspace
                       H5S_ALL,                     // file space identifier
                       H5P_DEFAULT,                 // data transfer property list
                       data2write_long);            // pointer to data array
    if (status < 0) printf( "ERROR: could not write dataset 'value' into file '%s'\n", filename);
    // terminate access and free ressources/identifiers for dataset
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset 'value'\n");

    // terminate access and free ressources/identifiers
    // data space
    status = H5Sclose(dataspace_id);
    if (status < 0) printf("ERROR: could not close dataspace'\n");
    // file 
    status = H5Fclose(file_id);
    if (status < 0) printf( "ERROR: could not close file '%s'\n", filename);
    
    return EXIT_SUCCESS;
}
//#endif

//#ifdef HDF5
int writeConfig2HDF( gridConfiguration *gridCfg, beamAntennaConfiguration *beamCfg, char filename[] ) {
    //#{{{

    long        data2write_long[1];
    double      data2write_double[1];

    // hdf related variables
    hid_t       file_id, dataset_id, dataspace_id;      // object identifiers
    hsize_t     dims[1];                                // size used for dimensions
    herr_t      status;                                 // function return value

    // note that shuffle and compression filter is not applied here, as only single values are saved

    // required for check if hdf5-file already exists
    struct      stat st;

    // check if specified hdf5 file already exists
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)

        // hdf5 version 1.8.0 introduced H5Lexists to check if link (to group or dataset) exists in hdf5-file
#if H5_VERS_MAJOR>=1 && H5_VERS_MINOR>=8
        if ( H5_VERS_MINOR >= 10 ) {
            printf( "WARNING: hdf5 version 1.10 (or larger is used)\n" );
            printf( "         behavior of H5Lexists was slightly changed in this version\n" );
            printf( "         for details, see https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists\n" );
        }
        if ( H5Lexists( file_id,                // file or group identifier
                        "/config",              // name of link (to group or dataset) to check
                        H5P_DEFAULT )           // link access property list identifiert
                > 0 ) {                         // NOTE: for version 1.8.10, this might be slightly different
            printf( "ERROR: dataset named '/config' already exists in file '%s'\n", filename );
            printf( "       dataset will NOT be saved (no overwrite by default)\n" );
            status = H5Fclose(file_id);
            if (status < 0) printf("ERROR: could not close file '%s'\n", filename);
            return EXIT_FAILURE;
        }
#endif
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }

    // create new group for config parameters
    H5Gcreate( file_id, 
               "/config",
               H5P_DEFAULT,
               H5P_DEFAULT,
               H5P_DEFAULT);

    // create simple data space for the dataset
    // (simple = regular N-dimensional array, i.e. data on regular rectangular grid)
    // (complex = e.g.: irregular grids resulting from dynamic refinement of mesh)
    dims[0] = 1;
    dataspace_id = H5Screate_simple( 1,     // number of dimensions of dataspace 
                                     dims,  // size of array in each dimension
                                     NULL); // allow stretching of data space (NULL=no)

    // period
    // create the dataset
    dataset_id = H5Dcreate( file_id,        // file identifier (or group identifier)
                            "/config/period",  // name of dataset (relative to group specified, if speficied)
                            H5T_NATIVE_LONG, // datatype to use when creating dataset
                            dataspace_id,   // dataspace identifier
                            H5P_DEFAULT,    // link creation property list (was dataset creating property list <=v1.6)
                            H5P_DEFAULT,    // dataset creation property list (added in HDF5v1.8)
                            H5P_DEFAULT);   // dataset access property list (added in HDF5v1.8)

    // write the dataset
    data2write_long[0]  = (long)PERIOD;
    status = H5Dwrite( dataset_id,          // dataset identifier
                       H5T_NATIVE_LONG,   // informs hdf about format of data in memory of computer
                       H5S_ALL,             // identifier of memory dataspace
                       H5S_ALL,             // file space identifier
                       H5P_DEFAULT,         // data transfer property list
                       data2write_long);     // pointer to data array
    if (status < 0) printf( "ERROR: could not write dataset '/config/period' into file '%s'\n", filename);
    // terminate access and free ressources/identifiers for dataset
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/period'\n");

    // D_ABSORB
    dataset_id = H5Dcreate( file_id, "/config/d_absorb", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)D_ABSORB;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/d_absorb' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/d_absorb'\n");

    // N_x
    dataset_id = H5Dcreate( file_id, "/config/N_x", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)NX;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/N_x' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/N_x'\n");

    // N_y
    dataset_id = H5Dcreate( file_id, "/config/N_y", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)NY;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/N_y' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/N_y'\n");

    // N_z
    dataset_id = H5Dcreate( file_id, "/config/N_z", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)NZ;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/N_z' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/N_z'\n");

    // ant_x
    dataset_id = H5Dcreate( file_id, "/config/ant_x", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)ANT_X;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_x' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_x'\n");

    // ant_y
    dataset_id = H5Dcreate( file_id, "/config/ant_y", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)ANT_Y;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_y' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_y'\n");

    // ant_z
    dataset_id = H5Dcreate( file_id, "/config/ant_z", H5T_NATIVE_LONG,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_long[0]  = (long)ANT_Z;
    status = H5Dwrite( dataset_id, H5T_NATIVE_LONG,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_long); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_z' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_z'\n");

    // ant_w0x
    dataset_id = H5Dcreate( file_id, "/config/ant_w0x", H5T_NATIVE_DOUBLE,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = ant_w0x;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_w0x' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_w0x'\n");

    // ant_w0y
    dataset_id = H5Dcreate( file_id, "/config/ant_w0y", H5T_NATIVE_DOUBLE,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = ant_w0y;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_w0y' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_w0y'\n");

    // z2waist
    dataset_id = H5Dcreate( file_id, "/config/z2waist", H5T_NATIVE_DOUBLE,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = z2waist;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/z2waist' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/z2waist'\n");

    // ant_lenght
    dataset_id = H5Dcreate( file_id, "/config/ant_lenght", H5T_NATIVE_DOUBLE,
        dataspace_id, 
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = ant_lenght/2;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                    H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_lenght' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_lenght'\n");

    // ant_radius
    dataset_id = H5Dcreate( file_id, "/config/ant_radius", H5T_NATIVE_DOUBLE,
        dataspace_id, 
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = ant_radius/2;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                    H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/ant_radius' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/ant_radius'\n");

    // run_time
    dataset_id = H5Dcreate( file_id, "/config/run_time", H5T_NATIVE_DOUBLE,
        dataspace_id, 
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    data2write_double[0]  = run_time;
    status = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE,
                    H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    data2write_double); 
    if (status < 0) printf( "ERROR: could not write dataset '/config/run_time' into file '%s'\n", filename);
    status = H5Dclose(dataset_id);
    if (status < 0) printf("ERROR: could not close dataset '/config/run_time'\n");

    // terminate access and free ressources/identifiers
    // data space
    status = H5Sclose(dataspace_id);
    if (status < 0) printf("ERROR: could not close dataspace'\n");
    // file 
    status = H5Fclose(file_id);
    if (status < 0) printf( "ERROR: could not close file '%s'\n", filename);
    
    return EXIT_SUCCESS;
}//#}}}
//#endif


//#ifdef HDF5
int readMyHDF( int dim0, int dim1, int dim2, char filename[], char dataset[], double array_3D[dim0][dim1][dim2]) {
    //#{{{

    // hdf handles
    hid_t           file_id, dset_id;
    herr_t          status;
    //hsize_t         dims[3] = { dim0, dim1, dim2};

    // open file using default properties
    file_id = H5Fopen( filename, H5F_ACC_RDONLY, H5P_DEFAULT);

    // open dataset using default properties
    dset_id = H5Dopen( file_id, dataset, H5P_DEFAULT);

    // Read the data using the default properties.
    status = H5Dread( dset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      array_3D );
    if (status < 0)
        printf( "ERROR: could not read dataset '%s' from file '%s'\n", dataset, filename );

    // close the dataset
    status = H5Dclose( dset_id);
    if (status < 0)
        printf( "ERROR: could not close dataset '%s' from file '%s'\n", dataset, filename );

    // close the file
    status = H5Fclose( file_id);
    if (status < 0)
        printf( "ERROR: could not close file '%s'\n", filename );

    return EXIT_SUCCESS;
}//#}}}
//#endif

int readMyHDF_1D( int dim0, char filename[], char dataset[], double array_1D[dim0]) {
    //#{{{

    // hdf handles
    hid_t           file_id, dset_id;
    herr_t          status;
    //hsize_t         dims[3] = { dim0, dim1, dim2};

    // open file using default properties
    file_id = H5Fopen( filename, H5F_ACC_RDONLY, H5P_DEFAULT);

    // open dataset using default properties
    dset_id = H5Dopen( file_id, dataset, H5P_DEFAULT);

    // Read the data using the default properties.
    status = H5Dread( dset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      array_1D );
    if (status < 0)
        printf( "ERROR: could not read dataset '%s' from file '%s'\n", dataset, filename );

    // close the dataset
    status = H5Dclose( dset_id);
    if (status < 0)
        printf( "ERROR: could not close dataset '%s' from file '%s'\n", dataset, filename );

    // close the file
    status = H5Fclose( file_id);
    if (status < 0)
        printf( "ERROR: could not close file '%s'\n", filename );

    return EXIT_SUCCESS;
}//#}}}
//#endif

int readMyHDFInt( const char *filename, const char *dataset, int *scalar_value ) {
    hid_t file_id, dset_id;
    herr_t status;

    // Open the file (read-only)
    file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    // Open the dataset
    dset_id = H5Dopen(file_id, dataset, H5P_DEFAULT);
    if (dset_id < 0) {
        fprintf(stderr, "ERROR: Could not open dataset '%s'\n", dataset);
        H5Fclose(file_id);
        return EXIT_FAILURE;
    }

    // Read the scalar value
    status = H5Dread(
        dset_id,                // Dataset ID
        H5T_NATIVE_INT,         // Memory datatype (double)
        H5S_ALL,                // Memory space (scalar)
        H5S_ALL,                // File space (scalar)
        H5P_DEFAULT,            // Transfer properties
        scalar_value            // Output buffer
    );

    if (status < 0) {
        fprintf(stderr, "ERROR: Could not read dataset '%s'\n", dataset);
        H5Dclose(dset_id);
        H5Fclose(file_id);
        return EXIT_FAILURE;
    }

    // Close resources
    status = H5Dclose(dset_id);
    if (status < 0) {
        fprintf(stderr, "WARNING: Could not close dataset '%s'\n", dataset);
    }

    status = H5Fclose(file_id);
    if (status < 0) {
        fprintf(stderr, "WARNING: Could not close file '%s'\n", filename);
    }

    return EXIT_SUCCESS;
}


//#if defined(HDF5) && defined(DETECTOR_ANTENNA_1D)
int detAnt1D_write2hdf5( int N_x, 
                         char filename[], char detAnt_groupName[], 
                         size_t detAnt_ypos, size_t detAnt_zpos,
                         double **detAnt_fields ){
    //#{{{ 

    // hdf related variables
    hid_t       file_id, dataset_id, dataspace_id,      // object identifiers
                group_id__detAnt,
                dataspace_id_i, 
                dataset_id_i;
    hsize_t     dims[1];                                // size used for dimensions
    herr_t      status;                                 // function return value

    // hdf5 related variables for applying shuffle and gzip filter
    hid_t           dcpl;
    hsize_t         chunk[1];
    unsigned int    filter_info;
    int             filter_avail;

    // required for check if hdf5-file already exists
    struct      stat st;
    int         ii;
    double       data2save[N_x/2];

    dims[0]     = N_x/2;
    chunk[0]    = N_x/2;

    // assume as default setting that filters are available
    filter_avail = 1;

    //data2save = dvector( 0, n_elem/2 );
    set2zero_1D( N_x/2, data2save );

    printf( "Will write data for detector antenna position y=%05ld, z=%05ld into file %s\n", 
            detAnt_ypos, detAnt_zpos, filename );

    // check if specified hdf5 file already exists
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }

    // create group for different data to be stored
    group_id__detAnt = H5Gcreate2( file_id,
                                   detAnt_groupName,
                                   H5P_DEFAULT,
                                   H5P_DEFAULT,
                                   H5P_DEFAULT);

    // check if filters for shuffle and gzip exists, if yes, apply them
    // (check is done, because the filters are optional parts of hdf5 library)
    // check if gzip-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_DEFLATE )) ) {
        printf( "WARNING: gzip filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_DEFLATE, &filter_info );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: gzip filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
    }
    // check if shuffle-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_SHUFFLE )) ) {
        printf( "WARNING: shuffle filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_SHUFFLE, &filter_info );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: shuffle filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
    }

    // apply shuffle and gzip filters, if available
    if (filter_avail) {
        // create dataset creation property list
        dcpl = H5Pcreate( H5P_DATASET_CREATE );
        // add shuffle filter and gzip compression filter
        // note that the order of filter is significant: first shuffle!
        // order of filters applied correspond to order in which they are invoked when writin gdata
        status = H5Pset_shuffle( dcpl );
        if (status < 0) printf( "ERROR: could not apply shuffle filter\n" );
        status = H5Pset_deflate( dcpl, 9 );
        if (status < 0) printf( "ERROR: could not apply gzip filter\n" );
        status = H5Pset_chunk(dcpl, 1, chunk );
        if (status < 0) printf( "ERROR: could not set size of chunk\n" );
    } 

    // store spatial coordinate
    // prepare array
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ) {
        data2save[ii/2] = (double)(ii) ;
    }
    // create data space
    dataspace_id_i = H5Screate_simple( 1,       // rank of array (number of dimensions of dataspace)
                                       dims,    // array of the size of each dimension
                                       NULL);   // allow stretching of data space (NULL=no)
//    printf( "dataspace_id_i=%d\n", dataspace_id_i);
    // create new dataset and links it to location in file
    printf( "start to create dataset 'j'\n" );
    if (filter_avail) {
        dataset_id_i = H5Dcreate( group_id__detAnt,     // file identifier (or group identifier)
                                  "i",                  // name of dataset (relative to group specified, if speficied)
                                  H5T_NATIVE_DOUBLE,    // datatype to use when creating dataset
                                  dataspace_id_i,       // dataspace identifier
                                  H5P_DEFAULT,          // link creation property list
                                  dcpl,                 // dataset creation property list
                                  H5P_DEFAULT);         // dataset access property list
    } else {
        dataset_id_i = H5Dcreate( group_id__detAnt,     // file identifier (or group identifier)
                                  "i",                  // name of dataset (relative to group specified, if speficied)
                                  H5T_NATIVE_DOUBLE,    // datatype to use when creating dataset
                                  dataspace_id_i,       // dataspace identifier
                                  H5P_DEFAULT,
                                  H5P_DEFAULT,
                                  H5P_DEFAULT);
    }
    // write the dataset
    status = H5Dwrite( dataset_id_i,        // dataset identifier
                       H5T_NATIVE_DOUBLE,   // informs hdf about format of data in memory of computer
                       H5S_ALL,             // identifier of memory dataspace
                       H5S_ALL,             // file space identifier
                       H5P_DEFAULT,         // data transfer property list
                       data2save);          // pointer to data array
    if (status < 0) printf( "ERROR: could not write dataset 'i'\n" );

    status       = H5Dclose(dataset_id_i);
    if (status < 0) printf( "ERROR: could not close dataset 'i'\n" );
    status       = H5Sclose(dataspace_id_i);
    if (status < 0) printf( "ERROR: could not close dataspace for 'i' dataset\n" );


    // store position
    dims[0] = 1;
    data2save[0] = (double)(detAnt_ypos);
    data2save[1] = (double)(detAnt_zpos);
    dataspace_id = H5Screate_simple( 1, dims, NULL); 
    // detAnt_ypos
    printf( "start to create dataset 'detAnt_ypos'\n" );
    dataset_id   = H5Dcreate( group_id__detAnt, "detAnt_ypos", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2save[0]);
    if (status < 0) printf( "ERROR: could not write dataset 'detAnt_ypos'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'detAnt_ypos'\n" );
    // detAnt_zpos
    printf( "start to create dataset 'detAnt_zpos'\n" );
    dataset_id   = H5Dcreate( group_id__detAnt, "detAnt_zpos", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2save[1]);
    if (status < 0) printf( "ERROR: could not write dataset 'detAnt_zpos'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'detAnt_zpos'\n" );
    status       = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for detAnt-position datasets\n" );

    // store sum_ExEx
    dims[0] = N_x/2;
    // since all following arrays have same dimension, dataspace_id needs to be created only once
    // and not closed with H5Sclose(dataspace_id) after each dataset
    dataspace_id = H5Screate_simple( 1, dims, NULL);
    // prepare array to be saved
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][0];
    printf( "start to create dataset 'E_x*E_x'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E_x*E_x", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E_x*E_x", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E_x*E_x'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E_x*E_x'\n" );

    // store sum_EyEy 
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][1];
    printf( "start to create dataset 'E_y*E_y'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E_y*E_y", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E_y*E_y", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E_y*E_y'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E_y*E_y'\n" );

    // store sum_EzEz
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][2];
    printf( "start to create dataset 'E_z*E_z'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E_z*E_z", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E_z*E_z", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E_z*E_z'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E_z*E_z'\n" );

    // store ErEr
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][3];
    printf( "start to create dataset 'E_r*E_r'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E_r*E_r", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E_r*E_r", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E_r*E_r'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E_r*E_r'\n" );

    // store E_th*E_th
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][4];
    printf( "start to create dataset 'E_th*E_th'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E_th*E_th", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E_th*E_th", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E_th*E_th'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E_th*E_th'\n" );
    
    // store sum_EE
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][5];
    printf( "start to create dataset 'EE'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "EE", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "EE", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'EE'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'EE'\n" );

    // store rmsE
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][6];
    printf( "start to create dataset 'rms_E'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "rms_E", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "rms_E", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'rmsE'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'rmsE'\n" );

    // store B_x
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][7];
    printf( "start to create dataset 'B_x*B_x'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B_x*B_x", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B_x*B_x", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B_x*B_x'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B_x*B_x'\n" );

    // store B_y
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][8];
    printf( "start to create dataset 'B_y*B_y'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B_y*B_y", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B_y*B_y", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B_y*B_y'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B_y*B_y'\n" );

    // store B_z
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][9];
    printf( "start to create dataset 'B_z*B_z'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B_z*B_z", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B_z*B_z", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B_z*B_z'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B_z*B_z'\n" );

    // store B_r
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][10];
    printf( "start to create dataset 'B_r*B_r'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B_r*B_r", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B_r*B_r", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B_r*B_r'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B_r*B_r'\n" );

    status       = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for datasets of E-fields\n" );

    // store B_th
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][11];
    printf( "start to create dataset 'B_th*B_th'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B_th*B_th", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B_th*B_th", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B_th*B_th'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B_th*B_th'\n" );

    // store BB
    set2zero_1D( N_x/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 )
        data2save[ii/2] = detAnt_fields[ii/2][12];
    printf( "start to create dataset 'BB'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "BB", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "BB", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'BB'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'BB'\n" );

    status       = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for datasets of E-fields\n" );
    
    // terminate access and free ressources/identifiers
    if (filter_avail) {
        status = H5Pclose( dcpl );
        if (status < 0) printf( "ERROR: could not close filter\n" );
    }
    status = H5Gclose( group_id__detAnt );
    if (status < 0) printf( "ERROR: could not close group detAnt\n" );
    // file 
    status = H5Fclose(file_id);
    if (status < 0) printf( "ERROR: could not close group file '%s'\n", filename );

    return EXIT_SUCCESS;
}//#}}}
//#endif

// Save 2D data sets
int detAnt2D_write2hdf5( int N_x, int N_y,
                         char filename[], char detAnt_groupName[], 
                         size_t detAnt_zpos,
                         double ***detAnt_fields ){
    //#{{{

    // hdf related variables
    hid_t       file_id, dataset_id, dataspace_id,      // object identifiers
                group_id__detAnt,
                dataspace_id_i, 
                dataset_id_i;
    hsize_t     dims[2];                                // size used for dimensions
    herr_t      status;                                 // function return value

    // hdf5 related variables for applying shuffle and gzip filter
    hid_t           dcpl;
    hsize_t         chunk[2];
    unsigned int    filter_info;
    int             filter_avail;

    // required for check if hdf5-file already exists
    struct      stat st;
    int         ii, jj;
    double       data2save[N_x/2][N_y/2];

    dims[0]     = N_x/2;
    dims[1]     = N_y/2;
    chunk[0]    = N_x/2;
    chunk[1]    = N_y/2;

    // assume as default setting that filters are available
    filter_avail = 1;

    //data2save = dvector( 0, n_elem/2 );
    set2zero_2D( N_x/2, N_y/2, data2save );
    printf("set2zero works.\n");

    printf( "Will write data for detector antenna position z=%05ld  into file %s\n", 
            detAnt_zpos, filename );

    // check if specified hdf5 file already exists
    // if not, create new one; if yes, open and add dataset to it
    if ( stat( filename, &st )==0 ) {
        // open file for read + write access
        file_id = H5Fopen( filename,            // filename
                           H5F_ACC_RDWR,        // allow read & write access (_RDONLY for read only)
                           H5P_DEFAULT);        // file access property list (default one)
    } else {
        // create a new file using default properties.
        file_id = H5Fcreate( filename,          // filename
                             H5F_ACC_TRUNC,     // how file should be created (removes existing file)
                             H5P_DEFAULT,       // file creating property list
                             H5P_DEFAULT);      // file access property list
    }

    // create group for different data to be stored
    group_id__detAnt = H5Gcreate2( file_id,
                                   detAnt_groupName,
                                   H5P_DEFAULT,
                                   H5P_DEFAULT,
                                   H5P_DEFAULT);

    // check if filters for shuffle and gzip exists, if yes, apply them
    // (check is done, because the filters are optional parts of hdf5 library)
    // check if gzip-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_DEFLATE )) ) {
        printf( "WARNING: gzip filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_DEFLATE, &filter_info );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: gzip filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
    }
    // check if shuffle-filter is available
    if ( !(H5Zfilter_avail( H5Z_FILTER_SHUFFLE )) ) {
        printf( "WARNING: shuffle filter not available (for hdf5)\n" );
        filter_avail = 0;
    } else {
        status = H5Zget_filter_info( H5Z_FILTER_SHUFFLE, &filter_info );
        if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
             !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
            printf( "WARNING: shuffle filter not available for encoding and decoding (for hdf5)\n" );
            filter_avail = 0;
        }
        if (status < 0) printf( "ERROR: could not get hdf5 filter info\n" );
    }

    // apply shuffle and gzip filters, if available
    if (filter_avail) {
        // create dataset creation property list
        dcpl = H5Pcreate( H5P_DATASET_CREATE );
        // add shuffle filter and gzip compression filter
        // note that the order of filter is significant: first shuffle!
        // order of filters applied correspond to order in which they are invoked when writin gdata
        status = H5Pset_shuffle( dcpl );
        if (status < 0) printf( "ERROR: could not apply shuffle filter\n" );
        status = H5Pset_deflate( dcpl, 9 );
        if (status < 0) printf( "ERROR: could not apply gzip filter\n" );
        status = H5Pset_chunk(dcpl, 2, chunk );
        if (status < 0) printf( "ERROR: could not set size of chunk\n" );
    } 

    // store spatial coordinate
    // prepare array
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ) {
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ) {
            data2save[ii/2][jj/2] = (double)(ii/2) ;
        }
    }
    // create data space
    dataspace_id_i = H5Screate_simple( 2,       // rank of array (number of dimensions of dataspace)
                                       dims,    // array of the size of each dimension
                                       NULL);   // allow stretching of data space (NULL=no)
//    printf( "dataspace_id_i=%d\n", dataspace_id_i);
    // create new dataset and links it to location in file
    printf( "start to create dataset 'j'\n" );
    if (filter_avail) {
        dataset_id_i = H5Dcreate( group_id__detAnt,     // file identifier (or group identifier)
                                  "i",                  // name of dataset (relative to group specified, if speficied)
                                  H5T_NATIVE_DOUBLE,    // datatype to use when creating dataset
                                  dataspace_id_i,       // dataspace identifier
                                  H5P_DEFAULT,          // link creation property list
                                  dcpl,                 // dataset creation property list
                                  H5P_DEFAULT);         // dataset access property list
    } else {
        dataset_id_i = H5Dcreate( group_id__detAnt,     // file identifier (or group identifier)
                                  "i",                  // name of dataset (relative to group specified, if speficied)
                                  H5T_NATIVE_DOUBLE,    // datatype to use when creating dataset
                                  dataspace_id_i,       // dataspace identifier
                                  H5P_DEFAULT,
                                  H5P_DEFAULT,
                                  H5P_DEFAULT);
    }
    // write the dataset
    status = H5Dwrite( dataset_id_i,        // dataset identifier
                       H5T_NATIVE_DOUBLE,   // informs hdf about format of data in memory of computer
                       H5S_ALL,             // identifier of memory dataspace
                       H5S_ALL,             // file space identifier
                       H5P_DEFAULT,         // data transfer property list
                       data2save);          // pointer to data array
    if (status < 0) printf( "ERROR: could not write dataset 'i'\n" );
    status       = H5Dclose(dataset_id_i);
    if (status < 0) printf( "ERROR: could not close dataset 'i'\n" );
    status       = H5Sclose(dataspace_id_i);
    if (status < 0) printf( "ERROR: could not close dataspace for 'i' dataset\n" );


    // store position
    dims[0] = 1;
    data2save[0][0] = (double)(detAnt_zpos);
    dataspace_id = H5Screate_simple( 2, dims, NULL); 
    // detAnt_zpos
    printf( "start to create dataset 'detAnt_zpos'\n" );
    dataset_id   = H5Dcreate( group_id__detAnt, "detAnt_zpos", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2save[0]);
    if (status < 0) printf( "ERROR: could not write dataset 'detAnt_zpos'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'detAnt_zpos'\n" );
    status       = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for detAnt-position datasets\n" );

    // store Poynting_Vector
    dims[0] = N_x/2;
    dims[1] = N_y/2;
    // since all following arrays have same dimension, dataspace_id needs to be created only once
    // and not closed with H5Sclose(dataspace_id) after each dataset
    dataspace_id = H5Screate_simple( 2, dims, NULL);
    // prepare array to be saved
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][0];
        }
    }
    
    printf( "start to create dataset 'Poynting_Vector'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Poynting_Vector", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Poynting_Vector", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Poynting_Vector'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Poynting_Vector'\n" );

    // store Wave_Polarization 
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][1];
        }
    }
    printf( "start to create dataset 'Wave_Polarization'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Wave_Polarization", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Wave_Polarization", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Wave_Polarization'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Wave_Polarization'\n" );

    // store Er*Er
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][2];
        }
    }
    printf( "start to create dataset 'Er*Er'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Er*Er", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Er*Er", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Er*Er'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Er*Er'\n" );

    // store Eth*Eth
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][3];
        }
    }
    printf( "start to create dataset 'Eth*Eth'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Eth*Eth", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Eth*Eth", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Eth*Eth'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Eth*Eth'\n" );

    // store Ez*Ez
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][4];
        }
    }
    printf( "start to create dataset 'Ez*Ez'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Ez*Ez", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Ez*Ez", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Ez*Ez'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Ez*Ez'\n" );
    
    // store sum_EE
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][5];
        }
    }
    printf( "start to create dataset 'E*E'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "E*E", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "E*E", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'E*E'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'E*E'\n" );

    // store BrBr component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][6];
        }
    }
    printf( "start to create dataset 'BrBr'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "BrBr", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "BrBr", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'BrBr'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'BrBr'\n" );

    // store Bth*Bth component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][7];
        }
    }
    printf( "start to create dataset 'Bth*Bth'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Bth*Bth", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Bth*Bth", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Bth*Bth'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Bth*Bth'\n" );

    // store Bz*Bz component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][8];
        }
    }
    printf( "start to create dataset 'Bz*Bz'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Bz*Bz", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Bz*Bz", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Bz*Bz'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Bz*Bz'\n" );

    // store Abs(B) component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][9];
        }
    }
    printf( "start to create dataset 'B*B'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "B*B", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "B*B", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'B*B'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'B*B'\n" );

    // store Power_proyection component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][10];
        }
    }
    printf( "start to create dataset 'Power_proyection'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Power_proyection", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Power_proyection", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Power_proyection'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Power_proyection'\n" );

    // store Total energy component
    set2zero_2D( N_x/2, N_y/2, data2save );
    for ( ii=2 ; ii<=N_x-2 ; ii+=2 ){
        for ( jj=2 ; jj<=N_y-2 ; jj+=2 ){
            data2save[ii/2][jj/2] = detAnt_fields[ii/2][jj/2][11];
        }
    }
    printf( "start to create dataset 'Wave_energy'\n" );
    if (filter_avail)
        dataset_id   = H5Dcreate( group_id__detAnt, "Wave_energy", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, dcpl, H5P_DEFAULT);  
    else
        dataset_id   = H5Dcreate( group_id__detAnt, "Wave_energy", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);  
    status       = H5Dwrite( dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data2save);
    if (status < 0) printf( "ERROR: could not write dataset 'Wave_energy'\n" );
    status       = H5Dclose(dataset_id);
    if (status < 0) printf( "ERROR: could not close dataset 'Wave_energy'\n" );

    status       = H5Sclose(dataspace_id);
    if (status < 0) printf( "ERROR: could not close dataspace for datasets of Wave-fields\n" );
    
    // terminate access and free ressources/identifiers
    if (filter_avail) {
        status = H5Pclose( dcpl );
        if (status < 0) printf( "ERROR: could not close filter\n" );
    }
    status = H5Gclose( group_id__detAnt );
    if (status < 0) printf( "ERROR: could not close group detAnt\n" );
    // file 
    status = H5Fclose(file_id);
    if (status < 0) printf( "ERROR: could not close group file '%s'\n", filename );

    return EXIT_SUCCESS;
}//#}}}
//#endif

