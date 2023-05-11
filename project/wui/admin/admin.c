/*
 *  Description:  admin manange web server management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



boole_t _ftpdebug( obj_t this, param_t param )
{
	struct stat st;
	const char *ptr;
    const char *object;
	talk_t ftpcfg, ftpbak;
    char rootdir[PATH_MAX];

	object = obj_combine( this );
	/* make the configure */
	ptr = PROJECT_TMP_DIR"/webpage";
	ftpcfg = sget( "storage@ftp", NULL );
	if ( ftpcfg == NULL )
	{
		ftpcfg = json_create( NULL );
	}
	ftpbak = talk_dup( ftpcfg );
	attr_sets_string( ftpcfg, "enable", "status" );
	attr_sets_string( ftpcfg, "user", "mode" );
	attr_sets_string( ftpcfg, ptr, "root" );
	attr_sets_string( ftpcfg, ptr, "user/media/path" );
	attr_sets_string( ftpcfg, "all", "user/media/permission" );
	/* set the ftp configure */
	if ( talk_equal( ftpcfg, ftpbak ) == false )
	{
		sset( "storage@ftp", ftpcfg, NULL );
	}
	talk_free( ftpcfg );
	talk_free( ftpbak );

	/* get the webpage directory */
	project_exe_path( rootdir, sizeof(rootdir), PROJECT_ID, obj_com(this) );
	if ( stat( ptr, &st) != 0 )
	{
		shell( "cp -r %s %s", rootdir, ptr );
	}

	/* run the service */
    sruns( 0, object, object, "service", ptr );
	return ttrue;
}
boole_t _nfsdebug( obj_t this, param_t param )
{
	struct stat st;
    const char *ptr;
	const char *host;
    const char *object;
    char rootdir[PATH_MAX];

	object = obj_combine( this );
	/* make the configure and mount the file system */
	ptr = PROJECT_TMP_DIR"/webpage";
	host = param_string( param, 1 );
    if ( host == NULL )
    {
    	host = "192.168.8.250:/nfs";
		shell( "ifconfig lan:8 192.168.8.3" );
    }

	project_exe_path( rootdir, sizeof(rootdir), PROJECT_ID, obj_com(this) );
	if ( stat( ptr, &st) != 0 )
	{
		shell( "cp -r %s %s", rootdir, ptr );
		shell( "mount -o nolock -o sync -t nfs %s %s", host, ptr );
	}
	else
	{
		shell( "umount %s", ptr );
		shell( "mount -o nolock -o sync -t nfs %s %s", host, ptr );
	}

	/* run the service */
    sruns( 0, object, object, "service", ptr );
	return ttrue;
}



