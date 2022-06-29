#ifndef H_MODEM_LANDWIFI_H
#define H_MODEM_LANDWIFI_H
/**
 * @brief wifi基带相关接口
 * @author dimmalex
 * @version 1.0
*/



#define SHELL_DEBUG
#ifdef SHELL_DEBUG
#define xshell( ... )  do \
	{ \
		debug( __VA_ARGS__ ); \
		shell( __VA_ARGS__ ); \
	} while( 0 )
#define xexecute( timeout, silent, ... ) do \
	{ \
		debug( __VA_ARGS__ ); \
		execute( timeout, silent, __VA_ARGS__ ); \
	} while( 0 )
#else
#define xshell( ... )                      shell( __VA_ARGS__ )
#define xexecute( timeout, silent, ... )   execute( timeout, silent, __VA_ARGS__ )
#endif



/* get the channel list */
talk_t country2chlist( const char *country, int a );

/* sdio netdev dev find */
const char *sdioeth_device_find( const char *syspath, char *buf, int buflen );
/* pci ethernet dev find */
const char *pcieth_device_find( const char *syspath, char *buf, int buflen );

/* wifi object get */
const char *wifi_object_get( const char *radio, const char *syspath, talk_t cfg, char *buf, int buflen );
/* wifi object free */
void wifi_object_free( const char *object );



void wireless_11n_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *gi );
void wireless_11a_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *gi );
void wireless_11ac_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *gi );



#endif   /* ----- #ifndef H_MODEM_LANDWIFI_H  ----- */

