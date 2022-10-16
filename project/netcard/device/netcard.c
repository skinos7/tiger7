/*
 *    Description:  netcard device management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"



boole_t _setup( obj_t this, param_t param )
{
	talk_t v;
	const char *netdev;
	const char *object;
	const char *archdev;

	object = obj_combine( this );
	/* 获取模块硬件信息 */
	archdev = register_pointer( object, "archdev" );
	if ( archdev == NULL || *archdev == '\0' )
	{
        fault( "%s cannot find archdev", object );
		return terror;
	}
	v = scalls( archdev, "status", NULL );
	if ( v == NULL )
	{
        fault( "%s cannot get archdev %s status", object, archdev );
		scalls( archdev, "reset", object );
		return terror;
	}
	netdev = json_string( v, "netdev" );

	info( "%s(%s) add to network frame", object, netdev?:"" );
	/* tell the network layer */
	scalls( NETWORK_COM, "add", "%s,%s", object, netdev?:"" );

	talk_free( v );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	const char *object;

	object = obj_combine( this );
    /* delete the network device from network frame */
	info( "%s delete from network frame", object );
    scalls( NETWORK_COM, "delete", object );
	/* disconect first */
	scall( object, "down", NULL );
	return ttrue;
}



talk_t _netdev( obj_t this, param_t param )
{
    const char *object;
	const char *netdev;

    object = obj_combine( this );
	netdev = register_pointer( object, "netdev" );
	if ( netdev == NULL )
	{
		return NULL;
	}
	return string2x( netdev );
}
boole_t _setmac( obj_t this, param_t param )
{
    talk_t v;
    talk_t ret;
    const char *mac;
    const char *netdev;

    ret = tfalse;
    mac = param_string( param, 1 );
    v = _netdev( this, NULL );
    netdev = x2string( v );
    if ( netdev != NULL && netdev_flags( netdev, IFF_BROADCAST ) > 0 )
    {
        info( "%s(%s) mac set to %s", obj_combine(this), netdev, mac);
        if ( netdev_flags( netdev, IFF_UP ) > 0 )
        {
            execute( 0, 1, "ifconfig %s down", netdev );
            execute( 0, 1, "ifconfig %s hw ether %s", netdev, mac );
            execute( 0, 1, "ifconfig %s up", netdev );
        }
        else
        {
            execute( 0, 1, "ifconfig %s hw ether %s", netdev, mac );
        }
        ret = ttrue;
    }

    talk_free( v );
    return ret;
}
boole_t _up( obj_t this, param_t param )
{
    talk_t ret;
	const char *object;
    const char *netdev;

	object = obj_combine( this );
	/* get the netdev */
	netdev = register_pointer( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	/* up the device */
    ret = tfalse;
    if ( netdev_flags( netdev, IFF_BROADCAST ) > 0 )
    {
        info( "%s(%s) up", object, netdev );
		shell( "ifconfig %s 0.0.0.0 up", netdev );
        ret = ttrue;
    }

    return ret;
}
boole_t _down( obj_t this, param_t param )
{
	talk_t ret;
	const char *object;
	const char *netdev;

	object = obj_combine( this );
	/* get the netdev */
	netdev = register_pointer( object, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
    /* down the device */
	ret = tfalse;
    if ( netdev_flags( netdev, IFF_BROADCAST ) > 0 )
	{
		info( "%s(%s) down", object, netdev );
		shell( "ifconfig %s down", netdev );
		ret = ttrue;
	}

	return ret;
}
talk_t _connect( obj_t this, param_t param )
{
    return ttrue;
}
boole_t _connected( obj_t this, param_t param )
{
    return ttrue;
}
talk_t _status( obj_t this, param_t param )
{
	return NULL;
}
talk_t _online( obj_t this, param_t param )
{
	return ttrue;
}



