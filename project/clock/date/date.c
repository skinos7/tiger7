/*
 *  Description:  system data management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



/* set the time to system */
static boole time_setting( const char *tt, const char *zone )
{
    int i;
    struct tm tm_current;

    memset( &tm_current, 0, sizeof( tm_current ) );
    i = sscanf( tt, "%d:%d:%d:%d:%d:%d", &(tm_current.tm_hour), &(tm_current.tm_min), \
                &(tm_current.tm_sec), &(tm_current.tm_mon), &(tm_current.tm_mday), &(tm_current.tm_year) );
    if ( i != 6 )
    {
        return false;
    }
    else
    {
        /* set the time */
        tm_current.tm_year -= 1900;
        tm_current.tm_mon--;
        tm_current.tm_isdst = -1;
        info( "system date modifyed to %s", tt );
        clock_setting( mktime( &tm_current ), zone );
		/* tell the hardware clock */
        shell( "hwclock -w >/dev/null 2>&1" );
        /* tell the system that time is ok */
		reg_set( NULL, "date_src", "set", sizeof("set") );
        joint_casts( "date/modify", "set" );
    }
    return true;
}
/* sync the time use ntp */
static boole ntpclient_sync( const char* server, const char* zone )
{
    boole ret;
    char path[PATHNAME_SIZE];

    if ( server == NULL || *server == '\0' )
    {
        return false;
    }
    ret = false;
    project_osc_path( path, sizeof(path), PROJECT_ID, "ntpclient" );
    /* sync the time */
    if ( 0 == exec( 20, 1, "%s -h %s -s" , path, server ) )
    {
        ret = true;
        info( "sync the system time from %s succeed", server );
        shell( "hwclock -w >/dev/null 2>&1" );
        /* tell the system that time is ok */
		reg_set( NULL, "date_src", "ntp", sizeof("ntp") );
        joint_casts( "date/modify", "ntp" );
        /*
        if ( NULL != zone && strlen(zone) )
        {
            // check time with zone
            ret = adjust_time_zone( zone );
        }
        */
    }
    return ret;
}