boole_t _ttyd_he( obj_t this, param_t param )
{
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

    shell( "ttyd -p %s /bin/hetui >/dev/null 2>&1 &", termport );
    return ttrue;
}   
boole_t _ttyd_lte( obj_t this, param_t param )
{
	talk_t stat;
	const char *ptr;
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

	object = param_string( param, 1 );
	if ( object == NULL )
	{
		object = MODEM_COM;
	}
	scall( USB_COM, "shut", NULL );
	stat = scall( object, "status", NULL );
	ptr = json_get_string( stat, "mtty" );
	if ( ptr == NULL || *ptr == '\0' )
	{
		ptr = json_get_string( stat, "stty" );
	}
	if ( ptr == NULL || *ptr == '\0' )
	{
		talk_free( stat );
		return tfalse;
	}
	scall( object, "shut", NULL );
	scall( USB_COM, "shut", NULL );
    shell( "ttyd -p %s /bin/tip -l %s >/dev/null 2>&1 &", termport, ptr );
	talk_free( stat );
    return ttrue;
}   
boole_t _ttyd_ping( obj_t this, param_t param )
{
	const char *dest;
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

	dest = param_string( param, 1 );
	if ( dest == NULL )
	{
		dest = "127.0.0.1";
	}
    shell( "ttyd -p %s /bin/ping %s >/dev/null 2>&1 &", termport, dest );
    return ttrue;
}   
boole_t _ttyd_traceroute( obj_t this, param_t param )
{
	const char *dest;
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

	dest = param_string( param, 1 );
	if ( dest == NULL )
	{
		dest = "127.0.0.1";
	}
    shell( "ttyd -p %s /usr/bin/traceroute %s >/dev/null 2>&1 &", termport, dest );
    return ttrue;
}   
boole_t _ttyd_tcpdump( obj_t this, param_t param )
{
	const char *dest;
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

	dest = param_string( param, 1 );
	if ( dest == NULL )
	{
		dest = "127.0.0.1";
	}
    shell( "ttyd -p %s /usr/sbin/tcpdump %s >/dev/null 2>&1 &", termport, dest );
    return ttrue;
}   
/* tip service loop */
boole_t _ttyd_serial( obj_t this, param_t param )
{
	talk_t cfg;
    const char *tty;
    const char *speed;
    const char *flow;
    const char *parity;
    const char *databit;
	const char *object;
	char termport[NAME_MAX];

	/* get the term port */
	object = obj_combine( this );
    shell( "killall -9 ttyd" );
	strncpy( termport, "81", sizeof(termport) );
	if ( config_sgets_string( termport, sizeof(termport), object, "termport" ) == NULL )
	{
		strncpy( termport, "81", sizeof(termport) );
	}

	object = param_string( param, 1 );
	if ( object == NULL )
	{
		object = "uart@serial";
	}
    /* get the configure */
	cfg = config_sget( object, NULL );
	tty = json_get_string( cfg, "ttydev" );
	if ( tty == NULL )
	{
		fault( "tip cannot find the tty" );
		talk_free( cfg );
		return tfalse;
	}
	scall( object, "shut", NULL );
	speed = json_get_string( cfg, "speed" );
	flow = json_get_string( cfg, "flow" );
	if ( flow != NULL && 0 == strcmp( flow, "hard") )
	{
		flow = "-r";
	}
	else if ( flow != NULL && 0 == strcmp( flow, "soft") )
	{
		flow = "-x";
	}
	else
	{
		flow = "";
	}
	parity = json_get_string( cfg, "parity" );
	if ( parity != NULL && 0 == strcmp( parity, "even") )
	{
		parity = "-e";
	}
	else if ( parity != NULL && 0 == strcmp( parity, "odd") )
	{
		parity = "-o";
	}
	else
	{
		parity = "";
	}
	databit = json_get_string( cfg, "databit" );
	if ( databit != NULL && 0 == strcmp( databit, "5") )
	{
		databit = "-5";
	}
	else if ( databit != NULL && 0 == strcmp( databit, "6") )
	{
		databit = "-6";
	}
	else if ( databit != NULL && 0 == strcmp( databit, "7") )
	{
		databit = "-7";
	}
	else
	{
		databit = "";
	}

    shell( "ttyd -p %s /bin/tip -s %s %s %s %s -l %s >/dev/null 2>&1 &", termport, speed, flow, parity, databit, tty );
	talk_free( cfg );
    return ttrue;
}
boole_t _ttyd_kill( obj_t this, param_t param )
{
    shell( "killall -9 ttyd" );
    return ttrue;
}   



