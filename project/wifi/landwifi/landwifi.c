/*
 *  Description:  land wifi library
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landwifi.h"
#define WIFI_OBJECT_MAX 10



typedef struct country_set_st
{
    const char *country;
    int n;
    int a;
} country_set_t;
static country_set_t country_list[] =
{
    {"DB", 5, 7 },
    {"AL", 1, 0 },
    {"DZ", 1, 0},

    {"AR", 1, 3},
    {"AM", 1, 2},
    {"AU", 1, 0},

    {"AT", 1, 1},
    {"AZ", 1, 2},
    {"BH", 1, 0},

    {"BY", 1, 0},
    {"BE", 1, 1},
    {"BZ", 1, 4},

    {"BO", 1, 4},
    {"BR", 1, 1},
    {"BN", 1, 4},

    {"BG", 1, 1},
    {"CA", 0, 0},
    {"CL", 1, 0},

    {"CN", 1, 0},
    {"CO", 0, 0},
    {"CR", 1, 0},

    {"HR", 1, 2},
    {"CY", 1, 1},
    {"CZ", 1, 2},

    {"DK", 1, 1},
    {"DO", 0, 0},
    {"EC", 1, 0},

    {"EG", 1, 2},
    {"SV", 1, 0},
    {"EE", 1, 1},

    {"FI", 1, 1},
    {"FR", 1, 2},
    {"GE", 1, 2},

    {"DE", 1, 1},
    {"GR", 1, 1},
    {"GT", 0, 0},

    {"HN", 1, 0},
    {"HK", 1, 0},
    {"HU", 1, 1},

    {"IS", 1, 1},
    {"IN", 1, 0},
    {"ID", 1, 4},

    {"IR", 1, 4},
    {"IE", 1, 1},
    {"IL", 1, 0},

    {"IT", 1, 1},
    {"JP", 5, 9},
    {"JO", 1, 0},

    {"KZ", 1, 0},
    {"KP", 1, 5},
    {"KR", 1, 5},

    {"KW", 1, 0},
    {"LV", 1, 1},
    {"LB", 1, 1},

    {"LI", 1, 1},
    {"LT", 1, 1},
    {"LU", 1, 1},

    {"MO", 1, 0},
    {"MK", 1, 0},
    {"MY", 1, 0},

    {"MX", 0, 0},
    {"MC", 1, 2},
    {"MA", 1, 0},

    {"NL", 1, 1},
    {"NZ", 1, 0},
    {"NO", 0, 0},

    {"OM", 1, 0},
    {"PK", 1, 0},
    {"PA", 0, 0},

    {"PE", 1, 4},
    {"PH", 1, 4},
    {"PL", 1, 1},

    {"PT", 1, 1},
    {"PR", 0, 0},
    {"QA", 1, 0},

    {"RO", 1, 0},
    {"RU", 1, 0},
    {"SA", 1, 0},

    {"SG", 1, 0},
    {"SK", 1, 1},
    {"SI", 1, 1},

    {"ZA", 1, 1},
    {"ES", 1, 1},
    {"SE", 1, 1},

    {"CH", 1, 1},
    {"SY", 1, 0},
    {"TW", 1, 3},

    {"TH", 1, 0},
    {"TT", 1, 2},
    {"TN", 1, 1},

    {"TR", 1, 2},
    {"UA", 1, 0},
    {"AE", 1, 0},

    {"GB", 1, 1},
    {"US", 0, 7},
    {"UY", 1, 5},

    {"UZ", 0, 1},
    {"VE", 1, 5},
    {"VN", 1, 0},

    {"YE", 1, 0},
    {"ZW", 1, 0},
    {NULL, 0, 0},
};
static int a_band[12][30] =
{
    {36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 0},
    {52, 56, 60, 64, 149, 153, 157, 161, 0},
    {149, 153, 157, 161, 165, 0},
    {149, 153, 157, 161, 0},
    {36, 40, 44, 48, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165, 0},
    {52, 56, 60, 64, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165, 0}
};
static int n_band[8][20] =
{
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,0},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,0},
    {10, 11,0},
    {10, 11, 12, 13,0},
    {14,0},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,0},
    {3, 4, 5, 6, 7, 8, 9, 0},
    {5, 6, 7, 8, 9, 10, 11, 12, 13,0},
};
talk_t country2chlist( const char *country, int a )
{
    int i;
    talk_t ret;
    int *ch;
    country_set_t *p;
    char buf[NAME_MAX];

    i = 0;
    ch = NULL;
    for ( i=0;; i++ )
    {
        p = &country_list[i];
        if ( p->country == NULL )
        {
            return NULL;
        }
        if ( 0 == strcasecmp( p->country, country ) )
        {
            if ( a != 0 )
            {
                ch = a_band[p->a];
            }
            else
            {
                ch = n_band[p->n];
            }
            break;
        }
    }
    ret = json_create( NULL );
    if ( ch != NULL )
    {
        while( *ch != 0 )
        {
            snprintf( buf, sizeof(buf), "%d", *ch );
            json_set_value( ret, buf, json_create( NULL ) );
            ch++;
        }
    }
    return ret;
}



/* sdio ethernet dev find */
const char *sdioeth_device_find( const char *syspath, char *buf, int buflen )
{
    DIR *pdir;
	const char *ret;
    struct dirent *pent;
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
	snprintf( netpath, sizeof(netpath), "%s/net", syspath );
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
	return ret;
}
/* pci ethernet dev find */
const char *pcieth_device_find( const char *syspath, char *buf, int buflen )
{
    DIR *pdir;
	const char *ret;
    struct dirent *pent;
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
	snprintf( netpath, sizeof(netpath), "%s/net", syspath );
	pdir = opendir( netpath );
	if ( pdir != NULL )
	{
		while( NULL != ( pent = readdir( pdir ) ) )
		{
			if ( pent->d_name == NULL || *pent->d_name == '\0' || *pent->d_name == '.' )
			{
				continue;
			}
			if ( 0 != strncmp( pent->d_name, "wlan", 4 ) )
			{
				continue;
			}
			strncpy( buf, pent->d_name, buflen );
			ret = buf;
			break;
		}
		closedir( pdir );
	}
	return ret;
}



