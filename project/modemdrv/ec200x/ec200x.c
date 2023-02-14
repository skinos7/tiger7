/*
 *  Description:  Fibcom NL668 modem driver
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
static int ec200x_lock_nettype( atcmd_t fd, talk_t status, const char *netmode )
{
    int ret;
    int type;
	const char *ptr;
    char cmd[LINE_MAX];

    ret = atcmd_send( fd, "AT+QCFG=\"nwscanmode\"", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    type = -1;
	ptr = atcmd_lastack( fd );
	if ( sscanf( ptr, "%*[^,],%d", &type ) != 1 )
	{
		return ATCMD_ret_failed;
	}
	if ( netmode == NULL || *netmode == '\0' )
	{
		netmode = "auto";
	}
	ret = ATCMD_ret_failed;
	cmd[0] = '\0';
    switch( netmode[0] )
    {
		case '2':
		case 'g':
			/* GSM */
			if ( type != 1 )
			{
				snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",1,1" );
			}
			break;
		case 'c':
			/* CDMA */
			if ( type != 6 )
			{
				snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",6,1" );
			}
			break;
		case 't':
			/* TDSCDMA */
			if ( type != 4 )
			{
				snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",4,1" );
			}
			break;
		case 'e':
            /* EVDO */
			if ( type != 7 )
			{
				snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",7,1" );
			}
			break;
		case '3':
        case 'w':
            /* WCDMA */
            if ( type != 2 )
            {
                snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",2,1" );
            }
            break;
		case '4':
        case 'l':
			/* LTE */
            if ( type != 3 )
            {
                snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",3,1" );
            }
            break;
		default:
			/* Auto */
			if ( type != 0 )
			{
				snprintf( cmd, sizeof(cmd), "AT+QCFG=\"nwscanmode\",0,1" );
			}
    }
    if ( cmd[0] != '\0' )
    {
        ret = atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
    }
    return ret;
}
/* lock the network band
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ec200x_lock_band( atcmd_t fd, const char *band )
{
	if ( band != NULL && *band != '\0' )
	{
	    return atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QCFG=\"band\",%s", band );
	}
	/* Cannot verify that it has been set, so only failure will be returned */
    return ATCMD_ret_failed;
}
/* get current register network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int ec200x_qnwinfo( atcmd_t fd, talk_t device )
{
	int i;
    int ret;
	char *ptr;
	char *tok;
	char *tokkey;
	char sysmode[NAME_MAX], plmn[NAME_MAX];
	char band[NAME_MAX], channel[NAME_MAX];

	// AT+QNWINFO返回如下可能的网络
	// NONE / CDMA1X / CDMA1X AND HDR / CDMA1X AND EHRPD / HDR / HDR-EHRPD 
	// GSM / GPRS / EDGE 
	// WCDMA / HSDPA / HSUPA / HSPA+
	// TDSCDMA / TDD LTE / FDD LTE
    // NR5G-NSA / NR5G-SA
    ret = atcmd_send( fd, "AT+QNWINFO", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	ret = ATCMD_ret_failed;
    ptr = atcmd_lastack( fd );
    /* mutli-line opt  */
    tokkey = tok = ptr;
    while( tokkey != NULL && *tok != '\0' )
    {
        tokkey = strstr( tok, "\n" );
        if ( tokkey != NULL )
        {
            *tokkey = '\0';
        }

		i = sscanf( tok, "%*[^:]:%[^,]", sysmode );
		if ( i == 1 )
		{
			if ( strstr( sysmode, "LTE" ) || strstr( sysmode, "FDD" ) || strstr( sysmode, "TDD" )
                    || strstr( sysmode, "GSM" ) || strstr( sysmode, "GPRS" ) || strstr( sysmode, "EDGE" )
                    || strstr( sysmode, "WCDMA" ) || strstr( sysmode, "HSDPA" ) || strstr( sysmode, "HSUPA" ) || strstr( sysmode, "HSPA" )
                    || strstr( sysmode, "TDSCDMA" ) )
			{
				// AT+QNWINFO
				// +QNWINFO: "WCDMA","46001","WCDMA 2100",10763
				// AT+QNWINFO
				// +QNWINFO: "FDD LTE","46001","LTE BAND 8",3740
				ret = ATCMD_ret_succeed;
				i = sscanf( tok, "%*[^:]:%[^,],%[^,],%[^,],%s", sysmode, plmn, band, channel );
				if ( i == 4 )
				{
					ptr = band+1;
					i = strlen( ptr )-1;
					*(ptr+i) = '\0';
					json_set_string( device, "band", ptr );
				}
                ptr = sysmode+2;
                i = strlen( ptr )-1;
                *(ptr+i) = '\0';
                json_set_string( device, "nettype", ptr );
            }
		}

        tok = tokkey+1;
    }
    return ret;
}

