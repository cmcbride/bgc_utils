
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef BINARY_OUTPUT_INCLUDED
#include "../binary_output.h"
#endif


static int BGC_VERBOSE = 0;

void
bgc_read_header( FILE * fp, OUTPUT_HEADER * hdr )
{
    int pad;

    assert( fp != 0 );

    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == 1024 );
    fread( hdr, sizeof( OUTPUT_HEADER ), 1, fp );
    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == 1024 );

    if( BGC_VERBOSE ) {
        printf( "READING HEADER INFORMATION:\n" );
        printf( "  total_files = %d\n", hdr->num_files );
        printf( "  ngroups = %d\n", hdr->ngroups );
        printf( "  starting at gid = %d\n", hdr->first_group_id );
        printf( "  nparticles = %d\n", hdr->npart );
        fflush( stdout );
    }
}

int *
bgc_read_grouplist( FILE * fp, const OUTPUT_HEADER hdr )
{
    int i, pad;

    int *nParticlesPerGroup;

    nParticlesPerGroup = calloc( hdr.ngroups + 1, sizeof( int ) );
    assert( nParticlesPerGroup != NULL );

    fread( &pad, sizeof( int ), 1, fp );
    for( i = 0; i < hdr.ngroups; i++ ) {
        int tmp;

        fread( &tmp, sizeof( int ), 1, fp );
        nParticlesPerGroup[i] = tmp;
        if( BGC_VERBOSE )
            printf( " grp %4d: %5d\n", ( i + hdr.first_group_id ), tmp );
    }
    fread( &pad, sizeof( int ), 1, fp );

    return nParticlesPerGroup;
}

/* Read particle data for one group. One must cast the result appropriately */
void *
bgc_read_particles( FILE * fp, const unsigned int npart, const int pdata_format )
{
    int pad;

    void *pd;

    size_t res;

    size_t size = bgc_sizeof_pdata( pdata_format );

    pd = malloc( npart * size );
    assert( pd != NULL );

    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == npart * size );
    res = fread( pd, size, npart, fp );
    assert( res == npart );
    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == npart * size );

    return ( void * )pd;
}

/* Read particle data for one group. One must cast the result appropriately */
void
bgc_read_part_into( FILE * fp, const unsigned int npart, const int pdata_format, void *pdata )
{
    int pad;

    size_t res;

    size_t size = bgc_sizeof_pdata( pdata_format );

    assert( pdata != NULL );

    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == npart * size );
    res = fread( pdata, size, npart, fp );
    assert( res == npart );
    fread( &pad, sizeof( int ), 1, fp );
    assert( pad == npart * size );

    return;
}

void
print_pdata_format( FILE * fp, const int pdata_format )
{
    switch ( pdata_format ) {
    case PDATA_FORMAT_ID:
        fprintf( fp, "ID" );
        break;
    case PDATA_FORMAT_IDBE:
        fprintf( fp, "IDBE" );
        break;
    case PDATA_FORMAT_POS:
        fprintf( fp, "POS" );
        break;
    case PDATA_FORMAT_POSBE:
        fprintf( fp, "POSBE" );
        break;
    case PDATA_FORMAT_PV:
        fprintf( fp, "PV" );
        break;
    case PDATA_FORMAT_PVBE:
        fprintf( fp, "PVBE" );
        break;
    case PDATA_FORMAT_PVM:
        fprintf( fp, "PVM" );
        break;
    case PDATA_FORMAT_PVMBE:
        fprintf( fp, "PVMBE" );
        break;
    case PDATA_FORMAT_GPVM:
        fprintf( fp, "KITCHEN SINK (GPVM)" );
        break;
    default:
        fprintf( stderr, "ERROR: unknown particle data format!  (format = %d)\n", pdata_format );
    }
}
