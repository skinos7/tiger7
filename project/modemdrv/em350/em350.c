/*
 *  Description:  Fibcom NL668 modem driver
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landmodem/atcmd.h"
#include "landmodem/landmodem.h"



/* get iccid
	0 for succeed
	>0 for failed
	<0 for error */
int em350_iccid( atcmd_t fd, talk_t status )
{
	int i;
    int ret;
    char *buf;
    char *ptr;
    const char *start;
    char iccidbuf[NAME_MAX];

	for ( i=0; i<3; i++ )
	{
		//at^ICCID?
		//^ICCID: 89860121802374570731
	    ret = atcmd_send( fd, "AT^ICCID?", 5, NULL, ATCMD_DEF );
	    if ( ret < ATCMD_ret_succeed || ret == ATCMD_ret_term )
	    {
	        return ret;
	    }
	    if ( ret == ATCMD_ret_succeed )
	    {
		    ret = ATCMD_ret_failed;
		    memset( iccidbuf, 0, sizeof(iccidbuf) );
		    buf = atcmd_lastack( fd );
		    if ( sscanf( buf, "%*[^:]:%s", iccidbuf ) == 1 )
		    {
		        // end 
		        ptr = iccidbuf+( strlen(iccidbuf)-1 );
		        while( *ptr != '\0' )
		        {
		            if ( isalnum( *ptr ) )
		            {
		                break;
		            }
		            *ptr = '\0';
		            ptr--;
		        }
		        // start 
		        start = ptr = iccidbuf;
		        while( *ptr != '\0' )
		        {
		            if ( isalnum( *ptr ) )
		            {
		                start = ptr;
		                break;
		            }
		            *ptr = '\0';
		            ptr++;
		        }
		        json_set_string( status, "iccid", start );
		        ret = ATCMD_ret_succeed;
				break;
		    }
	    }
		sleep( 1 );
	}
    return ret;
}
/* get network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int em350_sysinfoex( atcmd_t fd, talk_t status )
{
    int ret;
	int type;
	const char *ptr;

    ret = atcmd_send( fd, "AT^SYSINFOEX", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    type = 0;
    ptr = atcmd_lastack( fd );
    sscanf( ptr, "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%d", &type );
    switch( type )
    {
        case 0:
            json_set_string( status, "nettype", "No Service" );
            ret = ATCMD_ret_failed;
            break;
		case 1:
			json_set_string( status, "nettype", "GSM" );
			break;
        case 2:
            json_set_string( status, "nettype", "CDMA" );
            break;
        case 3:
            json_set_string( status, "nettype", "WCDMA" );
            break;
        case 4:
            json_set_string( status, "nettype", "TDSCDMA" );
            break;
        case 5:
            json_set_string( status, "nettype", "WIMAX" );
            break;
        case 6:
            json_set_string( status, "nettype", "LTE" );
            break;
    }
    return ret;
}
/* get current network connect state
	0 for succeed
	>0 for failed
	<0 for error */