boole_t _setup( obj_t this, param_t param )
{
    talk_t v;
	talk_t axp;
    talk_t cfg;
	const char *object;
    const char *ptr;
    const char *port;
    const char *sslport;
	boole manager_init;
	struct in_addr iptest;

	/* get the configure */
	object = obj_combine( this );
	cfg = config_sget( object, NULL );
	/* get the status */
	ptr = json_string( cfg, "status" );
	if ( ptr == NULL || 0 != strcmp( ptr, "enable" ) )
	{
		talk_free( cfg );
		return ttrue;
	}

    /* set the manager */
	axp = NULL;
	manager_init = false;
	port = json_string( cfg, "port" );
	sslport = json_string( cfg, "sslport" );
    v = json_value( cfg, "manager" );
	if ( json_check( v ) == true )
	{
		while( NULL != ( axp = json_next( v, axp ) ) )
		{
			ptr = axp_string( axp );
			if ( ptr == NULL || *ptr == '\0' )
			{
				continue;
			}
			if ( manager_init == false )
			{
				manager_init = true;
				iptables( "-t filter -N %s", object );
				iptables( "-t filter -F %s", object );
				if ( port != NULL && *port != '\0' )
				{
					iptables( "-t filter -D INPUT -p tcp --dport %s -j %s", port, object );
					iptables( "-t filter -A INPUT -p tcp --dport %s -j %s", port, object );
				}
				if ( sslport != NULL && *sslport != '\0' )
				{
					iptables( "-t filter -D INPUT -p tcp --dport %s -j %s", sslport, object );
					iptables( "-t filter -A INPUT -p tcp --dport %s -j %s", sslport, object );
				}
			}
			if ( inet_pton( AF_INET, ptr, &iptest ) == 1 )
			{
				iptables( "-A %s -s %s -j ACCEPT", object, ptr );
			}
			else
			{
				iptables( "-A %s -m mac --mac-source %s -j ACCEPT", object, ptr );
			}
		}
	}
	else
	{
		ptr = json_string( cfg, "manager" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			char *tok;
			char *tokkey;
			char buffer[LINE_MAX];
		
			memset( buffer, 0, sizeof(buffer) );
			strncpy( buffer, ptr, sizeof(buffer)-1 );
			tokkey = tok = buffer;
			while( tokkey != NULL && *tok != '\0' )
			{
				tokkey = strstr( tok, ";" );
				if ( tokkey != NULL )
				{
					*tokkey = '\0';
				}

				if ( manager_init == false )
				{
					manager_init = true;
					iptables( "-t filter -N %s", object );
					iptables( "-t filter -F %s", object );
					if ( port != NULL && *port != '\0' )
					{
						iptables( "-t filter -D INPUT -p tcp --dport %s -j %s", port, object );
						iptables( "-t filter -A INPUT -p tcp --dport %s -j %s", port, object );
					}
					if ( sslport != NULL && *sslport != '\0' )
					{
						iptables( "-t filter -D INPUT -p tcp --dport %s -j %s", sslport, object );
						iptables( "-t filter -A INPUT -p tcp --dport %s -j %s", sslport, object );
					}
				}
				if ( inet_pton( AF_INET, tok, &iptest ) == 1 )
				{
					iptables( "-A %s -s %s -j ACCEPT", object, tok );
				}
				else
				{
					iptables( "-A %s -m mac --mac-source %s -j ACCEPT", object, tok );
				}

				tok = tokkey+1;
			}
		}
	}
	if ( manager_init == true )
	{
		iptables( "-D %s -j DROP", object );
		iptables( "-A %s -j DROP", object );
	}

    /* start the service */
	ptr = json_string( cfg, "ftpdebug" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		_ftpdebug( this, param );
	}
	else
	{
		srun( 0, object, "service", NULL, object );
	}

    talk_free( cfg );
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
	const char *object;

	/* shutdown all the webserver */
	object = obj_combine( this );
    /* clear the manager */
    iptables( "-t filter -F %s", object );
    iptables( "-t filter -D INPUT -j %s", object );
    iptables( "-t filter -X %s", object );
	/* stop the service */
    sdelete( object );

    return ttrue;
}
boole_t _check( obj_t this, param_t param )
{
    const char *passwd;
    const char *account;

    account = param_string( param, 1 );
    passwd = param_string( param, 2 );
    if ( account == NULL || passwd == NULL )
    {
        return tfalse;
    }
    return scalls( AUTH_COM, "check", "%s,%s,%s", obj_com(this), account, passwd );
}
talk_t _service( obj_t this, param_t param )
{
    talk_t cfg;
	struct stat st;
	const char *ptr;
    const char *port;
	const char *sslport;
	const char *obj;
	const char *object;
    const char *login;
	const char *platform;
	const boole *config_ready;
    char prjpath[PATH_MAX];
    char httpbin[PATH_MAX];
    char rootdir[PATH_MAX];
    char authtxt[PATH_MAX];
    char routetxt[PATH_MAX];
    char httplisten[LINE_MAX];
    char httpslisten[LINE_MAX];

	/* get the component configure */
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return tfalse;
    }
	obj = obj_com( this );
	object = obj_combine( this );
	project_path( prjpath, sizeof(prjpath), WEBS_PROJECT );

    /* get the goahead path */
    project_exe_path( httpbin, sizeof(httpbin), WEBS_PROJECT, "goahead" );
    /* get the port to build "http:// *:80,https:// *:443" */
	httplisten[0] = httpslisten[0] = '\0';
    port = json_string( cfg, "port" );
    sslport = json_string( cfg, "sslport" );
    if ( port != NULL && *port != '\0' && sslport != NULL && *sslport != '\0' )
    {
		snprintf( httplisten, sizeof(httplisten), "http://*:%s", port );
		snprintf( httpslisten, sizeof(httpslisten), "https://*:%s", sslport );
    }
	else if ( sslport != NULL && *sslport != '\0' )
	{
		snprintf( httpslisten, sizeof(httpslisten), "https://*:%s", sslport );
	}
	else
	{
		if ( port == NULL || *port == '\0' )
		{
			port = "80";
		}
		snprintf( httplisten, sizeof(httplisten), "http://*:%s", port );
	}
    /* get defaul rootdir, command line arguments are preferre */
    project_exe_path( rootdir, sizeof(rootdir), PROJECT_ID, obj );
	ptr = param_string( param, 1 );
	if ( ptr != NULL && *ptr != '\0' )
	{
		strncpy( rootdir, ptr, sizeof(rootdir)-1 );
		rootdir[sizeof(rootdir)-1] = '\0';
	}
	if ( stat( rootdir, &st ) != 0 )
	{
		fault( "%s exit beacuse cannot find the wui page dir %s", object, rootdir );
		return TALK_EINVAL;
	}
    /* get auth.txt */
    snprintf( authtxt, sizeof(authtxt), "%s/auth.txt", rootdir );
    /* get route.txt */
    snprintf( routetxt, sizeof(routetxt), "%s/route.txt", rootdir );

    /* get the login need */
    login = json_string( cfg, "login" );
    if ( login != NULL && 0 == strcmp( login, "disable" ) )
    {
        snprintf( routetxt, sizeof(routetxt), "%s/route_nologin.txt", rootdir );
    }
    /* check the mac when factory mode */
	platform = register_value( LAND_PROJECT, "platform" );
	if ( platform == NULL || 0 != strcmp( platform, "host" ) )
	{
		config_ready = register_value( LAND_PROJECT, "config_ready" );
	    if ( config_ready == NULL || *config_ready == false )
	    {
	        snprintf( routetxt, sizeof(routetxt), "%s/route_factory.txt", rootdir );
	    }
	}

	// goahead-5.2.0
	/* /prj/webs/goahead --object wui@admin --home /prj/webs/ --auth /tmp/webpage/auth.txt --route /tmp/webpage/route.txt --debugger /tmp/webpage http:// *:80 */
	/* /prj/webs/goahead --object wui@admin --home /prj/webs/ --auth /prj/wui/admin/auth.txt --route /prj/wui/admin/route.txt --debugger --verbose /prj/wui/admin http:// *:80 */
	/* /prj/webs/goahead --object wui@admin --home /prj/webs/ --auth /prj/wui/admin/auth.txt --route /prj/wui/admin/route_factory.txt --debugger --verbose /prj/wui/admin http://192.168.8.1:80 */
	/* /prj/webs/goahead --object wui@admin --home /prj/webs/ --auth /tmp/webpage/auth.txt --route /tmp/webpage/route.txt --debugger --verbose /tmp/webpage http://192.168.8.1:80 https://192.168.8.1:443 */
	/* /prj/webs/goahead --object wui@admin --home /prj/webs/ --auth /prj/wui/admin/auth.txt --route /prj/wui/admin/route.txt --debugger --verbose /prj/wui/admin http://192.168.8.1:80 https://192.168.8.1:443 */
	debug( "%s --object %s --home %s --auth %s --route %s --debugger %s %s %s", httpbin, object, prjpath, authtxt, routetxt, rootdir, httplisten, httpslisten );
	if ( httplisten[0] != '\0' && httpslisten[0] != '\0' )
	{
		execlp( httpbin, httpbin, "--object", object, "--home", prjpath, "--auth", authtxt, "--route", routetxt, "--debugger", rootdir, httplisten, httpslisten, (char*)0 );
	}
	else if ( httpslisten[0] != '\0' )
	{
		execlp( httpbin, httpbin, "--object", object, "--home", prjpath, "--auth", authtxt, "--route", routetxt, "--debugger", rootdir, httpslisten, (char*)0 );
	}
	else
	{
		execlp( httpbin, httpbin, "--object", object, "--home", prjpath, "--auth", authtxt, "--route", routetxt, "--debugger", rootdir, httplisten, (char*)0 );
	}

	// embedthis-goahead-d19898e
	/* /prj/webs/goahead --object wui@admin --listen http://192.168.8.1:80 --rootdir /prj/wui/admin --default index.html -a /prj/wui/admin/auth.txt -r /prj/wui/admin/route.txt  --debugger --verbose */
	//debug( "%s --object %s --listen %s --rootdir %s --default %s -a %s -r %s --debugger", httpbin, obj, listenall, rootdir, defaultfile, authtxt, routetxt );
	//execlp( httpbin, httpbin, "--object", object, "--listen", listenall, "--rootdir", rootdir, "--default", defaultfile, "-a", authtxt, "-r", routetxt, "--debugger", (char*)0 );

    faulting( "exec the %s error", httpbin );
    return tfalse;
}



