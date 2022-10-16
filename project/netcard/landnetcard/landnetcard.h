#ifndef H_NETCARD_LANDNETCARD_H
#define H_NETCARD_LANDNETCARD_H
/**
 * @brief netcard相关的基本接口
 * @author dimmalex
 * @version 1.0
*/



const char *usbeth_device_find( const char *syspath, char *buf, int buflen );



/* lte object get */
const char *nic_object_get( const char *radio, const char *syspath, talk_t cfg, char *buf, int buflen );
/* lte object free */
void        nic_object_free( const char *object );



#endif   /* ----- #ifndef H_NETCARD_LANDNETCARD_H  ----- */
