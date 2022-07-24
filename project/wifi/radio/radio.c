/*
 *    Description:  platform IEEE802.11n device management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landwifi/landwifi.h"



boole_t _setup( obj_t this, param_t param )
{
	int i;
	int t;
    talk_t cfg;
    talk_t radio;
    const char *obj;
    const char *object;
	const char *rootdev;
	hp_mac_struct macst;
	char mac[NAME_MAX];
	char nmac[NAME_MAX];
	char netdev[NAME_MAX];
	char compath[NAME_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );

    /* get the radio configure */
    radio = config_get( this, NULL );
	rootdev = register_pointer( object, "netdev" );
	if ( rootdev == NULL || *rootdev == '\0' )
	{
		return tfalse;
	}
	if ( netdev_flags( rootdev, IFF_UP ) <= 0 )
	{
		xshell( "ifconfig %s up", rootdev );
	}

	/* get the mac offset */
	if ( netdev_info( rootdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) ) != 0 )
	{
		sgets_string( mac, sizeof(mac), MACHINE_COM, "mac" );
	}
	string2mac( mac, &macst );

    /* add the ssid and tell the network layer */
	for ( i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "ssid" );
			snprintf( netdev, sizeof(netdev), "%s%s", obj, "ssid" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "ssid", i+1 );
			snprintf( netdev, sizeof(netdev), "%s%s%d", obj, "ssid", i+1 );
		}
		/* check the configure is exist */
		cfg = config_sget( compath, NULL );
		if ( cfg != NULL )
		{
			com_register( compath, "wifi@ap" );
			register_set( compath, "radio", object, strlen(object)+1, 20 );
			register_set( compath, "rootdev", rootdev, strlen(rootdev)+1, 20 );
			register_set( compath, "netdev", netdev, strlen(netdev)+1, 20 );
			/* create the netdev */
			xshell( "iw dev %s interface add %s type managed", rootdev, netdev );
			t = netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, nmac, sizeof(nmac) );
			if ( t != 0 || 0 == strcmp( nmac, mac ) )
			{
				/* modify the mac */
				mac2add( &macst, 1 );
				mac2string( &macst, mac );
				xshell( "ifconfig %s hw ether %s", netdev, mac );
				xshell( "ifconfig %s up", netdev );
			}
			/* tell the network layer */
			info( "%s(%s) add to network frame", compath, netdev );
			scalls( NETWORK_COM, "add", "%s,%s", compath, netdev );
			talk_free( cfg );
		}
	}

    /* add the sta and tell the network layer */
	snprintf( compath, sizeof(compath), "%s%s", object, "sta" );
	snprintf( netdev, sizeof(netdev), "%s", rootdev );
	/* check the configure is exist */
	cfg = config_sget( compath, NULL );
	if ( cfg != NULL )
	{
		com_register( compath, "wifi@sta" );
		register_set( compath, "radio", object, strlen(object)+1, 20 );
		register_set( compath, "rootdev", rootdev, strlen(rootdev)+1, 20 );
		register_set( compath, "netdev", netdev, strlen(netdev)+1, 20 );
		/* create the netdev */
		//mac2add( &macst, 1 );
		//mac2string( &macst, mac );
		//xshell( "iw dev %s interface add %s type station", rootdev, netdev );
		//xshell( "ifconfig %s hw ether %s", netdev, mac );
		//xshell( "ifconfig %s up", netdev );
		/* tell the network layer */
		info( "%s(%s) add to network frame", compath, netdev );
		scalls( NETWORK_COM, "add", "%s,%s", compath, netdev );
		talk_free( cfg );
	}

    talk_free( radio );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	int i;
	talk_t cfg;
	const char *obj;
	const char *object;
	const char *rootdev;
	char name[NAME_MAX];
	char netdev[NAME_MAX];
	char compath[NAME_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return tfalse;
	}
	object = obj_combine( this );
	rootdev = register_pointer( object, "netdev" );
	if ( rootdev == NULL || *rootdev == '\0' )
	{
		return tfalse;
	}

    /* stop the hostapd */
	snprintf( name, sizeof(name), "%s-hostapd", object );
    service_stop( name );

	/* add the ssid/sta and tell the network layer */
	for ( i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "ssid" );
			snprintf( netdev, sizeof(netdev), "%s%s", obj, "ssid" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "ssid", i+1 );
			snprintf( netdev, sizeof(netdev), "%s%s%d", obj, "ssid", i+1 );
		}
		
		/* check the configure is exist */
		cfg = config_sget( compath, NULL );
		if ( cfg != NULL )
		{
			/* tell the network layer */
			info( "%s delete from network frame", compath );
			scalls( NETWORK_COM, "delete", compath );
			/* take over the component */
			com_unregister( compath );
			talk_free( cfg );
			shell( "ifconfig %s 0.0.0.0", netdev );
		}
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "sta" );
			snprintf( netdev, sizeof(netdev), "%s%s", obj, "sta" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "sta", i+1 );
			snprintf( netdev, sizeof(netdev), "%s%s%d", obj, "sta", i+1 );
		}
		/* check the configure is exist */
		cfg = config_sget( compath, NULL );
		if ( cfg != NULL )
		{
			/* tell the network layer */
			info( "%s delete from network frame", compath );
			scalls( NETWORK_COM, "delete", compath );
			/* take over the component */
			com_unregister( compath );
			talk_free( cfg );
			shell( "ifconfig %s 0.0.0.0", netdev );
		}
	}

	return ttrue;
}
talk_t _stalist( obj_t this, param_t param )
{
	int i;
	talk_t x;
	talk_t ret;
    const char *obj;
	const char *object;
	char compath[NAME_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	object = obj_combine( this );

	/* get ap stalist */
	ret = json_create( NULL );
	for ( i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "ssid" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "ssid", i+1 );
		}
		x = scall( compath, "stalist", param );
		if ( x != NULL )
		{
			talk_patch( x, ret );
			talk_free( x );
		}
	}
	return ret;
}
talk_t _stabeat( obj_t this, param_t param )
{
	int i;
	talk_t x;
	talk_t ret;
    const char *obj;
	const char *object;
	char compath[NAME_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	object = obj_combine( this );

	/* get ap stalist */
	ret = json_create( NULL );
	for ( i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "ssid" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "ssid", i+1 );
		}
		x = scall( compath, "stabeat", param );
		if ( x == ttrue )
		{
			ret = ttrue;
		}
	}
	return ret;
}
talk_t _chlist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ptr;
    const char *obj;
    char mode[NAME_MAX];
    char country[NAME_MAX];

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	/* get the mode */
	ptr = param_string( param, 1 );
	if ( ptr != NULL )
	{
		strncpy( mode, ptr, sizeof(mode) );
	}
	else
	{
	    ptr = config_gets_string( mode, sizeof(mode), this, "mode" );
	    if ( ptr == NULL )
	    {
	        return NULL;
	    }
	}
    /* get the country */
	ptr = param_string( param, 2 );
	if ( ptr != NULL )
	{
		strncpy( country, ptr, sizeof(country) );
	}
	else
	{
	    ptr = config_gets_string( country, sizeof(country), this, "country" );
	    if ( ptr == NULL )
	    {
	        return NULL;
	    }
	}
	
    /* get the channel list */
	if ( NULL == strstr( mode, "a" ) )
	{
		ret = country2chlist( country, 0 );
	}
	else
	{
		ret = country2chlist( country, 1 );
	}
	
	return ret;
}
talk_t _options( obj_t this, param_t param )
{
    const char *obj;
	const char *object;
	const char *drvcom;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return NULL;
	}
	object = obj_combine( this );
	drvcom = register_pointer( object, "drvcom" );
	if ( drvcom == NULL || *drvcom == '\0' )
	{
		return NULL;
	}
	return scalls( drvcom, "options", object );
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
	/* set the configur value */
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







