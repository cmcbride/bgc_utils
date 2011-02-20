#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/* idea is to compare two GRP files (tipsy array format) in a line-by-line fashion */

#define MAX_NGROUPS 10000000
static int grp_map[MAX_NGROUPS];

#define STATS_FRAC 0.1

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

inline int
same_group( const int id1, const int id2 )
{
    int gid;

    /* unbound only matches unbound */
    if( id1 == 0 )
        return ( id2 == 0 );

    /* both should now be bound,  */
    if( id2 == 0 )
        return FALSE;

    assert( id1 < MAX_NGROUPS );
    gid = grp_map[id1];

    if( gid == 0 ) {
        grp_map[id1] = id2;
        gid = id2;
    }

    return ( gid == id2 );
}



#define FLOAT_EPSILON 1e-5
int
eql_float( const float a, const float b )
{
    double diff = fabs( ( double )a - ( double )b );

    if( diff <= FLOAT_EPSILON )
        return TRUE;
    else
        return FALSE;
}

int
main( int argc, char **argv )
{
    FILE *fp1, *fp2;

    char *file1, *file2;

    unsigned int i, npart, tmp, miss = 0;

    int stats_on_n;

    if( argc == 3 ) {
        file1 = argv[1];
        file2 = argv[2];
    } else {
        printf( "Usage: \n" );
        printf( "  %s file1.grp file2.grp\n", argv[0] );
        exit( EXIT_FAILURE );
    }

    printf( "Reading GRP file1: %s\n", file1 );
    fflush( stdout );
    fp1 = fopen( file1, "r" );
    assert( fp1 != 0 );

    printf( "Reading GRP file2: %s\n", file2 );
    fflush( stdout );
    fp2 = fopen( file2, "r" );
    assert( fp2 != 0 );

    fscanf( fp1, "%u", &npart );
    fscanf( fp2, "%u", &tmp );
    if( npart == tmp ) {
        printf( "  there are %u particles in these GRP files\n", npart );
    } else {
        fprintf( stderr, "ERROR: Different number of particles in the GRP files!\n" );
        fprintf( stderr, " file1: %u\n", npart );
        fprintf( stderr, " file2: %u\n", tmp );
        exit( EXIT_FAILURE );
    }

    printf( "  processing ... \n" );
    fflush( stdout );

    stats_on_n = floor( STATS_FRAC * npart );

    for( i = 0; i < npart; i++ ) {
        int gid1, gid2;

        fscanf( fp1, "%d", &gid1 );
        fscanf( fp2, "%d", &gid2 );
        if( i % stats_on_n == 0 ) {
            printf( "  ... completed %5.1f%% (%u mismatches so far)\n",
                    100 * ( double )i / ( double )npart, miss );
            fflush( stdout );
        }

        if( same_group( gid1, gid2 ) )
            continue;

        miss++;
    }

    fclose( fp1 );
    fclose( fp2 );

    printf( "\nRESULTS:\n" );
    printf( "  particle ID mismatches: %u\n", miss );
    puts( "" );

    if( miss ) {
        puts( "ERROR: unsucessful match!" );
        return ( EXIT_FAILURE );
    } else {
        puts( "SUCCESS: all particles match!" );
        return ( EXIT_SUCCESS );
    }

}
