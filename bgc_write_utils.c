
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bgc.h"

/* Easy unformatted FORTRAN reading, with some sanity checking. 
 * Usage like C fread. but returns size of main object read in or "0" on an error */
size_t
ftwrite( const void const *ptr, const size_t size, const size_t nitems, FILE * stream )
{
    int nbytes;

    size_t nitem1;

    size_t res;

    char *msg_init = "ftwrite error";

    nbytes = ( int )size *nitems;

    assert( nbytes > 0 );

    /* unformatted data FORMAT, typically 4-byte boundaries */
    res = fwrite( &nbytes, sizeof( int ), 1, stream );
    if( res != 1 ) {
        fprintf( stderr, "%s: file open? \n", msg_init );
        perror( msg_init );
        return ( res );
    }
    nitem1 = fwrite( ptr, size, nitems, stream );
    if( nitem1 != nitems ) {
        fprintf( stderr, "%s: %lu items requested, %lu items written. \n", msg_init, nitems,
                 nitem1 );
    }
    res = fwrite( &nbytes, sizeof( int ), 1, stream );
    if( res != 1 ) {
        fprintf( stderr, "%s: write error on second byte label\n", msg_init );
        perror( msg_init );
        return ( res );
    }

    return ( size_t ) nbytes;
}

void
bgc_write_header( FILE * fp, const OUTPUT_HEADER hdr )
{
    size_t res;

    /* sanity checks */
    assert( sizeof( OUTPUT_HEADER ) == OUTPUT_HEADER_SIZE );
    assert( fp != NULL );

    /* force BGC header to be at beginning of file! */
    rewind( fp );

    res = ftwrite( &hdr, sizeof( OUTPUT_HEADER ), 1, fp );

    assert( res == OUTPUT_HEADER_SIZE );
}

void
bgc_write_grouplist( FILE * fp, const int ngroups, const int const *nParticlesPerGroup )
{
    size_t res;

    assert( nParticlesPerGroup != NULL );

    res = ftwrite( nParticlesPerGroup, sizeof( int ), ngroups, fp );
    assert( res == sizeof( int ) * ngroups );
}

void
bgc_write_pdata( FILE * fp, const unsigned int npart, const int pdata_format,
                 const void const *pdata )
{
    size_t size, res;

    assert( pdata != NULL );

    size = bgc_sizeof_pdata( pdata_format );

    res = ftwrite( pdata, size, npart, fp );

    assert( res == size * npart );

}
