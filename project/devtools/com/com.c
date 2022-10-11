/*
 *    Description:  component template
 *         Author:  devtools, zxx@ashyelf.com
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
COM_ID                String, component name, Name of the directory where this component resides
COM_IDPATH            String, Full name of a component in the system, PROJECT_ID@COM_ID
*/

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h				implementation of general linker list
syslog.h			log call implementation
talk.h				implementation of common communication data types
param.h 			implementation of parameter structure and related functions
path.h				implementation of structure and related functions for object path and attribute path
misc.h				miscellaneous function implementation
register.h			global register variable implementation
config.h			implementation function to get/set/list the config
project.h			provide unified project information operation interface for the system
com.h				implementation communication to other component function use talk structure or parameter structure
he2com.h			invokes the function implementation
reactor.h           libevent reactor assist functions
skinapi.h			define all the general component api
*/

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* usually used to initialize or deploy the component, is also usually registered in the project information file to be called at startup
 * This function can be called by the user at the he terminal, project@component.setup to call this function */
boole_t _setup( obj_t this, param_t param )
{
	info( "the %s setup has be called", COM_IDPATH );
	printf( "the %s setup has be called\n", COM_IDPATH );
    return ttrue;
}

/* Typically used to shut down or exit this component, this method is called when the system shuts down, which can be registered in the project information file
 * This function can be called by the user at the he terminal, project@component.shut to call this function */
boole_t _shut( obj_t this, param_t param )
{
	info( "the %s shut has be called", COM_IDPATH );
	printf( "the %s shut has be called\n", COM_IDPATH );
    return ttrue;
}

/* Usually it is started as a service process in other functions, so it will always run, and if it exits the system it will restart it */
boole_t _service( obj_t this, param_t param )
{
	info( "the %s service has be ran", COM_IDPATH );
	pause();
    return tfalse;
}


/* Typically used for event process */
talk_t _take( obj_t this, param_t param )
{
    talk_t ms;
	char *ptr;
	const char *event;

	/* get the joint event name */
	event = param_string( param, 1 );
	/* get the information that the joint event carries */
	ms = param_talk( param, 2 );
	/* converts the event information to a string */
	ptr = talk2string( ms );

	/* logger the joint event */
	info( "receive a event, name is %s, carry message is %s", event, ptr );

	/* free the string */
	free( ptr );
	/* To exit, remember to quit immediately after running, if you do not exit may cause the entire event handling to freeze here */
    return tfalse;
}



/* Functions that are triggered when viewing a component configuration are usually used to obtain the actual configuration and then calibrate and then return
 * This function can be called by the user at the he terminal, project@component to call this function */
talk_t _get( obj_t this, attr_t path )
{
    talk_t cfg;

    /* gets the configuration parameters for the component */
    cfg = config_get( this, path );

	info( "returns the configuration of the %s", COM_IDPATH );
    return cfg;
}
/* When you set a component parameter, you will be triggered to call this function, usually filtered by this function and then stored in the actual configuration
 * This function can be called by the user at the he terminal, project@component= to call this function */
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

    /* directly save the set parameters into the flash */
    ret = config_set( this, v, path );
    /* if the flash is successfully saved, the call is called by calling first _shut closing and then calling the _setup to restart the corresponding service */
    if ( ret == true )
    {
		info( "save the configuration of the %s and reset it", COM_IDPATH );
        _shut( this, NULL );
        _setup( this, NULL );
    }
    return ret;
}



