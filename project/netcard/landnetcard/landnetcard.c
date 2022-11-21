/*
 *    Description:  land netcard library
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/skin.h"
#include "landnetcard.h"
#define NIC_OBJECT_MAX 10



/* usb ethernet device name find, return device name, return NULL for nofound */
const char *usbeth_device_find( const char *syspath, char *buf, int buflen )
{
    DIR *pdir;
	struct stat st;
	const char *ret;
    struct dirent *pent;
	char path[PATH_MAX];
	char netpath[PATH_MAX];
	static char buffer[NAME_MAX];

	if ( syspath == NULL )
	{
		return NULL;
	}
	ret = NULL;
	if ( buf == NULL || buflen < 0 )
	{
		buf = buffer;
		buflen = sizeof(buffer);
	}
	netpath[0] = '\0';
    pdir = opendir( syspath );
    if ( pdir != NULL )
    {
        while( NULL != ( pent = readdir( pdir ) ) )
        {
            if ( pent->d_name == NULL || *pent->d_name == '\0' || *pent->d_name == '.' )
            {
                continue;
            }
			snprintf( path, sizeof(path), "%s/%s", syspath, pent->d_name );
			if ( stat( path, &st ) != 0 || S_ISDIR( st.st_mode ) == 0 )
			{
				continue;
			}
			snprintf( path, sizeof(path), "%s/%s/net", syspath, pent->d_name );
			if ( stat( path, &st ) != 0 || S_ISDIR( st.st_mode ) == 0 )
			{
				continue;
			}
			strncpy( netpath, path, sizeof(netpath) );
			break;
        }
        closedir( pdir );
    }
	if ( netpath[0] != '\0' )
	{
		pdir = opendir( netpath );
		if ( pdir != NULL )
		{
			while( NULL != ( pent = readdir( pdir ) ) )
			{
				if ( pent->d_name == NULL || *pent->d_name == '\0' || *pent->d_name == '.' )
				{
					continue;
				}
				strncpy( buf, pent->d_name, buflen );
				ret = buf;
				break;
			}
			closedir( pdir );
		}
	}
	return ret;
}



/* nic object get */
const char *nic_object_get( const char *radio, const char *syspath, talk_t cfg, char *buf, int buflen )
{
	int i;
	talk_t axp;
	talk_t caxp;
	talk_t list;
	const char *value;
	const char *ptr;
	const char *object;
	register_file_t h;
	char identify[NAME_MAX];
	static char buffer[NAME_MAX];

	if ( syspath == NULL )
	{
		return NULL;
	}
	if ( buf == NULL || buflen < 0 )
	{
		buf = buffer;
		buflen = sizeof(buffer);
	}

	/* search already bind */
	i = 0;
	while( i < NIC_OBJECT_MAX )
	{
		if ( i == 0 )
		{
			snprintf( identify, sizeof(identify), "%s", radio );
		}
		else
		{
			snprintf( identify, sizeof(identify), "%s%d", radio, i+1 );
		}
		value = register_value( NETCARD_PROJECT, identify );
		if ( value != NULL && 0 == strcmp( syspath, value ) )
		{
			object = identify;
			goto found;
		}
		i++;
	}

	/* search the config bind */
	axp = NULL;
	while( NULL != ( axp = json_each( cfg, axp ) ) )
	{
		object = axp_id( axp );
		list = json_value( axp_value( axp ), "syspath" );
		if ( json_check( list ) == true )
		{
			caxp = NULL;
			while( NULL != ( caxp = json_each( list, caxp ) ) )
			{
				ptr = axp_string( caxp );
				if ( 0 == strcmp( ptr, syspath ) )
				{
					goto binding;
				}
			}
		}
		else
		{
			ptr = x_text( list );
			if ( ptr != NULL && 0 == strcmp( ptr, syspath ) )
			{
				goto binding;
			}
		}
	}

	/* search the for new bind */
	i = 0;
	while( i < NIC_OBJECT_MAX )
	{
		if ( i == 0 )
		{
			snprintf( identify, sizeof(identify), "%s", radio );
		}
		else
		{
			snprintf( identify, sizeof(identify), "%s%d", radio, i+1 );
		}
		if ( json_value( json_value( cfg, identify ), "syspath" ) != NULL )
		{
			continue;
		}
		value = register_value( NETCARD_PROJECT, identify );
		if ( value == NULL || *value == '\0' )
		{
			object = identify;
			goto binding;
		}
		i++;
	}

	return NULL;
binding:
	h = register_open( NETCARD_PROJECT, O_RDWR, 0644, 0, 0 );
	register_value_set( h, object, syspath, strlen(syspath)+1, REGISTER_VAR_SIZE );
	register_close( h );
found:
	strncpy( buf, object, buflen );
	return buf;
}
/* lte object free */
void nic_object_free( const char *object )
{
	register_file_t h;

	h = register_open( NETCARD_PROJECT, O_RDWR, 0644, 0, 0 );
	register_value_set( h, object, NULL, 0, REGISTER_VAR_SIZE );
	register_close( h );
}