boole _set( obj_t this, talk_t v, attr_t attr )
{
	boole ret;
	talk_t axp;
	const char *ptr;
	const char *start;

	ret = false;
	if ( attr == NULL )
	{
		axp = json_cut_axp( v, "css_file" );
		if ( axp != NULL )
		{
			start = axp_string( axp );
			if ( start != NULL )
			{
				ptr = strrchr( start, '/' );
				if ( ptr != NULL )
				{
					ptr += 1;
					json_set_string( v, "css_file", ptr );
				}
				else
				{
					json_set_string( v, "css_file", start );
				}
			}
			talk_free( axp );
		}
		axp = json_cut_axp( v, "logo_file" );
		if ( axp != NULL )
		{
			start = axp_string( axp );
			if ( start != NULL )
			{
				ptr = strrchr( start, '/' );
				if ( ptr != NULL )
				{
					ptr += 1;
					json_set_string( v, "logo_file", ptr );
				}
				else
				{
					json_set_string( v, "logo_file", start );
				}
			}
			talk_free( axp );
		}
		axp = json_cut_axp( v, "login_file" );
		if ( axp != NULL )
		{
			start = axp_string( axp );
			if ( start != NULL )
			{
				ptr = strrchr( start, '/' );
				if ( ptr != NULL )
				{
					ptr += 1;
					json_set_string( v, "login_file", ptr );
				}
				else
				{
					json_set_string( v, "login_file", start );
				}
			}
			talk_free( axp );
		}
		axp = json_cut_axp( v, "index_file" );
		if ( axp != NULL )
		{
			start = axp_string( axp );
			if ( start != NULL )
			{
				ptr = strrchr( start, '/' );
				if ( ptr != NULL )
				{
					ptr += 1;
					json_set_string( v, "index_file", ptr );
				}
				else
				{
					json_set_string( v, "index_file", start );
				}
			}
			talk_free( axp );
		}
		ret = config_set( this, v, NULL );
	}
	else
	{
		ptr = attr_layer( attr, 1 );
		if ( ptr != NULL )
		{
			ret = config_set( this, v, attr );
		}
	}
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	talk_t ret;
	talk_t cfg;
	const char *css_file;
	const char *logo_file;
	const char *login_file;
	const char *index_file;
	
    cfg = config_get( this, NULL );

	// CSS
	css_file = json_string( cfg, "css_file" );
	if ( css_file == NULL || *css_file == '\0' )
	{
		css_file = "custom.css";
		json_set_string( cfg, "css_file", css_file );
	}

	// Logo
	logo_file = json_string( cfg, "logo_file" );
	if ( logo_file == NULL || *logo_file == '\0' )
	{
		logo_file = "logo.png";
		json_set_string( cfg, "logo_file", logo_file );
	}

	// login.html
	login_file = json_string( cfg, "login_file" );
	if ( login_file == NULL || *login_file == '\0' )
	{
		login_file = "login.html";
		json_set_string( cfg, "login_file", login_file );
	}

	// index.html
	index_file = json_string( cfg, "index_file" );
	if ( index_file == NULL || *index_file == '\0' )
	{
		index_file = "index.html";
		json_set_string( cfg, "index_file", index_file );
	}

    /* get the path attr */
    ret = attr_cut( cfg, path );
    if ( ret != cfg )
    {
        talk_free( cfg );
    }
	return ret;
}



