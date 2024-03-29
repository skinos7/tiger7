/*
 *  Description:  ntp server management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#define NTPSERVER_CONFIG_FILE PROJECT_CONF_DIR"/ntps.conf"



boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;

	/* get the component configure */
    cfg = config_sget( COM_IDPATH, NULL );
	/* get the attribute vaule of "status" from component configure */
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
    	/* start the service for ntp server */
        sstart( COM_IDPATH, "service", NULL, COM_IDPATH );
    }

	/* free the component configure */
    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	/* stop and delete the service */
    sdelete( COM_IDPATH );
    return ttrue;
}
boole_t _service( obj_t this, param_t param )
{
	talk_t cfg;
	struct stat st;
    const char *ptr;
    const char *netdev;
	const char *date_src;
    char local_interface[NAME_MAX];

	date_src = NULL;
	do
	{
		if ( date_src == NULL )
		{
			date_src = register_value( LAND_PROJECT, "date_src" );
		}
		if ( date_src != NULL && *date_src != '\0' )
		{
			break;
		}
		sleep( 30 );
	}while(1);

	cfg = config_sget( COM_IDPATH, NULL );
	netdev = NULL;
    ptr = json_string( cfg, "local" );
    if ( ptr == NULL || *ptr == '\0' )
    {
    	netdev = register_value( LAND_PROJECT, "local_netdev" );
	}
	else
	{
		netdev = scall_string( local_interface, sizeof(local_interface), ptr, "netdev", NULL );
	}
    if ( netdev == NULL || *netdev == '\0' )
    {
        fault( "no local interface" );
        talk_free( cfg );
        sleep( 10 );
        return tfalse;
    }

	if ( stat( "/usr/sbin/ntpd", &st ) == 0 )
	{
		/* busybox ntpd */
		string2file( "/etc/ntp.conf", "\n\n" );
		//ntpd -d -n -N -l -I lan
		execlp( "ntpd", "ntpd", "-n", "-N", "-l", "-I", netdev, (char*)0 );
		faulting( "execlp the ntpd(%s) error" , "ntpd" );
	}
	else
	{
		string2file( NTPSERVER_CONFIG_FILE, "restrict 127.0.0.1\n\n" );
		/* GPS(NMEA)+PPS */
		string3file( NTPSERVER_CONFIG_FILE, "server 127.127.1.0\n" );
		string3file( NTPSERVER_CONFIG_FILE, "fudge 127.127.1.0 stratum 0\n\n" );
		execlp( "ntpd", "ntpd", "-c", NTPSERVER_CONFIG_FILE, "-n", "-I", netdev, (char*)0 );
		faulting( "execlp the ntpd(%s) error" , "ntpd" );
	}
	return tfalse;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

    ret = config_sset( COM_IDPATH, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	return config_sget( COM_IDPATH, path );
}



