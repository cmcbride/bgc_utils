#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bgc_read_utils.c"


OUTPUT_HEADER hdr;

int *nParticlesPerGroup;

// WARNING: hard coded data format!
PARTICLE_DATA_IDBE **pmat;

void
read_binary_catalog( char *filename )
{
    FILE *fp;

    int i;

    fp = fopen( filename, "r" );
    assert( fp != 0 );

    BGC_VERBOSE = 1;

    bgc_read_header( fp, &hdr );

    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

    // read particles, FORMAT IS HARDCODED!
    pmat = malloc( hdr.ngroups * sizeof( PARTICLE_DATA_IDBE * ) );
    assert( pmat != NULL );
    pmat--;

    // read in group chunks, but store all of it for now
    puts( "Reading in particle data in group chunks.\n" );
    for( i = 0; i < hdr.ngroups; i++ ) {
        int npart = nParticlesPerGroup[i];

        printf( "grp %d .. \n", ( i + hdr.first_group_id ) );
        fflush( stdout );
        pmat[i] = ( PARTICLE_DATA_IDBE * ) bgc_read_particles( fp, npart, PDATA_FORMAT_IDBE );
    }
    fclose( fp );

    printf( "\n" );
    puts( "Finished reading all groups!\n" );
}

int
main( int argc, char **argv )
{
    if( argc != 2 ) {
        printf( "Usage: %s BINARY_CATALOG_FILE\n", argv[0] );
        exit( EXIT_FAILURE );
    }

    read_binary_catalog( argv[1] );


    return ( EXIT_SUCCESS );
}
