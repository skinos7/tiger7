/*
 *    Description:  executable program for Repeated call some function to test it
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
	const char *obj;
	const char *method;
	const char *time;

	if ( argc < 3 )
	{
	
	}
	obj = argc[1];
	method = argc[2];
	time = argc[3];
    return 0;
}