#define HOSTAPD_DIR "/var/run/hostapd"
boole_t _hostapd( obj_t this, param_t param )
{
	int i;
    FILE *fp;
	boole run;
	talk_t cfg;
	talk_t axp;
	talk_t radio;
	talk_t value;
	const char *ptr;
	const char *val;
	const char *obj;
	const char *object;
	const char *netdev;
	const char *rootdev;
	const char *drvcom;
	struct stat st;
    char mac[NAME_MAX];
	char path[PATH_MAX];
	char apath[PATH_MAX];
	char dpath[PATH_MAX];
	char compath[NAME_MAX];
	char ssidctl[PATH_MAX];
	char globalctl[PATH_MAX];
	talk_t hstinfo;
	talk_t hstopt;
	talk_t hstopt_n;
	talk_t hstopt_a;
	talk_t hstopt_ac;
	talk_t hstval_ht;
	talk_t hstval_vht;
	const char *mode;
	const char *country;
    const char *extcha;
    const char *channel;
    const char *bandwidth;
    const char *shortgi;
    const char *stbc;
    const char *ldpc;

	obj = obj_com( this );
	if ( 0 == strcmp( obj, COM_ID ) )
	{
		return terror;
	}
	object = obj_combine( this );
	rootdev = register_pointer( object, "netdev" );
	if ( rootdev == NULL || *rootdev == '\0' )
	{
		return terror;
	}
	drvcom = register_pointer( object, "drvcom" );
	if ( drvcom == NULL || *drvcom == '\0' )
	{
		return terror;
	}
	/* get the hostapd configure */
	hstinfo = scalls( drvcom, "hostapd_config", object );
	hstopt = json_value( hstinfo, "value" );
	hstval_ht = json_value( hstopt, "ht_capab" );
	hstval_vht = json_value( hstopt, "vht_capab" );
	hstopt = json_value( hstinfo, "options" );
	hstopt_n = json_value( hstopt, "n" );
	hstopt_a = json_value( hstopt, "a" );
	hstopt_ac = json_value( hstopt, "ac" );
	/* open the configure */
    project_var_path( path, sizeof(path), PROJECT_ID, "hostapd_%s.conf", obj );
    fp = fopen( path, "w" );
    if( fp == NULL )
    {
        faulting( "fopen error on %s write", path );
		return terror;
    }
	run = false;



	/* create the hostapd dir */
	if ( stat( HOSTAPD_DIR, &st ) != 0 )
	{
		mkdir( HOSTAPD_DIR, 0775 );
	}
    project_var_path( globalctl, sizeof(globalctl), PROJECT_ID, "hostapd_%s.ctrl", obj );
    /* set the log */
    fprintf( fp, "driver=nl80211\n" );
    fprintf( fp, "logger_syslog=127\n" );
    fprintf( fp, "logger_syslog_level=2\n" );
    fprintf( fp, "logger_stdout=127\n" );
    fprintf( fp, "logger_stdout_level=2\n" );
    fprintf( fp, "\n" );
    /* get the radio configure */
    radio = config_sget( object, NULL );



    /* get the wifi mode */
    mode = json_string( radio, "mode" );
	if ( mode == NULL || *mode == '\0' )
	{
		if ( hstopt_n != NULL ) mode = "n";
		else if ( hstopt_a != NULL ) mode = "a";
		else if ( hstopt_ac != NULL ) mode = "ac";
		else mode = "n";
	}
    /* get the radio country */
    country = json_string( radio, "country" );
	if ( country == NULL || *country == '\0' )
	{
		country = "cn";
	}
    /* get the radio channel */
	channel = register_pointer( object, "channel" );
	if ( channel == NULL || *channel == '\0' )
	{
	    channel = json_string( radio, "channel" );
		if ( channel == NULL || *channel == '\0' )
		{
			channel = "1";
		}
	}
    extcha = json_string( radio, "extcha" );
    bandwidth = json_string( radio, "bandwidth" );
	if ( bandwidth == NULL || *bandwidth == '\0' )
	{
		bandwidth = "20";
	}
    /* get the wifi ldpc */
    ldpc = json_string( radio, "ldpc" );
    /* get the radio stbc */
    stbc = json_string( radio, "stbc" );
    /* get the radio shortgi */
    shortgi = json_string( radio, "shortgi" );
	/* set the country */
    if ( country != NULL && *country != '\0' )
    {
		strncpy( mac, country, sizeof(mac) );
		lowtoupp(mac);
        fprintf( fp, "country_code=%s\n", mac );
        fprintf( fp, "ieee80211d=1\n" );
		if ( mode != NULL && NULL != strstr( mode, "a" ) )
		{
        	fprintf( fp, "ieee80211h=1\n" );
		}
    }



	/* 11A, 11AC capab option */
	if ( mode != NULL && NULL != strstr( mode, "a" ) )
	{
		fprintf( fp, "hw_mode=a\n" );
		fprintf( fp, "ieee80211n=1\n" );
		// ht_capab
		fprintf( fp, "ht_capab=" );
		wireless_11a_adjustment( fp, channel, extcha, bandwidth, country, shortgi );
		/* stbc */
		if ( stbc != NULL && 0 == strcmp( stbc, "enable" ) )
		{
			ptr = json_string( hstval_ht, "stbc" );
			if ( ptr != NULL )
			{
				fprintf( fp, "%s", ptr );
			}
		}
		/* ldpc */
		if ( ldpc != NULL && 0 == strcmp( ldpc, "enable" ) )
		{
			ptr = json_string( hstval_ht, "ldpc" );
			if ( ptr != NULL )
			{
				fprintf( fp, "%s", ptr );
			}
		}
		/* ht_capab add */
		ptr = json_string( hstopt, "ht_capab" );
		if ( ptr != NULL )
		{
			fprintf( fp, "%s", ptr );
		}
		fprintf( fp, "\n" );
		// vht_capab
		if ( mode != NULL && NULL != strstr( mode, "ac" ) )
		{
			fprintf( fp, "ieee80211ac=1\n" );		
			wireless_11ac_adjustment( fp, channel, extcha, bandwidth, country, shortgi );
			/* stbc */
			if ( stbc != NULL && 0 == strcmp( stbc, "enable" ) )
			{
				ptr = json_string( hstval_vht, "stbc" );
				if ( ptr != NULL )
				{
					fprintf( fp, "%s", ptr );
				}
			}
			if ( ldpc != NULL && 0 == strcmp( ldpc, "enable" ) )
			{
				ptr = json_string( hstval_vht, "ldpc" );
				if ( ptr != NULL )
				{
					fprintf( fp, "%s", ptr );
				}
			}
			/* vht_capab add */
			ptr = json_string( hstopt, "vht_capab" );
			if ( ptr != NULL )
			{
				fprintf( fp, "%s", ptr );
			}
			fprintf( fp, "\n" );
			/* 11ac options */
			axp = NULL;
			while( NULL != ( axp = json_next( hstopt_ac, axp ) ) )
			{
				ptr = axp_id( axp );
				val = axp_string( axp );
				fprintf( fp, "%s=%s\n", ptr, val );
			}
		}
		else
		{
			/* 11a options */
			axp = NULL;
			while( NULL != ( axp = json_next( hstopt_a, axp ) ) )
			{
				ptr = axp_id( axp );
				val = axp_string( axp );
				fprintf( fp, "%s=%s\n", ptr, val );
			}
		}
	}
	/* 11N capab option */
	else
	{
		fprintf( fp, "hw_mode=g\n" );
		fprintf( fp, "ieee80211n=1\n" );
		// ht_capab std
		fprintf( fp, "ht_capab=" );
		wireless_11n_adjustment( fp, channel, extcha, bandwidth, country, shortgi );
		/* stbc */
		if ( stbc != NULL && 0 == strcmp( stbc, "enable" ) )
		{
			ptr = json_string( hstval_ht, "stbc" );
			if ( ptr != NULL )
			{
				fprintf( fp, "%s", ptr );
			}
		}
		/* ldpc */
		if ( ldpc != NULL && 0 == strcmp( ldpc, "enable" ) )
		{
			ptr = json_string( hstval_ht, "ldpc" );
			if ( ptr != NULL )
			{
				fprintf( fp, "%s", ptr );
			}
		}
		/* ht_capab add */
		ptr = json_string( hstopt, "ht_capab" );
		if ( ptr != NULL )
		{
			fprintf( fp, "%s", ptr );
		}
		fprintf( fp, "\n" );
		/* 11n options */
		axp = NULL;
		while( NULL != ( axp = json_next( hstopt_n, axp ) ) )
		{
			ptr = axp_id( axp );
			val = axp_string( axp );
			fprintf( fp, "%s=%s\n", ptr, val );
		}
	}
	fprintf( fp, "\n" );


	
	/* set the raido common */
    ptr = json_string( radio, "rts_threshold" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( fp, "rts_threshold=%s\n", ptr );
    }
    ptr = json_string( radio, "frag_threshold" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( fp, "fragm_threshold=%s\n", ptr );
    }
    ptr = json_string( radio, "beacon" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( fp, "beacon_int=%s\n", ptr );
    }
    ptr = json_string( radio, "dtim" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( fp, "dtim_period=%s\n", ptr );
    }
	/* channel */
    if ( channel != NULL && *channel != '\0' && atoi(channel) != 0 )
    {
        fprintf( fp, "channel=%s\n", channel );
        //fprintf( fp, "chanlist=%s\n", channel ); comment for apclient switch the channel to connect
    }
    else
    {
        fprintf( fp, "channel=acs_survey\n" );
    }
    fprintf( fp, "\n" );
    fprintf( fp, "\n" );



	/* add the ssid to configure */
	for ( i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			snprintf( compath, sizeof(compath), "%s%s", object, "ssid" );
			snprintf( ssidctl, sizeof(ssidctl), "%s/%s%s", HOSTAPD_DIR, obj, "ssid" );
		}
		else
		{
			snprintf( compath, sizeof(compath), "%s%s%d", object, "ssid", i+1 );
			snprintf( ssidctl, sizeof(ssidctl), "%s/%s%s%d", HOSTAPD_DIR, obj, "ssid", i+1 );
		}
		netdev = register_pointer( compath, "netdev" );
		if ( netdev == NULL || *netdev == '\0' )
		{
            continue;
		}
        if ( netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) ) != 0 )
        {
            continue;
        }
		/* get the configure */
		cfg = config_sget( compath, NULL );
		if ( cfg == NULL )
		{
			continue;
		}
		/* status[ disable, enable ] */
        ptr = json_string( cfg, "status" );
        if ( ptr == NULL || 0 != strcmp( ptr, "enable" ) )
        {
        	talk_free( cfg );
            continue;
        }
		run = true;
		/* set the interface */
		fprintf( fp, "interface=%s\n", netdev );
		fprintf( fp, "ctrl_interface=%s\n", HOSTAPD_DIR );
		register_set( object, "ssidctl", ssidctl, strlen(ssidctl)+1, 80 );
        ptr = json_string( cfg, "isolated" );
        if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
        {
            fprintf( fp, "ap_isolate=1\n" );
        }
        ptr = json_string( cfg, "broadcast" );
        if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
        {
            fprintf( fp, "ignore_broadcast_ssid=0\n" );
        }
        else
        {
            fprintf( fp, "ignore_broadcast_ssid=1\n" );
        }
        fprintf( fp, "preamble=1\n" );
        ptr = json_string( cfg, "wmm" );
        if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
        {
            fprintf( fp, "wmm_enabled=1\n" );
            fprintf( fp, "uapsd_advertisement_enabled=1\n" );
        }
        /* secure */
        ptr = json_string( cfg, "secure" );
        if ( ptr != NULL && (  0 == strcmp( ptr, "wpapsk" ) || 0 == strcmp( ptr, "wpa2psk" ) || 0 == strcmp( ptr, "wpapskwpa2psk" ) ) )
        {
            if ( 0 == strcmp( ptr, "wpapsk" ) )
            {
            	fprintf( fp, "eapol_version=1\n" );
				fprintf( fp, "auth_algs=1\n" );
                fprintf( fp, "wpa=1\n" );
            }
            else if ( 0 == strcmp( ptr, "wpa2psk" ) )
            {
                fprintf( fp, "wpa=2\n" );
				fprintf( fp, "auth_algs=1\n" );
                fprintf( fp, "okc=0\n" );
                fprintf( fp, "disable_pmksa_caching=1\n" );
            }
            else
            {
            	fprintf( fp, "eapol_version=1\n" );
                fprintf( fp, "wpa=3\n" );
				fprintf( fp, "auth_algs=1\n" );
                fprintf( fp, "okc=0\n" );
                fprintf( fp, "disable_pmksa_caching=1\n" );
            }
            fprintf( fp, "wpa_key_mgmt=WPA-PSK\n" );
            ptr = json_string( cfg, "wpa_encrypt" );
            if ( ptr != NULL && 0 == strcmp( ptr, "aes" ) )
            {
                fprintf( fp, "wpa_pairwise=CCMP\n" );
            }
            else if ( ptr != NULL && 0 == strcmp( ptr, "tkip" ) )
            {
                fprintf( fp, "wpa_pairwise=TKIP\n" );
            }
            else
            {
                fprintf( fp, "wpa_pairwise=CCMP TKIP\n" );
            }
            ptr = json_string( cfg, "wpa_key" );
            fprintf( fp, "wpa_passphrase=%s\n", ptr );
            ptr = json_string( cfg, "wpa_rekey" );
            if ( ptr != NULL && atoi( ptr ) > 0 )
            {
                fprintf( fp, "wpa_group_rekey=%s\n", ptr );
                fprintf( fp, "wpa_gmk_rekey=%s\n", ptr  );
            }
			//fprintf( fp, "wpa_disable_eapol_key_retries=0\n" );
        }

        /* ssid */
        ptr = json_string( cfg, "ssid" );
        fprintf( fp, "ssid=%s\n", ptr );
        fprintf( fp, "utf8_ssid=1\n" );
		/* ssid options */
		axp = NULL;
		hstopt = json_value( cfg, "hostapd_options" );
		while( NULL != ( axp = json_next( hstopt, axp ) ) )
		{
			ptr = axp_id( axp );
			val = axp_string( axp );
			fprintf( fp, "%s=%s\n", ptr, val );
		}
        /* brdige id */
        ptr = scalls_string( NULL, 0, BRIDGE_COM, "search", compath );
        if ( ptr != NULL )
        {
        	ptr = scalls_string( NULL, 0, ptr, "netdev", NULL );
			if ( ptr != NULL )
			{
				fprintf( fp, "bridge=%s\n", ptr );
			}
        }
		/* wds */        
        ptr = json_string( cfg, "wds" );
        if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
        {
            fprintf( fp, "wds_sta=1\n" );
        }
        /* ACL action */
        ptr = json_string( cfg, "acl" );
        if ( ptr != NULL && 0 == strcasecmp( ptr, "accept" ) )
        {
            fprintf( fp, "macaddr_acl=1\n" );        
            project_var_path( apath, sizeof(apath), PROJECT_ID, "hostapd.%s_accept", netdev );
            unlink( apath );
            fprintf( fp, "accept_mac_file=%s\n", apath );
            value = json_value( cfg, "acl_table" );
            if ( value != NULL )
            {
                axp = NULL;
                while( NULL != ( axp = json_next( value, axp ) ) )
                {
                    ptr = axp_id( axp );
                    if ( ptr != NULL && *ptr != '\0' )
                    {
                        string3file( apath, "%s\n", ptr );
                    }
                }
            }
        }
        else if ( ptr != NULL && 0 == strcasecmp( ptr, "drop" ) )
        {
            fprintf( fp, "macaddr_acl=0\n" );        
            project_var_path( dpath, sizeof(dpath), PROJECT_ID, "hostapd.%s_deny", netdev );
            unlink( dpath );
            fprintf( fp, "deny_mac_file=%s\n", dpath );
            value = json_value( cfg, "acl_table" );
            if ( value != NULL )
            {
                axp = NULL;
                while( NULL != ( axp = json_next( value, axp ) ) )
                {
                    ptr = axp_id( axp );
                    if ( ptr != NULL && *ptr != '\0' )
                    {
                        string3file( dpath, "%s\n", ptr );
                    }
                }
            }
        }        
        /* Max Sta */
        ptr = json_string( cfg, "maxsta" );
        if ( ptr != NULL && *ptr != '\0' )
        {
            fprintf( fp, "max_num_sta=%s\n", ptr );        
        }
        fprintf( fp, "\n\n" );        
        talk_free( cfg );

	}
    fclose( fp );
	talk_free( radio );
	talk_free( hstinfo );


	if ( run == false )
	{
		pause();
	}
	else
	{
		/* exec the hostapd */
#if defined gPLATFORM__smtk2 || defined gPLATFORM__mtk2
		execlp( "hostapd", "hostapd", "-g", globalctl, path, (char *)0 );
#else
		execlp( "hostapd", "hostapd", "-s", "-g", globalctl, path, (char *)0 );
#endif
	}
	return tfalse;
}




