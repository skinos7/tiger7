/*
 *    Description:  common IEEE802.11n driver
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landwifi/landwifi.h"



boole_t _pci_match( obj_t this, param_t param )
{
    talk_t dev;
    talk_t cfg;
    const char *vid;
    const char *pid;
	const char *netdev;
    const char *object;
	const char *syspath;

	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
		return tfalse;
	}
    cfg = param_talk( param, 2 );
	/* get the device vid and pid */
	vid = json_string( dev, "vid" );
	pid = json_string( dev, "pid" );
	if ( vid == NULL || pid == NULL )
	{
		return tfalse;
	}
	/* compare the vid and vid is in support list */
	if ( ( 0 == strcasecmp( vid, "168c" ) && 0 == strcasecmp( pid, "003c" ) )
		)
	{
		syspath = json_string( dev, "syspath" );
		/* find the netdev */
		netdev = pcieth_device_find( syspath, NULL, 0 );
		if ( netdev == NULL )
		{
			return tfalse;
		}
		info( "PCI ath11ac-003c found(%s:%s)", vid , pid );
		json_set_string( dev, "netdev", netdev );
		/* get the object */
		object = wifi_object_get( ARADIO_COM, syspath, cfg, NULL, 0 );
		/* set the netdev for object */
		register_set( object, "vid", vid, strlen(vid)+1, 10 );
		register_set( object, "pid", pid, strlen(pid)+1, 10 );
		register_set( object, "netdev", netdev, strlen(netdev)+1, 40 );
		/* set the other infomation */
		json_set_string( dev, "name", "ath11ac-003c" );
		json_set_string( dev, "object", object );
		json_set_string( dev, "devcom", "wifi@radio" );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}
	else if ( ( 0 == strcasecmp( vid, "168c" ) && 0 == strcasecmp( pid, "0030" ) )
		)
	{
		syspath = json_string( dev, "syspath" );
		/* find the netdev */
		netdev = pcieth_device_find( syspath, NULL, 0 );
		if ( netdev == NULL )
		{
			return tfalse;
		}
		info( "PCI ath11bgna-0030 found(%s:%s)", vid , pid );
		json_set_string( dev, "netdev", netdev );
		/* get the object */
		object = wifi_object_get( NRADIO_COM, syspath, cfg, NULL, 0 );
		/* set the netdev for object */
		register_set( object, "vid", vid, strlen(vid)+1, 10 );
		register_set( object, "pid", pid, strlen(pid)+1, 10 );
		register_set( object, "netdev", netdev, strlen(netdev)+1, 40 );
		/* set the other infomation */
		json_set_string( dev, "name", "ath11bgna-0030" );
		json_set_string( dev, "object", object );
		json_set_string( dev, "devcom", "wifi@radio" );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}
	else if ( ( 0 == strcasecmp( vid, "168c" ) && ( 0 == strcasecmp( pid, "002a" ) || 0 == strcasecmp( pid, "002e" ) ) )
		)
	{
		syspath = json_string( dev, "syspath" );
		/* find the netdev */
		netdev = pcieth_device_find( syspath, NULL, 0 );
		if ( netdev == NULL )
		{
			return tfalse;
		}
		info( "PCI ath11bgn-002a found(%s:%s)", vid , pid );
		json_set_string( dev, "netdev", netdev );
		/* get the object */
		object = wifi_object_get( NRADIO_COM, syspath, cfg, NULL, 0 );
		/* set the netdev for object */
		register_set( object, "vid", vid, strlen(vid)+1, 10 );
		register_set( object, "pid", pid, strlen(pid)+1, 10 );
		register_set( object, "netdev", netdev, strlen(netdev)+1, 40 );
		/* set the other infomation */
		json_set_string( dev, "name", "ath11bgn-002a" );
		json_set_string( dev, "object", object );
		json_set_string( dev, "devcom", "wifi@radio" );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}
	return tfalse;
}
boole_t _sdio_match( obj_t this, param_t param )
{
    talk_t dev;
    talk_t cfg;
    const char *vid;
    const char *pid;
	const char *netdev;
    const char *object;
	const char *syspath;

	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
		return tfalse;
	}
    cfg = param_talk( param, 2 );
	/* get the device vid and pid */
	vid = json_string( dev, "vid" );
	pid = json_string( dev, "pid" );
	if ( vid == NULL || pid == NULL )
	{
		return tfalse;
	}
	/* compare the vid and vid is in support list */
	if ( ( 0 == strcasecmp( vid, "02d0" ) && 0 == strcasecmp( pid, "aae8" ) )
		)
	{
		syspath = json_string( dev, "syspath" );
		/* find the netdev */
		netdev = sdioeth_device_find( syspath, NULL, 0 );
		if ( netdev == NULL )
		{
			return tfalse;
		}
		info( "SDIO AP6275 found(%s:%s)", vid , pid );
		json_set_string( dev, "netdev", netdev );
		/* get the object */
		object = wifi_object_get( ARADIO_COM, syspath, cfg, NULL, 0 );
		/* set the netdev for object */
		register_set( object, "vid", vid, strlen(vid)+1, 10 );
		register_set( object, "pid", pid, strlen(pid)+1, 10 );
		register_set( object, "netdev", netdev, strlen(netdev)+1, 40 );
		/* set the other infomation */
		json_set_string( dev, "name", "AP6275" );
		json_set_string( dev, "object", object );
		json_set_string( dev, "devcom", "wifi@radio" );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}

	return tfalse;
}
talk_t _options( obj_t this, param_t param )
{
	const char *vid;
	const char *pid;
	const char *object;

	object = param_string( param, 1 );
	vid = register_value( object, "vid" );
	pid = register_value( object, "pid" );
	if ( vid == NULL || *vid == '\0' || pid == NULL || *pid == '\0' )
	{
		return NULL;
	}
	return config_sgets( COM_IDPATH, "%s/%s/options", vid, pid );
}
talk_t _hostapd_config( obj_t this, param_t param )
{
	const char *vid;
	const char *pid;
	const char *object;

	object = param_string( param, 1 );
	vid = register_value( object, "vid" );
	pid = register_value( object, "pid" );
	if ( vid == NULL || *vid == '\0' || pid == NULL || *pid == '\0' )
	{
		return NULL;
	}
	return config_sgets( COM_IDPATH, "%s/%s/hostapd", vid, pid );
}