/* set the apn and username/password
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ec200x_qicsgp( atcmd_t fd, const char *cid, const char *iptype, const char *apn, const char *auth, const char *username, const char *password, boole query )
{
	int ret;
	const char *ptr;
	char cmd[NAME_MAX];
	
	if ( query == true )
	{
		ret = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QICSGP=%s", cid );
		if ( ret != ATCMD_ret_succeed )
		{
			return ret;
		}
		ptr = atcmd_lastack( fd );
		//AT+QICSGP=1
		//+QICSGP: 1,"CRU.MP.UK","ANPR247","t4wRo-Rep#thac",1
		//
		//OK
		snprintf( cmd, sizeof(cmd), "%s,\"%s\",\"%s\",\"%s\",%s", cid, apn?:"", username?:"", password?:"", auth );
		if ( strstr( ptr, cmd ) != NULL )
		{
			return ATCMD_ret_failed;
		}
	}
	ret = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QCTPWDCFG=\"%s\",\"%s\"", username?:"", password?:"" );
	if ( ret < ATCMD_ret_succeed || ret == ATCMD_ret_term )
	{
		return ret;
	}
	return atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QICSGP=%s,%s,\"%s\",\"%s\",\"%s\",%s", cid, iptype, apn?:"", username?:"", password?:"", auth?:"3" );
}
/* set the APN profile
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ec200x_set_profileset( atcmd_t fd, talk_t profile )
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
	/* private network settings the apn, username, passward */
	i = ec200x_qicsgp( fd, cid, iptype, apn, auth, user, pass, true );
	if ( i < ATCMD_ret_succeed || i == ATCMD_ret_term )
	{
		return i;
	}
	else if ( i == ATCMD_ret_succeed )
	{
		ret = ATCMD_ret_succeed;
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
	if ( 0 == strcasecmp( vid, "2c7c" ) && ( 0 == strcasecmp( pid, "6026" ) || 0 == strcasecmp( pid, "6002" ) || 0 == strcasecmp( pid, "6005" ) ) )
	{
		info( "Quectel EC200X modem found(%s:%s)", vid , pid );
		/* insmod the usb driver */
		shell( "modprobe option" );
		syspath = json_string( dev, "syspath" );

		/* find the tty list */
		i = usbttylist_device_find( syspath, ttylist );
		if ( i < 3 )
		{
			usleep( 1000000 );
			i = usbttylist_device_find( syspath, ttylist );
			if ( i < 3	)
			{
				usleep( 2000000 );
				i = usbttylist_device_find( syspath, ttylist );
				if ( i < 3	)
				{
					fault( "Quectel EC200X modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "Quectel-%s", pid );
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
		json_set_string( dev, "stty", ttylist[1] );
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
	int t;
	int mode;
	atcmd_t fd;
    talk_t dev;
	talk_t cfg;
	const char *tok;
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

	/* switch the mode skip the power up misc URC */
	for( t=0; t<10; t++ )
	{
		i = atcmd_send( fd, "AT+QCFG=\"nat\"", 3, "\"nat\",", ATCMD_DEF );
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
			tok = strstr( ptr, "\"nat\"," );
			if ( tok != NULL )
			{
				if ( sscanf( tok, "%*[^,],%d", &mode ) == 1 )
				{
					if ( mode != 1 )
					{
						i = atcmd_send( fd, "AT+QCFG=\"nat\",1", 8, NULL, ATCMD_DEF );
						if ( i < ATCMD_ret_succeed )
						{
							return terror;
						}
						else if ( i == ATCMD_ret_term )
						{
							return tfalse;
						}
					}
					break;
				}
			}
		}
		sleep( 1 );
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
    i = atcmd_send( fd, "AT+QPOWD", 60, "POWERED DOWN", ATCMD_DEF );
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
    i = ec200x_lock_nettype( fd, dev, ptr );
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
	/* band setting */
	ptr = json_get_string( cfg, "lock_band" );
	i =ec200x_lock_band( fd, ptr );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	/* custom prefile setting */
	profile = json_value( cfg, "profile_cfg" );
	if ( profile != NULL )
	{
		/* set the custom apn */
		i = ec200x_set_profileset( fd, profile );
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
	if ( cfg == NULL )
	{
		return terror;
	}

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



	json_delete_axp( dev, "band" );
	json_delete_axp( dev, "nettype" );
	// AT+QNWINFO
	i = ec200x_qnwinfo( fd, dev );
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
	// 100 to 191
	json_delete_axp( dev, "csq" );
	json_delete_axp( dev, "rssi" );
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
		// 100 to 191, -116dBm to -25dBm  TDSCDMA
		else if ( csq > 100 && csq <= 191  )
		{
			i = csq -215;
			if ( i >= -75 ) { signal = 4; }
			else if ( i >= - 80 ) { signal = 3; }
			else if ( i >= - 90 ) { signal = 2; }
			else if ( i >= - 105 ) { signal = 1; }
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
	int i, t;
	talk_t dev;
	talk_t cfg;
	atcmd_t fd;
	talk_t profile;
    const char *cid;
    const char *netdev;
	char recvbuf[LINE_MAX];

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
	i = ec200x_set_profileset( fd, profile );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
    cid = json_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}

	/* dial */
	i = atcmd_tx( fd, 120, NULL, ATCMD_DEF, "AT+QIACT=%s", cid );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	else if ( i != ATCMD_ret_succeed )
	{
		for ( t=0; t<3; t++ )
		{
			i = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QIACT?" );
			if ( i < ATCMD_ret_succeed )
			{
				return terror;
			}
			else if ( i == ATCMD_ret_term )
			{
				return tfalse;
			}
			i = atcmd_rx( fd, recvbuf, sizeof(recvbuf) );
			if ( i > 0 && strstr( recvbuf, "." ) != NULL )
			{
				break;
			}
			sleep( 1 );
		}
		if ( t >= 3 )
		{
			fault( "pdp dail failed" );
			return tfalse;
		}
	}

	i = atcmd_tx( fd, 10, NULL, ATCMD_DEF, "AT+QNETDEVCTL=1,%s,1", cid );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i != ATCMD_ret_succeed )
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
	char recvbuf[LINE_MAX];

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

	i = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QIACT?" );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	i = atcmd_rx( fd, recvbuf, sizeof(recvbuf) );
	if ( i > 0 && strstr( recvbuf, "." ) != NULL )
	{
		return ttrue;
	}

	return tfalse;
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
	i = atcmd_send( fd, "AT+QIDEACT=1", 3, NULL, ATCMD_DEF );
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



