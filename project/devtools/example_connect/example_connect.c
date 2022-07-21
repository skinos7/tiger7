/*
 *  Description:  connect
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landnet/landnet.h"
#include <event.h>
#include <event2/event.h>



/* 管理句柄 */
int connect_unix = -1;
/* 配置 */
talk_t connect_cfg;
const char *type;
boole mo, bo, ko;
const char *m, *b, *k, *r;
const char *default_route;
/* 默认的signal mask */
sigset_t connect_sigmask;
/* 事件循环相关的结构 */
struct event_base *connect_base;
/* 定时检测事件结构 */
struct event *connect_timer;
/* 刷新信号事件结构 */
struct event *connect_hupsignaler;
/* 退出信号事件结构 */
struct event *connect_termsignaler;
/* 读管理句柄事件 */
struct event *connect_reader;
/* 当前工作模式 */
const char *reg_network_mode;
/* 默认本地接口 */
const char *reg_local_ifname;
/* 连接管理函数类型 */
typedef void (*connecter_t)( int fd, short what, void *arg );



/* 延迟统计 */
int dbdc_delay_flag;
int dbdc_delay_time;
int dbdc_delay_divide;
int dbdc_delay_diff;
int delay_count( int times, int level, int delay, talk_t link, talk_t link2 )
{
	int l, l2;
	talk_t axp;
	int i, t, totle;
	const char *ptr;

	if ( times <= 0 || link < TALK_ECODEMAX || link2 < TALK_ECODEMAX )
	{
		return 0;
	}
	/* count link */
	t = totle = 0;
	axp = NULL;
	while( NULL != ( axp = json_next( link, axp ) ) )
	{
		ptr = axp_string( axp );
		if ( ptr == NULL && *ptr == '\0' )
		{
			continue;
		}
		i = atoi( ptr );
		if ( i < 0 )
		{
			i = 3000;
		}
		totle += i;
		t++;
	}
	if ( t < times )
	{
		return 0;
	}
	l = totle/t;

	/* count link2 */
	t = totle = 0;
	axp = NULL;
	while( NULL != ( axp = json_next( link2, axp ) ) )
	{
		ptr = axp_string( axp );
		if ( ptr == NULL && *ptr == '\0' )
		{
			continue;
		}
		i = atoi( ptr );
		if ( i < 0 )
		{
			i = 3000;
		}
		totle += i;
		t++;
	}
	if ( t < times )
	{
		return 1;
	}
	l2 = totle/t;

	/* compare */
	if ( l <= level && l2 <= level )
	{
		return 0;
	}
	if ( l > level && l2 > level )
	{
		return 0;
	}
	if ( l2-l > delay )
	{
		return 1;
	}
	if ( l-l2 > delay )
	{
		return 2;
	}
	
	return 0;
}



