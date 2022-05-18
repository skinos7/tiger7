/*
 *    Description:  dropbear server management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landnet/landnet.h"



boole_t _setup( obj_t this, param_t param )
{
    talk_t v;
	talk_t axp;
    talk_t cfg;
    const char *ptr;
    const char *port;
	boole manager_init;
	struct in_addr iptest;
    const char *test_dropbear = "/tmp/.dropbear_exsit";

    /* test the dropbear have */
    shell( "which dropbear > %s", test_dropbear );
    ptr = file2string( test_dropbear, NULL, 0 );
    if ( ptr == NULL || strlen( ptr ) < 8 )
    {
        shell( "rm %s", test_dropbear );
        return tfalse;
    }
    shell( "rm %s", test_dropbear );
    /* get the configure */
    cfg = config_sget( COM_IDPATH, NULL );
    /* get the status */
    ptr = json_get_string( cfg, "status" );
    if ( ptr == NULL || 0 != strcmp( ptr, "enable" ) )
    {
        talk_free( cfg );
        return tfalse;
    }
    /* get the manager */
	axp = NULL;
	manager_init = false;
	port = json_string( cfg, "port" );
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
				iptables( "-t filter -N %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -D INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
				iptables( "-t filter -A INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
			}
			if ( inet_pton( AF_INET, ptr, &iptest ) == 1 )
			{
				iptables( "-A %s_%s -s %s -j ACCEPT", PROJECT_ID, COM_ID, ptr );
			}
			else
			{
				iptables( "-A %s_%s -m mac --mac-source %s -j ACCEPT", PROJECT_ID, COM_ID, ptr );
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
					iptables( "-t filter -N %s_%s", PROJECT_ID, COM_ID );
					iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
					iptables( "-t filter -D INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
					iptables( "-t filter -A INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
				}
				if ( inet_pton( AF_INET, tok, &iptest ) == 1 )
				{
					iptables( "-A %s_%s -s %s -j ACCEPT", PROJECT_ID, COM_ID, tok );
				}
				else
				{
					iptables( "-A %s_%s -m mac --mac-source %s -j ACCEPT", PROJECT_ID, COM_ID, tok );
				}

				tok = tokkey+1;
			}
		}

	}
	if ( manager_init == true )
	{
		iptables( "-D %s_%s -j DROP", PROJECT_ID, COM_ID );
		iptables( "-A %s_%s -j DROP", PROJECT_ID, COM_ID );
	}

    /* start the service */
    service_start( COM_IDPATH, COM_IDPATH, "service", NULL );

    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
    iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -D INPUT -j  %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -X %s_%s", PROJECT_ID, COM_ID );
    service_delete( COM_IDPATH );
    return ttrue;
}
boole_t _reset( obj_t this, param_t param )
{
    service_reset( COM_IDPATH, COM_IDPATH, "service", NULL );
    return ttrue;
}
boole_t _service( obj_t this, param_t param )
{
    talk_t cfg;
    const char *port;

    cfg = config_sget( COM_IDPATH, NULL );
    if ( cfg == NULL )
    {
        return terror;
    }
    /* get the port */
    port = json_get_string( cfg, "port" );
    if ( port == NULL || *port == '\0' )
    {
        port = "22";
    }
    debug( "dropbear -F -p %s", port );
    /* execl */
    execlp( "dropbear", "dropbear", "-F", "-p", port, "-K", "300", (char*)0 );

    faulting( "exec the dropbear error" );
    talk_free( cfg );
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



