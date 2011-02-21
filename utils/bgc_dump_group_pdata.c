#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define VERBOSE 0
#include "bgc_read_utils.c"

int
find_and_print_group( FILE * fp, const OUTPUT_HEADER hdr, const int search_gid )
{
    int i, k, n;

    int *nParticlesPerGroup;

    PARTICLE_DATA_PV *pdata;

    /* allocate array of structures based on the biggest halo, which means only once per file */
    pdata = calloc( hdr.max_npart, bgc_sizeof_pdata( hdr.format ) );
    assert( pdata != NULL );

    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

    // read in by group chunks and process
    for( i = 0; i < hdr.ngroups; i++ ) {
        int gid = i + hdr.first_group_id;

        int npart = nParticlesPerGroup[i];

        PARTICLE_DATA_PV pd;

        bgc_read_part_into( fp, npart, hdr.format, pdata );

        if( gid != search_gid )
            continue;

        fprintf( stdout, "# part_id(0) pos(1,2,3)  vel(4,5,6)\n" );

        for( n = 0; n < npart; n++ ) {
            pd = pdata[n];
            fprintf( stdout, "%11d ", pd.part_id );
            for( k = 0; k < 3; k++ ) {
                fprintf( stdout, " %16.8f", pd.pos[k] );
            }
            for( k = 0; k < 3; k++ ) {
                fprintf( stdout, " %16.8f", pd.vel[k] );
            }
            fprintf( stdout, "\n" );
        }

    }

    free( pdata );
    return hdr.ngroups;
}

int
search_for_gid( char *bgc_file, const int gid )
{
    FILE *fp;

    OUTPUT_HEADER hdr;

    int ngroups_read = 0;

    fprintf( stderr, "Reading BGC file: %s\n", bgc_file );
    fprintf( stdout, "# from BGC file: %s\n", bgc_file );
    fflush( stderr );
    fp = fopen( bgc_file, "r" );
    if( fp == NULL ) {
        fprintf( stderr, "ERROR: problem opening file '%s'\n", bgc_file );
        assert( fp != NULL );
    }

    bgc_read_header( fp, &hdr );

    if( gid < hdr.first_group_id ) {
        fprintf( stderr, "skipping '%s': search for gid = %d comes before first group id (%d)\n",
                 bgc_file, gid, hdr.first_group_id );
        return ( ngroups_read );
    }

    if( gid > ( hdr.first_group_id + hdr.ngroups ) ) {
        fprintf( stderr, "skipping '%s': search for gid = %d comes after last group id (%d)\n",
                 bgc_file, gid, hdr.first_group_id + hdr.ngroups );
        return ( ngroups_read );
    }

    if( PDATA_FORMAT_PV == hdr.format ) {
        ngroups_read = find_and_print_group( fp, hdr, gid );
    } else {
        fprintf( stderr, "ERROR: skipping '%s' -- PDATA_FORMAT not compatible (%d)\n", bgc_file,
                 hdr.format );
        return ( ngroups_read );
    }

    return ngroups_read;
}

int
main( int argc, char **argv )
{
    int i;

    int gid;

    if( argc < 3 ) {
        printf( "Usage: " );
        printf( "  %s GID BGC_file[s] \n", argv[0] );
        exit( EXIT_FAILURE );
    }

    sscanf( argv[1], "%d", &gid );

    /* loop over input files for processing */
    for( i = 2; i < argc; i++ ) {
        char *bgc_file = argv[i];

        search_for_gid( bgc_file, gid );
    }

    return ( EXIT_SUCCESS );
}