int em350_ndisstatqry( atcmd_t fd )
{
    int ret;
    char *buf;
    int type;

    ret = atcmd_send( fd, "AT^NDISSTATQRY?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    buf = atcmd_lastack( fd );
	type = 0;
    ret = ATCMD_ret_failed;
    if ( sscanf( buf, "%*[^:]:%d", &type ) == 1 )
    {
        if ( type == 1 )
        {
            ret = ATCMD_ret_succeed;
        }
    }
    return ret;
}

/* set the APN profile
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int em350_set_profileset( atcmd_t fd, talk_t profile )
{
	int i;
	int ret;
	talk_t cids;
	talk_t axp;
	const char *ptr;
	const char *cid;
	const char *apn;
	const char *iptype;

	ret = ATCMD_ret_failed;
    /* get the configure */
	iptype = json_get_string( profile, "type" );
	apn = json_get_string( profile, "apn" );
	cid = json_get_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}
    /* set the common apn */
	if ( apn != NULL && *apn != '\0' )
	{
		i = usbtty_cgdcont( fd, cid, iptype, apn, true );
		if ( i < ATCMD_ret_succeed || i == ATCMD_ret_term )
		{
			return i;
		}
		else if ( i == ATCMD_ret_succeed )
		{
			ret = ATCMD_ret_succeed;
		}
	}
    /* set the cids */
	ptr = json_get_string( profile, "cids" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		axp = NULL;
		cids = json_get_value( profile, "cids_cfg" );
		while ( NULL != ( axp = json_each( cids, axp ) ) )
		{
			ptr = axp_get_attr( axp );
			iptype = json_get_string( profile, "type" );
			apn = json_get_string( profile, "apn" );
			i = usbtty_cgdcont( fd, ptr, iptype, apn, true );
			if ( i < ATCMD_ret_succeed || i == ATCMD_ret_term )
			{
				return i;
			}
			else if ( i == ATCMD_ret_succeed )
			{
				ret = ATCMD_ret_succeed;
			}
		}
	}
	/* Cannot verify that it has been set, so only failure will be returned s*/
	return ret;
}



/* search the modem
	ttrue for matched
	tfalse for no match
	terror for error, need reset the modem */
