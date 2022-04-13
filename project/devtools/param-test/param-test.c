/*
 *    Description:  executable program for test the talk library function
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "land/skin.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX,      Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
 */

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h              implementation of general linker list
syslog.h            log call implementation
talk.h              implementation of common communication data types
param.h             implementation of parameter structure and related functions
path.h              implementation of structure and related functions for object path and attribute path
misc.h              miscellaneous function implementation
register.h          global register variable implementation
config.h            implementation function to get/set/list the config
project.h           provide unified project information operation interface for the system
com.h               implementation communication to other component function use talk structure or parameter structure
he2com.h            command line order parse and exeute for he command
skinapi.h		    define all the general component api
 */

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* dump the pararm*/
static void printf_param( param_t ptmp )
{
	int i;
    int len;
	char *buf;
	talk_t value;

	len = param_size( ptmp );
	for( i=1; i<=len; i++ )
	{
		printf( "%d[string]: %s\n", i, param_string( ptmp, i ) );
	}
	for( i=1; i<=len; i++ )
	{
		value = param_talk( ptmp, i );
		buf = talk2string( value );
		printf( "%d[talk]: %s\n", i, buf );
		efree( buf );
	}
	printf( "combine[%d]: %s\n", param_size(ptmp), param_combine( ptmp ) );
}

/* main function */
int main( int argc, const char **argv )
{
    int i;
    int len;
    char *cmd;
	param_t ptmp;

    /* get the command size */
    len = 0;
    for ( i=1; ((i<argc)&&(argv[i]!=NULL)); i++ )
    {
        len += strlen( argv[i] );
    }
    if ( len == 0 )
    {
        errno = EINVAL;
        perror( "he command" );
        return -1;
    }
    /* get the full command */
    cmd = calloc( 1, len+1 );
    if ( cmd == NULL )
    {
        faulting( "out of memory" );exit(-1);
        return -1;
    }
	/* assemble reserve space to support filename with options */
	if ( argv[1][0] == '/' )
	{
	    for ( i=1; i<argc; i++ )
	    {
	        strcat( cmd, argv[i] );
			if ( i < argc-1 )
			{
				strcat( cmd, " " );
			}
	    }
	}
	/* assemble only */
	else
	{
	    for ( i=1; ((i<argc)&&(argv[i]!=NULL)); i++ )
	    {
	        strcat( cmd, argv[i] );
	    }
	}

	while(1)
	{
		ptmp = param_create( cmd );
		if ( ptmp == NULL )
		{
			perror( "argument" );
			break;
		}
		printf( ">=========original========>\n" );
		printf_param( ptmp );
		printf( "\n" );

		while( param_shift( ptmp, 1) == true )
		{
			printf( ">=========shift========>\n" );
			printf_param( ptmp );
			printf( "\n" );
		}

		while( param_unshift( ptmp, 1) == true )
		{
			printf( ">=========unshift========>\n" );
			printf_param( ptmp );
			printf( "\n" );
		}

		param_free( ptmp );
	}


	efree( cmd );
    return 0;
}