/* 单外部链接管理 */
void connect_default( int fd, short what, void *arg )
{
	talk_t v;
	talk_t axp;
	talk_t value;
	const char *ip;
	const char *ifname;
	const char *status;
	char netdev[NAME_MAX];

	if ( route_info( "0.0.0.0", NULL, NULL, netdev ) == false )
	{
		v = scall( NETWORK_COM, "extern", NULL );
		axp = NULL;
		while( NULL != ( axp = json_next( v, axp ) ) )
		{
			ifname = axp_id( axp );
			value = axp_value( axp );
			status = json_string( value, "status" );
			ip = json_string( value, "ip" );
			if ( ip == NULL || *ip == '\0' || status == NULL || 0 != strcmp( status, "up" ) )
			{
				continue;
			}
			info( "connect switch gateway to %s", ifname );
			if ( route_switch( NULL, "0.0.0.0", NULL, NULL, value, true ) == true )
			{
				joint_callt( "network/online", value );
			}
			break;
		}
		talk_free( v );
	}
}
/* 备用链接管理 */
void connect_hot( int fd, short what, void *arg )
{
	const char *delay;
	const char *status;
	talk_t mst, bst, kst;
	const char *md, *bd, *kd;
	char netdev[NAME_MAX];

	md = bd = kd = NULL;
	/* get current state */
	mst = scall( m, "state", NULL );
	bst = scall( b, "state", NULL );
	kst = scall( k, "state", NULL );
	/* get main state */
	delay = json_string( mst, "delay" );
	status = json_string( mst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		mo = true;
		md = json_string( mst, "netdev" );
	}
	else
	{
		mo = false;
	}
	/* get backup state */
	delay = json_string( bst, "delay" );
	status = json_string( bst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		bo = true;
		bd = json_string( bst, "netdev" );
	}
	else
	{
		bo = false;
	}
	/* get king state */
	delay = json_string( kst, "delay" );
	status = json_string( kst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		ko = true;
		kd = json_string( kst, "netdev" );
	}
	else
	{
		ko = false;
	}

	/* get current gateway */
	netdev[0] = '\0';
	route_info( "0.0.0.0", NULL, NULL, netdev );

	/* switch the gateway depend current state */
	if ( ko == true )
	{
		/* swtich the default to king */
		if ( kd != NULL && 0 != strcmp( kd, netdev ) )
		{
			info( "switch the default gateway to %s when %s online", k, k );
			route_switch( NULL, "0.0.0.0", NULL, NULL, kst, true );
			joint_callt( "network/online", kst );
			default_route = k;
		}
	}
	else
	{
		if ( mo == true && bo == false )
		{
			/* swtich the default to main */
			if ( md != NULL && 0 != strcmp( md, netdev ) )
			{
				info( "switch the default gateway to %s when only %s online", m, m );
				route_switch( NULL, "0.0.0.0", NULL, NULL, mst, true );
				joint_callt( "network/online", mst );
				default_route = m;
			}
		}
		else if ( mo == false && bo == true )
		{
			/* swtich the default to backup */
			if ( bd != NULL && 0 != strcmp( bd, netdev ) )
			{
				info( "switch the default gateway to %s when only %s online", b, b );
				route_switch( NULL, "0.0.0.0", NULL, NULL, bst, true );
				joint_callt( "network/online", bst );
				default_route = b;
			}
		}
		else if ( mo == true && bo == true )
		{
			/* swtich the default to main */
			if ( md != NULL && 0 != strcmp( md, netdev ) )
			{
				info( "switch the default gateway to %s when only %s online", m, m );
				route_switch( NULL, "0.0.0.0", NULL, NULL, mst, true );
				joint_callt( "network/online", mst );
				default_route = m;
			}
		}
	}
	talk_free( mst );
	talk_free( bst );
	talk_free( kst );
}
/* DBDC链接管理 */
boole inital_shunts = false;
void connect_dbdc( int fd, short what, void *arg )
{
	talk_t axp;
	talk_t value;
	const char *ptr;
	const char *delay;
	const char *status;
	talk_t mst, bst, kst;
	const char *md, *bd, *kd;
	char netdev[NAME_MAX];

	md = bd = kd = NULL;
	/* get current state */
	mst = scall( m, "state", NULL );
	bst = scall( b, "state", NULL );
	kst = scall( k, "state", NULL );

	/* check the main tid route */
	delay = json_string( mst, "delay" );
	status = json_string( mst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		mo = true;
		md = json_string( mst, "netdev" );
		netdev[0] = '\0';
		ifname_table_exist( m, "0.0.0.0", NULL, NULL, netdev );
		if (  md != NULL && 0 != strcmp( md, netdev ) )
		{
			ptr = json_string( mst, "tid" );
			if ( ptr != NULL )
			{
				info( "switch the %s gateway to %s", ptr, m );
				route_switch( ptr, "0.0.0.0", NULL, NULL, mst, true );
			}
		}
	}
	else
	{
		mo = false;
	}
	/* check the backup tid route */
	delay = json_string( bst, "delay" );
	status = json_string( bst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		bo = true;
		bd = json_string( bst, "netdev" );
		netdev[0] = '\0';
		ifname_table_exist( b, "0.0.0.0", NULL, NULL, netdev );
		if (  bd != NULL && 0 != strcmp( bd, netdev ) )
		{
			ptr = json_string( bst, "tid" );
			if ( ptr != NULL )
			{
				info( "switch the %s gateway to %s", ptr, b );
				route_switch( ptr, "0.0.0.0", NULL, NULL, bst, true );
			}
		}
	}
	else
	{
		bo = false;
	}
	/* check the king tid route */
	delay = json_string( kst, "delay" );
	status = json_string( kst, "status" );
	if ( ( delay == NULL || 0 != strcmp( delay, "failed" ) ) && ( status != NULL && 0 == strcmp( status, "up" ) ) )
	{
		ko = true;
		kd = json_string( kst, "netdev" );
		netdev[0] = '\0';
		ifname_table_exist( k, "0.0.0.0", NULL, NULL, netdev );
		if (  kd != NULL && 0 != strcmp( kd, netdev ) )
		{
			ptr = json_string( kst, "tid" );
			if ( ptr != NULL )
			{
				info( "switch the %s gateway to %s", ptr, k );
				route_switch( ptr, "0.0.0.0", NULL, NULL, kst, true );
			}
		}
	}
	else
	{
		ko = false;
	}

	/* get current gateway */
	netdev[0] = '\0';
	route_info( "0.0.0.0", NULL, NULL, netdev );

	/* switch the gateway depend current state */
	if ( ko == true )
	{
		/* swtich the default to king */
		if ( kd != NULL && 0 != strcmp( kd, netdev ) )
		{
			info( "switch the default gateway to %s when %s online", k, k );
			route_switch( NULL, "0.0.0.0", NULL, NULL, kst, true );
			joint_callt( "network/online", kst );
			default_route = k;
		}
		if ( inital_shunts == true )
		{
			info( "disable the shunts when %s online", k );
			shunts_balancing_disable();
			inital_shunts = false;
		}
	}
	else
	{
		if ( mo == true && bo == false )
		{
			/* swtich the default to main */
			if ( md != NULL && 0 != strcmp( md, netdev ) )
			{
				info( "switch the default gateway to %s when only %s online", m, m );
				route_switch( NULL, "0.0.0.0", NULL, NULL, mst, true );
				joint_callt( "network/online", mst );
				default_route = m;
			}
			if ( inital_shunts == true )
			{
				info( "disable the shunts when only %s online", m );
				shunts_balancing_disable();
				inital_shunts = false;
			}
		}
		else if ( mo == false && bo == true )
		{
			/* swtich the default to backup */
			if ( bd != NULL && 0 != strcmp( bd, netdev ) )
			{
				info( "switch the default gateway to %s when only %s online", b, b );
				route_switch( NULL, "0.0.0.0", NULL, NULL, bst, true );
				joint_callt( "network/online", bst );
				default_route = b;
			}
			if ( inital_shunts == true )
			{
				info( "disable the shunts when only %s online", b );
				shunts_balancing_disable();
				inital_shunts = false;
			}
		}
		else if ( mo == true && bo == true )
		{
			if ( dbdc_delay_time > 0 )
			{
				talk_t mdl;
				talk_t bdl;

				/* count the delay to switch */
				mdl = scalls( KEEPLIVE_COM, "lately", "%s,%d", m, dbdc_delay_time );
				bdl = scalls( KEEPLIVE_COM, "lately", "%s,%d", b, dbdc_delay_time );
				dbdc_delay_flag = delay_count( dbdc_delay_time, dbdc_delay_divide, dbdc_delay_diff, mdl, bdl );
				if ( dbdc_delay_flag == 2 )
				{
					/* swtich the default to backup */
					if ( bd != NULL && 0 != strcmp( bd, netdev ) )
					{
						info( "switch the default gateway to %s when only %s delay ok", b, b );
						route_switch( NULL, "0.0.0.0", NULL, NULL, bst, true );
						joint_callt( "network/online", bst );
						default_route = b;
					}
					if ( inital_shunts == true )
					{
						info( "disable the shunts when only %s delay ok", b );
						shunts_balancing_disable();
						inital_shunts = false;
					}
					talk_free( mdl );
					talk_free( bdl );
					goto quit;
				}
				else if ( dbdc_delay_flag == 1 )
				{
					/* swtich the default to main */
					if ( md != NULL && 0 != strcmp( md, netdev ) )
					{
						info( "switch the default gateway to %s when only %s delay ok", m, m );
						route_switch( NULL, "0.0.0.0", NULL, NULL, mst, true );
						joint_callt( "network/online", mst );
						default_route = m;
					}
					if ( inital_shunts == true )
					{
						info( "disable the shunts when only %s delay ok", m );
						shunts_balancing_disable();
						inital_shunts = false;
					}
					talk_free( mdl );
					talk_free( bdl );
					goto quit;
				}
				talk_free( mdl );
				talk_free( bdl );
			}

			/* swtich the default to main */
			if ( md != NULL && 0 != strcmp( md , netdev ) )
			{
				info( "switch the defualt gateway to %s when %s and %s online", m, m, b );
				route_switch( NULL, "0.0.0.0", NULL, NULL, mst, true );
				joint_callt( "network/online", mst );
				default_route = m;
			}
			if ( inital_shunts == false )
			{
				info( "enable the shunts when %s and %s work", m, b );
				/* init the local rule to ifname route for balancing */
				value = scall( NETWORK_COM, "local", NULL );
				axp = NULL;
				while ( NULL != ( axp = json_next( value, axp ) ) )
				{
					ptr = json_string( axp_value( axp ), "netdev" );
					/* load balancing */
					shunts_balancing( 2, 0, ptr, m );
					shunts_balancing( 2, 1, ptr, b );
				}
				talk_free( value );
				inital_shunts = true;
			}
		}
	}
quit:	
	talk_free( mst );
	talk_free( bst );
	talk_free( kst );
}



