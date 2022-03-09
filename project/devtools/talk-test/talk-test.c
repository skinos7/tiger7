/*
 *    Description:  executable program for test the talk library function
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "land/farm.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX,      Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
 */

/* Available farm interfaces (specific headers are in the top /doc/ API directory) 
link.h              implementation of general linker list
log.h               log call implementation
talk.h              implementation of common communication data types
param.h             implementation of parameter structure and related functions
path.h              implementation of structure and related functions for object path and attribute path
misc.h              miscellaneous function implementation
register.h          global register variable implementation
config.h            implementation function to get/set/list the config
project.h           provide unified project information operation interface for the system
com.h               implementation communication to other component function use talk structure or parameter structure
service.h           service implementation
joint.h             joint implementation
 */

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* main function */
int main( int argc, const char **argv )
{
	char *buf;
	talk_t x;
	talk_t axp;
	talk_t json;
	talk_t topjson;
	const char *attr;

	while(1)
	{

		/* make a json 
		{
			"topattr":"topvalue",
			"topattr2":"2",
			"topattr3":
			{
				"lowattr":"lowvalue",
				"secattr":"second value"
			},
			"topattr4":"attr4value",
		}
		*/
		topjson = json_create(NULL);
		json_set_string( topjson, "topattr", "topvalue" );
		json_set_number( topjson, "topattr2", 2 );
		json_set_json( topjson, "topattr3", json_create( axp_create("lowattr","lowvalue",NULL) ) );
		axp = axp_create( "topattr5", "newvalue", NULL );
		axp_set_id( axp, "topattr6" );
		axp_set_string( axp, "attr6value" );
		json_attach_axp( topjson, axp );
		axp_set_id( axp, "topattr4" );
		axp_set_string( axp, "attr4value" );
		if ( json_check( topjson ) == false )
		{
			printf( "json check json failed\n" );
			break;
		}
		if ( axp_check( topjson ) == true )
		{
			printf( "axp check json not failed\n" );
			break;
		}
		x = json_json( topjson, "topattr3" );
		axp = axp_create( "lowattr2", "second value", NULL );
		json_attach_axp( x, axp );

		/* print the json attribute */
		printf( "topattr=%s\n", json_string(topjson,"topattr") );
		printf( "topattr2=%s\n", json_string(topjson,"topattr2") );
		x = json_json( topjson, "topattr3" );
		buf = json2string( x );
		printf( "topattr3=%s\n", buf );
		free( buf );
		/* print entire json */
		talk_print( topjson );
		printf( "\n\n" );

		/* delete or cut some attribute from json */
		json_delete_axp( topjson, "topattr2" );
		axp = json_cut_axp( topjson, "topattr3" );
		x = json_axp( topjson, "topattr" );
		json_detach_axp( topjson, x );

		/* free the json */
		json_free( topjson );

		/* make a json 
		{
			"topattr3":
			{
				"lowattr":"lowvalue",
				"secattr":"second value"
			},
			"topattr":"topvalue"
		}
		*/
		topjson = json_create( axp );
		json = talk_dup( topjson );
		json_free( topjson );
		json_attach_axp( json, x );
		axp = NULL;
		while( NULL != ( axp = json_next( json, axp ) ) )
		{
			attr = axp_id( axp );
			x = axp_json( axp );
			if ( json_check( x ) == true )
			{
				buf = json2string( x );
				printf( "%s=%s\n", attr, buf );
				free( buf );
			}
			else
			{
				printf( "%s=%s\n", attr, axp_string(axp) );
			}
		}
		talk_print( json );
		printf( "\n\n" );
		json_free( json );

	}

    return 0;
}



