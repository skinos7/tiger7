/*
 *  Description:  Fibcom NL668 modem driver
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landmodem/atcmd.h"
#include "landmodem/landmodem.h"



/* get network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int innofidei_sysinfo( atcmd_t fd, talk_t status )
{
    int ret;
	int type;
	const char *ptr;

    ret = atcmd_send( fd, "AT^SYSINFO", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    type = 0;
    ptr = atcmd_lastack( fd );
    sscanf( ptr, "%*[^,],%*[^,],%*[^,],%d", &type );
    switch( type )
    {
        case 0:
            json_set_string( status, "nettype", "No Service" );
            ret = ATCMD_ret_failed;
            break;
        case 3:
            json_set_string( status, "nettype", "GSM" );
            break;
        case 5:
            json_set_string( status, "nettype", "WCDMA" );
            break;
        case 15:
            json_set_string( status, "nettype", "TDSCDMA" );
            break;
        case 17:
            json_set_string( status, "nettype", "LTE" );
            break;
    }
    return ret;
}

/* get current network connect state
	0 for succeed
	>0 for failed
	<0 for error */
static int innofidei_cgpaddr( atcmd_t fd )
{
    int ret;
    char *buf;
    int type;

    ret = atcmd_send( fd, "AT+CGPADDR", 3, NULL, ATCMD_DEF );
    if ( ret < 0 )
    {
        return ret;
    }
    else if ( ret != ATCMD_ret_succeed )
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
static int innofidei_set_profileset( atcmd_t fd, talk_t profile )
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
	iptype = json_get_string( profile, "type" );
	apn = json_get_string( profile, "apn" );
	cid = json_get_string( profile, "cid" );
	user = json_get_string( profile, "user" );
	pass = json_get_string( profile, "passwd" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}

	/* 转换auth属性成AT指令能识别的选项 */
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
	/* 设置APN, 认证方式, 用户名及密码*/
	i = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+ZGPCOAUTH=%s,\"%s\",\"%s\",%s", cid, user?:"", pass?:"", auth );
	if ( i < ATCMD_ret_succeed || i == ATCMD_ret_term )
	{
		return i;
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
	const char *ptr;
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
	if ( 0 == strcasecmp( vid, "1d53" ) && 0 == strcasecmp( pid, "3d53" ) )
	{
		info( "Innofidei E7B01 modem found(%s:%s)", vid , pid );
		/* 加载option驱动 */
		insert_module( "option" );
		ptr = json_get_string( cfg, "mode" );
		if ( ptr == NULL || 0 != strcmp( ptr, "ppp" ) )
		{
			shell( "insmod %s", project_ko_path( NULL, 0, PDRIVER_PROJECT, "innofidei_cdc.ko" ) );
		}
		usleep( 2000000 );
		syspath = json_string( dev, "syspath" );

		/* find the tty list */
		i = usbttylist_device_find( syspath, ttylist );
		if ( i < 3 )
		{
			usleep( 2000000 );
			i = usbttylist_device_find( syspath, ttylist );
			if ( i < 3	)
			{
				usleep( 2000000 );
				i = usbttylist_device_find( syspath, ttylist );
				if ( i < 3	)
				{
					fault( "Innofidei E7B01 modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "Innofidei-%s", pid );
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
		json_set_string( dev, "stty", ttylist[0] );
		json_set_string( dev, "mtty", ttylist[0] );
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
	i = atcmd_send( fd, "AT+SCFG=armsleepingmode,0", 5, "OK", ATCMD_DEF );
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

	sleep( 2 );
	/* 确保网络已断开 */
	i = atcmd_send( fd, "AT+ZGACT=0,1", 5, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	i = atcmd_send( fd, "AT+CGACT=0,1", 5, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	sleep( 2 );

	ret = ttrue;
	/* custom prefile setting */
	profile = json_value( cfg, "profile_cfg" );
	if ( profile != NULL )
	{
		/* set the custom apn */
		i = innofidei_set_profileset( fd, profile );
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
	else if ( i == ATCMD_ret_term || i == ATCMD_ret_timeout )
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
		//AT+ZICCID?
		//^ICCID: 89860121802374570731
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



	json_delete_axp( dev, "rsrp" );
	json_delete_axp( dev, "rsrq" );
	json_delete_axp( dev, "nettype" );
	// AT+QNWINFO
	i = innofidei_sysinfo( fd, dev );
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

    /* 使用中兴微专用的网络小区查询指令获到当前注册上的小区信息 */
    i = atcmd_send( fd, "at+ZEMSCIQ=1", 3, NULL, ATCMD_DEF );
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
    	const char *ptr;
        char buff[NAME_MAX];
		char plmn[NAME_MAX];
        int stat, act, cellid, rac, lac, pci, arfcn, band;
        ptr = atcmd_lastack( fd );
        if ( sscanf( ptr, "%*[^:]:%d,%d,%d,%d,%d,%[^,],%d,%d,%d", &stat, &act, &cellid, &rac, &lac, plmn, &pci, &arfcn, &band ) == 9 )
        {
            //snprintf( buff, sizeof(buff), "%d", cellid );  // cell is the ci(Hex)
            //json_set_string( status, "cell", buff );
            snprintf( buff, sizeof(buff), "%d", band );
            json_set_string( dev, "band", buff );
            snprintf( buff, sizeof(buff), "%d", arfcn );
            json_set_string( dev, "arfcn", buff );
			if ( json_get_number( dev, "plmn") == 0 )
			{
				if ( sscanf( plmn, "\"%d\"", &i ) == 1 )
				{
					json_set_number( dev, "plmn", i );
				}
			}
        }
    }


	// 2 to 31, -113dBm to -53dBm  GSM/LTE
	// 100 to 191, -116dBm to -25dBm  TDSCDMA
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
		if ( csq > 0 && csq <= 31 )
		{
			i = ( csq*2-113 );
			json_set_number( dev, "rssi", i );
		}
		else if ( csq > 100 && csq <= 191  )
		{
			i = csq -215;
			json_set_number( dev, "rssi", i );
		}
		json_set_number( dev, "csq", csq );
	}


	// signal
	signal = 0;
	i = json_number( dev, "rssi" );
	if ( i != 0 )
	{
		if ( i >= -75 ) { signal = 4; }
		else if ( i >= -80 ) { signal = 3; }
		else if ( i >= -90 ) { signal = 2; }
		else if ( i >= -105 ) { signal = 1; }
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
	i = innofidei_set_profileset( fd, profile );
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
	i = atcmd_tx( fd, 5, NULL, ATCMD_DEF, "AT+CGACT=%s,1", cid );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i != ATCMD_ret_succeed )
	{
		return tfalse;
	}
	i = atcmd_tx( fd, 5, NULL, ATCMD_DEF, "AT+DHCPCTRL=1", cid );
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

	i = innofidei_cgpaddr( fd );
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
	i = atcmd_send( fd, "AT+ZGACT=0,1", 5, NULL, ATCMD_DEF );
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



/* dial the network
	ttrue for succeed
	tfalse for failed
	terror for error, need reset the modem */
boole_t _at_dial( obj_t this, param_t param )
{
	return _at_disconnect( this, param );
}


