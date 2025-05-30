/*
 *    Description:  ip connection
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landnet/landnet.h"
#include <ifaddrs.h>



boole_t _setup( obj_t this, param_t param )
{
	int tid;
    talk_t cfg;
    const char *ptr;
    const char *object;
	const char *ifdev;

    object = obj_combine( this );
    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return ttrue;
	}
    /* exit when the status is disable */
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
    {
        talk_free( cfg );
		return ttrue;
    }
	/* set the tid */
	ptr = json_string( cfg, "tid" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		tid = atoi( ptr );
		register_set( object, "tid", &tid, sizeof(tid), 0 );
	}
    /* get the ifdev */
	ifdev = register_pointer( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	/* need the ifdev exist */
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}

    /* run the app connection */
    info( "%s startup", object );
	sstart( object, "service", NULL, object );
    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
    const char *object;
	const char *ifdev;
    char path[PATH_MAX];

    object = obj_combine( this );
    info( "%s shut", object );
    /* call the offline */
    scalls( NETWORK_COM, "offline", object );

    /* stop the keeplive service */
    sdelete( "%s-keeplive", object );
    /* stop the automatic service */
    sdelete( "%s-automatic", object );
    /* stop the service */
    sdelete( object );
    /* delete online file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    unlink( path );
    /* delete upline file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
    unlink( path );
    /* down the ifdev */
	ifdev = register_value( object, "ifdev" );
    if ( ifdev != NULL && *ifdev != '\0' )
    {
    	scall( ifdev, "down", NULL );
		scalls( GPIO_COM, "action", "network/offline,%s", ifdev );
	}

    return ttrue;
}
talk_t _ifdev( obj_t this, param_t param )
{
    const char *object;
	const char *ifdev;

    object = obj_combine( this );
    /* get the ifdev */
	ifdev = register_value( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }

    return string2x( ifdev );
}
talk_t _netdev( obj_t this, param_t param )
{
    const char *object;
	const char *ifdev;
	const char *netdev;

    object = obj_combine( this );
    /* get the netdev */
	netdev = register_value( object, "netdev" );
    if ( netdev != NULL && *netdev != '\0' )
    {
    	return string2x( netdev );
    }
    /* get the ifdev */
	ifdev = register_value( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }
    /* get the ifdev netdev */
	netdev = register_value( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        return NULL;
    }

    return string2x( netdev );
}
talk_t _state( obj_t this, param_t param )
{
	int delay;
    talk_t ret;
    talk_t v;
	const int *tid;
    struct stat st;
    boole keeplive;
    const char *ptr;
    const char *object;
    const char *ifdev;
    const char *netdev;
	const char *device;
	const char *mode;
	const char *method;
	const char *custom_dns;
	const char *dns;
	const char *dns2;
	const char *custom_resolve;
	const char *resolve;
	const char *resolve2;
    char path[PATH_MAX];
	const int *reg_delay = NULL;

    object = obj_combine( this );
	netdev = device = NULL;
	/* get the ifdev */
	ifdev = register_value( object, "ifdev" );
    /* get the keeplive */
	ptr = register_value( object, "keeplive" );
    if ( ptr != NULL && ( 0 == strcmp( ptr, "icmp" ) || 0 == strcmp( ptr, "dns" ) || 0 == strcmp( ptr, "auto" ) ) )
    {
    	keeplive = true;
		register2int( object, "delay", reg_delay, delay, 0 );
    }
	else
	{
		keeplive = false;
	}
	/* get mode */
	tid = register_value( object, "tid" );
	mode = register_value( object, "mode" );
	method = register_value( object, "method" );
    /* get the custom_dns */
	custom_dns = register_value( object, "custom_dns" );
	dns = register_value( object, "dns" );
	dns2 = register_value( object, "dns2" );
	custom_resolve = register_value( object, "custom_resolve" );
	resolve = register_value( object, "resolve" );
	resolve2 = register_value( object, "resolve2" );

    /* get the ipv4 online status */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    if ( stat( path, &st ) != 0 )
    {
        ret = json_create( NULL );
		if ( ifdev != NULL && *ifdev != '\0' )
		{
			json_set_string( ret, "ifdev", ifdev );
			/* get the netdev */
			netdev = register_value( ifdev, "netdev" );
			if ( netdev != NULL && *netdev != '\0' )
			{
				json_set_string( ret, "netdev", netdev );
				/* get the mac */
				if ( netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, path, sizeof(path) ) == 0 )
				{
					json_set_string( ret, "mac", path );
				}
			}
		}
        if ( spid( object ) >= 0 )
        {
            json_set_string( ret, "status", "uping" );
        }
        else
        {
            json_set_string( ret, "status", "down" );
        }
    }
    else
    {
        char ip[20];
		char mac[20];
        char dstip[20];
        char mask[20];
        unsigned long long rt_bytes, rt_packets, rt_errs, rt_drops, tt_bytes, tt_packets, tt_errs, tt_drops;
        ip[0] = dstip[0] = mask[0] = mac[0] = '\0';
        rt_bytes = rt_packets = rt_errs = rt_drops = tt_bytes = tt_packets = tt_errs = tt_drops = 0;
        ret = file2talk( path );
        netdev = device = json_string( ret, "netdev" );
		if ( NULL == strstr( device, "ppp" ) )
		{
			netdev_info( device, ip, sizeof(ip), NULL, 0, mask, sizeof(mask), mac, sizeof(mac) );
		}
		else
		{
			netdev_info( device, ip, sizeof(ip), dstip, sizeof(dstip), mask, sizeof(mask), mac, sizeof(mac) );
		}
        if ( netdev_flags( device, IFF_UP ) <= 0 || *ip == '\0' )
        {
            if ( spid( object ) >= 0 )
            {
                json_set_string( ret, "status", "uping" );
            }
            else
            {
                json_set_string( ret, "status", "down" );
            }
        }
        else
        {
            json_set_string( ret, "status", "up" );
            json_set_string( ret, "ip", ip );
            json_set_string( ret, "mask", mask );
			if ( *dstip != '\0' )
			{
				json_set_string( ret, "dstip", dstip );
			}
            /* custom dns */
			if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
			{
				if ( dns != NULL && *dns != '\0' )
				{
					json_set_string( ret, "dns", dns );
				}
				if ( dns2 != NULL && *dns2 != '\0' )
				{
					json_set_string( ret, "dns2", dns2 );
				}
			}
            /* get the keeplive status */
            if ( keeplive == true )
            {
				if ( delay > 0 )
				{
                    json_set_number( ret, "delay", delay );
				}
				else if ( delay < 0 )
				{
                    json_set_string( ret, "delay", "failed" );
				}
				else
				{
                    json_set_string( ret, "delay", "block" );
				}
            }
            /* get the livetime */
			ptr = json_string( ret, "ontime" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				json_set_string( ret, "livetime", livetime_desc( atoll(ptr), path, sizeof(path) ) );
			}
			/* get the flow */
			netdev_flew( device, &rt_bytes, &rt_packets, &rt_errs, &rt_drops, &tt_bytes, &tt_packets, &tt_errs, &tt_drops );
			snprintf( path, sizeof(path), "%llu", rt_bytes );
			json_set_string( ret, "rx_bytes", path );
			snprintf( path, sizeof(path), "%llu", rt_packets );
			json_set_string( ret, "rx_packets", path );
			snprintf( path, sizeof(path), "%llu", tt_bytes );
			json_set_string( ret, "tx_bytes", path );
			snprintf( path, sizeof(path), "%llu", tt_packets );
			json_set_string( ret, "tx_packets", path );
        }
		/* get the mac */
		if ( 0 == strcmp( mac, "00:00:00:00:00:00" ) || 0 == strcasecmp( mac, "ff:ff:ff:ff:ff:ff" ) ) // ppp interface mac
		{
			/* get the netdev */
			netdev = register_value( ifdev, "netdev" );
			if ( netdev != NULL && *netdev != '\0' )
			{
				netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) );
			}
		}
		json_set_string( ret, "mac", mac );
    }

    /* get the mode of configure */
	if ( tid != NULL && *tid != 0 )
	{
		json_set_number( ret, "tid", *tid );
	}
	if ( mode != NULL && *mode != '\0' )
	{
		json_set_string( ret, "mode", mode );
	}

    /* get the ipv6 online status */
	if ( method != NULL && *method != '\0' && 0 != strcmp( method, "disable" ) )
	{
		int t;
		int rc;
		char *end;
		char host[NI_MAXHOST];
		struct ifaddrs *ifaddr, *ifa;

		json_set_string( ret, "method", method );
	    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
		v = file2talk( path );
	    json_patch( v, ret );
		talk_free( v );
		if ( netdev != NULL && *netdev != '\0' )
		{
			if ( getifaddrs( &ifaddr ) == 0 )
			{
				t = 1;
				for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
				{
					if ( ifa->ifa_addr == NULL )
					{
						continue;
					}
					if ( ifa->ifa_addr->sa_family != AF_INET6 )
					{
						continue;
					}
					if ( 0 != strcmp( ifa->ifa_name, netdev ) )
					{
						continue;
					}
					rc = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
					if ( rc == 0 )
					{
						if ( t <= 1 )
						{
							strcpy( path, "addr" );
						}
						else
						{
							sprintf( path, "addr%d", t );
						}
						end = strstr( host, "%" );
						if ( end != NULL )
						{
							*end = '\0';
						}
						json_set_string( ret, path, host );
						t++;
						//printf("dev: %-8s address: <%s> scope %d\n", ifa->ifa_name, host, in6->sin6_scope_id);
					}
				}
				freeifaddrs(ifaddr);					
			}
		}
		if ( custom_resolve != NULL && 0 == strcmp( custom_resolve, "enable" ) )
		{
			if ( resolve != NULL && *resolve != '\0' )
			{
				json_set_string( ret, "resolve", resolve );
			}
			if ( resolve2 != NULL && *resolve2 != '\0' )
			{
				json_set_string( ret, "resolve2", resolve2 );
			}
		}
	}

    return ret;
}
talk_t _status( obj_t this, param_t param )
{
	talk_t v;
	talk_t ret;
	const char *ptr;
	const char *ifdev;
	const char *object;

	ret = _state( this, param );
	if ( ret == NULL )
	{
		return NULL;
	}
	object = obj_combine( this );
	/* get the ifdev */
	ifdev = register_value( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }

    /* get the ifdev or main ifdev info */
	if ( com_sexist( ifdev, "state" ) == true )
	{
		v = scalls( ifdev, "state", object );
        if ( v > TALK_ECODEMAX )
        {
            ptr = json_string( ret, "netdev" );
            if ( ptr != NULL && *ptr != '\0' )
            {
                json_delete_axp( v, "netdev" );
            }
            talk_patch( v, ret );
            ptr = json_string( v, "state" );
            if ( ptr != NULL && 0 != strcmp( ptr, "connect" ) && 0 != strcmp( ptr, "up" ) )
            {
                json_set_string( ret, "status", ptr );
            }
            talk_free( v );
        }
    }
    return ret;
}

