#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bgc_read_utils.c"


/* this utility just does a quick read and dump of the BGC file header * */

int
main( int argc, char **argv )
{
    OUTPUT_HEADER hdr;

    FILE *fp;

    char *filename;

    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s BGC_FILE\n", argv[0] );
        exit( EXIT_FAILURE );
    }

    filename = argv[1];

    printf( "Reading '%s' ...\n", filename );
    fflush( stdout );
    fp = fopen( filename, "r" );
    assert( fp != 0 );

    BGC_VERBOSE = 0;

    printf( "Reading header information:\n" );
    fflush( stdout );
    /* all the header information */
    bgc_read_header( fp, &hdr );
    fclose( fp );

    printf( "  this is file_id = %d (data distributed into %d file%s)\n", hdr.file_id,
            hdr.num_files, hdr.num_files > 1 ? "s" : "" );
    printf( "  it has %d groups which contain a total of %u particles\n", hdr.ngroups, hdr.npart );
    printf( "  there are a total of %d groups in all BGC files\n", hdr.ngroups_total );
    printf( "  the first group_id = %d\n", hdr.first_group_id );
    printf( "  valid particle ids: %s\n", hdr.valid_part_ids ? "yes" : "no" );
    printf( "  the largest group contains %u particles\n", hdr.max_npart );
    printf( "  part_mass = %g\n", hdr.part_mass );
    printf( "  BGC_FORMAT: " );
    print_pdata_format( stdout, hdr.format );
    printf( "\n" );
    printf( "  min_group_part:  %d\n", hdr.min_group_part );
    printf( "  linking length = %g\n", hdr.linkinglength );
    printf( "  redshift = %f\n", hdr.redshift );
    printf( "  boxsize = %g\n", hdr.BoxSize );
    fflush( stdout );

    return ( EXIT_SUCCESS );
}
