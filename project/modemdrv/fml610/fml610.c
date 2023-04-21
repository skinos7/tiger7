/*
 *  Description:  Fibcom FM610 modem driver
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landmodem/atcmd.h"
#include "landmodem/landmodem.h"



/* set the network type:
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int fm610_lock_nettype( atcmd_t fd, talk_t status, const char *netmode )
{
	int ret;
    int type;
	const char *ptr;
    char cmd[LINE_MAX];

	if ( netmode == NULL || *netmode == '\0' )
	{
		netmode = "auto";
	}

	/* network mode */
    ret = atcmd_send( fd, "AT+GTRAT?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    type = -1;
    ptr = atcmd_lastack( fd );
	sscanf( ptr, "%*[^:]:%d", &type );

	/* make the network command */
	cmd[0] = '\0';
    switch( netmode[0] )
    {
		case '2':
		case 'g':
		case 'c':
			/* GSM */
            if ( type != 0 )
            {
                snprintf( cmd, sizeof(cmd), "AT+GTRAT=0" );
            }
            break;
        case '3':
		case 'w':
		case 't':
		case 'e':
            /* WCDMA */
            if ( type != 2 )
            {
                snprintf( cmd, sizeof(cmd), "AT+GTRAT=2" );
            }
            break;
        case '4':
		case 'l':
			/* LTE */
            if ( type != 3 )
            {
                snprintf( cmd, sizeof(cmd), "AT+GTRAT=3" );
            }
            break;
		default:
			/* Auto */
            if ( type != 10 )
            {
                snprintf( cmd, sizeof(cmd), "AT+GTRAT=10,3,0" );
            }
    }
	if ( cmd[0] == '\0' )
	{
		return ATCMD_ret_failed;
	}

	/* set the network mode */
    return atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
}

/* get current register network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int fm610_cops( atcmd_t fd, talk_t status )
{
	int i;
	int ret;
	char *ptr;
	char *end;
	char *type;

	ret = atcmd_send( fd, "AT+COPS?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	ret = ATCMD_ret_failed;
	ptr = atcmd_lastack( fd );
	/* find the " start */
	while( *ptr != '"' && *ptr != '\0' )
	{
		ptr++;
	}
	if ( *ptr == '"' )
	{
		/* find the " end */
		ptr++;
		end = ptr;
		while( *end != '"' && *end != '\0' )
		{
			end++;
		}
		if ( *end == '"' )
		{
			/* get the plmn */
			*end = '\0';
			if ( strlen( ptr ) >= 5 )
			{
				json_set_string( status, "plmn", ptr );
				ret = ATCMD_ret_succeed;
			}
			/* get the network type */
			type = end+1;
			while( *type != '\0' )
			{
				if ( isdigit( *type ) )
				{
					break;
				}
				type++;
			}
			if ( isdigit( *type ) )
			{
				i = atoi( type );
				if ( i == 0 || i == 1 || i == 3 )
				{
					json_set_string( status, "nettype", "GSM" );
				}
				else if ( i == 2 || i == 4 || i == 5 || i == 6 )
				{
					json_set_string( status, "nettype", "HSPA" );
				}
				else if ( i == 7 || i == 8 || i == 9 )
				{
					json_set_string( status, "nettype", "LTE" );
				}
			}
		}
	}
	return ret;
}
/* get current register network signal
	0 for succeed
	>0 for failed
	<0 for error */
