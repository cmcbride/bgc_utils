
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bgc.h"

static int BGC_VERBOSE = 0;     /* make this 1 to increase verbosity for debugging, etc. */

/* Easy unformatted FORTRAN reading, with some sanity checking. 
 * Usage like C fread. but returns size of main object read in or "0" on an error */
size_t
ftread( void *ptr, size_t size, size_t nitems, FILE * stream )
{
    int nbyte1, nbyte2;

    size_t nitem1;

    size_t res;

    char *msg_init = "ftread error";

    /* unformatted data FORMAT, typically 4-byte boundaries */
    res = fread( &nbyte1, sizeof( int ), 1, stream );
    if( res != 1 ) {
        fprintf( stderr, "%s: file empty? \n", msg_init );
        perror( msg_init );
        return ( res );
    }
    nitem1 = fread( ptr, size, nitems, stream );
    if( nitem1 != nitems ) {
        fprintf( stderr, "%s: %lu items expected, %lu items read. \n", msg_init, nitems, nitem1 );
    }
    res = fread( &nbyte2, sizeof( int ), 1, stream );
    if( res != 1 ) {
        fprintf( stderr, "%s: file too short? \n", msg_init );
        perror( msg_init );
        return ( res );
    }
    if( nbyte1 != nbyte2 ) {
        fprintf( stderr,
                 "%s: byte paddings do not match, nbyte1 = %i, nbyte2 = %i \n",
                 msg_init, nbyte1, nbyte2 );
        return ( 0 );
    }
    if( ( size_t ) nbyte1 != size * nitems ) {
        fprintf( stderr,
                 "%s: expected byte size does not match item*size nbyte = %i, size = %lu, nitems = %lu\n",
                 msg_init, nbyte1, size, nitems );
        return ( 0 );
    }

    return ( size_t ) nbyte1;
}

void
bgc_read_header( FILE * fp, OUTPUT_HEADER * hdr )
{
    int size;

    assert( fp != 0 );
    size = ( int )ftread( hdr, sizeof( OUTPUT_HEADER ), 1, fp );
    assert( size == OUTPUT_HEADER_SIZE );

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
    int i;

    int *nParticlesPerGroup;

    nParticlesPerGroup = calloc( hdr.ngroups + 1, sizeof( int ) );
    assert( nParticlesPerGroup != NULL );

    ftread( nParticlesPerGroup, sizeof( int ), hdr.ngroups, fp );

    if( BGC_VERBOSE )
        for( i = 0; i < hdr.ngroups; i++ ) {
            printf( " grp %4d: %5d\n", ( i + hdr.first_group_id ), nParticlesPerGroup[i] );
        }

    return nParticlesPerGroup;
}

/* Read particle data for one group. One must cast the result appropriately */
void *
bgc_read_particles( FILE * fp, const unsigned int npart, const int pdata_format )
{
    void *pd;

    size_t size = bgc_sizeof_pdata( pdata_format );

    pd = calloc( npart, size );
    assert( pd != NULL );

    ftread( pd, size, npart, fp );

    return ( void * )pd;
}

/* Read particle data for one group into *pdata.  Assumes memory is properly allocated to 
 * contain FULL data.  One must cast the result appropriately to use it */
void
bgc_read_part_into( FILE * fp, const unsigned int npart, const int pdata_format, void *pdata )
{
    size_t size = bgc_sizeof_pdata( pdata_format );

    assert( pdata != NULL );
    ftread( pdata, size, npart, fp );

    return;
}

/* skip over group without reading it */
int
bgc_skip_particles( FILE * fp, const unsigned int npart, const int pdata_format )
{
    size_t size = bgc_sizeof_pdata( pdata_format );

    long offset;

    offset = npart * size + 2 * sizeof( int );

    return fseek( fp, offset, SEEK_CUR );
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