talk_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;
    const char *zone;

    zone = "CTT-8";
	/* get the configure */
    cfg = config_sget( COM_IDPATH, NULL );
    ptr = json_string( cfg, "timezone" );
    if ( ptr != NULL )
    {
        if ( 0 == strcasecmp( ptr, "-12" ) ){zone = "GMT12";}
        else if ( 0 == strcasecmp( ptr, "-11" ) ){zone = "GMT11";}
        else if ( 0 == strcasecmp( ptr, "-10" ) ){zone = "GMT10";}
        else if ( 0 == strcasecmp( ptr, "-9" ) ){zone = "GMT9";}
        else if ( 0 == strcasecmp( ptr, "-8" ) ){zone = "GMT8";}
        else if ( 0 == strcasecmp( ptr, "-7" ) ){zone = "GMT7";}
        else if ( 0 == strcasecmp( ptr, "-6" ) ){zone = "GMT6";}
        else if ( 0 == strcasecmp( ptr, "-5" ) ){zone = "GMT5";}
        else if ( 0 == strcasecmp( ptr, "-4" ) ){zone = "GMT4";}
        else if ( 0 == strcasecmp( ptr, "-3:30" ) ){zone = "GMT3:30";}
        else if ( 0 == strcasecmp( ptr, "-2" ) ){zone = "GMT2";}
        else if ( 0 == strcasecmp( ptr, "-1" ) ){zone = "GMT1";}
        else if ( 0 == strcasecmp( ptr, "0" ) ){zone = "UTC0";}
        else if ( 0 == strcasecmp( ptr, "1" ) ){zone = "ECT-1";}
        else if ( 0 == strcasecmp( ptr, "2" ) ){zone = "EET-2";}
        else if ( 0 == strcasecmp( ptr, "3" ) ){zone = "EAT-3";}
        else if ( 0 == strcasecmp( ptr, "3:30" ) ){zone = "GMT-3:30";}
        else if ( 0 == strcasecmp( ptr, "4" ) ){zone = "NET-4";}
        else if ( 0 == strcasecmp( ptr, "4:30" ) ){zone = "GMT-4:30";}
        else if ( 0 == strcasecmp( ptr, "5" ) ){zone = "PLT-5";}
        else if ( 0 == strcasecmp( ptr, "5:30" ) ){zone = "GMT-5:30";}
        else if ( 0 == strcasecmp( ptr, "6" ) ){zone = "BST-6";}
        else if ( 0 == strcasecmp( ptr, "7" ) ){zone = "VST-7";}
        else if ( 0 == strcasecmp( ptr, "8" ) ){zone = "CTT-8";}
        else if ( 0 == strcasecmp( ptr, "9" ) ){zone = "JST-9";}
        else if ( 0 == strcasecmp( ptr, "9:30" ) ){zone = "GMT-9:30";}
        else if ( 0 == strcasecmp( ptr, "10" ) ){zone = "AET-10";}
        else if ( 0 == strcasecmp( ptr, "11" ) ){zone = "SST-11";}
        else if ( 0 == strcasecmp( ptr, "12" ) ){zone = "NST-12";}
    }
    /* set time zone */
    string2file( "/etc/TZ", "%s\n", zone ); /* because the TZ file in the /tmp/TZ, so you can write anytime */

	/* read from the RTC when have RTC */
	/* XXXXXXXXXXXXXXX */

    /* run the service of ntpclient depend configure */
    ptr = json_string( cfg, "ntpclient" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        serv_start( this, "ntploop", NULL, COM_IDPATH );
    }

    talk_free( cfg );
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
	/* stop the service */
    serv_stop( COM_IDPATH );
	/* kill the ntpclient to prevent the ntpclient pause */
	shell( "killall ntpclient" );
    return ttrue;
}
talk_t _ntploop( obj_t this, param_t param )
{
    int t;
    boole ret;
    talk_t cfg;
    int interval;
    const char *ptr;
    const char *zone;
    char key[IDENTIFY_SIZE];

    /* wait the online */
    do
    {
        if ( route_exist( "0.0.0.0", NULL, NULL, NULL ) == true )
        {
            break;
        }
        sleep( 3 );
    }while(1);

    /* get the configure */
    ret = false;
    interval = 0;
    cfg = config_sget( COM_IDPATH, NULL );
    zone = json_get_string( cfg, "timezone" );
    ptr = json_get_string( cfg, "ntpinterval" );
    if ( ptr != NULL )
    {
        interval = atoi( ptr );
    }
    /* loop it */
    while(1)
    {
        /* sync every server util succeed */
        for ( t=0; t<10; t++ )
        {
            if ( t==0 )
            {
                strncpy( key, "ntpserver", sizeof(key) );
            }
            else
            {
                snprintf( key, sizeof(key), "ntpserver%d", t );
            }
            ptr = json_get_string( cfg, key );
            if ( ptr == NULL || *ptr == '\0' )
            {
                continue;
            }
            ret = ntpclient_sync( ptr, zone );
            if ( ret == true )
            {
                break;
            }
        }
        if ( ret == true )
        {
            shell( "hwclock -w >/dev/null 2>&1" );
            /* wait interval time */
            if ( interval <= 0 )
            {
                pause();
            }
            else
            {
                sleep( interval );
            }
        }
        else
        {
            sleep( 10 );
        }
    }
    
    talk_free( cfg );
    return tfalse;
}
talk_t _status( obj_t this, param_t param )
{
	talk_t ret;
	const char *ptr;
	char buffer[IDENTIFY_SIZE];

	ret = json_create();
	ptr = reg_get( NULL, "date_src" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		json_set_string( ret, "source", ptr );
	}
	ptr = time_getting( buffer, sizeof(buffer) );
	if ( ptr != NULL )
	{
		json_set_string( ret, "current", ptr );
	}
	ptr = livetime_getting( buffer, sizeof(buffer) );
	if ( ptr != NULL )
	{
		json_set_string( ret, "livetime", ptr );
	}
	ptr = uptime_getting( buffer, sizeof(buffer) );
	if ( ptr != NULL )
	{
		json_set_string( ret, "uptime", ptr );
	}
	return ret;
}
talk_t _ntpsync( obj_t this, param_t param )
{
    int t;
    boole ret;
    talk_t cfg;
    const char *ptr;
    const char *zone;
    char key[IDENTIFY_SIZE];

    ret = false;
    ptr = param_option( param, 1 );
    cfg = config_sget( COM_IDPATH, NULL );
    zone = json_get_string( cfg, "timezone" );
    if ( ptr != NULL )
    {
        ret = ntpclient_sync( ptr, zone );
    }
    else
    {
        for ( t=0; t<10; t++ )
        {
            if ( t==0 )
            {
                strncpy( key, "ntpserver", sizeof(key) );
            }
            else
            {
                snprintf( key, sizeof(key), "ntpserver%d", t );
            }
            ptr = json_get_string( cfg, key );
            if ( ptr == NULL || *ptr == '\0' )
            {
                continue;
            }
            ret = ntpclient_sync( ptr, zone );
            if ( ret == true )
            {
                break;
            }
        }
    }
    talk_free( cfg );
    if ( ret == true )
    {
        return ttrue;
    }
    return tfalse;
}
talk_t _current( obj_t this, param_t param )
{
    const char *ptr;

	ptr = param_option( param, 1 );
	if ( time_setting( ptr, NULL ) == true )
	{
        _shut( this, NULL );
        _setup( this, NULL );
		return ttrue;
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



