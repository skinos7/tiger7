/*
  * Description:  auto restart plan management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



talk_t _setup( obj_t this, param_t param )
{
    const char *ptr;

    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "mode" );
    if ( ptr != NULL 
        && ( ( 0 == strcmp( ptr, "age" ) )
        || ( 0 == strcmp( ptr, "point" ) )
        || ( 0 == strcmp( ptr, "idle" ) ) ) )
    {
        service_start( COM_IDPATH, COM_IDPATH, "service", NULL );
    }

    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    service_stop( COM_IDPATH );
    return ttrue;
}
talk_t _service( obj_t this, param_t param )
{
    char *end;
    talk_t cfg;
    unsigned int i;
    char string[64];
    const char *ptr;
    const char *mode;
    unsigned int pass;

    /* sleep 2 second for action, get pass time */
    sleep( 120 );
    memset( string, 0, sizeof(string) );
    file2string( "/proc/uptime", string, sizeof(string) );
    end = strstr( string, "." );
    if ( end == NULL )
    {
        return NULL;
    }
    *end = '\0';
    pass = atoll( string );

    /* restart */
    cfg = config_sget( COM_IDPATH, NULL );
    mode = json_string( cfg, "mode" );
    if ( mode != NULL && 0 == strcmp( mode, "age" ) )
    {
        i = 0;
        ptr = json_string( cfg, "age" );
        if ( ptr != NULL )
        {
            i = atoi( ptr );
        }
        if ( i >= 300 )
        {
            i -= pass;
            if ( i > 0 )
            {
                sleep( i );
            }
            info( "restart the system by %s for %s mode", COM_IDPATH, mode );
            scall( MACHINE_COM, "restart", NULL );
        }
    }

    else if ( mode != NULL && 0 == strcmp( mode, "point" ) )
    {
    	int up;
        int hour;
        int minute;
        time_t now;
        struct tm *ptime;
        const char *date_src;
        unsigned int age;

        date_src = NULL;
		age = 208800;        // two day
        hour = 3;            // 03
		minute = 30;         // 30
        ptr = json_string( cfg, "point_hour" );
        if ( ptr != NULL )
        {
            hour = atoi( ptr );
        }
        ptr = json_string( cfg, "point_minute" );
        if ( ptr != NULL )
        {
            minute = atoi( ptr );
        }
        ptr = json_string( cfg, "point_age" );
        if ( ptr != NULL )
        {
            age = atoi( ptr );
        }
		info( "restart at the %d:%d or max runtime %d", hour, minute, age );
        do
        {
            if ( date_src == NULL )
            {
                date_src = register_pointer( LAND_PROJECT, "date_src" );
            }
            if ( date_src != NULL && *date_src != '\0' )
            {
				time( &now );
				ptime = localtime( &now );
                if ( hour == ptime->tm_hour && minute == ptime->tm_min )
                {
                    info( "restart the system by %s for %s mode(%u:%u)", COM_IDPATH, mode, hour, minute );
                    scall( MACHINE_COM, "restart", NULL );
                }
            }
			up = uptime_int();
			if ( up >= age )
			{
				info( "restart the system by %s for %s mode(age:%d:%d)", COM_IDPATH, mode, age, up );
				scall( MACHINE_COM, "restart", NULL );
			}
            sleep( 50 );
        }while(1);
    }

    else if ( mode != NULL && 0 == strcmp( mode, "idle" ) )
    {
        talk_t v;
        unsigned int t;
        unsigned int idle_count, sleep_count;
        unsigned int start, idle, age;
        
        start = 172800;  // two day
        idle = 300;       // five minute
        age = 604800; // one week
        ptr = json_string( cfg, "idle_start" );
        if ( ptr != NULL )
        {
            start = atoi( ptr );
        }
        ptr = json_string( cfg, "idle_wireless_time" );
        if ( ptr != NULL )
        {
            idle = atoi( ptr );
        }
        ptr = json_string( cfg, "idle_age" );
        if ( ptr != NULL )
        {
            age = atoi( ptr );
        }
        if ( start >= 300 && idle > 0 && start < age )
        {
            start -= pass;
            age -= pass;
            if ( start > 0 )
            {
                sleep( start );
            }
            age -= start;
            i = 0;
            t = 0;
            sleep_count = age/30;
            idle_count = idle/30;
            do
            {
                v  = scall( STATION_COM, "list", NULL );
                if ( json_each( v, NULL ) != NULL )
                {
                    i = 0;
                }
                else
                {
                    i++;
                }
                talk_free( v );
                t++;
                if ( i>=idle_count || t>=sleep_count )
                {
                    break;
                }
                sleep( 30 );
            }while(1);
            if ( i>=idle_count )
            {
                info( "restart the system by %s for %s mode wireless idle(%u)", COM_IDPATH, mode, idle );
            }
            else
            {
                info( "restart the system by %s for %s mode wireless age(%s)", COM_IDPATH, mode, ptr );
            }
            scall( MACHINE_COM, "restart", NULL );
            return ttrue;
        }
    }
    warn( "configure incorrect pause here" );
    pause();
    return tfalse;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
	obj_t o;
	boole ret;

	o = obj_create( COM_IDPATH );
    ret = config_set( o, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	obj_free( o );
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	obj_t o;
	talk_t ret;

	o = obj_create( COM_IDPATH );
	ret = config_get( o, path );
	obj_free( o );
	return ret;
}



