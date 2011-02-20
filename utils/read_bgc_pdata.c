#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bgc_read_utils.c"


/* this utility just reads a SINGLE BGC file to verify the size matches: 
 *  - it uses header info to check how many files
 *  - it allocates a single chunk to hold largest group in the file
 *  - it reads all groups to ensure the file isn't truncated 
 *  - it does nothing with this particle data
 * */


void
read_bgc_pdata( char *filename )
{
    FILE *fp;

    int i;

    OUTPUT_HEADER hdr;

    void *pdata;

    int *nParticlesPerGroup;

    size_t ssize;

    printf( "Reading '%s' ...\n", filename );
    fflush( stdout );
    fp = fopen( filename, "r" );
    assert( fp != 0 );

    BGC_VERBOSE = 0;

    printf( "Reading header information:\n" );
    fflush( stdout );
    /* all the header information */
    bgc_read_header( fp, &hdr );

    printf( "  this is file_id = %d (data distributed into %d file%s)\n", hdr.file_id,
            hdr.num_files, hdr.num_files > 1 ? "s" : "" );
    printf( "  it has %d groups which contain a total of %u particles\n", hdr.ngroups, hdr.npart );
    printf( "  the first group_id = %d\n", hdr.first_group_id );
    printf( "  the largest group contains %u particles\n", hdr.max_npart );
    printf( "  part_mass = %g\n", hdr.part_mass );
    printf( "  BGC_FORMAT: " );
    print_pdata_format( stdout, hdr.format );
    printf( "\n" );
    fflush( stdout );

    printf( "Reading group list...\n" );
    nParticlesPerGroup = bgc_read_grouplist( fp, hdr );

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

        int gid = hdr.first_group_id + i;

        if( BGC_VERBOSE )
            printf( "Reading GID = %d (%d particles) ... ", gid, npart );
        bgc_read_part_into( fp, npart, hdr.format, pdata );
        if( BGC_VERBOSE ) {
            printf( "OK\n" );
            fflush( stdout );
        }
    }
    puts( "Finished reading all groups!\n" );

    if( ( fread( &i, sizeof( int ), 1, fp ) == 0 ) && feof( fp ) ) {
        printf( "OK.  Filesize check passed: no unread data remaining.\n" );
    } else {
        printf
            ( "WARNING: something is wrong, there seems to be more data at the end of the file\n" );
    }

    fclose( fp );
}

int
main( int argc, char **argv )
{
    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s BGC_FILE\n", argv[0] );
        exit( EXIT_FAILURE );
    }

    read_bgc_pdata( argv[1] );

    return ( EXIT_SUCCESS );
}