/* wifi object get */
const char *wifi_object_get( const char *radio, const char *syspath, talk_t cfg, char *buf, int buflen )
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

	/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
	/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
	/* XXXXXXXXX 在多种WIFI设备共享下需要加锁 XXXXXXX */
	/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
	/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
	/* search already bind */
	i = 0;
	while( i < WIFI_OBJECT_MAX )
	{
		if ( i == 0 )
		{
			snprintf( identify, sizeof(identify), "%s", radio );
		}
		else
		{
			snprintf( identify, sizeof(identify), "%s%d", radio, i+1 );
		}
		value = register_value( WIFI_PROJECT, identify );
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
	while( i < WIFI_OBJECT_MAX )
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
		value = register_value( WIFI_PROJECT, identify );
		if ( value == NULL || *value == '\0' )
		{
			object = identify;
			goto binding;
		}
		i++;
	}

	return NULL;
binding:
	h = register_open( WIFI_PROJECT, O_RDWR, 0644, 0, 0 );
	register_value_set( h, object, syspath, strlen(syspath)+1, REGISTER_VAR_SIZE );
	register_close( h );
found:
	strncpy( buf, object, buflen );
	return buf;
}
/* wifi object free */
void wifi_object_free( const char *object )
{
	register_file_t h;

	h = register_open( WIFI_PROJECT, O_RDWR, 0644, 0, 0 );
	register_value_set( h, object, NULL, 0, REGISTER_VAR_SIZE );
	register_close( h );
}



