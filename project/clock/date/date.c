/*
 *  Description:  system data management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
char *reg_date_src;



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
        date_set( mktime( &tm_current ), zone );
		/* tell the hardware clock */
        shell( "hwclock -w >/dev/null 2>&1" );
        /* tell the system that time is ok */
		string2reg( "date_src", reg_date_src, "set", 20 );
        joint_calls( "date/modify", "set" );
    }
    return true;
}
/* sync the time use ntp */
static boole ntpclient_sync( const char* server, const char* zone )
{
    boole ret;
    char path[PATH_MAX];

    if ( server == NULL || *server == '\0' )
    {
        return false;
    }
    ret = false;
    project_exe_path( path, sizeof(path), PROJECT_ID, "ntpclient" );
    /* sync the time */
    if ( 0 == execute( 120, true, "%s -h %s -s" , path, server ) )
    {
        ret = true;
        info( "sync the system time from %s succeed", server );
        execute( 0, true, "hwclock -w" );
        /* tell the system that time is ok */
		string2reg( "date_src", reg_date_src, "ntp", 20 );
        joint_calls( "date/modify", "ntp" );
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



boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;
    const char *ozone;
    const char *nzone;

    ozone = "CTT-8";
    nzone = "GMT+8";
	/* get the configure */
    cfg = config_sget( COM_IDPATH, NULL );
    /* set time zone with old style(ulibc) */
    ptr = json_string( cfg, "timezone" );
    if ( ptr != NULL )
    {
        if ( 0 == strcasecmp( ptr, "-12" ) ){ozone = "GMT12";nzone = "GMT-12";}
        else if ( 0 == strcasecmp( ptr, "-11" ) ){ozone = "GMT11";nzone = "GMT+11";}
        else if ( 0 == strcasecmp( ptr, "-10" ) ){ozone = "GMT10";nzone = "GMT+10";}
        else if ( 0 == strcasecmp( ptr, "-9" ) ){ozone = "GMT9";nzone = "GMT+9";}
        else if ( 0 == strcasecmp( ptr, "-8" ) ){ozone = "GMT8";nzone = "GMT+8";}
        else if ( 0 == strcasecmp( ptr, "-7" ) ){ozone = "GMT7";nzone = "GMT+7";}
        else if ( 0 == strcasecmp( ptr, "-6" ) ){ozone = "GMT6";nzone = "GMT+6";}
        else if ( 0 == strcasecmp( ptr, "-5" ) ){ozone = "GMT5";nzone = "GMT+5";}
        else if ( 0 == strcasecmp( ptr, "-4" ) ){ozone = "GMT4";nzone = "GMT+4";}
        else if ( 0 == strcasecmp( ptr, "-3:30" ) ){ozone = "GMT3:30";nzone = "GMT+4";}
        else if ( 0 == strcasecmp( ptr, "-2" ) ){ozone = "GMT2";nzone = "GMT+2";}
        else if ( 0 == strcasecmp( ptr, "-1" ) ){ozone = "GMT1";nzone = "GMT+1";}
        else if ( 0 == strcasecmp( ptr, "0" ) ){ozone = "UTC0";nzone = "GMT0";}
        else if ( 0 == strcasecmp( ptr, "1" ) ){ozone = "ECT-1";nzone = "GMT-1";}
        else if ( 0 == strcasecmp( ptr, "2" ) ){ozone = "EET-2";nzone = "GMT-2";}
        else if ( 0 == strcasecmp( ptr, "3" ) ){ozone = "EAT-3";nzone = "GMT-3";}
        else if ( 0 == strcasecmp( ptr, "3:30" ) ){ozone = "GMT-3:30";nzone = "GMT-3";}
        else if ( 0 == strcasecmp( ptr, "4" ) ){ozone = "NET-4";nzone = "GMT-4";}
        else if ( 0 == strcasecmp( ptr, "4:30" ) ){ozone = "GMT-4:30";nzone = "GMT-4";}
        else if ( 0 == strcasecmp( ptr, "5" ) ){ozone = "PLT-5";nzone = "GMT-5";}
        else if ( 0 == strcasecmp( ptr, "5:30" ) ){ozone = "GMT-5:30";nzone = "GMT-5";}
        else if ( 0 == strcasecmp( ptr, "6" ) ){ozone = "BST-6";nzone = "GMT-6";}
        else if ( 0 == strcasecmp( ptr, "7" ) ){ozone = "VST-7";nzone = "GMT-7";}
        else if ( 0 == strcasecmp( ptr, "8" ) ){ozone = "CTT-8";nzone = "GMT-8";}
        else if ( 0 == strcasecmp( ptr, "9" ) ){ozone = "JST-9";nzone = "GMT-9";}
        else if ( 0 == strcasecmp( ptr, "9:30" ) ){ozone = "GMT-9:30";nzone = "GMT-9";}
        else if ( 0 == strcasecmp( ptr, "10" ) ){ozone = "AET-10";nzone = "GMT-10";}
        else if ( 0 == strcasecmp( ptr, "11" ) ){ozone = "SST-11";nzone = "GMT-11";}
        else if ( 0 == strcasecmp( ptr, "12" ) ){ozone = "NST-12";nzone = "GMT-12";}
    }
    string2file( "/etc/TZ", "%s\n", ozone ); /* because the TZ file in the /tmp/TZ, so you can write anytime */
    /* set time zone with new style(libc) */
	unlink( "/etc/localtime" );
	shell( "ln -s /usr/share/zoneinfo/%s /etc/localtime", nzone );
	/* read from the RTC when have RTC */
	/* XXXXXXXXXXXXXXX */

    /* run the service of ntpclient depend configure */
    ptr = json_string( cfg, "ntpclient" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        sstart( COM_IDPATH, "ntploop", NULL, COM_IDPATH );
    }

    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	/* stop the service */
    sdelete( COM_IDPATH );
	/* kill the ntpclient to prevent the ntpclient pause */
	shell( "killall ntpclient" );
    return ttrue;
}
boole_t _ntploop( obj_t this, param_t param )
{
    int t;
    boole ret;
    talk_t cfg;
    int interval;
    const char *ptr;
    const char *zone;
    char key[NAME_MAX];

    /* wait the online */
    do
    {
        if ( route_info( "0.0.0.0", NULL, NULL, NULL ) == true )
        {
            break;
        }
        sleep( 3 );
    }while(1);

    /* get the configure */
    ret = false;
    interval = 0;
    cfg = config_sget( COM_IDPATH, NULL );
    zone = json_string( cfg, "timezone" );
    ptr = json_string( cfg, "ntpinterval" );
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
            ptr = json_string( cfg, key );
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
	char buffer[NAME_MAX];

	ret = json_create( NULL );
	ptr = register_pointer( LAND_PROJECT, "date_src" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		json_set_string( ret, "source", ptr );
	}
	ptr = date_desc( buffer, sizeof(buffer) );
	if ( ptr != NULL )
	{
		json_set_string( ret, "current", ptr );
	}
	ptr = uptime_desc( buffer, sizeof(buffer) );
	if ( ptr != NULL )
	{
		json_set_string( ret, "livetime", ptr );
	}
	json_set_number( ret, "uptime", uptime_int() );
	return ret;
}
boole_t _ntpsync( obj_t this, param_t param )
{
    int t;
    boole ret;
    talk_t cfg;
    const char *ptr;
    const char *zone;
    char key[NAME_MAX];

    ret = false;
    ptr = param_string( param, 1 );
    cfg = config_sget( COM_IDPATH, NULL );
    zone = json_string( cfg, "timezone" );
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
            ptr = json_string( cfg, key );
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
boole_t _current( obj_t this, param_t param )
{
    const char *ptr;

	ptr = param_string( param, 1 );
	if ( time_setting( ptr, NULL ) == true )
	{
		return ttrue;
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