/* only for apclient */
talk_t _aplist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;
	const char *object;

	object = obj_combine( this );
	ret = NULL;
	/* get the ifdev */
	ifdev = register_value( object, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "aplist", NULL );
	}
	return ret;
}
/* only for apclient */
talk_t _chlist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;
	const char *object;

	object = obj_combine( this );
	ret = NULL;
	/* get the ifdev */
	ifdev = register_value( object, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "chlist", NULL );
	}
	return ret;
}



boole_t _service( obj_t this, param_t param )
{
    int ready;
    int check;
    talk_t v;
	talk_t ret;
    talk_t cfg;
    talk_t value;
    const char *ptr;
	const char *mode;
    const char *object;
	const char *ifdev;
	const char *netdev;
	const char *method;
	int connect_failed;
	int failed_timeout;
	int failed_threshold;
	int failed_threshold2;
	int failed_everytime;
	int *reg_connect_failed;

    object = obj_combine( this );
    /* offline to clear first */
    if ( scalls( NETWORK_COM, "extern", object ) == ttrue )
    {
        scalls( NETWORK_COM, "offline", object );
    }
	/* get the ifdev */
	ifdev = register_pointer( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
		fault( "cannot found %s ifdev", object );
		sleep( 5 );
        return tfalse;
    }
	if ( com_sexist( ifdev, NULL ) == false )
	{
		fault( "ifdev %s inexistence", ifdev );
		sleep( 5 );
        return tfalse;
	}

    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
		fault( "cannot found %s configure", object );
    	return terror;
    }
	failed_timeout = 45;
	failed_threshold = 3;
	failed_threshold2 = 15;
	failed_everytime = 24;
	ptr = json_string( cfg, "failed_timeout" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_timeout = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	mode = json_string( cfg, "mode" );
	if ( mode == NULL || *mode == '\0' )
	{
		mode = "dhcpc";
	}
	register_set( object, "mode", mode, strlen(mode)+1, 20 );
	method = json_string( cfg, "method" );
	if ( method == NULL || *method == '\0' )
	{
		method = "disable";
	}
	/* disable the method when ppp mode */
	if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
	{
		method = "disable";
	}
	register_set( object, "method", method, strlen(method)+1, 20 );

	/* get the count */
	ret = ttrue;
	connect_failed = 0;
	reg_connect_failed = register_pointer( object, "connect_failed" );
	if ( reg_connect_failed != NULL )
	{
		connect_failed = *reg_connect_failed;
	}
	if ( connect_failed > 0 )
	{
		if ( connect_failed == failed_threshold )
		{
			ret = terror;
		}
		else if ( connect_failed == failed_threshold2 )
		{
			ret = terror;
		}
		else if ( (connect_failed%failed_everytime) == 0 )
		{
			ret = terror;
		}
		warn( "%s cannot connect %d times", object, connect_failed );
	}
	connect_failed++;
	int2register( object, "connect_failed", reg_connect_failed, connect_failed );
	if ( ret == terror )
	{
		if ( com_sexist( ifdev, "reset" ) == true )
		{
			scall( ifdev, "reset", NULL );
		}
		talk_free( cfg );
		sleep( 5 );
		return tfalse;
	}

    /* ifdev up take this cfg */
	json_set_string( cfg, "ifname", object );
    info( "%s up", ifdev );
    if ( scallt( ifdev, "up", cfg ) != ttrue )
    {
        warn( "%s up failed", ifdev );
        talk_free( cfg );
        sleep( 5 );
        return tfalse;
    }
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );



    /* get the netdev */
	netdev = register_value( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        fault( "%s netdev get error", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }
	/* get the local_netdev2 when have */
	if ( 0 == strcmp( object, LAN2_COM ) )
	{
		/* set the shunt balancing for lan2 when have */
		ptr = register_value( LAND_PROJECT, "local_netdev2" );
		if ( ptr == NULL || *ptr == '\0' )
		{
			register_file_t h;
			/* globe value setttings */
			h = register_open( LAND_PROJECT, O_RDWR, 0644, 100, 25600 );
			if ( h != NULL )
			{
				register_value_set( h, "local_netdev2", netdev, strlen(netdev)+1, 20 );
				register_value_set( h, "local_ifname2", object, strlen(object)+1, 20 );
				register_close( h );
			}
		}
	}

    /* ifdev connect */
    info( "%s connect", ifdev );
    if ( scall( ifdev, "connect", NULL ) != ttrue )
    {
        fault( "%s connect failed", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }

    /* set the mac */
    ptr = json_string( cfg, "mac" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        scalls( ifdev, "setmac", ptr );
    }

	/* check connected */
	ready = 0;
	check = 0;
	while( check < failed_timeout )
	{
		if ( scall( ifdev, "connected", NULL ) == ttrue )
		{
			ready++;
			if ( ready >= 3 )
			{
				info( "%s connected ready", ifdev );
				break;
			}
			usleep( 300000 );
			continue;
		}
		ready = 0;
		check++;
		sleep( 1 );
	}
	if ( check >= failed_timeout )
	{
		warn( "%s connect timeout", ifdev );
		scall( ifdev, "down", NULL );
		talk_free( cfg );
		return tfalse;
	}

	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	/* static ip setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		v = json_value( cfg, "static" );
		static_ip_enable( netdev, v );
	}
	else if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
	{
		v = json_value( cfg, "dhcpc" );
		ptr = json_string( v, "static" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_value( cfg, "static" );
			static_ip_enable( netdev, v );
		}
	}

	/* slaac setting */
	if ( method != NULL && 0 == strcmp( method, "slaac" ) )
	{
		slaac_ip_enable( netdev );
	}
	else
	{
		slaac_ip_disable( netdev );
	}
	/* manual ip setting */
	if ( method != NULL && 0 == strcmp( method, "manual" ) )
	{
		/* set the static ip */
		v = json_value( cfg, "manual" );
		manual_ip_enable( netdev, v );
	}
	else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		v = json_value( cfg, "automatic" );
		ptr = json_string( v, "manual" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_value( cfg, "manual" );
			manual_ip_enable( netdev, v );
		}
	}

	ret = tfalse;
	value = json_create( NULL );
	/* ipv4 static setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		if ( mode_static( object, ifdev, netdev, cfg, value ) == true )
		{
			scallt( NETWORK_COM, "online", value );
		}
		/* ipv6 static setting */
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, value ) == true )
			{
				scallt( NETWORK_COM, "upline", value );
			}
		}
		/* ipv6 automatic setting */
		else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
		{
			v = json_value( cfg, "manual" );
			ret = automatic_client_connect( object, ifdev, netdev, v );
		}
		ret = ttrue;
	}
	else
	{
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, value ) == true )
			{
				scallt( NETWORK_COM, "upline", value );
			}
		}
		/* ipv6 automatic setting */
		else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
		{
			sstart( object, "automatic", NULL, "%s-automatic", object );
		}
		/* ipv4 dhcp client setting */
		if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
		{
			v = json_value( cfg, "dhcpc" );
			ret = dhcp_client_connect( object, ifdev, netdev, v );
		}
		/* ipv4 pppoe setting */
		else if ( mode != NULL && 0 == strcmp( mode, "pppoec" ) )
		{
			const char *mtu;
			v = json_value( cfg, "pppoec" );
			mtu = json_string( cfg, "mtu" );
			if ( mtu != NULL && atoi(mtu) > 0 )
			{
				json_set_string( v, "mtu", mtu );
			}
			ret = pppoe_client_connect( object, ifdev, netdev, v );
		}
	}

	talk_free( value );
    talk_free( cfg );
    return ret;
}
boole_t _automatic( obj_t this, param_t param )
{
	talk_t ret;
    talk_t cfg;
    const char *object;
	const char *method;
	const char *ifdev;
	const char *netdev;

    object = obj_combine( this );
    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
        return terror;
    }
	method = json_string( cfg, "method" );

	/* get the ifdev */
	ifdev = register_value( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	/* need the ifdev exist */
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}
    /* get the netdev */
	netdev = register_value( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        fault( "%s netdev get error", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }

	ret = terror;
	/* automatic setting */
	if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		ret = automatic_client_connect( object, ifdev, netdev, json_value( cfg, "manual" ) );
	}

    talk_free( cfg );
    return ret;
}



