/*
 *  Description:  Lierda NE16U modem driver
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landmodem/atcmd.h"
#include "landmodem/landmodem.h"



/* set the network type:    AT+LNWSCFG?  AT+LNWSCFG=0
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ne16u_lock_nettype( atcmd_t fd, talk_t status, const char *netmode )
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
    ret = atcmd_send( fd, "AT+LNWSCFG?", 3, NULL, ATCMD_DEF );
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
			/* GSM */
		case 'c':
			/* CDMA */
		case 't':
			/* TDSCDMA */
		case 'e':
			/* EVDO */
		case '3':
		case 'w':
			/* WCDMA */
			if ( type != 1 )
			{
				snprintf( cmd, sizeof(cmd), "AT+LNWSCFG=1" );
			}
			break;
		case '4':
		case 'l':
			/* LTE */
			if ( type != 2 )
			{
				snprintf( cmd, sizeof(cmd), "AT+LNWSCFG=2" );
			}
			break;
		case 'n':
			/* NSA */
			if ( type != 8 )
			{
				snprintf( cmd, sizeof(cmd), "AT+LNWSCFG=8" );
			}
			break;
		case 's':
			/* SA */
			if ( type != 5 )
			{
				snprintf( cmd, sizeof(cmd), "AT+LNWSCFG=5" );
			}
			break;
		default:
			/* Auto */
			if ( type != 0 )
			{
				snprintf( cmd, sizeof(cmd), "AT+LNWSCFG=0" );
			}
	}
	if ( cmd[0] == '\0' )
	{
		return ATCMD_ret_failed;
	}

	/* set the network mode */
	return atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
}

/* lock the network band
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ne16u_lock_band( atcmd_t fd, const char *band )
{
	char *unescape;
	char cmd[LINE_MAX];

	if ( band == NULL || *band == '\0' )
	{
		return ATCMD_ret_failed;
	}
	unescape = json_unescape( band );

	/* make the band command */
	snprintf( cmd, sizeof(cmd), "AT+LBAND=%s", unescape );
	/* set the band */
	return atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
}

/* lock the network cell
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ne16u_lock_cell( atcmd_t fd, const char *cell )
{
	char *unescape;
	char cmd[LINE_MAX];

	if ( cell == NULL || *cell == '\0' )
	{
		return ATCMD_ret_failed;
	}
	unescape = json_unescape( cell );

	/* make the band command */
	snprintf( cmd, sizeof(cmd), "AT+QNWLOCK=%s", unescape );
	/* set the band */
	return atcmd_send( fd, cmd, 8, NULL, ATCMD_DEF );
}

/* set the apn and username/password
	0 for setings is succeed
	>0 for failed or not need settings
	<0 for error */
static int ne16u_qicsgp( atcmd_t fd, const char *cid, const char *iptype, const char *apn, const char *auth, const char *username, const char *password, boole query )
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
static int ne16u_set_profileset( atcmd_t fd, talk_t profile )
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
	i = ne16u_qicsgp( fd, cid, iptype, apn, auth, user, pass, true );
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

/* get current register network mode
	0 for succeed
	>0 for failed
	<0 for error */
static int ne16u_qnwinfo( atcmd_t fd, talk_t device )
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
            else if ( strstr( sysmode, "NR5G-SA" ) )
            {
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
            else if ( strstr( sysmode, "NR5G-NSA" ) )
            {
				ret = ATCMD_ret_succeed;
				i = sscanf( tok, "%*[^:]:%[^,],%[^,],%[^,],%s", sysmode, plmn, band, channel );
				if ( i == 4 )
				{
					ptr = band+1;
					i = strlen( ptr )-1;
					*(ptr+i) = '\0';
					json_set_string( device, "band2", ptr );
				}
                ptr = sysmode+2;
                i = strlen( ptr )-1;
                *(ptr+i) = '\0';
                json_set_string( device, "nettype", "LTE" );
                json_set_string( device, "nettype2", ptr );
            }
		}

        tok = tokkey+1;
    }
    return ret;
}

/* get the cell detail at+qeng="servingcell"
	0 for succeed
	>0 for failed
	<0 for error */
