/*
 *  Description:  test for ip connection
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landnet/landnet.h"



boole_t _setup( obj_t this, param_t param )
{
	talk_t v;
    talk_t state;
    const char *obj;
    const char *object;

	obj = obj_com( this );
    object = obj_combine( this );
	/* need the ethernet exist */
	state = scall( "arch@ethernet", "status", NULL );
	v = json_value( state, obj );
	if ( v == NULL )
	{
		talk_free( state );
        return tfalse;
	}
	/* run the test service */
	sstart( object, "service", NULL, object );
    talk_free( state );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
    const char *object;

    object = obj_combine( this );
    sstop( object );
    return ttrue;
}



int echo_port = 520;
int echo_sock = -1;
const char *echo_object;
const char *echo_device;
const char *echo_context;
int echo_create( void )
{
	int sock;
	int wwrite;
	int nwrite;
	struct ifreq ifr;
	struct sockaddr_in inanyadd;

	/* make broadcast socket */
	bzero( &inanyadd, sizeof(inanyadd) );
	inanyadd.sin_family = AF_INET;
	inanyadd.sin_addr.s_addr = htonl(INADDR_ANY);
	inanyadd.sin_port = htons( echo_port );
	if ( ( sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		faulting( "echo create socket() error" );
		return -1;
	}
	socket_reuse( sock );
	/* bind a socket to a device name (might not work on all systems) */
	memset( &ifr, 0, sizeof(struct ifreq) );
	strncpy( ifr.ifr_name, echo_device, sizeof(ifr.ifr_name)-1 );
	ioctl( sock, SIOCGIFINDEX, &ifr );
	if ( setsockopt( sock, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, sizeof(struct ifreq) ) < 0 )
	//if ( setsockopt( sock, SOL_SOCKET, SO_BINDTODEVICE, "lan", 3 ) < 0 )
	{
		faulting( "echo setsockopt the SO_BINDTODEVICE broadcast(%d) error", echo_port );
		return -1;
	}
	/* add dont route */
	wwrite = 1;
	if ( setsockopt( sock, SOL_SOCKET, SO_DONTROUTE, (char*)&wwrite, sizeof(wwrite) ) < 0 )
	{
		faulting( "echo setsockopt the SO_DONTROUTE broadcast(%d) error", echo_port );
		return -1;
	}
	/* add broadcast */
	wwrite = 1;
	if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&wwrite, sizeof(wwrite) ) < 0 )
	{
		faulting( "echo setsockopt the SO_BROADCAST broadcast(%d) error", echo_port );
		return -1;
	}
	/* disable loop date */
	wwrite = 0;
	if ( setsockopt( sock, IPPROTO_IP, IP_MULTICAST_LOOP, &wwrite, sizeof(wwrite) ) < 0 )
	{
		faulting( "echo setsockopt the IP_MULTICAST_LOOP(%d) error", echo_port );
		return -1;
	}
	/* bind the port */
	if ( bind( sock,(struct sockaddr *)&inanyadd, sizeof(struct sockaddr_in)) < 0 )
	{
		faulting( "echo bind the broadcast(%d) error", echo_port );
		return -1;
	}
	fd_nonblock( sock );

	/* beacon first */
	inanyadd.sin_addr.s_addr = htonl( INADDR_BROADCAST );
	wwrite = strlen( echo_context );
	nwrite = sendto( sock, echo_context, wwrite, 0, (struct sockaddr*)&inanyadd, sizeof(struct sockaddr_in) );
	if ( nwrite != wwrite )
	{
		faulting( "beacon send broadcast( %d ) error(%d)", echo_port, nwrite );
		return -1;
	}

	return sock;
}
void echo_boardcast( int fd, short what, void *arg )
{
	int wwrite;
	int nwrite;
	struct sockaddr_in inanyadd;

	if ( echo_sock < 0 )
	{
		return;
	}
	/* make broadcast dest */
	bzero( &inanyadd, sizeof(inanyadd) );
	inanyadd.sin_family = AF_INET;
	inanyadd.sin_port = htons( echo_port );
	inanyadd.sin_addr.s_addr = htonl( INADDR_BROADCAST );
	/* beacon */
	wwrite = strlen( echo_context );
	nwrite = sendto( echo_sock, echo_context, wwrite, 0, (struct sockaddr*)&inanyadd, sizeof(struct sockaddr_in) );
	if ( nwrite != wwrite )
	{
		faulting( "broadcast beacon( %d ) error(%d)", echo_port, nwrite );
		reactor_break( fd, what, arg );
		return;
	}
}
void echo_read( int fd, short what, void *arg )
{
	int wread;
	int nread;
	socklen_t socklen;
	char srcip[NAME_MAX];
	char readbuf[1500];
	struct sockaddr_in sockaddr;

	do
	{
		/* read the socket */
		wread = sizeof(readbuf);
		socklen = sizeof(sockaddr);
		memset( &sockaddr, 0, sizeof(sockaddr) );
		nread = recvfrom( fd, readbuf, wread, 0, (struct sockaddr*)&sockaddr, &socklen );
		if ( nread < 0 )
		{
			if ( errno != EWOULDBLOCK && errno != EINPROGRESS )
			{
				faulting( "echo_read( %d, %p, %d ) error", fd, readbuf, wread );
				reactor_break( fd, what, arg );
				return;
			}
			break;
		}
		else if ( nread == 0 )
		{
			warning( "echo_read( %d, %p, %d ) zero return", fd, readbuf, wread );
			reactor_break( fd, what, arg );
			return;
		}
		/* end the string */
		*(readbuf+nread) = '\0';
		/* print */
		inet_ntop( AF_INET, &sockaddr.sin_addr, srcip, sizeof(srcip) );
		printf( "%s recv %s:%s\n", echo_object, srcip, readbuf );
		if ( nread == strlen(echo_context) && strcmp( readbuf, echo_context ) != 0 )
		{
			register_set( echo_object, "recv", readbuf, nread, 1500 );
		}
	}while( nread > 0 );
}
boole_t _service( obj_t this, param_t param )
{
	int interval;
	const char *ifdev;
	const char *netdev;
	const char *object;
	unsigned int randi;
	const int *randp;
	char ip[NAME_MAX];
	char mac[NAME_MAX];
    struct event *hander;
	struct event_base *base;
    struct event_config *econfig;
    enum event_method_feature efeature;

	object = obj_combine( this );
	/* get the ifdev */
	ifdev = register_pointer( object, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
		fault( "cannot found %s ifdev", object );
        sleep( 3 );
        return tfalse;
    }
    /* get the netdev */
	netdev = register_value( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        fault( "%s netdev get error", ifdev );
        sleep( 3 );
        return tfalse;
    }
	/* modify the mac and ip */
	ip[0] = mac[0] = '\0';
	randp = register_value( LAND_PROJECT, "rand" );
	if ( randp != NULL )
	{
		randi = *randp;
		if ( 0 == strcmp( object, "ifname@lan1" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.1.1" );
			snprintf( mac, sizeof(mac), "62%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan2" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.2.1" );
			snprintf( mac, sizeof(mac), "64%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan3" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.3.1" );
			snprintf( mac, sizeof(mac), "66%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan4" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.4.1" );
			snprintf( mac, sizeof(mac), "68%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan5" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.5.1" );
			snprintf( mac, sizeof(mac), "6A%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan6" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.6.1" );
			snprintf( mac, sizeof(mac), "6C%u", randi );
		}
		else if ( 0 == strcmp( object, "ifname@lan7" ) )
		{
			snprintf( ip, sizeof(ip), "192.168.7.1" );
			snprintf( mac, sizeof(mac), "6E%u", randi );
		}
	}
	if ( mac[0] != '\0' )
	{
		scalls( ifdev, "setmac", mac );
		warn( "%s auto mac address %s", object, mac );
	}
	if ( ip[0] != '\0' )
	{
		shell( "ifconfig %s %s netmask 255.255.255.0", netdev, ip );
		shell( "arping -U -c 1 -I %s %s", netdev, ip );
	}

	/* event loop */
	echo_port = 520;
	echo_object = object;
	echo_device = netdev;
	echo_context = object;
	interval = 1;
    econfig = event_config_new();
    efeature = EV_FEATURE_FDS;
    event_config_require_features( econfig, efeature );
    base = event_base_new_with_config( econfig );
	if ( base != NULL )
	{
		reactor_signal_break( base, SIGINT );
		reactor_signal_break( base, SIGTERM );
		echo_sock = echo_create();
		if ( echo_sock >= 0 )
		{
			hander = event_new( base, echo_sock, EV_READ|EV_PERSIST, echo_read, base );
			event_add( hander, NULL );
			reactor_timer_create( base, echo_boardcast, interval, 0 );
			event_base_dispatch( base );
		}
		event_base_free(base);
	}
	else
	{
		fault( "%s event_base_new error", COM_IDPATH );
	}

	/* exit */
	warn( "%s exit", COM_IDPATH );
	return tfalse;
}
talk_t _status( obj_t this, param_t param )
{
    const char *object;
	const char *buffer;

	object = obj_combine( this );
	buffer = register_value( object, "recv" );
	if ( buffer != NULL && *buffer != '\0' )
	{
		return ttrue;
	}
	return tfalse;
}