boole_t _online( obj_t this, param_t param )
{
	int i;
	talk_t v;
	talk_t cfg;
	talk_t value;
	const int *tid;
	const char *ptr;
	const char *object;
	const char *ifdev;
	const char *netdev;
	const char *mode;
	const char *gateway;
	const char *custom_dns;
	const char *dns;
	const char *dns2;
	const char *metric;
    const char *mtu;
	char path[PATH_MAX];
	char ipaddr[NAME_MAX];

	object = obj_combine( this );
	v = param_talk( param, 1 );
	/* get ifdev */
	ifdev = json_string( v, "ifdev" );
	/* get netdev */
	netdev = json_string( v, "netdev" );

	/* get the configure */
	cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return tfalse;
	}
	/* get the keeplive */
	ptr = json_string( json_value( cfg, "keeplive"), "type" );
	register_set( object, "keeplive", ptr, stringlen(ptr)+1, 20 );
	/* get the metric */
	metric = json_string( cfg, "metric" );
	register_set( object, "metric", metric, stringlen(metric)+1, 20 );
	json_set_string( v, "metric", metric );

	/* get mode */
	mode = register_pointer( object, "mode" );

	gateway = json_string( v, "gw" );
	/* get the custom_dns */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	value = json_value( cfg, mode );
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
	{
		dns = json_string( value, "dns" );
		dns2 = json_string( value, "dns2" );
		register_set( object, "custom_dns", "enable", strlen("enable")+1, 20 );
	}
	else
	{
		dns = json_string( v, "dns" );
		dns2 = json_string( v, "dns2" );
		ptr = json_string( value, "domain" );
		if ( ptr != NULL )
		{
			string3file( path, "search %s\n", ptr );
		}
		register_set( object, "custom_dns", "disable", strlen("disable")+1, 20 );
	}
	if ( dns != NULL && *dns != '\0' )
	{
		string3file( path, "nameserver %s\n", dns );
		if ( gateway == NULL || 0 != strcmp( gateway, dns ) )
		{
			route_switch( dns, NULL, NULL, v, true );
		}
	}
	register_set( object, "dns", dns, stringlen(dns)+1, 20 );
	if ( dns2 != NULL && *dns2 != '\0' )
	{
		string3file( path, "nameserver %s\n", dns2 );
		if ( gateway == NULL || 0 != strcmp( gateway, dns2 ) )
		{
			route_switch( dns2, NULL, NULL, v, true );
		}
	}
	register_set( object, "dns2", dns2, stringlen(dns2)+1, 20 );

	netdev_info( netdev, ipaddr, sizeof(ipaddr), NULL, 0, NULL, 0, NULL, 0 );
	if ( gateway != NULL && *gateway != '\0' )
	{
		info( "%s(%s) %s online[ %s, %s ]", object, netdev, ipaddr, gateway?:"", dns?:"" );
		register_set( object, "gateway", gateway, stringlen(gateway)+1, 20 );
	}
	else
	{
		info( "%s(%s) %s online", object, netdev, ipaddr );
		register_set( object, "gateway", NULL, 0, 20 );
	}

	/* clear the failed count, dnon't clear at the icmp keeplive(clear in the keeplive) */
	ptr = json_string( json_value( cfg, "keeplive" ), "type" );
	if ( ptr == NULL || 0 != strcmp( ptr, "icmp" ) )
	{
		i = 0;
		register_set( object, "connect_failed", &i, sizeof(i), 0 );
	}

	/* masq */
	iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	ptr = json_string( cfg, "masq" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		iptables("-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}
	/* mtu */
	mtu = json_string( cfg, "mtu" );
	if ( mtu != NULL && atoi(mtu) > 0 )
	{
		shell( "ifconfig %s mtu %s", netdev, mtu );
	}
	/* ppp tcp mss */
	if ( 0 == strncmp( netdev, "ppp", 3 ) )
	{
		value = json_value( cfg, "ppp" );
		ptr = json_string( value, "txqueuelen" );
		if ( ptr == NULL || *ptr == '\0' )
		{
			ptr = "500";
		}
		txqueue_set_ifname( object, netdev, ptr );
	}
	/* set the pmtu on wan */
	if ( 0 != strncmp( object, LAN_COM, strlen(LAN_COM) ) )
	{
		pmtu_adjust_ifname( object, netdev, mtu );
	}

	/* tid route table init */
	tid = register_pointer( object, "tid" );
	if ( tid != NULL && *tid != 0 )
	{
		routes_ifname( *tid, v );
	}

	/* tell the ifdev */
	if ( ifdev != NULL )
	{
		scalls( ifdev, "online", object );
		scalls( GPIO_COM, "action", "network/online,%s", ifdev );
	}

	talk_free( cfg );
	return ttrue;
}
talk_t _offline( obj_t this, param_t param )
{
	talk_t cfg;
	const char *mtu;
	const char *object;
	const char *ifdev;
	const char *netdev;
	char path[PATH_MAX];

	object = obj_combine( this );
	/* dns file */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	/* get the netdev */
	netdev = register_value( object, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		/* get the configure */
		iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
		/* tcp mss */
		cfg = config_get( this, NULL );
		mtu = json_string( cfg, "mtu" );
		pmtu_clear_ifname( object, netdev, mtu );
		talk_free( cfg );
		info( "%s(%s) offline", object, netdev );
	}
	else
	{
		info( "%s offline", object );
	}
	/* tell the ifdev */
	ifdev = register_value( object, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		scalls( ifdev, "offline", object );
	}

	return ttrue;
}
boole_t _upline( obj_t this, param_t param )
{
	talk_t v;
	talk_t cfg;
	talk_t value;
	const char *ptr;
	const char *object;
	const char *netdev;
	const char *method;
	const char *hop;
	const char *custom_resolve;
	const char *resolve;
	const char *resolve2;
	char path[PATH_MAX];

	object = obj_combine( this );
	v = param_talk( param, 1 );
	/* get netdev */
	netdev = json_string( v, "netdev" );
	/* get the configure */
	cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return tfalse;
	}

	/* get mode */
	method = register_pointer( object, "method" );

	hop = json_string( v, "hop" );
	/* get the custom_resolve */
	snprintf( path, sizeof(path), "%s/%s.ipv6", RESOLV_DIR, object );
	unlink( path );
	value = json_value( cfg, method );
	custom_resolve = json_string( value, "custom_resolve" );
	if ( custom_resolve != NULL && 0 == strcmp( custom_resolve, "enable" ) )
	{
		resolve = json_string( value, "resolve" );
		resolve2 = json_string( value, "resolve2" );
		register_set( object, "custom_resolve", "enable", strlen("enable")+1, 20 );
	}
	else
	{
		resolve = json_string( v, "resolve" );
		if ( resolve == NULL || *resolve == '\0' )
		{
			resolve = "8.8.8.8";
		}
		resolve2 = json_string( v, "resolve2" );
		if ( resolve2 == NULL || *resolve2 == '\0' )
		{
			resolve2 = "114.114.114.114";
		}
		ptr = json_string( value, "domain" );
		if ( ptr != NULL )
		{
			string3file( path, "search %s\n", ptr );
		}
		register_set( object, "custom_resolve", "disable", strlen("disable")+1, 20 );
	}
	if ( resolve != NULL && *resolve != '\0' )
	{
		string3file( path, "nameserver %s\n", resolve );
	}
	register_set( object, "resolve", resolve, stringlen(resolve)+1, 20 );
	if ( resolve2 != NULL && *resolve2 != '\0' )
	{
		string3file( path, "nameserver %s\n", resolve2 );
	}
	register_set( object, "resolve2", resolve2, stringlen(resolve2)+1, 20 );

	info( "%s(%s) upline[ %s, %s ]", object, netdev, hop?:"", resolve?:"" );

	/* masquerade */
	ip6tables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	ptr = json_string( cfg, "masquerade" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		ip6tables("-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}

	talk_free( cfg );
	return ttrue;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
	const char *object;

	object = obj_combine( this );
	if ( NULL == strstr( object, LAN_COM ) )
	{
		_shut( this, NULL );
		ret = config_set( this, v, path );
		_setup( this, NULL );
	}
	else
	{
		ret = config_set( this, v, path );
	}
    return ret;
}
talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}    



