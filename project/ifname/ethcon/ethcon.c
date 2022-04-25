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
    talk_t v;
    talk_t cfg;
    talk_t ret;
    const char *ptr;
    const char *obj;
    const char *object;
	const char *ifdev;
    char path[PATH_MAX];
    char buffer[NAME_MAX];

    /* get the com name */
    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return tfalse;
    }
    object = obj_combine( this );
    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
    /* exit when the status is disable */
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
    {
        talk_free( cfg );
        return tfalse;
    }
    /* get the ifdev */
	snprintf( path, sizeof(path), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, path );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}
    /* setup record file, avoid duplicate setup */
    project_var_path( path, sizeof(path), PROJECT_ID, "%s.st", obj );
    v = file2talk( path );
    if ( v != NULL )
    {
        warn( "%s already startup", object );
        talk_free( v );
        talk_free( cfg );
        return ttrue;
    }
    v = json_create( NULL );
    snprintf( buffer, sizeof(buffer), "%lu", time(NULL) );
    json_set_string( v, "starttime", buffer );
    talk2file( v , path );
    talk_free( v );

    /* run the app connection */
    ret = tfalse;
    info( "%s startup", object );
	ret = service_start( object, object, "service", NULL );
    talk_free( cfg );
    return ret;
}
boole_t _shut( obj_t this, param_t param )
{
    talk_t v;
    talk_t cfg;
    const char *obj;
    const char *object;
	const char *ifdev;
    char path[PATH_MAX];

    /* get the com name */
    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return tfalse;
    }
    object = obj_combine( this );
    /* delete setup record file */
    project_var_path( path, sizeof(path), PROJECT_ID, "%s.st", obj );
    v = file2talk( path );
    if ( v == NULL )
    {
        return tfalse;
    }
    talk_free( v );
    unlink( path );
    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
        return tfalse;
    }
    info( "%s shut", object );

    /* stop the service */
    service_stop( object );
    /* delete online file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    unlink( path );
    /* delete upline file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
    unlink( path );
    /* down the ifdev */
	snprintf( path, sizeof(path), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, path );
    if ( ifdev != NULL && *ifdev != '\0' )
    {
    	scall( ifdev, "down", NULL );
	}

    talk_free( cfg );
    return ttrue;
}
talk_t _ifdev( obj_t this, param_t param )
{
    const char *obj;
    const char *object;
	const char *ifdev;
    char name[NAME_MAX];

    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return NULL;
    }
    object = obj_combine( this );

    /* get the ifdev */
	snprintf( name, sizeof(name), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, name );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }

    return string2x( ifdev );
}
talk_t _netdev( obj_t this, param_t param )
{
    const char *obj;
    const char *object;
	const char *ifdev;
	const char *netdev;
    char name[NAME_MAX];

    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return NULL;
    }
    object = obj_combine( this );

    /* get the netdev */
	snprintf( name, sizeof(name), "%s"IFNAME_NETDEV_POSTFIX, object );
	netdev = register_pointer( NETWORK_PROJECT, name );
    if ( netdev != NULL && *netdev != '\0' )
    {
    	return string2x( netdev );
    }
    /* get the ifdev */
	snprintf( name, sizeof(name), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, name );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }
    /* get the ifdev netdev */
	snprintf( name, sizeof(name), "%s"IFDEV_NETDEV_POSTFIX, ifdev );
	netdev = register_pointer( NETWORK_PROJECT, name );
    if ( netdev == NULL || *netdev == '\0' )
    {
        return NULL;
    }

    return string2x( netdev );
}
int *reg_delay;
char *reg_keeplive;
char *reg_ifdev;
char *reg_netdev;
char *reg_tid;
char *reg_mode;
char *reg_method;
char *reg_custom_dns;
char *reg_dns;
char *reg_dns2;
char *reg_custom_resolve;
char *reg_resolve;
char *reg_resolve2;
char *reg_resolve2;
int *reg_connect_failed;
talk_t _status( obj_t this, param_t param )
{
	int delay;
    talk_t ret;
    talk_t v;
    struct stat st;
    boole keeplive;
    const char *ptr;
    const char *obj;
    const char *object;
	const char *tid;
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
    char name[NAME_MAX];
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return NULL;
    }
    object = obj_combine( this );
	netdev = device = NULL;

    /* get the keeplive */
	register2string( object, "keeplive", reg_keeplive, ptr, NULL );
    if ( ptr != NULL && 0 == strcmp( ptr, "icmp" ) )
    {
    	keeplive = true;
		register2int( object, "delay", reg_delay, delay, 0 );
    }
	else
	{
		keeplive = false;
	}
	/* get mode */
	register2string( object, "tid", reg_tid, tid, NULL );
	register2string( object, "mode", reg_mode, mode, NULL );
	register2string( object, "method", reg_method, method, NULL );
    /* get the custom_dns */
	register2string( object, "custom_dns", reg_custom_dns, custom_dns, NULL );
	register2string( object, "dns", reg_dns, dns, NULL );
	register2string( object, "dns2", reg_dns2, dns2, NULL );
	register2string( object, "custom_resolve", reg_custom_resolve, custom_resolve, NULL );
	register2string( object, "resolve", reg_resolve, resolve, NULL );
	register2string( object, "resolve2", reg_resolve2, resolve2, NULL );

    /* get the ipv4 online status */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    if ( stat( path, &st ) != 0 )
    {
        ret = json_create( NULL );
		/* get the ifdev */
		snprintf( name, sizeof(name), "%s"IFNAME_IFDEV_POSTFIX, object );
		nreg2string( name, reg_netdev, ifdev, NULL );
		if ( ifdev != NULL && *ifdev != '\0' )
		{
			json_set_string( ret, "ifdev", ifdev );
			/* get the netdev */
			snprintf( name, sizeof(name), "%s"IFDEV_NETDEV_POSTFIX, ifdev );
			nreg2string( name, reg_netdev, netdev, NULL );
			if ( netdev != NULL && *netdev != '\0' )
			{
				json_set_string( ret, "netdev", netdev );
				/* get the mac */
				if ( netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, name, sizeof(name) ) == 0 )
				{
					json_set_string( ret, "mac", name );
				}
			}
		}
        if ( service_pid( object ) != NULL )
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
        char ip[16];
		char mac[20];
        char dstip[16];
        char mask[16];
        unsigned long long rt_bytes, rt_packets, rt_errs, rt_drops, tt_bytes, tt_packets, tt_errs, tt_drops;
        ip[0] = dstip[0] = mask[0] = mac[0] = '\0';
        rt_bytes = rt_packets = rt_errs = rt_drops = tt_bytes = tt_packets = tt_errs = tt_drops = 0;
        ret = file2talk( path );
        ifdev = json_string( ret, "ifdev" );
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
            if ( service_pid( object ) != NULL )
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
			if ( *dstip != '\0' )
			{
				json_set_string( ret, "dstip", dstip );
			}
            json_set_string( ret, "mask", mask );
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
                    json_set_number( ret, "keeplive", delay );
				}
				else
				{
                    json_set_string( ret, "keeplive", "failed" );
				}
            }
            /* get the livetime */
			ptr = json_string( ret, "ontime" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				json_set_string( ret, "livetime", livetime_desc( atoll(ptr), name, sizeof(name) ) );
			}
			/* get the flow */
			netdev_flew( device, &rt_bytes, &rt_packets, &rt_errs, &rt_drops, &tt_bytes, &tt_packets, &tt_errs, &tt_drops );
			snprintf( name, sizeof(name), "%llu", rt_bytes );
			json_set_string( ret, "rx_bytes", name );
			snprintf( name, sizeof(name), "%llu", rt_packets );
			json_set_string( ret, "rx_packets", name );
			snprintf( name, sizeof(name), "%llu", tt_bytes );
			json_set_string( ret, "tx_bytes", name );
			snprintf( name, sizeof(name), "%llu", tt_packets );
			json_set_string( ret, "tx_packets", name );
        }
		/* get the mac */
		if ( 0 == strcmp( mac, "00:00:00:00:00:00" ) || 0 == strcasecmp( mac, "ff:ff:ff:ff:ff:ff" ) ) // ppp interface mac
		{
			/* get the netdev */
			snprintf( name, sizeof(name), "%s"IFDEV_NETDEV_POSTFIX, ifdev );
			nreg2string( name, reg_netdev, netdev, NULL );
			if ( netdev != NULL && *netdev != '\0' )
			{
				netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) );
			}
		}
		json_set_string( ret, "mac", mac );
    }

    /* get the mode of configure */
	if ( tid != NULL && *tid != '\0' )
	{
		json_set_string( ret, "tid", tid );
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
							strcpy( name, "addr" );
						}
						else
						{
							sprintf( name, "addr%d", t );
						}
						end = strstr( host, "%" );
						if ( end != NULL )
						{
							*end = '\0';
						}
						json_set_string( ret, name, host );
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

    /* get the ifdev or main ifdev info */
	if ( (com_sexist( ifdev, "state" ) == true) && (NULL != ( v = scalls( ifdev, "state", object ) ) ) )
	{
		talk_patch( v, ret );
	    talk_free( v );
	}
	else
	{
		talk_t axp;
		talk_t list;
		char mifdev[NAME_MAX];

		/* only for repeator */
		axp = NULL;
		list = scalls( ifdev, "list", NULL );
		if ( list != NULL )
		{
			mifdev[0] = '\0';
			while( NULL != ( axp = json_each( list, axp ) ) )
			{
				ptr = axp_get_attr( axp );
				if ( strstr( ptr, "sta" ) != NULL )
				{
					strncpy( mifdev, ptr, sizeof(mifdev)-1 );
					mifdev[sizeof(mifdev)-1] = '\0';
					break;
				}
			}
			if ( *mifdev != '\0' )
			{
				if ( (com_sexist( mifdev, "state" ) == true) && (NULL != ( v = scalls( mifdev, "state", object ) ) ) )
				{
					talk_patch( v, ret );
					talk_free( v );
				}
			}
			talk_free( list );
		}
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
    const char *obj;
	const char *tid;
	const char *mode;
    const char *object;
	const char *ifdev;
	const char *netdev;
	const char *method;
    char name[NAME_MAX];

    /* get the component identify */
    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return terror;
    }
    object = obj_combine( this );
	/* get the ifdev */
	snprintf( name, sizeof(name), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, name );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return tfalse;
    }
	if ( com_sexist( ifdev, NULL ) == false )
	{
        return tfalse;
	}

    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
    	return terror;
    }
	json_set_string( cfg, "ifname", object );
	tid = json_string( cfg, "tid" );
	string2register( object, "tid", reg_tid, tid, 20 );
	mode = json_string( cfg, "mode" );
	string2register( object, "mode", reg_mode, mode, 20 );
	method = json_string( cfg, "method" );
	string2register( object, "method", reg_method, method, 20 );

    /* ifdev up take this cfg */
    info( "%s up", ifdev );
    if ( scallt( ifdev, "up", cfg ) != ttrue )
    {
        warn( "%s up failed", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
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

    /* get the netdev */
	snprintf( name, sizeof(name), "%s"IFDEV_NETDEV_POSTFIX, ifdev );
	netdev = register_pointer( NETWORK_PROJECT, name );
    if ( netdev == NULL || *netdev == '\0' )
    {
        fault( "%s netdev get error", ifdev );
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
	while( check < 30 )
	{
		if ( scallt( ifdev, "connected", cfg ) == ttrue )
		{
			ready++;
			if ( ready >= 5 )
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
	if ( check >= 30 )
	{
		warn( "%s connect timeout", ifdev );
		scall( ifdev, "down", NULL );
		talk_free( cfg );
		return tfalse;
	}

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
		pause();
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
			service_start( name, COM_IDPATH, "automatic", NULL );
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
			v = json_value( cfg, "pppoec" );
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
    const char *obj;
    const char *object;
	const char *method;
	const char *ifdev;
	const char *netdev;
    char name[NAME_MAX];

    /* get the com name */
    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return tfalse;
    }
    object = obj_combine( this );
    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
        return terror;
    }
	method = json_string( cfg, "method" );

	/* get the ifdev */
	snprintf( name, sizeof(name), "%s"IFNAME_IFDEV_POSTFIX, object );
	ifdev = register_pointer( NETWORK_PROJECT, name );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}
    /* get the netdev */
	snprintf( name, sizeof(name), "%s"IFDEV_NETDEV_POSTFIX, ifdev );
	netdev = register_pointer( NETWORK_PROJECT, name );
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
	const char *ptr;
	const char *obj;
	const char *object;
	const char *ifdev;
	const char *netdev;
	const char *mode;
	const char *gateway;
	const char *custom_dns;
	const char *dns;
	const char *dns2;
	char path[PATH_MAX];

    obj = obj_com( this );
    if ( 0 == strcmp( obj, COM_ID ) )
    {
        return tfalse;
    }
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
	if ( ptr != NULL )
	{
		string2register( object, "keeplive", reg_keeplive, ptr, 20 );
	}

	/* get mode */
	register2string( object, "mode", reg_mode, mode, NULL );

    /* get the custom_dns */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	value = json_value( cfg, mode );
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enbale" ) )
	{
		dns = json_string( value, "dns" );
		dns2 = json_string( value, "dns2" );
		string2register( object, "custom_dns", reg_custom_dns, custom_dns, 20 );
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
		string2register( object, "custom_dns", reg_custom_dns, "disable", 20 );
	}
	if ( dns != NULL && *dns != '\0' )
	{
		string2register( object, "dns", reg_dns, dns, 20 );
		string3file( path, "nameserver %s\n", dns );
		route_switch( NULL, dns, NULL, NULL, v, true );
	}
	if ( dns2 != NULL && *dns2 != '\0' )
	{
		string2register( object, "dns2", reg_dns2, dns2, 20 );
		string3file( path, "nameserver %s\n", dns2 );
		route_switch( NULL, dns2, NULL, NULL, v, true );
	}

	gateway = json_string( v, "gw" );
	info( "%s(%s) online[ %s, %s, %s ]", object, netdev, gateway?:"", dns?:"", dns2?:"" );

	/* clear the connect failed count */
	i = 0;
	int2register( object, "connect_failed", reg_connect_failed, i );

	/* masq */
	iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	ptr = json_string( cfg, "masq" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		iptables("-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}
	/* ppp tcp mss */
	if ( 0 == strncmp( netdev, "ppp", 3 ) )
	{
		ptr = json_string( v, "txqueuelen" );
		if ( ptr == NULL || *ptr == '\0' )
		{
			ptr = "500";
		}
		execute( 0, 1, "ifconfig %s txqueuelen %s", netdev, ptr );
	}
	if ( gateway != NULL && *gateway != '\0' )
	{
		iptables( "-t mangle -D POSTROUTING -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -m tcpmss --mss 1400:1536 -j TCPMSS --clamp-mss-to-pmtu", netdev );
		iptables( "-t mangle -A POSTROUTING -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -m tcpmss --mss 1400:1536 -j TCPMSS --clamp-mss-to-pmtu", netdev );
	}

	/* tell the ifdev */
	if ( ifdev != NULL )
	{
		scalls( ifdev, "online", object );
	}

	talk_free( cfg );
	return ttrue;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
	const char *object;

    ret = config_set( this, v, path );
    if ( ret == true )
    {
    	object = obj_combine( this );
		if ( NULL == strstr( object, LAN_COM ) )
		{
	        _shut( this, NULL );
	        _setup( this, NULL );
		}
    }
    return ret;
}
talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}    



