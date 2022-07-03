/*
 *    Description:  executable program template
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "land/skin.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX       Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
EXE_ID                String, execute program name, Name of the directory where this execute program resides
EXE_IDPATH            String, Full name of a execute program name in the system, PROJECT_ID@EXE_ID
*/

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h              implementation of general linker list
syslog.h               log call implementation
talk.h              implementation of common communication data types
param.h             implementation of parameter structure and related functions
path.h              implementation of structure and related functions for object path and attribute path
misc.h              miscellaneous function implementation
register.h          global register variable implementation
config.h            implementation function to get/set/list the config
project.h           provide unified project information operation interface for the system
com.h               implementation communication to other component function use talk structure or parameter structure
he2com.h            invokes the function implementation
skinapi.h		    define all the general component api
*/

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* main function */
int main( int argc, const char **argv )
{
	info( "the %s of %s has be executed", EXE_ID, PROJECT_ID );
	printf( "the %s of %s has be executed\n", EXE_ID, PROJECT_ID );
    return 0;
}