void wireless_11n_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *shortgi )
{
	int ch = 0;

	if ( channel != NULL )
	{
		ch = atoi( channel );
	}
	/* 20M */
    if ( bandwidth == NULL || 0 == strcmp( bandwidth, "20" ) )
    {
    	if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
            fprintf( fp, "[SHORT-GI-20]" );
		}
    }
	/* 40M */
	else
	{
	    if ( ch >=5 && ch <= 9 )
	    {
	        if ( ext != NULL && 0 == strcmp( ext, "below" ) )
	        {
	            fprintf( fp, "[HT40-]" );
	        }
	        else
	        {
	            fprintf( fp, "[HT40+]" );
	        }
	    }
	    else if ( ch < 5 )
	    {
			fprintf( fp, "[HT40+]" );
	    }
	    else if ( ch > 9 && ch < 14 )
	    {
			fprintf( fp, "[HT40-]" );
	    }
		/* GI */
		if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
			fprintf( fp, "[SHORT-GI-20][SHORT-GI-40]" );
		}
	}
}
void wireless_11a_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *shortgi )
{
	int ch = 0;

	if ( channel != NULL )
	{
		ch = atoi( channel );
	}
	/* 40M 80M 160M 80+80M */
	if ( bandwidth != NULL && ( 0 == strcmp( bandwidth, "40" ) || 0 == strcmp( bandwidth, "80" ) || 0 == strcmp( bandwidth, "160" ) ) )
	{
	    if ( ch == 40 || ch == 48 || ch == 56 || ch == 64
	            || ch == 104 || ch == 112 || ch == 120 || ch == 128
	            || ch == 136 || ch == 153 || ch == 161 )
	    {
			fprintf( fp, "[HT40-]" );
	    }
	    else if ( ch == 0 || ch == 36 || ch == 44 || ch == 52 || ch == 60
	            || ch == 100 || ch == 108 || ch == 116 || ch == 124
	            || ch == 132 || ch == 149 || ch == 157 )
	    {
			fprintf( fp, "[HT40+]" );
	    }
		/* GI */
		if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
			fprintf( fp, "[SHORT-GI-20][SHORT-GI-40]" );
		}
	}
	/* 20M */
	else if ( bandwidth != NULL && ( 0 == strcmp( bandwidth, "20" ) ) )
    {
    	if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
            fprintf( fp, "[SHORT-GI-20]" );
		}
    }
}
void wireless_11ac_adjustment( FILE *fp, const char *channel, const char *ext, const char *bandwidth, const char *country, const char *shortgi )
{
	int ch = 0;
	const char *idx = "";

	if ( channel != NULL )
	{
		ch = atoi( channel );
	}
    // 20M
    if ( bandwidth != NULL && 0 == strcasecmp( bandwidth, "20" ) )
    {
		fprintf( fp, "vht_oper_chwidth=0\n" );
		fprintf( fp, "vht_oper_centr_freq_seg0_idx=\n" );
		fprintf( fp, "vht_capab=" );
    }
	// 40M
    else if ( bandwidth != NULL && 0 == strcasecmp( bandwidth, "40" ) )
    {
		fprintf( fp, "vht_oper_chwidth=0\n" );
		if ( ch == 0 )
		{
			idx = "-2";
		}
		else if ( ch == 36 || ch == 40 )
		{
			idx = "38";
		}
		else if ( ch == 44 || ch == 48 )
		{
			idx = "46";
		}
		else if ( ch == 52 || ch == 56 )
		{
			idx = "54";
		}
		else if ( ch == 60 || ch == 64 )
		{
			idx = "62";
		}
		else if ( ch == 100 || ch == 104 )
		{
			idx = "102";
		}
		else if ( ch == 108 || ch == 112 )
		{
			idx = "110";
		}
		else if ( ch == 116 || ch == 120 )
		{
			idx = "118";
		}
		else if ( ch == 124 || ch == 128 )
		{
			idx = "126";
		}
		else if ( ch == 132 || ch == 136 )
		{
			idx = "134";
		}
		else if ( ch == 140 || ch == 144 )
		{
			idx = "142";
		}
		fprintf( fp, "vht_oper_centr_freq_seg0_idx=%s\n", idx );
		fprintf( fp, "vht_capab=" );
    }
    // 80M
    else if ( bandwidth != NULL && 0 == strcasecmp( bandwidth, "80" ) )
    {
		fprintf( fp, "vht_oper_chwidth=1\n" );
		if ( ch == 0 )
		{
			idx = "-6";
		}
		else if ( ch == 36 || ch == 40 || ch == 44 || ch == 48 )
		{
			idx = "42";
		}
		else if ( ch == 52 || ch == 56 || ch == 60 || ch == 64 )
		{
			idx = "58";
		}
		else if ( ch == 100 || ch == 104 || ch == 108 || ch == 112 )
		{
			idx = "106";
		}
		else if ( ch == 116 || ch == 120 || ch == 124 || ch == 128 )
		{
			idx = "122";
		}
		else if ( ch == 132 || ch == 136 || ch == 140 || ch == 144 )
		{
			idx = "138";
		}
		fprintf( fp, "vht_oper_centr_freq_seg0_idx=%s\n", idx );
		fprintf( fp, "vht_capab=" );
		/* GI */
		if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
			fprintf( fp, "[SHORT-GI-80]" );
		}
    }
	// 160M
    else if ( bandwidth != NULL && 0 == strcasecmp( bandwidth, "160" ) )
    {
		fprintf( fp, "vht_oper_chwidth=2\n" );
		if ( ch == 0 )
		{
			idx = "acs_survey";
		}
		else if ( ch == 36 || ch == 40 || ch == 44 || ch == 48 || ch == 52 || ch == 56 || ch == 60 || ch == 64 )
		{
			idx = "50";
		}
		else if ( ch == 100 || ch == 104 || ch == 108 || ch == 112 || ch == 116 || ch == 120 || ch == 124 || ch == 128 )
		{
			idx = "114";
		}
		else
		{
			idx = channel;
		}
		fprintf( fp, "vht_oper_centr_freq_seg0_idx=%s\n", idx );
		fprintf( fp, "vht_capab=" );
		/* GI */
		if ( shortgi != NULL && 0 == strcmp( shortgi, "enable" ) )
		{
			fprintf( fp, "[SHORT-GI-80][SHORT-GI-160]" );
		}
		else
		{
			fprintf( fp, "[VHT160]" );
		}
    }
}



