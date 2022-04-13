/*
 *    Description:  system date management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#define NTPSERVER_CONFIG_FILE "/tmp/.ntps.conf"



const char *_intro( obj_t this )
{
	const char *str = \
"{"\
"\n    \"setup\":\"setup the ntp server\","\
"\n    \"shut\":\"shutdown the ntp server\""\
"\n}\n";
    return str;
}
talk_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;

    cfg = config_sget( COM_IDPATH, NULL );
    ptr = json_get_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        serv_start( this, "service", NULL, COM_IDPATH );
    }

    talk_free( cfg );
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    serv_stop( COM_IDPATH );
    return ttrue;
}
talk_t _service( obj_t this, param_t param )
{
	talk_t cfg;
	struct stat st;
    const char *ptr;
    const char *ifname;
	const char *date_src;
    char local_interface[IDENTIFY_SIZE];

	date_src = NULL;
	do
	{
		if ( date_src == NULL )
		{
			date_src = reg_get( NULL, "date_src" );
		}
		if ( date_src != NULL && *date_src != '\0' )
		{
			break;
		}
		sleep( 30 );
	}while(1);

	cfg = config_sget( COM_IDPATH, NULL );
    ifname = json_get_string( cfg, "local" );
    if ( ifname == NULL || *ifname == '\0' )
    { 
    	ifname = LAN_COM;
	}
    ptr = com_scall_string( ifname, "device", NULL, local_interface, sizeof(local_interface) );
    if ( ptr == NULL )
    {
        fault( "no local interface" );
        talk_free( cfg );
        sleep( 10 );
        return tfalse;
    }

	if ( stat( "/usr/sbin/ntpd", &st ) == 0 )
	{
		/* busybox ntpd */
		write_string( "/etc/ntp.conf", "\n\n" );
		//ntpd -d -n -N -l -I lan
		execlp( "ntpd", "ntpd", "-n", "-N", "-l", "-I", local_interface, (char*)0 );
		faulting( "execlp the ntpd(%s) error" , "ntpd" );
	}
	else
	{
		write_string( NTPSERVER_CONFIG_FILE, "restrict 127.0.0.1\n\n" );
		/* GPS(NMEA)+PPS */
		append_string( NTPSERVER_CONFIG_FILE, "server 127.127.1.0\n" );
		append_string( NTPSERVER_CONFIG_FILE, "fudge 127.127.1.0 stratum 0\n\n" );

		execlp( "ntpd", "ntpd", "-c", NTPSERVER_CONFIG_FILE, "-n", "-I", local_interface, (char*)0 );
		faulting( "execlp the ntpd(%s) error" , "ntpd" );
	}
	return tfalse;
}




boole _set( obj_t this, path_t path, talk_t v )
{
    obj_t o;
    boole ret;

    o = obj_create( COM_IDPATH );
    ret = config_set( o, path, v );
    if ( ret == true )
    {
        _shut( this, NULL );
        _setup( this, NULL );
    }
    obj_free( o );
    return ret;
}
talk_t _get( obj_t this, path_t path )
{
    obj_t o;
    talk_t ret;

    o = obj_create( COM_IDPATH );
    ret = config_get( o, path );
    obj_free( o );
    return ret;
}



