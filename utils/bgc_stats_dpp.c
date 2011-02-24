#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#define VERBOSE 0
#include "bgc_read_utils.c"

/* We calculate a proxy for the gravitational potential energy to find the position of the halo 
 * center.  We identify the deepest potential particle (DPP), which is most bound neglecting 
 * any velocity info.  We assume cosmology and scale factor is fixed within the group particles, 
 * which is exactly true excepting lightcone outputs.  The "proxy" means we do not scale by mass, 
 * nor cosmology, so we never output the gravitational potential we calculate.  The velocity 
 * statistics use the COM velocity and its dispersion.
 **/

int
calc_stats_dpp( FILE * fp, const OUTPUT_HEADER hdr, const double eps )
{
    int i, k, n;

    int *nParticlesPerGroup;

    PARTICLE_DATA_PV *pdata;

    double plength_check = hdr.BoxSize / 5.0;

    double neg_plength_check = -1.0 * plength_check;

    // ensure that the box-wrap around check is sufficiently large (> 10 Mpc)
    assert( plength_check > 10.0 );

    /* allocate array of structures based on the biggest halo, which means only once per file */
    pdata = calloc( hdr.max_npart, bgc_sizeof_pdata( hdr.format ) );
    assert( pdata != NULL );

    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

    // read in by group chunks and process
    for( i = 0; i < hdr.ngroups; i++ ) {
        int gid = i + hdr.first_group_id;

        int npart = nParticlesPerGroup[i];

        int flag_periodic = 0;

        double xsel[3], vm[3], vd[3];

        double vdisp;

        double gmass = hdr.part_mass * ( double )npart;

        double r2 = 0.0, pot_max = 0.0;

        float *x1, *x2;

        PARTICLE_DATA_PV pd;

        bgc_read_part_into( fp, npart, hdr.format, pdata );

        for( k = 0; k < 3; k++ ) {
            xsel[k] = vm[k] = vd[k] = 0.0;
        }

        for( n = 0; n < npart; n++ ) {
            int m;

            double del = 0.0, pot = 0.0;

            pd = pdata[n];
            x1 = pd.pos;

            // second loop over particles for potential calculation
            for( m = 0; m < npart; m++ ) {
                if( n == m ) {
                    continue;
                }
                x2 = pdata[m].pos;
                r2 = 0.0;
                for( k = 0; k < 3; k++ ) {
                    del = x1[k] - x2[k];

                    /* now we have to check for periodic boundary wraparound */
                    if( del > plength_check ) {
                        // del = (x1[k] - hdr.BoxSize) - x2[k];
                        del -= hdr.BoxSize;
                        flag_periodic = 1;
                    } else if( del < neg_plength_check ) {
                        // del = (x1[k] + hdr.BoxSize) - x2[k];
                        del += hdr.BoxSize;
                        flag_periodic = 1;
                    }
                    // assert( abs(del) < plength_check );
                    r2 += del * del;
                }
                pot += 1.0 / sqrt( r2 + eps * eps );
            }


            {                   // check potential energy, update as necessary
                int update_pot = FALSE;

                if( n == 0 ) {
                    update_pot = TRUE;
                } else if( pot > pot_max ) {
                    update_pot = TRUE;
                }

                if( update_pot ) {
                    pot_max = pot;
                    for( k = 0; k < 3; k++ ) {
                        xsel[k] = pd.pos[k];
                    }
                }
            }

            /* one pass stable algorithm by Knuth and/or Welford
             * http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance */
            for( k = 0; k < 3; k++ ) {
                del = pd.vel[k] - vm[k];
                vm[k] += del / ( n + 1 );

                vd[k] += del * ( pd.vel[k] - vm[k] );   // this is intended to use the updated mean value
            }
        }

        {                       /* calculate velocity dispersion */
            double var = 0;

            for( k = 0; k < 3; k++ ) {
                var += ( vd[k] / npart );
            }
            vdisp = sqrt( var );
        }

        fprintf( stdout, "%8d %10d %15.6f", gid, npart, gmass );
        for( k = 0; k < 3; k++ ) {
            fprintf( stdout, " %12.4f", xsel[k] );
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
group_stats( char *bgc_file, const double eps )
{
    FILE *fp;

    OUTPUT_HEADER hdr;

    int ngroups_read = 0;

    fprintf( stderr, "Reading BGC file: %s\n", bgc_file );
    fflush( stderr );
    fp = fopen( bgc_file, "r" );
    if( fp == NULL ) {
        fprintf( stderr, "ERROR: problem opening file '%s'\n", bgc_file );
        assert( fp != NULL );
    }

    bgc_read_header( fp, &hdr );

    if( PDATA_FORMAT_PV == hdr.format ) {
        ngroups_read = calc_stats_dpp( fp, hdr, eps );
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

    double eps;

    if( argc < 3 ) {
        fprintf( stderr, "Usage: " );
        fprintf( stderr, "  %s GADGET_SOFTENING BGC_file[s] \n", argv[0] );
        exit( EXIT_FAILURE );
    }

    {                           /* convert 1st argument into double softening: eps */
        char **endptr;

        endptr = NULL;
        eps = strtod( argv[1], endptr );
    }

    fprintf( stderr, "Calculating DPP using plummer softened potential: eps = %g\n", eps );
    fprintf( stderr,
             "Output 11 columns:\n  gid(0) npart(1) group_mass(2) position(3,4,5)  velocity(6,7,8) vdisp(9) periodic_wrap?(10)\n" );

    /* loop over input files for processing */
    for( i = 2; i < argc; i++ ) {
        char *bgc_file = argv[i];

        group_stats( bgc_file, eps );
    }

    return ( EXIT_SUCCESS );
}
