/*
 *    Description:  dropbear server management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landnet/landnet.h"



talk_t _setup( obj_t this, param_t param )
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
	        iptables( "-t filter -A INPUT -p tcp --dport %s -j  %s_%s", port, PROJECT_ID, COM_ID );
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
	if ( manager_init == true )
	{
		iptables( "-A %s_%s -j DROP", PROJECT_ID, COM_ID );
	}

    /* start the service */
    service_start( COM_IDPATH, COM_IDPATH, "service", NULL );

    talk_free( cfg );
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -D INPUT -j  %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -X %s_%s", PROJECT_ID, COM_ID );
    service_reset( COM_IDPATH );
    return ttrue;
}
talk_t _reset( obj_t this, param_t param )
{
    service_reset( COM_IDPATH );
    return ttrue;
}
char *reg_local_netdev;
talk_t _service( obj_t this, param_t param )
{
    int i;
    talk_t cfg;
    const char *port;
    const char *remote;
    char lanip[NAME_MAX];
    char address[NAME_MAX];
	const char *local_netdev = NULL;

    cfg = config_sget( COM_IDPATH, NULL );
    if ( cfg == NULL )
    {
        return TALK_ERROR;
    }
	/* get the local netdev */
	nreg2string( "local_netdev", reg_local_netdev, local_netdev, NULL );
    /* get the port */
    port = json_get_string( cfg, "port" );
    if ( port == NULL || *port == '\0' )
    {
        port = "22";
    }
    /* get the remote allow */
    remote = json_get_string( cfg, "remote" );
    if ( remote != NULL && 0 == strcmp( remote, "enable" ) )
    {
        debug( "dropbear -F -p %s", port );
        execlp( "dropbear", "dropbear", "-F", "-p", port, "-K", "300", (char*)0 );
    }
    else
    {
        address[0] = '\0';
        /* wait the lan up */
		if ( local_netdev != NULL || *local_netdev != '\0' )
		{
	        for ( i=0; i<5; i++ )
	        {
	            if ( netdev_info( local_netdev, lanip, sizeof(lanip), NULL, 0, NULL, 0, NULL, 0 ) == 0 )
	            {
	                snprintf( address, sizeof(address), "%s:%s", lanip, port );
	                break;
	            }
	            sleep( 2 );
	        }
		}
        if ( address[0] == '\0' )
        {
            debug( "dropbear -F -p %s", port );
            execlp( "dropbear", "dropbear", "-F", "-p", port, "-K", "300", (char*)0 );
        }
        else
        {
            debug( "dropbear -F -p %s", address);
            execlp( "dropbear", "dropbear", "-F", "-p", address, "-K", "300", (char*)0 );
        }
    }

    talk_free( cfg );
    /* execl */
    faulting( "exec the dropbear error" );
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
        scall( "forward@firewall", "setup", NULL );
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