static int ne16u_servingcell( atcmd_t fd, talk_t device )
{
	int i;
    int ret;
    char *buf;
	char *tok;
	char *tokkey;
	const char *start;
	char tdd[NAME_MAX], other[NAME_MAX];
	char sysmode[NAME_MAX], mnc[NAME_MAX], mcc[NAME_MAX];
	char lac[NAME_MAX], rac[NAME_MAX], rscp[NAME_MAX], ecno[NAME_MAX];
	char pcid[NAME_MAX], rsrp[NAME_MAX], sinr[NAME_MAX], rsrq[NAME_MAX];
	char cellid[NAME_MAX], earfcn[NAME_MAX], freq_band_ind[NAME_MAX], dl_bandwidth[NAME_MAX], ul_bandwidth[NAME_MAX], rssi[NAME_MAX], tac[NAME_MAX];

    ret = atcmd_send( fd, "at+qeng=\"servingcell\"", 3, NULL, ATCMD_DEF );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
	ret = ATCMD_ret_failed;
    buf = atcmd_lastack( fd );
    /* mutli-line opt  */
    tokkey = tok = buf;
    while( tokkey != NULL && *tok != '\0' )
    {
        tokkey = strstr( tok, "\n" );
        if ( tokkey != NULL )
        {
            *tokkey = '\0';
        }

		if ( ( start = strstr( tok, "WCDMA" ) ) != NULL )
		{
			// ONLY WCDMA
		    // at+qeng="servingcell"
			// +QENG: "servingcell","LIMSRV","WCDMA",460,01,A538,1EB578B,10763,60,2,-106,-13,-,-,-,-,-
			sysmode[0] = mcc[0] = mnc[0] = lac[0] = cellid[0] = earfcn[0] = rac[0] = rscp[0] = ecno[0] = other[0] = '\0';
			i = sscanf( start, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", sysmode, mcc, mnc, lac, cellid, earfcn, rac, rscp, ecno, other );
			if ( i == 10 )
			{
				//debug( "sysmode=%s, mcc=%s, mnc=%s, lac=%s, cellid=%s, earfcn=%s, rac=%s, rscp=%s, ecno=%s, other=%s", sysmode, mcc, mnc, lac, cellid, earfcn, rac, rscp, ecno, other );
				i = atoi( rscp );
				/* rscp 0/96==-121/-25 */
				if ( i > 0 && i <= 96 )
				{
					json_set_string( device, "netera", "3G" );
					json_set_string( device, "nettype", "WCDMA" );
					ret = ATCMD_ret_succeed;
				}
			}
		}
		else if ( ( start = strstr( tok, "LTE" ) ) != NULL )
		{
			// ONLY LTE
			// at+qeng="servingcell"
			// +QENG: "servingcell","NOCONN","CDMA",460,03,3614,24D52,1019,-86,-155,0
			// +QENG: "servingcell","NOCONN","LTE","FDD",460,11,D196A00,462,100,1,5,5,7794,-109,-11,-76,11,-
			// LTE and NSA
			// +QENG: "servingcell","NOCONN"
			// +QENG: "LTE","FDD",460,11,D196A00,462,100,1,5,5,7794,-107,-12,-73,10,8,-32768,-
			// +QENG: "NR5G-NSA",460,11,562,-32768,-32768,-32768			
			sysmode[0] = tdd[0] = mcc[0] = mnc[0] = cellid[0] = pcid[0] = earfcn[0] = freq_band_ind[0] = dl_bandwidth[0] = ul_bandwidth[0] = tac[0] = rssi[0] = rsrp[0] = rsrq[0] = sinr[0] = other[0] = '\0';
			i = sscanf( start, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", sysmode, tdd, mcc, mnc, cellid, pcid, earfcn, freq_band_ind, ul_bandwidth, dl_bandwidth, tac, rsrp, rsrq, rssi, sinr, other );
			if ( i == 16 )
			{
				//debug( "sysmode=%s, tdd=%s, mcc=%s, mnc=%s, cellid=%s, pcid=%s, earfcn=%s, freq_band_ind=%s, ul_bandwidth=%s, dl_bandwidth=%s, tac=%s, rsrp=%s, rsrq=%s, rssi=%s, sinr=%s", sysmode, tdd, mcc, mnc, cellid, pcid, earfcn, freq_band_ind, ul_bandwidth, dl_bandwidth, tac, rsrp, rsrq, rssi, sinr );
				i = atoi( pcid );
				if ( i != 65535 && i != -32768 )
				{
					json_set_string( device, "pcid", pcid );
					json_set_string( device, "netera", "4G" );
					i = atoi( rsrp );
					if ( i <= -44 && i >= -140 )
					{
						json_set_string( device, "rsrp", rsrp );
						json_set_string( device, "nettype", "LTE" );
						ret = ATCMD_ret_succeed;
					}
					i = atoi( rssi );
					if ( i <= -44 && i >= -140 )
					{
						json_set_string( device, "rssi", rssi );
						ret = ATCMD_ret_succeed;
					}
					i = atoi( rsrq );
					if ( i <= -3 && i >= -20 )
					{
						json_set_string( device, "rsrq", rsrq );
					}
					i = atoi( sinr );
					if ( i <= 250 && i >= 0 )
					{
						i = -20+(i/5);
						json_set_number( device, "sinr", i );
					}
				}
			}
		}

		else if ( ( start = strstr( tok, "NR5G-SA" ) ) != NULL )
		{
			// at+qeng="servingcell"
			// +QENG: "servingcell","NOCONN","NR5G-SA","TDD",460,88,0,301,342600,629952,78,3,-79,-11,0,8015
			// +QENG: "servingcell","NOCONN","NR5G-SA","TDD", 460,88,0,301,342800,629952,78,3,-112,-12,0,1664
			// +QENG: "servingcell","NOCONN","NR5G-SA","TDD", 460,88,0,301,342800,629952,78,11,-110,-13,0,-
			sysmode[0] = tdd[0] = mcc[0] = mnc[0] = cellid[0] = pcid[0] = earfcn[0] = freq_band_ind[0] = dl_bandwidth[0] = tac[0] = rsrp[0] = rsrq[0] = sinr[0] = other[0] = '\0';
			i = sscanf( start, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", sysmode, tdd, mcc, mnc, cellid, pcid, earfcn, freq_band_ind, dl_bandwidth, tac, rsrp, rsrq, sinr, other );
			if ( i == 14 )
			{
				//debug( "sysmode=%s, tdd=%s, mcc=%s, mnc=%s, cellid=%s, pcid=%s, earfcn=%s, freq_band_ind=%s, dl_bandwidth=%s, tac=%s, rsrp=%s, rsrq=%s, sinr=%s", sysmode, tdd, mcc, mnc, cellid, pcid, earfcn, freq_band_ind, dl_bandwidth, tac, rsrp, rsrq, sinr );
				i = atoi( pcid );
				if ( i != 65535 && i != -32768 )
				{
					json_set_string( device, "pcid", pcid );
					json_set_string( device, "netera", "5G" );
					i = atoi( rsrp );
					if ( i <= -44 && i >= -140 )
					{
						json_set_string( device, "rsrp", rsrp );
						json_set_string( device, "rssi", rsrp );
						json_set_string( device, "nettype", "NR5G-SA" );
						ret = ATCMD_ret_succeed;
					}
					i = atoi( rsrq );
					if ( i <= -3 && i >= -20 )
					{
						json_set_string( device, "rsrq", rsrq );
					}
					i = atoi( sinr );
					if ( i <= 250 && i >= 0 )
					{
						i = -20+(i/5);
						json_set_number( device, "sinr", i );
					}
				}
			}
		}

		else if ( ( start = strstr( tok, "NR5G-NSA" ) ) != NULL )
		{
			// ONLY NSA
			// at+qeng="servingcell"
			// +QENG: "NR5G-NSA", 460, 00,570,-90,64,-11 
			// LTE and NSA
			// at+qeng="servingcell"
			// +QENG: "servingcell","NOCONN"
			// +QENG: "LTE","FDD",460,11,D196A00,462,100,1,5,5,7794,-107,-12,-73,10,8,-32768,-
			// +QENG: "NR5G-NSA",460,11,562,-32768,-32768,-32768			
			i = sscanf( start, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%s", sysmode, mcc, mnc, pcid, rsrp, sinr, rsrq );
			if ( i == 7 )
			{
				//debug( "sysmode=%s, mcc=%s, mnc=%s, pcid=%s, rsrp=%s, sinr=%s, rsrq=%s", sysmode, mcc, mnc, pcid, rsrp, sinr, rsrq );
				i = atoi( pcid );
				if ( i != 65535 && i != -32768 )
				{
					json_set_string( device, "pcid", pcid );
					json_set_string( device, "netera", "5G" );
					i = atoi( rsrp );
					if ( i <= -44 && i >= -140 )
					{
						json_set_string( device, "rsrp2", rsrp );
						json_set_string( device, "nettype2", "NR5G-NSA" );
						ret = ATCMD_ret_succeed;
					}
					i = atoi( rsrq );
					if ( i <= -3 && i >= -20 )
					{
						json_set_string( device, "rsrq2", rsrq );
					}
					i = atoi( sinr );
					if ( i <= 250 && i >= 0 )
					{
						i = -20+(i/5);
						json_set_number( device, "sinr2", i );
					}
				}
			}
		}

        tok = tokkey+1;
    }
    return ret;
}
/* get current network connect state
	0 for succeed
	>0 for failed
	<0 for error */
static int ne16u_qnetdevstatus( atcmd_t fd, const char *cid )
{
    int ret;
	const char *ptr;

    ret = atcmd_tx( fd, 3, NULL, ATCMD_DEF, "AT+QNETDEVSTATUS=%s", cid );
    if ( ret != ATCMD_ret_succeed )
    {
        return ret;
    }
    ptr = atcmd_lastack( fd );
	if ( strstr( ptr, "ERROR" ) != NULL )
	{
		ret = ATCMD_ret_succeed;
	}
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
	if ( ( 0 == strcasecmp( vid, "3505" ) && 0 == strcasecmp( pid, "1001" ) ) )
	{
		info( "Lierda NE16U modem found(%s:%s)", vid , pid );
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
					fault( "Lierda NE16U modem cannot find the specified serial port(%d), system maybe cracked", i );
					return terror;
				}
			}
		}
		/* set the name */
		snprintf( buffer, sizeof(buffer), "Lierda-%s", pid );
		json_set_string( dev, "name", buffer );
		/* get the object */
		object = lte_object_get( LTE_COM, syspath, cfg, NULL, 0 );
		json_set_string( dev, "object", object );
		/* find the netdev */
		netdev = usbeth_device_find( syspath, NULL, 0 );
		if ( netdev != NULL )
		{
			json_set_string( dev, "netdev", netdev );
			json_set_string( dev, "hwnat", "enable" );
		}
		json_set_string( dev, "stty", ttylist[2] );
		json_set_string( dev, "gtty", ttylist[3] );
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
	boole r;
	int t, i;
	atcmd_t fd;
    talk_t dev;
	talk_t cfg;
	int usbmode;
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
	/* switch the dial mode skip the power up misc URC AT+QCFG="usbnet" */
	for( t=0; t<10; t++ )
	{
		i = atcmd_send( fd, "AT+QCFG=\"usbnet\"", 3, "\"usbnet\",", ATCMD_DEF );
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
			tok = strstr( ptr, "\"usbnet\"," );
			if ( tok != NULL )
			{
				if ( sscanf( tok, "%*[^,],%d", &usbmode ) == 1 )
				{
					if ( usbmode != 1 )
					{
						i = atcmd_send( fd, "AT+QCFG=\"usbnet\",1", 8, NULL, ATCMD_DEF );
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
							return terror;
						}
						else if ( i == ATCMD_ret_succeed )
						{
							r = true;
						}
					}
					break;
				}
			}
		}
		sleep( 1 );
	}

	/* switch the mode skip the power up misc URC AT+QCFG="nat" */
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
				if ( sscanf( tok, "%*[^,],%d", &usbmode ) == 1 )
				{
					if ( usbmode != 0 )
					{
						i = atcmd_send( fd, "AT+QCFG=\"nat\",0", 8, NULL, ATCMD_DEF );
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
							r = true;
						}
					}
					break;
				}
			}
		}
		sleep( 1 );
	}

	/* switch voice call disable skip the power up misc URC AT+QCFG="ims" */
	for( t=0; t<10; t++ )
	{
		i = atcmd_send( fd, "AT+QCFG=\"ims\"", 3, "\"ims\",", ATCMD_DEF );
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
			tok = strstr( ptr, "\"ims\"," );
			if ( tok != NULL )
			{
				if ( sscanf( tok, "%*[^,],%d", &usbmode ) == 1 )
				{
					if ( usbmode != 2 )
					{
						i = atcmd_send( fd, "AT+QCFG=\"ims\",2", 8, NULL, ATCMD_DEF );
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
							r = true;
						}
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
	i = atcmd_tx( fd, 10, NULL, ATCMD_DEF, "AT+QNETDEVCTL=1,0" );
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
    i = atcmd_send( fd, "AT+LPOWD", 60, "POWERED DOWN", ATCMD_DEF );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	sleep( 5 );

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

	ret = ttrue;
	cfg = param_talk( param, 2 );
	/* network type setting */
    ptr = json_get_string( cfg, "lock_nettype" );
    i = ne16u_lock_nettype( fd, dev, ptr );
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
	i = ne16u_lock_band( fd, ptr );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	/* cell setting */
	ptr = json_get_string( cfg, "lock_cell" );
	i = ne16u_lock_cell( fd, ptr );
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
		i = ne16u_set_profileset( fd, profile );
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
    i = usbtty_c5gregmode( fd, 2 );
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
	json_delete_axp( dev, "rsrq" );
	json_delete_axp( dev, "sinr" );
	json_delete_axp( dev, "band" );
	json_delete_axp( dev, "netera" );
	json_delete_axp( dev, "nettype" );
	json_delete_axp( dev, "rsrp2" );
	json_delete_axp( dev, "rsrq2" );
	json_delete_axp( dev, "sinr2" );
	json_delete_axp( dev, "band2" );
	json_delete_axp( dev, "nettype2" );
	// at+qeng="servingcell"
	i = ne16u_servingcell( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}
	// AT+QNWINFO
	i = ne16u_qnwinfo( fd, dev );
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
	i = usbtty_c5greg( fd, dev );
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


	// AT+COPS?
	// +COPS: 0,0,"46000",7\nOK
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
	i = usbtty_cops( fd, dev );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_term )
	{
		return tfalse;
	}


	// 2 to 31, -113dBm to -53dBm  GSM/LTE
	// 100 to 191, -116dBm to -25dBm  TDSCDMA
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
		if ( (csq >0 && csq <= 31) || (csq > 100 && csq <= 191) )
		{
			if ( csq > 0 && csq <= 31 )
			{
				i = ( csq*2-113 );
				if ( json_get_number( dev, "rssi" ) == 0 )
				{
					json_set_number( dev, "rssi", i );
				}
			}
			else if ( csq > 100 && csq <= 191  )
			{
				i = csq -215;
				if ( json_get_number( dev, "rssi" ) == 0 )
				{
					json_set_number( dev, "rssi", i );
				}
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
	i = ne16u_set_profileset( fd, profile );
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
	i = atcmd_tx( fd, 20, NULL, ATCMD_DEF, "AT+QNETDEVCTL=%s,1,1", cid );
	if ( i < ATCMD_ret_succeed )
	{
		return terror;
	}
	else if ( i == ATCMD_ret_succeed )
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
	talk_t cfg;
	atcmd_t fd;
	talk_t profile;
	const char *cid;

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

	/* prefile setting get */
	profile = json_value( cfg, "profile_cfg" );
	cid = json_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}

	i = ne16u_qnetdevstatus( fd, cid );
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
	talk_t cfg;
	atcmd_t fd;
	talk_t profile;
	const char *cid;

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

	/* prefile setting get */
	profile = json_value( cfg, "profile_cfg" );
	cid = json_string( profile, "cid" );
	if ( cid == NULL || *cid == '\0' )
	{
		cid = "1";
	}

	/* disconnection */
	i = atcmd_tx( fd, 10, NULL, ATCMD_DEF, "AT+QNETDEVCTL=%s,0", cid );
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



