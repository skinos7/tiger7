/*
 *    Description:  lte ip connection
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landnet/landnet.h"
#include <ifaddrs.h>
#define LTE_STATE_RESET 6



/* meke the ppp options file, succeed return 0 */
static void ppp_config_build( FILE *opfile, FILE *scfile, talk_t pppcfg, talk_t profile )
{
    const char *ptr;
    const char *name;
    const char *passwd;
    const char *auth;
    const char *mtu;
    const char *ppp_pppopt;
    const char *oper_pppopt;

    if ( opfile == NULL || scfile == NULL )
    {
        return;
    }
    name = json_string( profile, "user" );
    passwd = json_string( profile, "passwd" );
    auth = json_string( profile, "auth" );
    oper_pppopt = json_string( profile, "opt" );
    mtu = json_string( pppcfg, "mtu" );
    ppp_pppopt = json_string( pppcfg, "pppopt" );

    /* username */
    if ( name != NULL && *name != '\0' )
    {
        if ( passwd != NULL && *passwd != '\0' )
        {
            fprintf( scfile, "\"%s\"\t*\t\"%s\"\t*\n", name, passwd );
        }
        else
        {
            fprintf( scfile, "\"%s\"\t*\t\"\"\t*\n", name );
        }
        fprintf( opfile, "name \"%s\"\n", name );
        if ( auth != NULL && *auth != '\0' )
        {
            if ( 0 == strcmp( auth, "pap" ) )
            {
                fprintf( opfile, "require-pap\n");
            }
            else if ( 0 == strcmp( auth, "chap" ) )
            {
                fprintf( opfile, "require-chap\n");
            }
        }
    }

    /* mtu */
    if ( mtu != NULL && *mtu != '\0' )
    {
        fprintf( opfile, "mtu %s\n", mtu );
    }

    /* lcp echo  */
    ptr = json_string( pppcfg, "lcp_echo_interval" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( opfile, "lcp-echo-interval %s\n", ptr );
    }
    ptr = json_string( pppcfg, "lcp_echo_failure" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        fprintf( opfile, "lcp-echo-failure %s\n", ptr );
    }
    
    /* network pppopt  */
    if ( oper_pppopt != NULL && *oper_pppopt != '\0' )
    {
        char *tok;
        char *tokkey;
        char buffer[LINE_MAX];
        
        memset( buffer, 0, sizeof(buffer) );
        strncpy( buffer, oper_pppopt, sizeof(buffer)-1 );
        tokkey = tok = buffer;
        while( tokkey != NULL && *tok != '\0' )
        {
            tokkey = strstr( tok, ";" );
            if ( tokkey != NULL )
            {
                *tokkey = '\0';
            }
            fprintf( opfile, "%s\n", tok );
            tok = tokkey+1;
        }
    }
    /* pppopt  */
    if ( ppp_pppopt != NULL && *ppp_pppopt != '\0' )
    {
        char *tok;
        char *tokkey;
        char buffer[LINE_MAX];
        
        memset( buffer, 0, sizeof(buffer) );
        strncpy( buffer, ppp_pppopt, sizeof(buffer)-1 );
        tokkey = tok = buffer;
        while( tokkey != NULL && *tok != '\0' )
        {
            tokkey = strstr( tok, ";" );
            if ( tokkey != NULL )
            {
                *tokkey = '\0';
            }
            fprintf( opfile, "%s\n", tok );
            tok = tokkey+1;
        }
    }
    return;
}/* make the ppp chat */
static void ppp_chat_build( const char *chatfile, talk_t pppcfg, talk_t profile )
{
#define LTE_APNCHAT_FORMAT \
"ABORT 'ERROR'\n"\
"ABORT 'BUSY'\n"\
"ABORT 'NO CARRIER'\n"\
"'' AT\n"\
"TIMEOUT 5\n"\
"OK AT+CGDCONT=%s,\"%s\",\"%s\"\n"\
"TIMEOUT 15\n"\
"OK ATD%s\n"\
"TIMEOUT 25\n"\
"CONNECT ''"
#define LTE_NOAPNCHAT_FORMAT \
"ABORT 'ERROR'\n"\
"ABORT 'BUSY'\n"\
"ABORT 'NO CARRIER'\n"\
"'' AT\n"\
"TIMEOUT 5\n"\
"OK ATD%s\n"\
"TIMEOUT 25\n"\
"CONNECT ''"
    const char *apn;
    const char *dial;
    const char *type;
    const char *cid;
    const char *iptype;

    apn = json_string( profile, "apn" );
    dial = json_string( profile, "dial" );
	if ( dial == NULL || *dial == '\0' )
	{
		dial = "*99#";
	}
    if ( apn != NULL && *apn != '\0' )
    {
        type = json_string( profile, "type" );
        iptype = "IP";
        if ( type != NULL && 0 == strcasecmp( type, "ipv4v6" ) )
        {
            iptype = "IPV4V6";
        }
        else if ( type != NULL && 0 == strcasecmp( type, "ipv4" ) )
        {
            iptype = "IPV4";
        }
        else if ( type != NULL && 0 == strcasecmp( type, "ipv6" ) )
        {
            iptype = "IPV6";
        }
        cid = json_string( profile, "cid" );
        if ( cid == NULL || *cid == '\0' )
        {
            cid = "1";
        }
        string2file( chatfile, LTE_APNCHAT_FORMAT, cid, iptype, apn, dial );
    }
    else
    {
        string2file( chatfile, LTE_NOAPNCHAT_FORMAT, dial );
    }
    return;
}
/* online the network, return -1 on error, 1 on failed, 0 on succeed  */
static talk_t ppp_client_connect( const char *object, const char *ifdev, talk_t cfg, talk_t profile )
{
    talk_t pppcfg;
	const char *mtty;
    FILE *pppoptions_fd;
    FILE *pppsecrets_fd;
    char pppd[PATH_MAX];
    char chat[PATH_MAX];
    char pppchat[PATH_MAX];
    char pppoptions[PATH_MAX];
    char pppsecrets[PATH_MAX];

    /* get the network operator */
	mtty = json_string( profile, "mtty" );
    if ( mtty == NULL || *mtty == '\0' )
    {
        faulting( "%s cannot found %s mtty port", object ); 
		return terror;
    }

    /* init the tmp value */
    pppoptions_fd = NULL;
    pppsecrets_fd = NULL;
    /* get the ppp var file */
    project_var_path( pppoptions, sizeof(pppoptions), PROJECT_ID, "%s.pppopt", object );
    project_var_path( pppsecrets, sizeof(pppsecrets), PROJECT_ID, "%s.pppsec", object );
    project_var_path( pppchat, sizeof(pppchat), PROJECT_ID, "%s.pppchat", object );
    if ( NULL == ( pppoptions_fd = fopen( pppoptions, "w+" ) ) )
    {
        faulting( "open the file(%s) error", pppoptions ); 
		return terror;
    }
    if ( NULL == ( pppsecrets_fd = fopen( pppsecrets, "w+" ) ) )
    {
        faulting( "open the file(%s) error", pppsecrets );
        fclose( pppoptions_fd );
		return terror;
    }
    /* make the pre config */
    fprintf( pppoptions_fd, "noauth\n" );
    fprintf( pppoptions_fd, "nomppe\n" );
    fprintf( pppoptions_fd, "noipdefault\n" );
    fprintf( pppoptions_fd, "usepeerdns\n" );
    fprintf( pppoptions_fd, "nodetach\n" );
    fprintf( pppoptions_fd, "pap-timeout 60\n" );
    fprintf( pppoptions_fd, "lock\n" );
    fprintf( pppoptions_fd, "modem\n" );
    fprintf( pppoptions_fd, "noendpoint\n" );
    fprintf( pppoptions_fd, "nomagic\n" );
    fprintf( pppoptions_fd, "nolog\n" );
    //fprintf( pppoptions_fd, "nopersist\n" );
    fprintf( pppoptions_fd, "holdoff 10\n" );

    /* make the basic config */
    pppcfg = json_get_value( cfg, "ppp" );
    ppp_config_build( pppoptions_fd, pppsecrets_fd, pppcfg, profile );

    /* make the basic path */
    fprintf( pppoptions_fd, "pap-file %s\n", pppsecrets );
    fprintf( pppoptions_fd, "chap-file %s\n", pppsecrets );
    fprintf( pppoptions_fd, "srp-file %s\n", pppsecrets );
    fprintf( pppoptions_fd, "online-id %s\n", object );
    /* make the chat script */;
    ppp_chat_build( pppchat, pppcfg, profile );
	project_exe_path( chat, sizeof(chat), NETWORK_PROJECT, "chat" );
    fprintf( pppoptions_fd, "connect \"%s -v -f %s\"\n", chat, pppchat );
    fclose( pppoptions_fd );
    fclose( pppsecrets_fd );

    /* ppp dial */
	debug( "pppd %s file %s", mtty, pppoptions );
	project_exe_path( pppd, sizeof(pppd), NETWORK_PROJECT, "pppd" );
    execlp( pppd, "pppd", mtty, "file", pppoptions, (char*)0 );
    faulting( "exec the pppd error" );    
    return tfalse;
}



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
	
    /* exit when the status is disable */
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
    {
        talk_free( cfg );
        return tfalse;
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
                    json_set_number( ret, "keeplive", delay );
				}
				else
				{
                    json_set_string( ret, "delay", "failed" );
                    json_set_string( ret, "keeplive", "failed" );
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
	const int *stepp;
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

    /* get the ifdev or main ifdev info */
	if ( ifdev != NULL && com_sexist( ifdev, "state" ) == true )
	{
		v = scalls( ifdev, "state", object );
		if ( v > TALK_ECODEMAX )
		{
			talk_patch( v, ret );
			ptr = json_string( v, "state" );
			if ( ptr != NULL && 0 != strcmp( ptr, "connected" ) )
			{
				json_set_string( ret, "status", ptr );
			}
			talk_free( v );
		}
		else
		{
			json_set_string( ret, "status", "nodevice" );
			stepp = register_value( ifdev, "state" );
			if ( stepp != NULL )
			{
				if ( *stepp == LTE_STATE_RESET )	 // LTE_STATE_RESET
				{
					json_set_string( ret, "status", "reset" );
				}
			}
		}
	}
	else
	{
		json_set_string( ret, "status", "nodevice" );
		stepp = register_value( ifdev, "state" );
		if ( stepp != NULL )
		{
			if ( *stepp == LTE_STATE_RESET )   // LTE_STATE_RESET
			{
				json_set_string( ret, "status", "reset" );
			}
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
	const int *bsim_mode;
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
		/************** bsim process *********************/
		bsim_mode = register_pointer( ifdev, "bsim_mode" );
		if ( bsim_mode != NULL && *bsim_mode == 3 )
		{
			const int *dfailed = register_pointer( ifdev, "bsim_dial_failed" );
			if ( dfailed != NULL && *dfailed > 0 && (connect_failed%(*dfailed))==0 )
			{
				int *bsim_setting;
				bsim_setting = register_pointer( ifdev, "bsim_setting" );
				if ( *bsim_setting == 1 )
				{
					info( "%s need switch to backup simcard", ifdev );
					*bsim_setting = 2;
				}
				else
				{
					info( "%s need switch to main simcard", ifdev );
					*bsim_setting = 1;
				}
				ret = terror;
			}
		}
		/************** bsim process *********************/
		else
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



	/* ifdev connect */
	info( "%s connect", ifdev );
	if ( scallt( ifdev, "connect", cfg ) != ttrue )
	{
		fault( "%s connect failed", ifdev );
		talk_free( cfg );
		sleep( 5 );
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
		if ( scallt( ifdev, "connected", cfg ) == ttrue )
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



    /* get the netdev */
	netdev = register_value( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
		warn( "%s modify the mode to ppp when cannot find netdev", ifdev );
    	mode = "ppp";
		method = "disable";
		json_set_string( cfg, "mode", "ppp" );
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
		/* ipv4 ppp setting */
		if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
		{
			v = scalls( ifdev, "profile", NULL );
			ret = ppp_client_connect( object, ifdev, cfg, v );
			talk_free( v );
		}
		/* ipv4 dhcp client setting */
		else
		{
			v = json_value( cfg, "dhcpc" );
			ret = dhcp_client_connect( object, ifdev, netdev, v );
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
	const char *ttl;
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
	char path[PATH_MAX];

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

	/* get the custom_dns */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	value = json_value( cfg, mode );
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enbale" ) )
	{
		dns = json_string( value, "dns" );
		dns2 = json_string( value, "dns2" );
		register_set( object, "custom_dns", "enbale", strlen("enbale")+1, 20 );
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
		route_switch( dns, NULL, NULL, v, true );
	}
	register_set( object, "dns", dns, stringlen(dns)+1, 20 );
	if ( dns2 != NULL && *dns2 != '\0' )
	{
		string3file( path, "nameserver %s\n", dns2 );
		route_switch( dns2, NULL, NULL, v, true );
	}
	register_set( object, "dns2", dns2, stringlen(dns2)+1, 20 );

	gateway = json_string( v, "gw" );
	info( "%s(%s) online[ %s, %s ]", object, netdev, gateway?:"", dns?:"" );

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
	pmtu_adjust_ifname( object, netdev );

	/* ttl */
	ttl = json_string( cfg, "ttl" );
	ttl_set_ifname( object, netdev, ttl );

	/* tid route table init */
	tid = register_pointer( object, "tid" );
	if ( tid != NULL && *tid != 0 )
	{
		routes_create_ifname( *tid, v );
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
		iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
		if ( 0 == strncmp( netdev, "ppp", 3 ) )
		{
			/* ppp tcp mss */
			iptables( "-t mangle -D POSTROUTING -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -m tcpmss --mss 1400:1536 -j TCPMSS --clamp-mss-to-pmtu", netdev );
		}
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



boole _set( obj_t this, talk_t v, attr_t path )
{
	int i;
    boole ret;
    boole dret;
    talk_t axp;
    talk_t bak;
    talk_t cfg;
    talk_t info;
    talk_t mcfg;
    const char *ptr;
	const char *ifdev;
	const char *object;

	object = obj_combine( this );
    /* get the ifdev */
	ifdev = register_pointer( object, "ifdev" );

    ret = dret = false;
    ptr = attr_layer( path, 1 );
    if ( ptr == NULL || *ptr == '\0' )
    {
        /* delete the configure */
        if ( v == NULL )
        {
            dret = config_sset( ifdev, NULL, NULL );
            ret = config_set( this, NULL, NULL );
        }
        else
        {
            /* separation the modem cfg and ifname cfg */
            axp = NULL;
            cfg = json_create( NULL );
            mcfg = json_create( NULL );
            while ( NULL != ( axp = json_next( v, axp ) ) )
            {
                ptr = axp_id( axp );
				info = axp_get_value( axp );
				if ( 0 == strcmp( ptr, "lock_imei" )
					|| 0 == strcmp( ptr, "lock_imsi" )
					|| 0 == strcmp( ptr, "lock_pin" )
					|| 0 == strcmp( ptr, "lock_nettype" )
					|| 0 == strcmp( ptr, "lock_band" )
					|| 0 == strcmp( ptr, "lock_arfcn" )
					|| 0 == strcmp( ptr, "lock_cellid" )
					|| 0 == strcmp( ptr, "gnss" )

					|| 0 == strcmp( ptr, "need_simcard" )
					|| 0 == strcmp( ptr, "simcard_failed_threshold" )
					|| 0 == strcmp( ptr, "simcard_failed_threshold2" )
					|| 0 == strcmp( ptr, "simcard_failed_threshold3" )
					|| 0 == strcmp( ptr, "simcard_failed_everytime" )

					|| 0 == strcmp( ptr, "need_plmn" )
					|| 0 == strcmp( ptr, "need_signal" )
					|| 0 == strcmp( ptr, "signal_failed_threshold" )
					|| 0 == strcmp( ptr, "signal_failed_threshold2" )
					|| 0 == strcmp( ptr, "signal_failed_threshold3" )
					|| 0 == strcmp( ptr, "signal_failed_everytime" )

					|| 0 == strcmp( ptr, "watch_interval" )

					|| 0 == strcmp( ptr, "profile" )
					|| 0 == strcmp( ptr, "profile_cfg" )

					|| 0 == strcmp( ptr, "bsim" )
					|| 0 == strcmp( ptr, "bsim_cfg" )

					|| 0 == strcmp( ptr, "ssim" )
					|| 0 == strcmp( ptr, "ssim_cfg" )

					|| 0 == strcmp( ptr, "sms" )
					|| 0 == strcmp( ptr, "sms_cfg" )
					)
				{
					json_set_value( mcfg, ptr, talk_dup(info) );
				}
                else
                {
                    json_set_value( cfg, ptr, talk_dup(info) );
                }
            }
            /* save the modem config */
			bak = config_sget( ifdev, NULL );
			if ( talk_equal( bak, mcfg ) == false )
			{
				dret = config_sset( ifdev, mcfg, NULL );
			}
			talk_free( bak );
            talk_free( mcfg );
            /* save the ifname config */
			bak = config_get( this, NULL );
			if ( talk_equal( bak, cfg ) == false )
			{
	            ret = config_set( this, cfg, NULL );
			}
			talk_free( bak );
            talk_free( cfg );
        }
    }
    else
    {
		if ( 0 == strcmp( ptr, "lock_imei" )
			|| 0 == strcmp( ptr, "lock_imsi" )
			|| 0 == strcmp( ptr, "lock_pin" )
			|| 0 == strcmp( ptr, "lock_nettype" )
			|| 0 == strcmp( ptr, "lock_band" )
			|| 0 == strcmp( ptr, "lock_arfcn" )
			|| 0 == strcmp( ptr, "lock_cellid" )
			|| 0 == strcmp( ptr, "gnss" )
		
			|| 0 == strcmp( ptr, "need_simcard" )
			|| 0 == strcmp( ptr, "simcard_failed_threshold" )
			|| 0 == strcmp( ptr, "simcard_failed_threshold2" )
			|| 0 == strcmp( ptr, "simcard_failed_threshold3" )
			|| 0 == strcmp( ptr, "simcard_failed_everytime" )

			|| 0 == strcmp( ptr, "need_plmn" )
			|| 0 == strcmp( ptr, "need_signal" )
			|| 0 == strcmp( ptr, "signal_failed_threshold" )
			|| 0 == strcmp( ptr, "signal_failed_threshold2" )
			|| 0 == strcmp( ptr, "signal_failed_threshold3" )
			|| 0 == strcmp( ptr, "signal_failed_everytime" )

			|| 0 == strcmp( ptr, "watch_interval" )
		
			|| 0 == strcmp( ptr, "profile" )
			|| 0 == strcmp( ptr, "profile_cfg" )
		
			|| 0 == strcmp( ptr, "bsim" )
			|| 0 == strcmp( ptr, "bsim_cfg" )
		
			|| 0 == strcmp( ptr, "ssim" )
			|| 0 == strcmp( ptr, "ssim_cfg" )

			|| 0 == strcmp( ptr, "sms" )
			|| 0 == strcmp( ptr, "sms_cfg" )
			)
		{
			dret = config_sset( ifdev, v, path );
		}
        else
        {
            ret = config_set( this, v, path );
        }
    }

    /* shut and setup the lte */
    if ( dret == true )
    {
        scall( ifdev, "shut", NULL );
		/* clear the failed count */
		i = 0;
		register_set( object, "connect_failed", &i, sizeof(i), 0 );
		/* mark the config not sync */
		i = 0;
		register_set( ifdev, "setting_sync", &i, sizeof(i), 0 );
        scall( ifdev, "setup", NULL );
    }
    else if ( ret == true )
    {
        _shut( this, NULL );
		/* clear the failed count */
		i = 0;
		register_set( object, "connect_failed", &i, sizeof(i), 0 );
        _setup( this, NULL );
    }
    return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	talk_t ret;
    talk_t cfg;
	talk_t mcfg;
	const char *ifdev;
	const char *object;

	object = obj_combine( this );
	/* get the ifname cfg */
    cfg = config_get( this, NULL );
    /* get the modem cfg */
	ifdev = register_value( object, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
    {
        /* combination the cfg */
        mcfg = config_sget( ifdev, NULL );
        if ( cfg == NULL )
        {
            cfg = mcfg;
        }
        else
        {
            json_patch( mcfg, cfg );
            talk_free( mcfg );
        }
    }

    /* get the path attr */
    ret = attr_cut( cfg, path );
    if ( ret != cfg )
    {
        talk_free( cfg );
    }
	return ret;
}    



