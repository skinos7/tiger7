/*
 *  Description:  wireless staion management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landwifi/landwifi.h"
#define DISABLE_HOSTAPD_TO_MODIFY_CHANNEL 1



int dbm2signal( int rssi )
{
	if ( rssi >= -55 )
	{
		return 4;
	}
	else if ( rssi >= -66 )
	{
		return 3;
	}
	else if ( rssi >= -88 )
	{
		return 2;
	}
	else if ( rssi > -99 )
	{
		return 1;
	}
    return 0;
}



static talk_t station_dev_aplist( const char *netdev, const char *ssid, const char *bssid, const char *ssid2, const char *ssid3, boole good )
{
    int i;
    FILE *fp;
	int signal;
    char *ptr;
	talk_t x;
	talk_t ret;
    talk_t tree;
    talk_t child;
    talk_t value;
	const char *s;
	const char *peermac;
    char path[1024];
    char readbuf[1024];

	shell( "ifconfig %s up", netdev );
    snprintf( path, sizeof(path), "/tmp/.iwinfo_scan_%s", netdev );
    shell( "iwinfo %s scan > %s", netdev, path );
    fp = fopen( path, "r");
    if( fp == NULL )
    {
    	return NULL;
    }

    ret = tree = x = NULL;
	tree = json_create( NULL );
    while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
    {
        if ( strncmp( readbuf, "Cell", 4 ) == 0 )
        {
            ptr = readbuf+19;
            *(ptr+17) = '\0';
            lowtoupp( ptr );
			x = json_create( NULL );
			json_set_value( tree, ptr, x );
        }
        else if ( x != NULL )
        {
            ptr = strstr( readbuf, "ESSID: \"" );
            if ( ptr != NULL )
            {
                ptr += 8;
                i = strlen(ptr);
                ptr[i-2] = '\0';
                if ( *ptr != '\0' )
                {
					json_set_string( x, "ssid", ptr );
                }
                continue;
            }
			s = strstr( readbuf, "Mode:" );
            ptr = strstr( readbuf, "Channel:" );
            if ( s != NULL && ptr != NULL )
            {
                if ( sscanf( ptr, "%*[^:]: %d", &i ) == 1 )
                {
					json_set_number( x, "channel", i );
                }
                continue;
            }
            ptr = strstr( readbuf, "Signal:" );
            if ( ptr != NULL )
            {
                if ( sscanf( readbuf, "%*[^:]: %d", &i ) == 1 )
                {
					json_set_number( x, "rssi", i );
					i = dbm2signal( i );
					json_set_number( x, "signal", i );
                }
                continue;
            }
            ptr = strstr( readbuf, "Encryption:" );
            if ( ptr != NULL )
            {
                /* NONE */
                if ( strstr( readbuf, "none" ) != NULL )
                {
					json_set_string( x, "secure", "disable" );
                }
                /* wpapsk aes */
                else if ( strstr( readbuf, "WPA PSK (CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "aes" );
                }
                /* wpapsk tkip */
                else if ( strstr( readbuf, "WPA PSK (TKIP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
                }
                /* wpapsk tkipaes */
                else if ( strstr( readbuf, "WPA PSK (TKIP, CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
                }
                /* wpa2psk aes */
                else if ( strstr( readbuf, "WPA2 PSK (CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
                }
                /* wpa2psk tkip */
                else if ( strstr( readbuf, "WPA2 PSK (TKIP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
                }
                /* wpa2psk tkipaes */
                else if ( strstr( readbuf, "WPA2 PSK (TKIP, CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
                }
                /* wpapskwpa2psk aes */
                else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (TKIP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
                }
                /* wpapskwpa2psk tkip */
                else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
                }
                /* wpapskwpa2psk tkipaes */
                else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (TKIP, CCMP)" ) != NULL )
                {
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
                }
                continue;
            }
        }
    }
    fclose( fp );

    /* find your need ssid */
    if ( bssid != NULL && *bssid != '\0' )
    {
        child = json_cut_value( tree, bssid );
        talk_free( tree );
        ret = child;
    }
    else if ( ssid == NULL )
    {
        ret = tree;
    }
    else
    {
    	signal = 0;
        child = NULL;
		peermac = NULL;
        while( NULL != ( child = json_each( tree, child ) ) )
        {
        	value = axp_get_value( child );
            s = json_get_string( value, "ssid" );
            if ( s == NULL )
            {
                continue;
            }
            if ( 0 == strcmp( ssid, s ) || 0 == strcmp( ssid2, s ) || 0 == strcmp( ssid3, s ) )
            {
            	if ( good == true )
				{
					s = json_get_string( value, "sig" );
					if ( s != NULL )
					{
						i = atoi( s );
						if ( i > signal )
						{
							signal = i;
							peermac = axp_get_attr( child );
							json_set_string( value, "peermac", peermac );
						}
					}
				}
				else
				{
					peermac = axp_get_attr( child );
				}
            }
        }
		if ( peermac != NULL )
		{
			ret = json_cut_value( tree, peermac );
		}
		talk_free( tree );
    }

	return ret;
}
static int station_dev_connected( const char *object, const char *netdev )
{
	int ret;
	FILE *fp;
	char *ptr;
    char readbuf[256];
	char wpa_cli[256];

	/* get the command result */	
    snprintf( wpa_cli, sizeof(wpa_cli), "/tmp/.wpa_cli_%s_status", netdev );
    if (  shell( "wpa_cli -i %s status > %s", netdev, wpa_cli ) != 0 )
    {
        return 1;
    }
	/* parse the wpa_cli */
    fp = fopen( wpa_cli, "r");
    if( fp == NULL )
    {
        return 1;
    }
	ret = 1;
    readbuf[0] = '\0';
    while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
    {
        if ( strncmp( readbuf, "wpa_state=", 10 ) == 0 )
        {
        	ptr = readbuf+10;
			if ( strncmp( ptr, "COMPLETED", 9 ) == 0 )
			{
				ret = 0;
			}
        }
    }
    fclose( fp );
	return ret;
}



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
	talk_t cfg;
	talk_t opt;
	talk_t ret;
	talk_t value;
	const char *ptr;
	const char *obj;
	const char *radio;
	const char *object;
	const char *netdev;
	const char *ifname;
	const char *status;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;
	const char *peermode;
	const char *strong;
	const char *channel;
	const char *chext;
	const char *secure;
	const char *wpa_encrypt;
	const char *wpa_key;
	char path[PATH_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

	/* get the radio */
	radio = register_pointer( object, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return tfalse;
	}
	/* get the netdev */
	netdev = register_pointer( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	fd = lock_open( path, O_RDWR|O_CREAT|O_EXCL, 0666, -1 );
    if ( fd < 0 )
    {
		info( "%s(%s) already up", object, netdev );
		return ttrue;
    }
	/* up the deivce */
	info( "%s(%s) up", object, netdev );

	ret = tfalse;
	/* get the configure */
	value = cfg = NULL;
	peer = peer2 = peer3 = peermac = NULL;
	opt = param_talk( param, 1 );
	if ( opt != NULL )
	{
		peer = json_string( opt, "peer" );
		peer2 = json_string( opt, "peer2" );
		peer3 = json_string( opt, "peer3" );
		peermac = json_string( opt, "peermac" );
	}
	if ( peer != NULL || peer2 != NULL || peer2 != NULL || peermac != NULL )
	{
		value = opt;
	}
	else
	{
		value = cfg = config_get( this, NULL );
	}
	/* keeplive stop */
	sstop( "%s-keeplive", netdev );
	/* relayd stop */
	sstop( "%s-relayd", netdev );
	/* wpa_supplicant stop */
	sstop( "%s-wpa", netdev );
	/* status[enable/disable]  */
	status = json_string( value, "status" );
	peer = json_string( value, "peer" );
	peer2 = json_string( value, "peer2" );
	peer3 = json_string( value, "peer3" );
	peermac = json_string( value, "peermac" );
	if ( ( status != NULL && 0 == strcmp( status, "disable" ) ) ||
		( ( peer == NULL || *peer == '\0' ) && ( peer2 == NULL || *peer2 == '\0' ) && ( peer == NULL || *peer3 == '\0' ) && ( peermac == NULL || *peermac == '\0' ) ) 
	)
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
		/* get all configure */
		ifname = json_string( value, "ifname" );
		peermode = json_string( value, "peermode" );
		strong = json_string( value, "strong" );
		channel = json_string( value, "channel" );
		chext = json_string( value, "chext" );
		secure = json_string( value, "secure" );
		wpa_encrypt = json_string( value, "wpa_encrypt" );
		wpa_key = json_string( value, "wpa_key" );
		/* save the configure */
		register_set( object, "peer", peer, stringlen(peer)+1, 40 );
		register_set( object, "peer2", peer2, stringlen(peer2)+1, 40 );
		register_set( object, "peer3", peer3, stringlen(peer3)+1, 40 );
		register_set( object, "peermac", peermac, stringlen(peermac)+1, 20 );
		register_set( object, "ifname", ifname, stringlen(ifname)+1, 20 );
		register_set( object, "peermode", peermode, stringlen(peermode)+1, 20 );
		register_set( object, "strong", strong, stringlen(strong)+1, 20 );
		register_set( object, "channel", channel, stringlen(channel)+1, 20 );
		register_set( object, "chext", chext, stringlen(chext)+1, 20 );
		register_set( object, "secure", secure, stringlen(secure)+1, 20 );
		register_set( object, "wpa_encrypt", wpa_encrypt, stringlen(wpa_encrypt)+1, 20 );
		register_set( object, "wpa_key", wpa_key, stringlen(wpa_key)+1, 200 );
		/* up the device */
		sstart( object, "wpa", NULL, "%s-wpa", netdev );
		sstart( object, "keeplive", NULL, "%s-keeplive", netdev );
		ret = ttrue;
		/* mark the up state */
		ptr = uptime_desc( NULL, 0 );
		if ( ptr != NULL )
		{
			write( fd, ptr,	strlen(ptr) );
		}
		lock_close( fd );
	}

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
	project_var_path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	unlink( path );
	
    /* down the deivce */
	if ( netdev != NULL && netdev_flags( netdev, IFF_BROADCAST ) > 0 )
	{
		info( "%s(%s) down", object, netdev );
		/* keeplive stop */
		sstop( "%s-keeplive", netdev );
		/* relayd stop */
		sstop( "%s-relayd", netdev );
		/* wpa_supplicant stop */
		sstop( "%s-wpa", netdev );
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
	talk_t ret;
	const char *prj;
	const char *object;
	const char *netdev;

	prj = obj_prj( this );
	if ( 0 != strcmp( prj, WIFI_PROJECT ) )
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

	/* test the connected */
	ret = tfalse;
	if ( station_dev_connected( object, netdev ) == 0 )
	{
		ret = ttrue;
	}

	return ret;
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
		snprintf( path, sizeof(path), "%s-wpa", netdev );
		if ( spid( path ) >= 0 )
		{
			json_set_string( ret, "state", "uping" );
		}
		else
		{
			json_set_string( ret, "state", "down" );
		}
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
	project_var_path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
    if ( file2string( path, buffer, sizeof(buffer) ) > 0 )
    {
        json_set_string( ret, "ontime", buffer );
        json_set_string( ret, "livetime", livetime_desc( atoi(buffer), path, sizeof(path) ) );
    }

	{
		int i;
		FILE *fp;
		char *ptr;
		char tok[64];
		char readbuf[256];
		char path[PATH_MAX];
		
		/* get the command result */	
		snprintf( path, sizeof(path), "/tmp/.wpa_cli_%s_status", netdev );
		shell( "wpa_cli -i %s status > %s", netdev, path );
		/* parse the wpa_cli */
		fp = fopen( path, "r");
		if( fp != NULL )
		{
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				i = strlen( readbuf );
				readbuf[i-1] = '\0';
				if ( strncmp( readbuf, "wpa_state=", 10 ) == 0 )
				{
					ptr = readbuf+10;
					if ( strcmp( ptr, "SCANNING" ) == 0 )
					{
						json_set_string( ret, "state", "scanning" );
					}
					else if ( strcmp( ptr, "COMPLETED" ) == 0 )
					{
						json_set_string( ret, "state", "up" );
					}
				}
				else if ( strncmp( readbuf, "ssid=", 5 ) == 0 )
				{
					ptr = readbuf+5;
					json_set_string( ret, "peer", ptr );
				}
				else if ( strncmp( readbuf, "bssid=", 6 ) == 0 )
				{
					ptr = readbuf+6;
					lowtoupp( ptr );
					json_set_string( ret, "peermac", ptr );
				}
				else if ( strncmp( readbuf, "address=", 8 ) == 0 )
				{
					ptr = readbuf+8;
					lowtoupp( ptr );
					json_set_string( ret, "mac", ptr );
				}
			}
			fclose( fp );
		}
		/* parse the iwinfo */
		snprintf( path, sizeof(path), "/tmp/.iwinfo_%s_info", netdev );
		shell( "iwinfo %s info > %s", netdev, path );
		fp = fopen( path, "r");
		if( fp != NULL )
		{
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				if ( strstr( readbuf, "Signal:" ) )
				{
					int rssi = 0;
					int sinr = 0;
					i = sscanf( readbuf, "%*[^:]: %d %*[^:]: %d", &rssi, &sinr );
					if ( i >= 1 )
					{
						if ( rssi != 0 )
						{
							json_set_number( ret, "rssi", rssi );
							i = dbm2signal( rssi );
							json_set_number( ret, "signal", i );
						}
						if ( sinr != 0 )
						{
							json_set_number( ret, "sinr", sinr );
						}
					}
				}
				else if ( strstr( readbuf, "Bit Rate:" ) )
				{
					i = sscanf( readbuf, "%*[^:]: %s", tok );
					if ( i == 1 && 0 != strcasecmp( tok, "unknown" ) )
					{
						json_set_string( ret, "rate", tok );
					}
				}
				else if ( (ptr = strstr( readbuf, "Channel:" ) ) != NULL )
				{
					int ch = 0;
					i = sscanf( ptr, "%*[^:]: %d", &ch );
					if ( i == 1 )
					{
						json_set_number( ret, "channel", ch );
					}
				}
			}
			fclose( fp );
		}
	}

	talk_free( cfg );
	return ret;
}
talk_t _state( obj_t this, param_t param )
{
	talk_t ret;

	ret = _status( this, param );
	json_delete_axp( ret, "rx_bytes" );
	json_delete_axp( ret, "rx_packets" );
	json_delete_axp( ret, "rx_errs" );
	json_delete_axp( ret, "rx_drops" );
	json_delete_axp( ret, "tx_bytes" );
	json_delete_axp( ret, "tx_packets" );
	json_delete_axp( ret, "tx_errs" );
	json_delete_axp( ret, "tx_drops" );
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



talk_t _aplist( obj_t this, param_t param )
{
	talk_t ret;
	boole good;
	const char *obj;
    const char *radio;
	const char *object;
	const char *netdev;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;
	const char *strong;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );
	radio = register_value( object, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return NULL;
	}

	/* get the netdev */
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}

	/* get the paramter */
	good = false;
	peer = param_string( param, 1 );
	peermac = param_string( param, 2 );
	peer2 = param_string( param, 3 );
	peer3 = param_string( param, 4 );
	strong = param_string( param, 5 );
	if ( strong != NULL && 0 == strcmp( strong, "enable" ) )
	{
		good = true;
	}