/* 日志函数 */
void connect_log( int severity, const char *msg )
{
    switch (severity)
    {
        case EVENT_LOG_DEBUG : debug( msg ); break;
        case EVENT_LOG_MSG   : info( msg );   break;
        case EVENT_LOG_WARN  : warn( msg );  break;
        case EVENT_LOG_ERR   : fault( msg ); break;
    }
}
/* 停止启动服务 */
void connect_free( void )
{
	if ( connect_termsignaler != NULL )
	{
		event_del( connect_termsignaler );
	}
	if ( connect_hupsignaler != NULL )
	{
		event_del( connect_hupsignaler );
	}
	if ( connect_reader != NULL )
	{
		event_del( connect_reader );
	}
	if ( connect_unix >= 0 )
	{
		close( connect_unix );
	}
	event_base_free( connect_base );
	sigprocmask( SIG_SETMASK, &connect_sigmask, NULL );
}
/* 退出循环 */
void connect_break( int fd, short what, void *arg )
{
	event_base_loopbreak( connect_base );
}
/* 交互接口 */
talk_t connect_adjust( talk_t req )
{
	talk_t x;
	talk_t ret;
	const char *order;

	order = json_string( req, "order" );
	if ( order == NULL || *order == '\0' )
	{
        errno = EINVAL;
		return terror;
	}
	ret = NULL;
	if ( 0 == strcmp( order, "status" ) )
	{
		ret = json_create( NULL );
		if ( k != NULL && *k != '\0' )
		{
			x = json_create( NULL );
			json_set_value( ret, k, x );
			if ( ko == true )
			{
				json_set_string( x, "status", "valid" );
				json_set_string( x, "inuse", "enable" );
			}
			else
			{
				json_set_string( x, "status", "invalid" );
				json_set_string( x, "inuse", "disable" );
			}
		}
		if ( m != NULL && *m != '\0' )
		{
			x = json_create( NULL );
			json_set_value( ret, m, x );
			if ( mo == true )
			{
				json_set_string( x, "status", "valid" );
			}
			else
			{
				json_set_string( x, "status", "invalid" );
			}
			if ( ko == false )
			{
				if ( inital_shunts == true || ( default_route != NULL && 0 == strcmp( default_route, m ) ) )
				{
					json_set_string( x, "inuse", "enable" );
				}
				else
				{
					json_set_string( x, "inuse", "disable" );
				}
			}
			else
			{
				json_set_string( x, "inuse", "disable" );
			}
		}
		if ( b != NULL && *b != '\0' )
		{
			x = json_create( NULL );
			json_set_value( ret, b, x );
			if ( bo == true )
			{
				json_set_string( x, "status", "valid" );
			}
			else
			{
				json_set_string( x, "status", "invalid" );
			}
			if ( ko == false )
			{
				if ( inital_shunts == true || ( default_route != NULL && 0 == strcmp( default_route, b ) ) )
				{
					json_set_string( x, "inuse", "enable" );
				}
				else
				{
					json_set_string( x, "inuse", "disable" );
				}
			}
			else
			{
				json_set_string( x, "inuse", "disable" );
			}
		}
	}

	return ret;
}
void connect_read( int fd, short what, void *arg )
{
	talk_t r;
	talk_t a;
	char *ack;
    int nread;
    int nwrite;
	int acklen;
    socklen_t socklen;
    struct sockaddr_un sockaddr;
	static char connect_ack[2];
	static char connect_request[4096];

    do
    {
        /* get the request */
        socklen = sizeof(sockaddr);
        nread = recvfrom( fd, connect_request, sizeof(connect_request)-1, 0, (struct sockaddr*)&sockaddr, &socklen );
        if ( nread < 0 )
        {
            if ( errno != EWOULDBLOCK && errno != EINPROGRESS )
            {
                faulting( "connect recvfrom unix socket error, create it again" );
				event_base_loopbreak( connect_base );
				return;
            }
            break;
        }
        else if ( nread == 0 )
        {
            continue;
        }
        connect_request[nread] = '\0';
        //verbose( "%s<<<%d:%s", sockaddr.sun_path, nread, service_request );
		r = string2talk( connect_request );
		if ( r < TALK_ECODEMAX )
		{
			continue;
		}
		/* process the request */
		ack = NULL;
		acklen = 0;
		a = connect_adjust( r );
		/* send the ack */
		if ( a > TALK_ECODEMAX )
		{
			ack = json2string( a );
			acklen = strlen( ack );
			talk_free( a );
			//verbose( "%s>>>%d:%s", sockaddr.sun_path, acklen, ack );
		}
		else if ( a == NULL )
		{
			connect_ack[0] = 0;
			acklen = 1;
			ack = connect_ack;
			//verbose( "%s>>>%d:NULL", sockaddr.sun_path, acklen );
		}
		else if ( a == ttrue )
		{
			connect_ack[0] = ttrue_code;
			acklen = 1;
			ack = connect_ack;
			//verbose( "%s>>>%d:ttrue", sockaddr.sun_path, acklen );
		}
		else if ( a == tfalse )
		{
			connect_ack[0] = tfalse_code;
			acklen = 2;
			connect_ack[1] = (char)errno;
			ack = connect_ack;
			//verbose( "%s>>>%d:tfalse/%d", sockaddr.sun_path, acklen, errno );
		}
		else if ( a == terror )
		{
			connect_ack[0] = terror_code;
			acklen = 2;
			connect_ack[1] = (char)errno;
			ack = connect_ack;
			//verbose( "%s>>>%d:terror/%d", sockaddr.sun_path, acklen, errno );
		}
		talk_free( r );
		if ( acklen > 0 )
		{
	        /* send the msg */
	        nwrite = sendto( fd, ack, acklen, 0, (struct sockaddr*)&sockaddr, socklen );
	        if ( nwrite <= 0 )
	        {
	            if ( errno != EWOULDBLOCK && errno != EINPROGRESS )
	            {
	                faulting( "connect sendto unix socket error, create it again" );
					event_base_loopbreak( connect_base );
					return;
	            }
	        }
			if ( ack != connect_ack )
			{
				free( ack );
			}
		}
    }while( nread >= 0 );
}
/* 主函数 */
int main( int argc, const char **argv )
{
    pid_t pid;
	talk_t v;
	talk_t axp;
	talk_t value;
	const char *ptr;
	const char *mode;
    struct timeval tv;
	connecter_t connecter;
	const char *local_ifname;
    struct event_config *econfig;
    enum event_method_feature efeature;

	// 防止调试
    //if ( ptrace( PTRACE_TRACEME, 0, 1, 0 ) < 0 )
    //{
    //    return -1;
    //}
	// 常用工具命令
    if ( argc == 2 )
    {
        if ( strcmp( argv[1], "exit" ) == 0 )
        {
            pid = file2int( CONNECT_PID_FILE );
            if ( pid > 0 )
            {
                kill( pid, SIGTERM );
            }
        }
        if ( strcmp( argv[1], "flush" ) == 0 )
        {
            pid = file2int( CONNECT_PID_FILE );
            if ( pid > 0 )
            {
                kill( pid, SIGHUP );
            }
        }
        return 0;
    }


	/* 加载配置 */
	reg2string( "network_mode", reg_network_mode, mode, NULL);
	if ( mode == NULL || *mode == '0' )
	{
		exit( EXIT_EINVAL );
		return -1;
	}
	connect_cfg = config_sgets( NETWORK_COM, "%s/connect", mode );
	/* 启动链接 */
	m = json_string( connect_cfg, "main" );
	if ( m != NULL && *m != '\0' )
	{
		scall( m, "setup", NULL );
	}
	b = json_string( connect_cfg, "back" );
	if ( b != NULL && *b != '\0' )
	{
		scall( b, "setup", NULL );
	}
	k = json_string( connect_cfg, "king" );
	if ( k != NULL && *k != '\0' )
	{
		scall( k, "setup", NULL );
	}
	r = json_string( connect_cfg, "reserve" );
	if ( r != NULL && *r != '\0' )
	{
		scall( r, "setup", NULL );
	}
	/* 初始化模式 */
	connecter = connect_default;
	type = json_string( connect_cfg, "type" );
	if ( type != NULL && 0 == strcmp( type, "hot") )
	{
		connecter = connect_hot;
	}
	else if ( connect_cfg != NULL )
	{
		/* get the dbdc configure */
		dbdc_delay_time = 0;
		dbdc_delay_divide = 200;
		dbdc_delay_diff = 150;
		ptr = json_string( connect_cfg, "delay_count" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			dbdc_delay_time = atoi( ptr );
		}
		if ( dbdc_delay_time > 0 )
		{
			ptr = json_string( connect_cfg, "delay_divide" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				dbdc_delay_divide = atoi( ptr );
			}
			ptr = json_string( connect_cfg, "delay_diff" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				dbdc_delay_diff = atoi( ptr );
			}
		}
		/* wait the local netdev up */
		reg2string( "local_ifname", reg_local_ifname, local_ifname, NULL );
		do
		{
			v = scall( local_ifname, "status", NULL );
			if ( v != NULL )
			{
				ptr = json_string( v, "status" );
				if ( ptr != NULL && 0 == strcmp( ptr, "up" ) )
				{
					break;
				}
			}
			sleep( 1 );
		}
		while (1);
		/* clear the ifname table */
		ifname_route_clear( m );
		ifname_route_clear( b );
		ifname_route_clear( k );
		/* init the local rule to ifname route*/
		value = scall( NETWORK_COM, "local", NULL );
		axp = NULL;
		while ( NULL != ( axp = json_next( value, axp ) ) )
		{
			v = axp_value( axp );
            info( "init the extern ifname(%s,%s,%s) table for %s start", m?:"", b?:"", k?:"", axp_id(axp) );
			ifname_route_local( m, v );
			ifname_route_local( b, v );
			ifname_route_local( k, v );
		}
		talk_free( value );
		/* fwmark rule to the ifname table */
		ifname_rule_mark( m, 5 );
		ifname_rule_mark( b, 5 );
		ifname_rule_mark( k, 5 );
		/* create the shunts */
		shunts_create();



		/* custom the dest ip to shunts */
		shunts_deattch();
		value = json_create( NULL );
		v = json_create( NULL );
		json_set_string( v, "dest", "111.229.150.228" );
		json_set_value( value, "rule1", v );
		shunts_attach( value );
		talk_free( value );


		
		connecter = connect_dbdc;
	}

    /* 记录当前的pid号及sigmask */
	pid = getpid();
	info( "connect(%d) start", pid );
    int2file( CONNECT_PID_FILE, (unsigned int)pid );
    sigprocmask( 0, NULL, &connect_sigmask );


    /* 初始化event */
    econfig = event_config_new();
    efeature = EV_FEATURE_FDS;
    event_config_require_features( econfig, efeature );
    connect_base = event_base_new_with_config( econfig );
    event_config_free( econfig );
    event_set_log_callback( connect_log );
    /* 注册退出信号 */
    connect_termsignaler = evsignal_new( connect_base, SIGTERM, connect_break, NULL );  
    if ( connect_termsignaler == NULL)  
    {
        faulting( "connect event_new SIGTERM error" );
		goto quit;
    }
    event_add( connect_termsignaler, NULL );
    /* 注册刷新服务信号 */
    connect_hupsignaler = evsignal_new( connect_base, SIGHUP, connecter, NULL );  
    if ( connect_hupsignaler == NULL)  
    {
        faulting( "connect event_new SIGHUP error" );
		goto quit;
    }
    event_add( connect_hupsignaler, NULL );
    /* 注册定时检测 */
	connect_timer = event_new( connect_base, -1, EV_TIMEOUT|EV_PERSIST, connecter, NULL );
	if ( connect_timer == NULL )
	{
		faulting( "watchdog event_new timeout event failed" );
		return -1;
	}
	tv.tv_sec = 5;
	tv.tv_usec = 100000;  // 1000000 = 1sec
	evtimer_add( connect_timer, &tv );
	/* 注册UNIX域的通信接口 */
	connect_unix = unix_listen( CONNECT_UNIX_FILE );
	fd_nonblock( connect_unix );
	socket_reuse( connect_unix );
	/* init the event */
	connect_reader = event_new( connect_base, connect_unix, EV_READ|EV_PERSIST, connect_read, NULL );
	if ( connect_reader == NULL )
	{
		faulting( "service create unix read event failed" );
		return -1;
	}
	event_add( connect_reader, NULL );


    /* loop */
    event_base_dispatch( connect_base );


quit:
	/* exit */
	fault( "connect(%d) exit", pid );
	connect_free();
	talk_free( connect_cfg );
	exit( EXIT_tfalse );
	return -1;
}



