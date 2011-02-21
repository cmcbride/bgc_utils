#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <string.h>
#include <libgen.h>

/* this reads a BGC file, and copies it to test the writing functions */
#include "bgc_read_utils.c"
#include "bgc_write_utils.c"

void
copy_bgc( char *infile, char *outfile )
{
    FILE *fpin, *fpout;

    int i;

    OUTPUT_HEADER hdr;

    void *pdata;

    int *nParticlesPerGroup;

    size_t ssize;

    printf( "Copying '%s' -> '%s'\n", infile, outfile );
    fflush( stdout );
    fpin = fopen( infile, "r" );
    fpout = fopen( outfile, "w" );
    assert( fpin != NULL );
    assert( fpout != NULL );

    BGC_VERBOSE = 0;

    printf( "Reading header information:\n" );
    fflush( stdout );
    /* all the header information */
    bgc_read_header( fpin, &hdr );
    bgc_write_header( fpout, hdr );

    printf( "Reading group list...\n" );
    nParticlesPerGroup = bgc_read_grouplist( fpin, hdr );
    bgc_write_grouplist( fpout, hdr.ngroups, nParticlesPerGroup );

    ssize = bgc_sizeof_pdata( hdr.format );
    printf( "Reading particle data by groups ...\n" );
    printf( "  allocating %g MB for particle store\n",
            ( double )( hdr.max_npart * ssize ) / ( 1024.0 * 1024.0 ) );
    fflush( stdout );

    // allocate maximum size of one group
    pdata = malloc( hdr.max_npart * ssize );
    assert( pdata != NULL );

    fflush( stdout );
    // read in group chunks
    for( i = 0; i < hdr.ngroups; i++ ) {
        int npart = nParticlesPerGroup[i];

        bgc_read_part_into( fpin, npart, hdr.format, pdata );
        bgc_write_pdata( fpout, npart, hdr.format, pdata );
    }
    puts( "Finished reading all groups!\n" );

    if( ( fread( &i, sizeof( int ), 1, fpin ) == 0 ) && feof( fpin ) ) {
        printf( "OK.  Filesize check passed: no unread data remaining.\n" );
    } else {
        printf
            ( "WARNING: something is wrong, there seems to be more data at the end of the file\n" );
    }

    fclose( fpin );
    fclose( fpout );
}

int
main( int argc, char **argv )
{
    size_t len, add;

    char *infile, *outfile;

    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s BGC_FILE\n", argv[0] );
        exit( EXIT_FAILURE );
    }

    infile = argv[1];

    len = strlen( infile );
    add = 10;
    outfile = calloc( len + add, sizeof( char * ) );

    strncat( outfile, basename( infile ), len );
    strncat( outfile, ".copy", add );

    copy_bgc( infile, outfile );

    return ( EXIT_SUCCESS );
}