static int fm610_cesq( atcmd_t fd, talk_t device )
{
	int ret;
	int csq;
	int rssi;
	const char *ptr;
    const char *nettype;
    int rxlev, ber, rscp, ecno, rsrq, rsrp;

	ret = atcmd_send( fd, "AT+CESQ", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	ret = ATCMD_ret_failed;
	csq = rssi = 0;
    ptr = atcmd_lastack( fd );
    if ( sscanf( ptr, "%*[^:]:%d,%d,%d,%d,%d,%d", &rxlev, &ber, &rscp, &ecno, &rsrq, &rsrp ) == 6 )
    {
        nettype = json_get_string( device, "nettype" );
        if ( nettype != NULL && NULL != strstr( nettype, "GSM" ) )
        {
			/* get the gsm csq, 0/63==-111/-48 */
			if ( rxlev > 0 && rxlev <= 63 )
			{
				csq = rxlev;
				rssi = csq-111;
				ret = ATCMD_ret_succeed;
			}
        }
        else if ( nettype != NULL && ( NULL != strstr( nettype, "3G" ) || NULL != strstr( nettype, "HSDPA" ) || NULL != strstr( nettype, "HSUPA" ) || NULL != strstr( nettype, "HSPA" ) ) )
        {
			/* get the umts csq, 0/96==-121/-25 */
			if ( rscp > 0 && rscp <= 96 )
			{
				csq = rscp;
				rssi = csq-121;
				ret = ATCMD_ret_succeed;
			}
        }
        else
        {
			/* get the lte rsrp */
			if ( rsrp > 0 && rsrp <= 97 )
			{
				// 1 to 97, -44 to -140
				rssi = rsrp = rsrp-140;
				json_set_number( device, "rsrp", rsrp );
				ret = ATCMD_ret_succeed;
			}
			/* get the lte rsrq */
			if ( rsrq > 0 && rsrq <= 34 )
			{
				// 1 to 34, -19.5 to 3
				json_set_number( device, "rsrq", (-19+((rsrq*5)/10)) );
			}
        }
		/* get the lte signal */
		if ( csq != 0 )
		{
			json_set_number( device, "csq", csq );
		}
		if ( rssi != 0 )
		{
			json_set_number( device, "rssi", rssi );
		}
    }

    return ret;
}
/* get current network connect state
	0 for succeed
	>0 for failed
	<0 for error */
int fm610_gtrndis( atcmd_t fd )
{
    int ret;
    int type;
	const char *ptr;
	const char *str;

    ret = atcmd_send( fd, "AT+GTRNDIS?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    ret = ATCMD_ret_failed;
    type = 0;
    ptr = atcmd_lastack( fd );
	str = strstr( ptr, "GTRNDIS:" );
	if ( str != NULL )
	{
	    if ( sscanf( str, "%*[^:]:%d", &type ) == 1 )
	    {
	        if ( type == 1 )
	        {
	            ret = ATCMD_ret_succeed;
	        }
		}
	}
    return ret;
}

/* set the APN profile
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int fm610_set_profileset( atcmd_t fd, talk_t profile )
{
	int i;
	int ret;
	talk_t cids;
	talk_t axp;
	const char *ptr;
	const char *cid;
	const char *apn;
	const char *auth;
	const char *user;
	const char *pass;
	const char *iptype;

	ret = ATCMD_ret_failed;
    /* get the configure */
	apn = json_get_string( profile, "apn" );
	user = json_get_string( profile, "user" );
	pass = json_get_string( profile, "passwd" );
	cid = json_get_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}
    /* transition the username/password */
	auth = "1";
	ptr = json_get_string( profile, "auth" );
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
    /* transition the ip type */
	iptype	= "1";
	ptr = json_get_string( profile, "type" );
	if ( ptr != NULL && ( 0 == strcasecmp( ptr, "ipv4v6" ) || 0 == strcasecmp( ptr, "ipv6" ) ) )
	{
		iptype = "2";
	}
	else
	{
		iptype = "1";
	}

	/* set the username and password */
	i = ATCMD_ret_failed;
	if ( 0 == strcmp( auth, "0" ) )
	{
		i = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+MGAUTH=%s,%s", cid, auth );
	}
	else
	{
		i = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+MGAUTH=%s,%s,\"%s\",\"%s\"", cid, auth, user?:"", pass?:"" );
	}
	if ( i < ATCMD_ret_succeed || i == ATCMD_ret_term )
	{
		return i;
	}
	else if ( i == ATCMD_ret_succeed )
	{
		ret = ATCMD_ret_succeed;
	}
	sleep( 2 );


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
	char buffer[NAME_MAX];
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
	if ( 0 == strcasecmp( vid, "1782" ) && ( 0 == strcasecmp( pid, "4d10" ) || 0 == strcasecmp( pid, "4d11" ) ) )
	{
		info( "Fibocom L610 modem found(%s:%s)", vid , pid );
		/* insmod the usb driver */
		shell( "modprobe option" );
		usleep( 2000000 );
		syspath = json_string( dev, "syspath" );

		/* find the tty list */
		i = usbttylist_device_find( syspath, ttylist );
		if ( i < 5 )
		{
			usleep( 2000000 );
			i = usbttylist_device_find( syspath, ttylist );
			if ( i < 5	)
			{
				usleep( 2000000 );
				i = usbttylist_device_find( syspath, ttylist );
				if ( i < 5	)
				{
					fault( "Fibocom L610 modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "Fibocom-%s", pid );
		json_set_string( dev, "name", buffer );
		/* get the object */
		object = lte_object_get( LTE_COM, syspath, cfg, NULL, 0 );
		json_set_string( dev, "object", object );
		/* find the netdev */
		netdev = usbeth_device_find( syspath, NULL, 0 );
		if ( netdev != NULL )
		{
			json_set_string( dev, "netdev", netdev );
		}
		json_set_string( dev, "stty", ttylist[4] );
		json_set_string( dev, "mtty", ttylist[3] );
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
	int t;
	boole r;
	atcmd_t fd;
    talk_t dev;
	talk_t cfg;
	const char *ptr;
	const char *tok;

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

	r = false;
	/* switch the dial mode skip the power up misc URC */
	for( t=0; t<10; t++ )
	{
		i = atcmd_send( fd, "AT+GTUSBMODE?", 3, "GTUSBMODE:", ATCMD_DEF );
		if ( i < ATCMD_ret_succeed )
		{
			return terror;
		}
		else if ( i != ATCMD_ret_succeed )
		{
			if ( t >= 2 )
			{
				break;
			}
		}
		else
		{
			ptr = atcmd_lastack( fd );
			tok = strstr( ptr, "GTUSBMODE:" );
			if ( tok != NULL )
			{
				if ( sscanf( tok, "%*[^:]:%d", &i ) == 1 )
				{
					if ( i != 32 )
					{
						i = atcmd_send( fd, "AT+GTUSBMODE=32", 8, NULL, ATCMD_DEF );
						if ( i < ATCMD_ret_succeed )
						{
							return terror;
						}
						else if ( i == ATCMD_ret_term )
						{
							return tfalse;
						}
						r = true;
					}
					break;
				}
			}
		}
		sleep( 1 );
	}
    if ( r == true )
    {
        return terror;
    }


	/* disconnection */
	i = atcmd_send( fd, "AT+GTRNDIS=0,1", 3, NULL, ATCMD_DEF );
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
    /* get the version */
    i = usbtty_cgmr( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	/* sleep for setup */
	sleep( 5 );

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
    i = atcmd_send( fd, "AT+CPWROFF", 5, "OK", ATCMD_DEF );
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
	const char *ptr;

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
	/* network type setting */
    ptr = json_get_string( cfg, "lock_nettype" );
    i = fm610_lock_nettype( fd, dev, ptr );
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

	/* custom prefile setting */
	profile = json_value( cfg, "profile_cfg" );
	if ( profile != NULL )
	{
		/* set the custom apn */
		i = fm610_set_profileset( fd, profile );
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
    i = usbtty_ceregmode( fd, 2 );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
    i = usbtty_cgregmode( fd, 2 );
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
	int i;
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
	json_delete_axp( dev, "rssi" );
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
		json_set_string( dev, "iccid", "sim" );
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
		//AT+CCID
		//+CCID: 89860121801097564807\nOK
		i = usbtty_ccid( fd , dev );
		if ( i < ATCMD_ret_succeed )
		{
			return terror;
		}
		else if ( i == ATCMD_ret_term )
		{
			return tfalse;
		}
	}



	json_delete_axp( dev, "csq" );
	json_delete_axp( dev, "rsrp" );
	json_delete_axp( dev, "nettype" );
	// AT+COPS?
	// +COPS: 0,0,"46000",7\nOK
	i = fm610_cops( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}

	// AT+CESQ
	// +CESQ: 99,99,255,255,22,41,255,255,255
	i = fm610_cesq( fd, dev );
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
	i = usbtty_cereg( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	i = usbtty_cgreg( fd, dev );
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


	// 2 to 31, -113dBm to -53dBm  GSM/LTE
	if ( json_number( dev, "csq" ) == 0 )
	{
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
		if ( csq > 0 && csq <= 31 )
		{
			i = ( csq*2-113 );
			if ( json_get_number( dev, "rssi" ) == 0 )
			{
				json_set_number( dev, "rssi", i );
			}
			json_set_number( dev, "csq", csq );
		}
	}


	// signal
	signal = 0;
	i = json_number( dev, "rsrp" );
	if ( i != 0 )
	{
		if ( i != 0 )
		{
			if ( i >= -105 ) { signal = 4; }
			else if ( i >= -110 ) { signal = 3; }
			else if ( i >= -114 ) { signal = 2; }
			else if ( i >= -120 ) { signal = 1; }
		}
	}
	else
	{
		i = json_number( dev, "rssi" );
		if ( i != 0 )
		{
			if ( i >= -75 ) { signal = 4; }
			else if ( i >= -80 ) { signal = 3; }
			else if ( i >= -90 ) { signal = 2; }
			else if ( i >= -105 ) { signal = 1; }
		}
	}
	if ( signal != 0 )
	{
		json_set_number( dev, "signal", signal );
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
    const char *cid;
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
	i = fm610_set_profileset( fd, profile );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	/* transition the username/password */
    cid = json_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}

	/* dial */
	i = atcmd_tx( fd, 20, NULL, ATCMD_DEF, "AT+GTRNDIS=1,%s", cid );
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

	i = fm610_gtrndis( fd );
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
	i = atcmd_send( fd, "AT+GTRNDIS=0,1", 3, NULL, ATCMD_DEF );
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



