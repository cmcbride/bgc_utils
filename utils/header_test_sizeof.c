#include <stdio.h>
#include "../binary_output.h"

int
main(  )
{
    printf( "Testing various type sizes:\n" );
    printf( "  int:           %zu bytes\n", sizeof( int ) );
    printf( "  unsigned int:  %zu bytes\n", sizeof( unsigned int ) );
    printf( "  double:        %zu bytes\n", sizeof( double ) );
    printf( "  char*:         %zu bytes\n", sizeof( char * ) );
    printf( "  output header: %zu bytes\n", sizeof( OUTPUT_HEADER ) );

    if( sizeof( OUTPUT_HEADER ) != OUTPUT_HEADER_SIZE ) {
        printf( "\n --> WARNING: output header is *NOT* %d bytes!!\n", OUTPUT_HEADER_SIZE );
    }

    return 0;
}
