#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define VERBOSE 0
#include "bgc_read_utils.c"

int *
read_grp( const char *filename, int *nelem )
{
    FILE *fp;

    int i;

    int *grp;

    fp = fopen( filename, "r" );
    assert( fp != 0 );

    fscanf( fp, "%d", nelem );

    grp = calloc( *nelem + 1, sizeof( int ) );
    assert( grp != NULL );

    grp[0] = 0;
    for( i = 1; i <= *nelem; i++ ) {
        fscanf( fp, "%d", &grp[i] );
    }
    fclose( fp );

    return grp;
}

float *
read_be( const char *filename, int *nelem )
{
    FILE *fp;

    int i;

    float *be;

    fp = fopen( filename, "r" );
    assert( fp != 0 );

    fscanf( fp, "%d", nelem );

    be = calloc( *nelem + 1, sizeof( float ) );
    assert( be != NULL );

    be[0] = 0;
    for( i = 1; i <= *nelem; i++ ) {
        fscanf( fp, "%f", &be[i] );
    }
    fclose( fp );

    return be;
}

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define FLOAT_EPSILON 1e-5
int
eql_float( const float a, const float b )
{
    double diff = fabs( ( double )a - ( double )b );

//     printf("eql_float: abs(%g - %g) = %g\n", a,b,diff);
    if( diff <= FLOAT_EPSILON )
        return TRUE;
    else
        return FALSE;
}

int
main( int argc, char **argv )
{
    FILE *fp;

    char *grp_file, *be_file, *bgc_file;

    int i, npart, max_gid;

    int miss_npart, miss_ngroup, miss_be;

    int *grp, *grpcount;

    float *be = NULL;;
    OUTPUT_HEADER hdr;

    int *nParticlesPerGroup;

    PARTICLE_DATA_IDBE *pdata;

    if( argc == 3 ) {
        grp_file = argv[1];
        be_file = NULL;
        bgc_file = argv[2];

    } else if( argc == 4 ) {
        grp_file = argv[1];
        be_file = argv[2];
        bgc_file = argv[3];
    } else {
        printf( "Usage: \n" );
        printf( "  %s GRP_file BGC_file          :: checks particle IDs in groups \n", argv[0] );
        printf( "  %s GRP_file BE_file  BGC_FILE :: checks both IDs and binding energy \n",
                argv[0] );
        exit( EXIT_FAILURE );
    }

    printf( "Reading GRP file: %s\n", grp_file );
    fflush( stdout );
    grp = read_grp( grp_file, &npart );
    printf( "  there are %d particles in this GRP file\n", npart );

    if( be_file ) {
        int be_npart;

        printf( "Reading BE file: %s\n", be_file );
        fflush( stdout );
        be = read_be( be_file, &be_npart );
        printf( "  there are %d particles in this BE file\n", be_npart );
        if( npart != be_npart ) {
            puts( "ERROR: GRP and BE files don't have the same number of particles" );
            exit( EXIT_FAILURE );
        }
    }

    printf( "Opening BGC file: %s\n", bgc_file );
    fflush( stdout );
    fp = fopen( bgc_file, "r" );
    assert( fp != NULL );

    printf( "Reading BGC header...\n" );
    bgc_read_header( fp, &hdr );
    printf( "  there are %d particles in this BGC file\n", hdr.npart );

    /* assume BGC corresponds to GRP file, so constants are consistent - but check it */
    if( npart != hdr.npart_orig ) {
        printf
            ( "ERROR: GRP and BGC files don't seem to match (original particle numbers are mismatched)\n" );
        exit( EXIT_FAILURE );
    }
    max_gid = hdr.ngroups_total;

    grpcount = calloc( max_gid + 1, sizeof( int ) );
    assert( grpcount != NULL );

    for( i = 1; i <= npart; i++ ) {
        int gid = grp[i];

        assert( gid <= max_gid );
        grpcount[gid] += 1;
    }

    printf( "Reading group list for %d groups\n", hdr.ngroups );
    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

    // read particles, FORMAT IS HARDCODED!
    size_t size = bgc_sizeof_pdata( PDATA_FORMAT_IDBE );

    pdata = malloc( size * hdr.max_npart );
    assert( pdata != NULL );

    miss_npart = miss_ngroup = miss_be = 0;

    printf( "\nVerifying groups from BGC file: GID %d to %d ... \n", hdr.first_group_id,
            hdr.first_group_id + hdr.ngroups - 1 );
    printf( "(this is verifying %d of %d total groups)\n", hdr.ngroups, hdr.ngroups_total );
    if( be_file ) {
        printf( "\nBinding Energy Comparison: difference accurate to less than %e\n",
                FLOAT_EPSILON );
    }

    for( i = 0; i < hdr.ngroups; i++ ) {        // read in group chunks and compare
        int k, grpgid;

        int npart = nParticlesPerGroup[i];

        int gid = i + hdr.first_group_id;

        assert( npart <= hdr.max_npart );
        bgc_read_part_into( fp, npart, PDATA_FORMAT_IDBE, pdata );

        if( grpcount[gid] != npart ) {
            // check to see if GRP and BGC disagree with the number of particles
            // if they weren't the same, the particle check might not be comprehensive, since we use 
            // the BGC catalog to loop over.
            miss_ngroup++;
        }

        for( k = 0; k < npart; k++ ) {
            int id = pdata[k].part_id;

            grpgid = grp[id];
            if( grpgid != gid ) {
                miss_npart++;
            }
            if( be_file ) {
                float part_be = be[id];

                float bgc_be = pdata[k].binding_energy;

                if( !eql_float( part_be, bgc_be ) ) {
                    miss_be++;
                }
            }

        }
    }

    fclose( fp );
    free( grp );
    free( grpcount );
    free( pdata );
    if( be_file ) {
        free( be );
    }

    printf( "\nRESULTS:\n" );
    printf( "  group count mismatches: %d\n", miss_ngroup );
    printf( "  particle ID mismatches: %d\n", miss_npart );
    if( be_file ) {
        printf( "  binding energy misses:  %d\n", miss_be );
    }

    if( miss_npart || miss_ngroup || miss_be ) {
        puts( "ERROR: unsucessful match!" );
        return ( EXIT_FAILURE );
    } else {
        puts( "SUCCESS: groups match!" );
        return ( EXIT_SUCCESS );
    }

}
