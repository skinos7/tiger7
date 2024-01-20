/*
 *  Description:  test the network connect management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



boole_t _setup( obj_t this, param_t param )
{
    const char *ptr;

    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        sstart( COM_IDPATH, "service", NULL, COM_IDPATH );
    }

    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
    sdelete( COM_IDPATH );
    return ttrue;
}

talk_t _service( obj_t this, param_t param )
{
	int i;
	int type;
	boole on, on2;
	char ip[NAME_MAX];
	char ip2[NAME_MAX];

	type = 0;
	do
	{
		for( i=0; i<200; i++ )
		{
			on = on2 = false;
			if ( netdev_flags( "usb0", IFF_UP) > 0 && netdev_info( "usb0", ip, sizeof(ip), NULL, 0, NULL, 0, NULL, 0 ) == 0 )
			{
				on = true;
			}
			if ( netdev_flags( "usb1", IFF_UP) > 0 && netdev_info( "usb1", ip2, sizeof(ip2), NULL, 0, NULL, 0, NULL, 0 ) == 0 )
			{
				on2 = true;
			}
			if ( on == true && on2 == true )
			{
				break;
			}
			sleep( 3 );
		}
		info( "################### all online %s %s #######################", ip, ip2 );

		sleep( 20 );
		if ( (type%3) == 0 )
		{
			info( "################### crack keeplive %d with kill modem #######################", type );
			sreset( NULL, NULL, NULL, LTE_COM );
			sreset( NULL, NULL, NULL, LTE2_COM );
		}
		else if ( (type%5) == 0 )
		{
			info( "################### crack keeplive %d with kill connect #######################", type );
			sreset( NULL, NULL, NULL, WLTE_COM );
			sreset( NULL, NULL, NULL, WLTE2_COM );
		}
		else if ( type%10 == 0 )
		{
			info( "################### crack keeplive %d with power modem #######################", type );
			scalls( GPIO_COM, "out", "%s_reset,0", LTE_COM );
			scalls( GPIO_COM, "out", "%s_reset,0", LTE2_COM );
		}
		else
		{
			shell( "ifconfig usb0 down" );
			shell( "ifconfig usb1 down" );
		}
		type++;
		sleep( 30 );

	}
	while (1);

	while(1)
	{
		info( "################### keeplive lost #######################" );
		scalls( GPIO_COM, "action", "test/succeed" );
		sleep( 30 );
	}


	return tfalse;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

    ret = config_sset( COM_IDPATH, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	return config_sget( COM_IDPATH, path );
}



