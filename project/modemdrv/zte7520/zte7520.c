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
static int zte7520_ccid( atcmd_t fd, talk_t status )
{
    int ret;
	const char *ptr;
	char iccidbuf[NAME_MAX];

	ret = atcmd_send( fd, "AT+ZICCID?", 5, NULL, ATCMD_DEF );
	if ( ret < ATCMD_ret_succeed || ret == ATCMD_ret_term )
	{
		return ret;
	}
	if ( ret != ATCMD_ret_succeed )
	{
		ret = atcmd_send( fd, "AT+ZICCID", 5, NULL, ATCMD_DEF );
		if ( ret != ATCMD_ret_succeed )
		{
			return ret;
		}
	}
	ret = ATCMD_ret_failed;
	memset( iccidbuf, 0, sizeof(iccidbuf) );
	ptr = atcmd_lastack( fd );
	if ( sscanf( ptr, "%*[^:]:%s", iccidbuf ) == 1 )
	{
		json_set_string( status, "iccid", iccidbuf );
		ret = ATCMD_ret_succeed;
	}
	return ret;
}
/* get network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int zte7520_sysinfo( atcmd_t fd, talk_t status )
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
/* get current register network signal
	0 for succeed
	>0 for failed
	<0 for error */
static int zte7520_cesq( atcmd_t fd, talk_t device )
{
	int ret;
	const char *ptr;
    int rxlev, ber, rscp, ecno, rsrq, rsrp;

    ret = atcmd_send( fd, "AT+CESQ", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	ret = ATCMD_ret_failed;
    ptr = atcmd_lastack( fd );
    if ( sscanf( ptr, "%*[^:]:%d,%d,%d,%d,%d,%d", &rxlev, &ber, &rscp, &ecno, &rsrq, &rsrp ) == 6 )
    {
		ret = ATCMD_ret_succeed;
		/* get the lte rsrp */
		if ( rsrp > 0 && rsrp <= 97 )
		{
			// 1 to 97, -44 to -140
			rsrp = rsrp-140;
			json_set_number( device, "rsrp", rsrp );
		}
		/* get the lte rsrq */
		if ( rsrq > 0 && rsrq <= 34 )
		{
			// 1 to 34, -19.5 to 3
			json_set_number( device, "rsrq", (-19+((rsrq*5)/10)) );
		}
    }

    return ret;
}

/* get current network connect state
	0 for succeed
	>0 for failed
	<0 for error */
static int zte7520_zgact( atcmd_t fd )
{
    int ret;
	const char *ptr;

    ret = atcmd_send( fd, "AT+ZGACT?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    ptr = atcmd_lastack( fd );
    ret = ATCMD_ret_failed;
    if ( strstr( ptr, "1,1" ) != NULL )
    {
        ret = ATCMD_ret_succeed;
    }
    return ret;
}
/* set the network type:
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int zte7520_sysconfig( atcmd_t fd, talk_t status, const char *netmode )
{
    int ret;
	const char *ptr;
    char cmd[LINE_MAX];
    char recvbuf[ATCMD_ATBUFFSIZE];
    int type, acqorder, roam, srvdomain;

    type = acqorder = roam = srvdomain = 0;
	/* 发送 AT^SYSCONFIG? 指令 */
    ret = atcmd_send( fd, "AT^SYSCONFIG?", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	/* 获取 AT^SYSCONFIG? 指令的返回 */
	atcmd_rx( fd, recvbuf, sizeof(recvbuf) );
	ptr = strstr( recvbuf, "SYSCONFIG:" );
	if ( ptr == NULL )
	{
		return ATCMD_ret_failed;
	}
	/* 分析出当前模式模式的代码 */
    if ( sscanf( ptr, "%*[^:]:%d,%d,%d,%d", &type, &acqorder, &roam, &srvdomain ) != 4 )
	{
		return ATCMD_ret_failed;
	}
	/* 如果未指定网络模式则不操作 */
	if ( netmode == NULL || *netmode == '\0' )
	{
		netmode = "auto";
	}

	/* 清空要发送的指令 */
	ret = ATCMD_ret_failed;
	cmd[0] = '\0';
	/* 判断要求的网络模式 */
    switch( netmode[0] )
    {
		case '2':
		case 'g':
            if ( type != 13 )
            {
                snprintf( cmd, sizeof(cmd), "AT^SYSCONFIG=13,0,1,2" );
            }
            break;
		case 'c':
			/* 判断当前是否为CDMA, 如果不是则生成设置为CDMA模式的AT指令 */
		case 't':
			/* 判断当前是否为TDSCDMA, 如果不是则生成设置为TDSCDMA模式的AT指令 */
            if ( type != 15 )
            {
                snprintf( cmd, sizeof(cmd), "AT^SYSCONFIG=15,0,1,2" );
            }
            break;
		case 'e':
			/* 判断当前是否为EVDO, 如果不是则生成设置为EVDO模式的AT指令 */
		case '3':
		case 'w':
			/* 判断当前是否为WCDMA, 如果不是则生成设置为WCDMA模式的AT指令 */
            if ( type != 14 )
            {
                snprintf( cmd, sizeof(cmd), "AT^SYSCONFIG=14,0,1,2" );
            }
            break;
        case '4':
		case 'l':
			/* 判断当前是否为LTE, 如果不是则生成设置为LTE模式的AT指令 */
            if ( type != 17 )
            {
                snprintf( cmd, sizeof(cmd), "AT^SYSCONFIG=17,0,1,2" );
            }
            break;
		default:
			/* 判断当前是否为自动模式, 如果不是则生成设置为自动模式的AT指令 */
			if ( type != 2 )
			{
				snprintf( cmd, sizeof(cmd), "AT^SYSCONFIG=2,0,1,2" );
			}
    }

	/* 如果设置网络模式的指令已生成则发送此指令 */
    if ( cmd[0] != '\0' )
    {
        ret = atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
    }
    return ret;
}
/* 设置频段, 0 for oK, >0 for failed, <0 for error */
static int zte7520_band( atcmd_t fd, const char *bandsets )
{
    int i;
    boole set;
    char *tok;
    char *tokkey;
	const char *ptr;
    unsigned char band[8];
    char buffer[LINE_MAX];
    char cmd[LINE_MAX];

	/* 如果未指定频段则不操作 */
	if ( bandsets == NULL || *bandsets == '\0' )
	{
		return ATCMD_ret_failed;
	}
	/* 转换频段设置为AT参数 */
    set = false;
    memset( band, 0, sizeof(band) );
    strncpy( buffer, bandsets, sizeof(buffer)-1 );
    tokkey = tok = buffer;
    while( tokkey != NULL && *tok != '\0' )
    {
        tokkey = strstr( tok, "," );
        if ( tokkey != NULL )
        {
            *tokkey = '\0';
        }
        i = atoi( tok );
        if ( i >= 57 )
        {
            i-=56;
            band[7] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 49 )
        {
            i-=48;
            band[6] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 41 )
        {
            i-=40;
            band[5] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 33 )
        {
            i-=32;
            band[4] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 25 )
        {
            i-=24;
            band[3] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 17 )
        {
            i-=16;
            band[2] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 9 )
        {
            i-=8;
            band[1] |= ( 1 << (i-1) );
            set = true;
        }
        else if ( i >= 1 )
        {
            band[0] |= ( 1 << (i-1) );
            set = true;
        }
        tok = tokkey+1;
    }
	i = ATCMD_ret_failed;
    if ( set == true )
    {
        snprintf( buffer, sizeof(buffer), "%d,%d,%d,%d,%d,%d,%d,%d", band[0], band[1], band[2], band[3], band[4], band[5], band[6], band[7] );
        i = atcmd_send( fd, "AT+ZLTEBAND?", 3, NULL, ATCMD_DEF );
		if ( i == ATCMD_ret_succeed )
        {
	        ptr = atcmd_lastack( fd );
	        if ( strstr( ptr, buffer ) == NULL )
	        {
	            snprintf( cmd, sizeof(cmd), "AT+ZLTEBAND=%s", buffer );
	            i = atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
	        }
		}
    }
	return i;
}

/* set the APN profile
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int zte7520_set_profileset( atcmd_t fd, talk_t profile )
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
	if ( 0 == strcasecmp( vid, "19d2" ) && 0 == strcasecmp( pid, "0579" ) )
	{
		info( "ZTE 7520 modem found(%s:%s)", vid , pid );
		/* 加载option驱动 */
		insert_module( "cdc_ether" );
		insert_module( "option" );
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
					fault( "ZTE 7520 modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "ZTE-%s", pid );
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
		json_set_string( dev, "mtty", ttylist[0] );
		json_set_string( dev, "devcom", MODEM_COM );
		json_set_string( dev, "drvcom", COM_IDPATH );
		return ttrue;
	}
	else if ( 0 == strcasecmp( vid, "19d2" ) && 0 == strcasecmp( pid, "0199" ) )
	{
		info( "ZTE ME3760 modem found(%s:%s)", vid , pid );
		/* insmod the usb driver */
		shell( "rmmod option" );
		shell( "rmmod me3760_cdc_ether" );
		shell( "rmmod me3760_cdc_encap" );
		shell( "rmmod rndis_host" );
		shell( "rmmod cdc_ether" );
		shell( "insmod /prj/pdriver/me3760_cdc_encap.ko" );
		shell( "insmod /prj/pdriver/me3760_cdc_ether.ko" );
		shell( "modprobe option" );
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
					fault( "ZTE ME3760 modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "ZTE-%s", pid );
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
		json_set_string( dev, "mtty", ttylist[1] );
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
	/* 设置为ECM模式 */
    i = atcmd_send( fd, "AT+ZNETCARDTYPE=1", 3, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
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

	/* 因很多模块型号使用此相同的VID及PID, 因此需要通过CGMM的返回来区别 */
    i = atcmd_send( fd, "AT+CGMM", 3, NULL, ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
    ptr = atcmd_lastack( fd );
    /* 判断返回中是否有7810, 如果有表示模块为THINKWILL-ML781 */
    if ( strstr( ptr, "7810" ) != NULL )
    {
		json_set_string( dev, "name", "THINKWILL-ML7810" );
		/* 获取模块USB模式 */
        i = atcmd_send( fd, "AT+ZNCARD?", 3, NULL, ATCMD_DEF );
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
	        ptr = atcmd_lastack( fd );
	        if ( sscanf( ptr, "%*[^:]:%d", &i ) == 1 )
	        {
	            if ( i != 1 )
	            {
					/* 切换模块的USB模式 */
	                atcmd_send( fd, "AT+ZNCARD=1", 8, NULL, ATCMD_DEF );
	                return terror;
	            }
	        }
		}
	}
	/* 否则为ZTE-7520    */
	else
	{
		/* 获取模块USB模式 */
		i = atcmd_send( fd, "AT+ZLANENABLE?", 3, NULL, ATCMD_DEF );
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
			ptr = atcmd_lastack( fd );
			if ( sscanf( ptr, "%*[^:]:%d", &i ) == 1 )
			{
				if ( i != 0 )
				{
					/* 切换模块的USB模式 */
					atcmd_send( fd, "AT+ZLANENABLE=0", 8, NULL, ATCMD_DEF );
	                return terror;
				}
			}
		}
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
	return _at_off( this, param );
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
	/* network type setting */
    ptr = json_get_string( cfg, "lock_nettype" );
    i = zte7520_sysconfig( fd, dev, ptr );
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
	i = zte7520_band( fd, ptr );
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
		i = zte7520_set_profileset( fd, profile );
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
		//AT+ZICCID?
		//^ICCID: 89860121802374570731
		i = zte7520_ccid( fd , dev );
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
	i = zte7520_sysinfo( fd, dev );
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
	i = zte7520_cesq( fd, dev );
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

	// 2 to 31
	// 100 to 197
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
	if ( (csq >0 && csq <= 31) || (csq > 100 && csq <= 197) )
	{
		signal = 0;
		json_set_number( dev, "csq", csq );
		// 2 to 31, -113dBm to -53dBm  GSM
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
		else if ( csq > 100 && csq <= 197  )
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
	i = zte7520_set_profileset( fd, profile );
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
	i = atcmd_tx( fd, 5, NULL, ATCMD_DEF, "AT+ZGACT=%s,1", cid );
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

	i = zte7520_zgact( fd );
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
	i = atcmd_send( fd, "AT+CGACT=0,1", 5, NULL, ATCMD_DEF );
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