#ifdef DISABLE_HOSTAPD_TO_MODIFY_CHANNEL
	/* stop the hostapd to update the channel */
	sstop( "%s-hostapd", radio );
#endif

	/* scanning */
	ret = station_dev_aplist( netdev, peer, peermac, peer2, peer3, good );

#ifdef DISABLE_HOSTAPD_TO_MODIFY_CHANNEL
	/* start the hostapd */
	sstart( radio, "hostapd", NULL, "%s-hostapd", radio );
#endif

	return ret;
}
#define WPA_SUPPLICANT_DIR "/var/run/wpa_supplicant"
boole_t _wpa( obj_t this, param_t param )
{
	FILE *fp;
    const char *obj;
    const char *radio;
    const char *object;
    const char *netdev;
	const char *ssidctl;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;
	//const char *ifname;
	//const char *peermode;
	//const char *strong;
	//const char *channel;
	//const char *chext;
	const char *secure;
	//const char *wpa_encrypt;
	const char *wpa_key;
	char path[PATH_MAX];
	char pidfile[PATH_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	object = obj_combine( this );
	radio = register_value( object, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return NULL;
	}
	netdev = register_value( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}

	/* get the hostapd ctrl file */
	ssidctl = register_value( radio, "ssidctl" );
    /* get the configure */
	peer = register_value( object, "peer" );
	peer2 = register_value( object, "peer2" );
	peer3 = register_value( object, "peer3" );
	peermac = register_value( object, "peermac" );
	//ifname = register_value( object, "ifname" );
	//peermode = register_value( object, "peermode" );
	//strong = register_value( object, "strong" );
	//channel = register_value( object, "channel" );
	//chext = register_value( object, "chext" );
	secure = register_value( object, "secure" );
	//wpa_encrypt = register_value( object, "wpa_encrypt" );
	wpa_key = register_value( object, "wpa_key" );

	project_var_path( pidfile, sizeof(pidfile), PROJECT_ID, "wpa_supplicant_%s.pid", netdev );
    project_var_path( path, sizeof(path), PROJECT_ID, "wpa_supplicant_%s.conf", netdev );
    fp = fopen( path, "w" );
    if( fp == NULL )
    {
        faulting( "fopen error on %s write", path );
		return terror;
    }
    fprintf( fp, "ap_scan=1\n" );

    /* set the peer */
	if ( peer != NULL && *peer != '\0' )
		{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    //fprintf( fp, "\t\t");
	    //fprintf( fp, "beacon_int=%s\n", beacon );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=3\n" );
	    fprintf( fp, "}\n" );
	}
    /* set the peer2 */
	if ( peer2 != NULL && *peer2 != '\0' )
	{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    //fprintf( fp, "\t\t");
	    //fprintf( fp, "beacon_int=%s\n", beacon );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer3 );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}	
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=2\n" );
	    fprintf( fp, "}\n" );
	}
    /* set the peer3 */
	if ( peer3 != NULL && *peer3 != '\0' )
	{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    //fprintf( fp, "\t\t");
	    //fprintf( fp, "beacon_int=%s\n", beacon );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer3 );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}	
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=1\n" );
	    fprintf( fp, "}\n" );
	}
	
    fclose( fp );


#ifdef DISABLE_HOSTAPD_TO_MODIFY_CHANNEL
	/* stop the hostapd to update the channel */
	sstop( "%s-hostapd", radio );
	ssidctl = NULL;
#endif

    /* exec the wpa_supplicant */
#if defined gPLATFORM__smtk || defined gPLATFORM__mtk || defined gPLATFORM__smtk2 || defined gPLATFORM__mtk2
	if ( ssidctl == NULL )
	{
		debug( "wpa_supplicant -D nl80211 -i %s -c %s -P %s -C %s", netdev, path, pidfile, WPA_SUPPLICANT_DIR );
    	execlp( "wpa_supplicant", "wpa_supplicant", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", WPA_SUPPLICANT_DIR, (char *)0 );
	}
	else
	{
		debug( "wpa_supplicant -D nl80211 -i %s -c %s -P %s -C %s -H %s", netdev, path, pidfile, WPA_SUPPLICANT_DIR, ssidctl );
    	execlp( "wpa_supplicant", "wpa_supplicant", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", WPA_SUPPLICANT_DIR, "-H", ssidctl, (char *)0 );
	}
#else
	if ( ssidctl == NULL )
	{
		debug( "wpa_supplicant -s -D nl80211 -i %s -c %s -P %s -C %s", netdev, path, pidfile, WPA_SUPPLICANT_DIR );
		execlp( "wpa_supplicant", "wpa_supplicant", "-s", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", WPA_SUPPLICANT_DIR, (char *)0 );
	}
	else
	{
		debug( "wpa_supplicant -s -D nl80211 -i %s -c %s -P %s -C %s -H %s", netdev, path, pidfile, WPA_SUPPLICANT_DIR, ssidctl );
		execlp( "wpa_supplicant", "wpa_supplicant", "-s", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", WPA_SUPPLICANT_DIR, "-H", ssidctl, (char *)0 );
	}
#endif
    faulting( "run the wpa_supplicant error" );

    return NULL;
}
boole_t _relayd( obj_t this, param_t param )
{
	const char *ptr;
    const char *obj;
    const char *object;
	const char *netdev;
	talk_t bridge_status;
	char bridge[NAME_MAX];
	char bridge_netdev[NAME_MAX];

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
	/* brdige id */
	ptr = scalls_string( bridge, sizeof(bridge), BRIDGE_COM, "search", object );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* brdige netdev */
	ptr = scalls_string( bridge_netdev, sizeof(bridge_netdev), bridge, "netdev", NULL );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* brdige status */
	bridge_status = scall( bridge, "status", NULL );

	/* run the relayd */
	ptr = json_string( bridge_status, "gw" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		execlp( "relayd", "relayd", "-I", bridge_netdev, "-I", netdev, "-B", "-D", "-G", ptr, (char*)0 );
	}
	else
	{
		execlp( "relayd", "relayd", "-I", bridge_netdev, "-I", netdev, "-B", "-D", (char*)0 );
	}
	faulting( "execlp the relayd(%s) error" , "relayd" );

	talk_free( bridge_status );
	return tfalse;
}
boole_t _keeplive( obj_t this, param_t param )
{
	int i;
	talk_t v;
    const char *obj;
	const char *radio;
    const char *object;
	const char *netdev;
	const char *ifname;
	const char *channel;

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
	/* get the ifname */
	ifname = register_value( object, "ifname" );

start:
    /* first check */
    debug( "check %s(%s) connect state", object, netdev );
    for ( i=0; i<90; i++ )
    {
        if ( station_dev_connected( object, netdev ) == 0 )
        {
            break;
        }
		debug( "%s(%s) connecting", object, netdev );
		sleep( 1 );
    }
	if ( i >= 90 )
	{
		warn( "%s(%s) connecting timeout", object, netdev );
		goto reset;
	}
    info( "%s(%s) connect succeed", object, netdev );
	/* run the relayd */
	sstart( object, "relayd", NULL, "%s-relayd", netdev );

#ifdef DISABLE_HOSTAPD_TO_MODIFY_CHANNEL
	/* start the hostapd */
	v = _status( this, param );
	channel = json_string( v, "channel" );
	if ( channel != NULL )
	{
		register_set( radio, "channel", channel, strlen(channel)+1, 20 );
	}
	talk_free( v );
	sstart( radio, "hostapd", NULL, "%s-hostapd", radio );
#endif

	/* check and check forever */
	i = 0;
	while( 1 )
	{
		if ( station_dev_connected( object, netdev ) == 0 )
		{
			if ( i != 0 )
			{
				i = 0;
				debug( "%s(%s) is connected", object, netdev );
			}
			sleep( 5 );
		}
		else
		{
			if ( i >= 10 )
			{
				info(  "%s(%s) connection lost", object, netdev );
				goto reset;
			}
			else
			{
				debug( "%s(%s) connection broke", object, netdev );
				i++;
				usleep( 200000 );
			}
		}
	}

reset:
	info( "%s(%s) reconnect", object, netdev );
	/* stop the relayd */
	sstop( "%s-relayd", netdev );
	/* down the netdev */
	if ( netdev_flags( netdev, IFF_UP ) > 0 )
	{
		xexecute( 0, 1, "ifconfig %s down", netdev );
	}
	/* reset the wisp ifname */
	if ( ifname != NULL && strstr( ifname, WISP_COM ) != NULL )
	{
		sstop( "%s-wpa", netdev );
		sreset( NULL, NULL, NULL, ifname );
		return ttrue;
	}
	/* reset the wpa to connect */
	sreset( object, "wpa", NULL, "%s-wpa", netdev );
	goto start;

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