boole_t _usb_match( obj_t this, param_t param )
{
	int i;
    talk_t dev;
	talk_t cfg;
    const char *vid;
    const char *pid;
	const char *netdev;
    const char *object;
	const char *syspath;
    char ttylist[10][NAME_MAX];

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
	if ( 0 == strcasecmp( vid, "12d1" ) && 0 == strcasecmp( pid, "15c3" ) )
	{
		info( "Huawei EM350 modem found(%s:%s)", vid , pid );
		/* insmod the usb driver */
		shell( "modprobe option" );
		usleep( 2000000 );
		syspath = json_string( dev, "syspath" );

		/* find the tty list */
		i = usbttylist_device_find( syspath, ttylist );
		if ( i < 4 )
		{
			usleep( 2000000 );
			i = usbttylist_device_find( syspath, ttylist );
			if ( i < 4	)
			{
				usleep( 2000000 );
				i = usbttylist_device_find( syspath, ttylist );
				if ( i < 4	)
				{
					fault( "Huawei EM350 modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		json_set_string( dev, "name", "Huawei-EM350" );
		/* get the object */
		object = lte_object_get( LTE_COM, syspath, cfg, NULL, 0 );
		json_set_string( dev, "object", object );
		/* find the netdev */
		netdev = usbeth_device_find( syspath, NULL, 0 );
		if ( netdev != NULL )
		{
			json_set_string( dev, "netdev", netdev );
		}
		json_set_string( dev, "mtty", ttylist[2] );
		json_set_string( dev, "devcom", MODEM_COM );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}

    return tfalse;
}
/* set the modem
	ttrue for setup ok
	tfalse for failed, need restart
	terror for error, need reset the modem */
boole_t _at_setup( obj_t this, param_t param )
{
	int i;
	atcmd_t fd;
    talk_t dev;
	talk_t cfg;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}
	cfg = param_talk( param, 2 );
	if ( cfg == NULL )
	{
        return terror;
	}

    /* setup the modem */
	i = usbtty_setup( fd, false );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i != ATCMD_ret_succeed )
	{
		return tfalse;
	}
    /* cfun the modem */
    i = usbtty_cfun1( fd, false );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i != ATCMD_ret_succeed )
	{
		return tfalse;
	}

	/* disconnection */
	i = atcmd_send( fd, "AT^NDISDUP=1,0", 3, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

    /* sms setting */
    i = usbtty_smsmode( fd, 1 );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

    /* get the imei */
    i = usbtty_cgsn( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	return ttrue;
}
/* take off the function
	ttrue for setup ok
	tfalse for failed, need restart
	terror for error, need reset the modem */
boole_t _at_off( obj_t this, param_t param )
{
	int i;
	atcmd_t fd;
    talk_t dev;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}

    /* shut the modem */
    i = usbtty_cfun0( fd );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	return ttrue;
}
/* reset the modem
	ttrue for setup ok
	tfalse for failed, need restart
	terror for error, need reset the modem */
boole_t _at_shut( obj_t this, param_t param )
{
	int i;
	atcmd_t fd;
    talk_t dev;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}

    /* shutdown the modem */
    i = atcmd_send( fd, "AT^RESET", 10, "OK", ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	return ttrue;
}
/* set the modem
	NULL for setings some things( need restart modem )
	ttrue for setting ok(don't set any things)
	tfalse for failed, need restart
	terror for error, need reset the modem */
boole_t _at_setting( obj_t this, param_t param )
{
	int i;
	atcmd_t fd;
    talk_t dev;
	talk_t cfg;
	boole_t ret;
	talk_t profile;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}
	cfg = param_talk( param, 2 );
	if ( cfg == NULL )
	{
        return terror;
	}

	ret = ttrue;
	/* custom prefile setting */
	profile = json_value( cfg, "profile_cfg" );
	if ( profile != NULL )
	{
		/* set the custom apn */
		i = em350_set_profileset( fd, profile );
		if ( i < ATCMD_ret_succeed )
		{
			return terror;
		}
		else if ( i == ATCMD_ret_term )
		{
			return tfalse;
		}
		else if ( i == ATCMD_ret_succeed )
		{
			ret = NULL;
		}
	}

    /* set cops return plmn code */
    i = usbtty_copsformat( fd, 2 );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
    /* enable creg urc */
    i = usbtty_cregmode( fd, 2 );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	return ret;
}

/* watch the modem status
	ttrue for succeed
	tfalse for failed, need restart
	terror for error, need reset the modem */
boole_t _at_watch( obj_t this, param_t param )
{
	int i, t;
    talk_t dev;
	talk_t cfg;
	atcmd_t fd;
	int csq, signal;
	const char *lock_pin;

	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}

	cfg = param_talk( param, 2 );
	json_delete_axp( dev, "plmn" );
	json_delete_axp( dev, "signal" );



	// +CPIN: READY\nOK
	// +CME ERROR: 10
	// SIM PIN
	// SIM PUK
	lock_pin = json_string( cfg, "lock_pin" );
	i = usbtty_cpin( fd , dev, lock_pin );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	else if ( i == 100 )
	{
		json_set_string( dev, "iccid", "pin" );
	}
	else if ( i == 101 )
	{
		json_set_string( dev, "iccid", "puk" );
	}
	else if ( i == ATCMD_ret_succeed )
	{
		//at+CIMI
		//460015356123463\nOK
		i = usbtty_cimi( fd , dev );
		if ( i < ATCMD_ret_succeed )
		{
			return terror;
		}
		else if ( i == ATCMD_ret_term )
		{
			return tfalse;
		}
		//at^ICCID?
		//^ICCID: 89860121802374570731
		t = em350_iccid( fd , dev );
		if ( i < ATCMD_ret_succeed )
		{
			return terror;
		}
		else if ( i == ATCMD_ret_term )
		{
			return tfalse;
		}
		if ( i != ATCMD_ret_succeed && t != ATCMD_ret_succeed )
		{
			json_delete_axp( dev, "imsi" );
			json_delete_axp( dev, "iccid" );
		}
	}



	json_delete_axp( dev, "nettype" );
	// AT+QNWINFO
	i = em350_sysinfoex( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}



	// AT+COPS?
	// +COPS: 0,0,"46000",7\nOK
	i = usbtty_cops( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

    // +CREG: 0,1\nOK
	i = usbtty_creg( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	if ( json_string( dev, "plmn" ) == NULL )
	{
		if ( i == 200 )
		{
			json_set_string( dev, "plmn", "noreg" );
		}
		else if ( i == 201 )
		{
			json_set_string( dev, "plmn", "dereg" );
		}
		else if ( i == 202 )
		{
			json_set_string( dev, "plmn", "unreg" );
		}
	}

	// 2 to 31
	json_delete_axp( dev, "csq" );
	csq = 0;
    i = usbtty_csq( fd, &csq );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	if ( (csq >0 && csq <= 31) || (csq > 100 && csq <= 191) )
	{
		signal = 0;
		json_set_number( dev, "csq", csq );
		// 2 to 31, -113dBm to -53dBm  GSM/LTE
		if ( csq > 0 && csq <= 31 )
		{
			i = ( csq*2-113 );
			if ( i >= -75 ) { signal = 4; }
			else if ( i >= -80 ) { signal = 3; }
			else if ( i >= -90 ) { signal = 2; }
			else if ( i >= -105 ) { signal = 1; }
			if ( json_get_number( dev, "rssi" ) == 0 )
			{
				json_set_number( dev, "rssi", i );
			}
			json_set_number( dev, "signal", signal );
		}
	}


    return ttrue;
}



/* connect to the network
	ttrue for succeed
	tfalse for failed
	terror for error, need reset the modem */
boole_t _at_connect( obj_t this, param_t param )
{
	int i;
	talk_t dev;
	talk_t cfg;
	atcmd_t fd;
	talk_t profile;
	const char *ptr;
    const char *cid;
	const char *apn;
	const char *auth;
	const char *user;
	const char *pass;
    const char *netdev;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
        return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
        return terror;
	}
	cfg = param_talk( param, 2 );
	if ( cfg == NULL )
	{
        return terror;
	}

    /* get the device */
    netdev = json_string( dev, "netdev" );
    /* clear the last dial */
	if ( netdev )
	{
		shell( "ifconfig %s up", netdev );
	}

	/* prefile setting get */
	profile = json_value( cfg, "profile_cfg" );
	/* set the apn */
	i = em350_set_profileset( fd, profile );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	/* transition the username/password */
	apn = json_get_string( profile, "apn" );
	user = json_get_string( profile, "user" );
	pass = json_get_string( profile, "passwd" );
    cid = json_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}
    auth = "1";
	ptr = json_string( profile, "auth" );
    if ( ptr == NULL || *ptr == '\0' )
    {
        if ( user != NULL && *user != '\0' && pass != NULL && *pass != '\0' )
        {
            auth = "1";
        }
        else
        {
            auth = "0";
        }
    }
    else
    {
        if ( 0 == strcmp( ptr, "pap" ) )
        {
            auth = "1";
        }
        else if ( 0 == strcmp( ptr, "chap" ) )
        {
            auth = "2";
        }
        else if ( 0 == strcmp( ptr, "papchap" ) )
        {
            auth = "3";
        }
        else if ( 0 == strcmp( ptr, "disable" ) )
        {
			auth = "0";
			user = NULL;
			pass = NULL;
        }
    }

	/* dial */
	i = atcmd_tx( fd, 15, NULL, ATCMD_DEF, "AT^NDISDUP=%s,1,\"%s\",\"%s\",\"%s\",%s", cid, apn?:"", user?:"", pass?:"", auth );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	/* succeed exit to dhcp */
	return ttrue;
}
/* test whether the connection is successful
	ttrue for connect succeed
	tfalse for not connect
	terror for error, need reset the modem */
boole_t _at_connected( obj_t this, param_t param )
{
	int i;
	talk_t dev;
	atcmd_t fd;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
		return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
		return terror;
	}

	i = em350_ndisstatqry( fd );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i != ATCMD_ret_succeed )
	{
		return tfalse;
	}

	return ttrue;
}
/* disconnect the network
	ttrue for succeed
	tfalse for failed
	terror for error, need reset the modem */
boole_t _at_disconnect( obj_t this, param_t param )
{
	int i;
	talk_t dev;
	atcmd_t fd;

	/* get the information */
	dev = param_talk( param, 1 );
	if ( dev == NULL )
	{
		return terror;
	}
	fd = json_pointer( dev, "fd" );
	if ( fd == NULL )
	{
		return terror;
	}

	/* disconnection */
	i = atcmd_send( fd, "AT^NDISDUP=1,0", 3, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	return ttrue;
}



