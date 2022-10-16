/*
 *  Description:  Android rndis netcard driver
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landnetcard/landnetcard.h"



/* search the netcard
	ttrue for matched
	tfalse for no match
	terror for error, need reset the netcard */
boole_t _usb_match( obj_t this, param_t param )
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
	if ( 0 == strcasecmp( vid, "0e8d" ) && ( 0 == strcasecmp( pid, "2005" ) ) )
	{
		info( "Android RNDIS netcard found(%s:%s)", vid , pid );
		syspath = json_string( dev, "syspath" );

		/* set the name */
		json_set_string( dev, "name", "Android-RNDIS" );
		/* get the object */
		object = nic_object_get( NIC_COM, syspath, cfg, NULL, 0 );
		json_set_string( dev, "object", object );
		/* find the netdev */
		netdev = usbeth_device_find( syspath, NULL, 0 );
		if ( netdev != NULL )
		{
			json_set_string( dev, "netdev", netdev );
		}
		json_set_string( dev, "devcom", NETCARD_COM );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}

    return tfalse;
}



