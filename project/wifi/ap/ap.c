/*
 *    Description:  platform IEEE802.11n ssid management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landwifi/landwifi.h"



boole_t _setup( obj_t this, param_t param )
{
    talk_t ret;
    const char *obj;
	const char *object;
	const char *netdev;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	ret = tfalse;
	netdev = register_value( object, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		info( "%s(%s) add to network frame", object, netdev );
		/* tell the network layer */
		ret = scalls( NETWORK_COM, "add", "%s,%s", object, netdev );
	}

    return ret;
}
boole_t _shut( obj_t this, param_t param )
{
    const char *obj;
	const char *object;
	const char *netdev;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	netdev = register_value( object, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		/* tell the network layer */
		info( "%s delete from network frame", object );
		scalls( NETWORK_COM, "delete", object );
	}

	return ttrue;
}



talk_t _netdev( obj_t this, param_t param )
{
    const char *obj;
	const char *object;
	const char *netdev;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}
	return string2x( netdev );
}
boole_t _up( obj_t this, param_t param )
{
	int fd;
	talk_t ret;
	talk_t cfg;
    const char *obj;
	const char *ptr;
	const char *radio;
	const char *object;
	const char *netdev;
    char path[PATH_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	/* get the radio */
	radio = register_value( object, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return tfalse;
	}
	/* get the netdev */
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	/* get the configure */
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return tfalse;
	}

    /* ignore it if up already */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	fd = lock_open( path, O_RDWR|O_CREAT|O_EXCL, 0666, -1 );
    if ( fd < 0 )
    {
		info( "%s(%s) already up", object, netdev );
		talk_free( cfg );
		return ttrue;
    }
    /* up the deivce */
	info( "%s(%s) up", object, netdev );

	ret = tfalse;
    /* stop the hostapd */
    sdelete( "%s-hostapd", radio );
    /* status[enable/disable]  */
    ptr = json_string( cfg, "status" );
    if (  ptr == NULL || 0 != strcmp( ptr, "enable" ) )
    {
        if ( netdev_flags( netdev, IFF_UP ) > 0 )
        {
            xexecute( 0, 1, "ifconfig %s down", netdev );
        }
		ret = ttrue;
		lock_close( fd );
		unlink( path );
    }
	else
	{
        if ( netdev_flags( netdev, IFF_UP ) <= 0 )
        {
            xexecute( 0, 1, "ifconfig %s up", netdev );
        }
		/* mark the up state */
		ptr = uptime_desc( NULL, 0 );
		if ( ptr != NULL )
		{
			write( fd, ptr,	strlen(ptr) );
		}
		lock_close( fd );
	}

	/* start the hostapd */
	sstart( radio, "hostapd", NULL, "%s-hostapd", radio );
	
	talk_free( cfg );
    return ret;
}
boole_t _down( obj_t this, param_t param )
{
    const char *obj;
	const char *object;
	const char *netdev;
    char path[PATH_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	/* get the netdev */
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	/* delete the mark file */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	unlink( path );

    /* down the deivce */
	if ( netdev != NULL && netdev_flags( netdev, IFF_BROADCAST ) > 0 )
	{
		info( "%s(%s) down", object, netdev );
		/* down the netdev */
		if ( netdev_flags( netdev, IFF_UP ) > 0 )
		{
			xexecute( 0, 1, "ifconfig %s down", netdev );
		}
	}

	return ttrue;
}
boole_t _connect( obj_t this, param_t param )
{
	return ttrue;
}
boole_t _connected( obj_t this, param_t param )
{
	return ttrue;
}
talk_t _status( obj_t this, param_t param )
{
	talk_t ret;
	talk_t cfg;
    const char *obj;
	const char *object;
	const char *netdev;
    char mac[NAME_MAX];
    char path[PATH_MAX];
    char buffer[NAME_MAX];
    unsigned long long rt_bytes, rt_packets, rt_errs, rt_drops, tt_bytes, tt_packets, tt_errs, tt_drops;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	/* get the netdev */
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}
	/* get the configure */
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return NULL;
	}

    ret = json_create( NULL );
	/* get the state */
	if ( netdev_flags( netdev, IFF_UP ) > 0 )
	{
		json_set_string( ret, "state", "up" );
	}
	else
	{
		json_set_string( ret, "state", "down" );
	}
    /* get the secure mode */
    json_set_string( ret, "secure", json_string( cfg, "secure" ) );
    /* flew get */
    rt_bytes = rt_packets = rt_errs = rt_drops = tt_bytes = tt_packets = tt_errs = tt_drops = 0;
    netdev_flew( netdev, &rt_bytes, &rt_packets, &rt_errs, &rt_drops, &tt_bytes, &tt_packets, &tt_errs, &tt_drops );
    snprintf( buffer, sizeof(buffer), "%llu", rt_bytes );
    json_set_string( ret, "rx_bytes", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", rt_packets );
    json_set_string( ret, "rx_packets", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", rt_errs );
    json_set_string( ret, "rx_errs", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", rt_drops );
    json_set_string( ret, "rx_drops", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", tt_bytes );
    json_set_string( ret, "tx_bytes", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", tt_packets );
    json_set_string( ret, "tx_packets", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", tt_errs );
    json_set_string( ret, "tx_errs", buffer );
    snprintf( buffer, sizeof(buffer), "%llu", tt_drops );
    json_set_string( ret, "tx_drops", buffer );
    /* get the mac */
    if ( netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) ) == 0 )
	{
		json_set_string( ret, "mac", mac );
	}
    /* get uptime and livetime_string */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
    if ( file2string( path, buffer, sizeof(buffer) ) > 0 )
    {
        json_set_string( ret, "ontime", buffer );
        json_set_string( ret, "livetime", livetime_desc( atoi(buffer), path, sizeof(path) ) );
    }

	{
		FILE *fp;
		char readbuf[1024];
		char *ssid;
		char *bssid;
		char *channel;
		char *end;
		char *value;
		snprintf( path, sizeof(path), "iw dev %s info", netdev );
		fp = popen( path, "r" );
		memset( readbuf, 0, sizeof(readbuf) );
		fread( readbuf, sizeof(readbuf), 1, fp );
		pclose( fp );
		ssid = strstr( readbuf, "ssid " );
		bssid = strstr( readbuf, "addr " );
		channel = strstr( readbuf, "channel " );
		if ( ssid != NULL )
		{
			value = ssid+5;
			end = strchr( value, '\n' );
			if ( end != NULL )
			{
				*end = '\0';
				json_set_string( ret, "ssid", value );
			}
		}
		if ( bssid != NULL )
		{
			value = bssid+5;
			end = strchr( value, '\n' );
			if ( end != NULL )
			{
				*end = '\0';
				lowtoupp( value );
				json_set_string( ret, "bssid", value );
			}
		}
		if ( channel != NULL )
		{
			value = channel+8;
			end = strchr( value, ' ' );
			if ( end != NULL )
			{
				*end = '\0';
				json_set_string( ret, "channel", value );
			}
		}
	}

	talk_free( cfg );
	return ret;
}
boole_t _online( obj_t this, param_t param )
{
	return ttrue;
}
boole_t _offline( obj_t this, param_t param )
{
	return ttrue;
}



talk_t _stalist( obj_t this, param_t param )
{
    int i;
    FILE *fp;
    char *ptr;
	talk_t x;
	talk_t ret;
    const char *obj;
	const char *object;
	const char *netdev;
    char path[1024];
    char readbuf[1024];
    unsigned long long ei;
    unsigned int day, hour, minute, second;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	object = obj_combine( this );
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}
    snprintf( path, sizeof(path), "/tmp/.iw_station_dump_%s", netdev );
    shell( "iw dev %s station dump > %s", netdev, path );
    fp = fopen( path, "r");
    if( fp == NULL )
    {
    	return NULL;
    }

	x = NULL;
	ret = json_create( NULL );
    memset( readbuf, 0, sizeof(readbuf) );
    while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
    {
        if ( strncmp( readbuf, "Station", 7 ) == 0 )
        {
            ptr = readbuf+8;
            *(ptr+17) = '\0';
            lowtoupp( ptr );
			x = json_create( NULL );
			json_set_value( ret, ptr, x );
        }
        else if ( x != NULL )
        {
            ptr = strstr( readbuf, "	signal:" );
            if ( ptr != NULL )
            {
                sscanf( readbuf, "%*[^:]:%d", &i );
				json_set_number( x, "rssi", i );
                continue;
            }
            ptr = strstr( readbuf, "connected time:" );
            if ( ptr != NULL )
            {
				ei = 0;
                day = hour = minute = second = 0;
                sscanf( readbuf, "%*[^:]:%llu", &ei );
                day = ei / (24*60*60); ei %= (24*60*60);
                hour = ei / (60*60); ei %= (60*60);
                minute = ei / 60; ei %= 60;
                second = ei;
				snprintf( path, sizeof(path), "%02u:%02u:%02u:%u", hour, minute, second, day );
				json_set_string( x, "livetime", path );
                continue;
            }
        }
    }

    fclose( fp );
	return ret;
}
boole_t _stabeat( obj_t this, param_t param )
{
    return tfalse;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;
    const char *obj;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return false;
	}
	ret = config_set( this, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
    const char *obj;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	return config_get( this, path );
}



