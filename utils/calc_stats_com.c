#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define VERBOSE 0
#include "../binary_output.h"
#include "bgc_read_utils.c"

int
calc_stats_com( FILE * fp, const OUTPUT_HEADER hdr )
{
    int i, k, n;

    int *nParticlesPerGroup;

    PARTICLE_DATA_PV *pdata;

    double plength_check = hdr.BoxSize / 5.0;

    /* allocate array of structures based on the biggest halo, which means only once per file */
    pdata = calloc( hdr.max_npart, bgc_sizeof_pdata( hdr.format ) );
    assert( pdata != NULL );

    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

    // read in by group chunks and process
    for( i = 0; i < hdr.ngroups; i++ ) {
        int gid = i + hdr.first_group_id;

        int npart = nParticlesPerGroup[i];

        int flag_periodic = 0;

        PARTICLE_DATA_PV pd;

        double xm[3], vm[3], vd[3], vdisp;

        double gmass = hdr.part_mass * ( double )npart;

        bgc_read_part_into( fp, npart, hdr.format, pdata );

        for( k = 0; k < 3; k++ ) {
            xm[k] = vm[k] = vd[k] = 0;
        }

        /* one pass stable algorithm by Knuth and/or Welford
         * http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance */
        for( n = 0; n < npart; n++ ) {
            double del;

            pd = pdata[n];
//             if(gid == 347252)
//                 fprintf(stderr, "%4d", n);
            for( k = 0; k < 3; k++ ) {
                del = pd.pos[k] - xm[k];

//                 if(gid == 347252)
//                     fprintf(stderr, " %17.8f", del);

                /* now we have to check for periodic boundary wraparound */
                if( n > 0 ) {
                    /* given our algorithm, we want to skip the first particle */
                    if( del > plength_check ) {
                        flag_periodic = 1;
                        del = ( pd.pos[k] - hdr.BoxSize ) - xm[k];
                    }
                    if( del < -1.0 * plength_check ) {
                        flag_periodic = 1;
                        del = ( pd.pos[k] + hdr.BoxSize ) - xm[k];
                    }
                }
                xm[k] += del / ( n + 1 );

                del = pd.vel[k] - vm[k];
                vm[k] += del / ( n + 1 );

                vd[k] += del * ( pd.vel[k] - vm[k] );   // this is intended to use the updated mean value
            }
//             if(gid == 347252) 
//                 fprintf(stderr, "\n");
        }

        /* check if our COM is beyond the boundary of the box, and fix it if necessary */
        if( flag_periodic ) {
            for( k = 0; k < 3; k++ ) {
                if( xm[k] < 0 )
                    xm[k] += hdr.BoxSize;

                if( xm[k] > hdr.BoxSize )
                    xm[k] -= hdr.BoxSize;
            }
        }
        {
            double var = 0;

            for( k = 0; k < 3; k++ ) {
                var += ( vd[k] / npart );
            }
            vdisp = sqrt( var );
        }

        fprintf( stdout, "%8d %10d %15.6f", gid, npart, gmass );
        for( k = 0; k < 3; k++ ) {
            fprintf( stdout, " %12.4f", xm[k] );
        }
        for( k = 0; k < 3; k++ ) {
            fprintf( stdout, " %12.4f", vm[k] );
        }
        fprintf( stdout, " %12.4f", vdisp );
        fprintf( stdout, "  %d\n", flag_periodic );
    }

    free( pdata );
    return hdr.ngroups;
}

int
group_stats( char *bgc_file )
{
    FILE *fp;

    OUTPUT_HEADER hdr;

    int ngroups_read = 0;

    fprintf( stderr, "Reading BGC file: %s\n", bgc_file );
//     fprintf(stdout, "# from BGC file: %s\n", bgc_file);
    fflush( stderr );
    fp = fopen( bgc_file, "r" );
    if( fp == NULL ) {
        fprintf( stderr, "ERROR: problem opening file '%s'\n", bgc_file );
        assert( fp != NULL );
    }

    bgc_read_header( fp, &hdr );

    if( PDATA_FORMAT_PV == hdr.format ) {
//         fprintf(stdout, "# 11 columns: gid(0) npart(1) group_mass(2) pos_com(3,4,5) vel_com(6,7,8) vdisp(9) periodic_needed(10)\n");
        ngroups_read = calc_stats_com( fp, hdr );
    } else if( PDATA_FORMAT_PVBE == hdr.format ) {
//         ngroups_read = calc_stats_mbp(fp, hdr);
        fprintf( stderr, "USING BINDING ENERGY IS NOT YET IMPLEMENTED!\n" );
        return ( EXIT_FAILURE );
    } else {
        fprintf( stderr, "ERROR: skipping '%s' -- PDATA_FORMAT not compatible (%d)\n", bgc_file,
                 hdr.format );
        return ( EXIT_FAILURE );
    }

    return ngroups_read;
}

int
main( int argc, char **argv )
{
    int i;

    if( argc < 2 ) {
        fprintf( stderr, "Usage: " );
        fprintf( stderr, "  %s BGC_file[s] \n", argv[0] );
        exit( EXIT_FAILURE );
    }

    /* loop over input files for processing */
    for( i = 1; i < argc; i++ ) {
        char *bgc_file = argv[i];

        group_stats( bgc_file );
    }

    return ( EXIT_SUCCESS );
}
