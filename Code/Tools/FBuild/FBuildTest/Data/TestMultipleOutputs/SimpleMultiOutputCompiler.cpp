//
// An simple compiler that takes an input filename, an output filename, and a list of extensions.
// It creates the output file as well as one additional file per extension.
// All extensions must be three characters.
//
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
		fprintf( stderr, "Bad Args!\n");
        return 1;
    }

    //const char* input = argv[1];
	char* output = argv[2];

	int outputLength = strlen( output );
	if( outputLength < 4 || output[outputLength - 4] != '.' )
	{
		fprintf( stderr, "Output must have a three character extension" );
		return 2;
	}
	
	// Output first file with default extension
	{
		FILE* f = fopen( output, "wb" );
		fwrite( (char*)&argc, sizeof( argc ), 1, f );
		fclose( f );
	}

	for( int i = 3; i < argc; ++i )
	{
		const char* extension = argv[i];
		if( strlen( extension ) != 3 )
		{
			fprintf( stderr, "Extension must be three characters" );
			return 3;
		}
		// Replace extension
		strncpy( &output[outputLength - 3], extension, 3 );

		FILE* f = fopen( output, "wb" );
		fwrite( (char*)&argc, sizeof( argc ), 1, f );
		fclose( f );
	}

    return 0;
}
