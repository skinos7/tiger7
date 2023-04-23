/*
 *    Description:  command component call depend on sline
 * 		Author:  dimmalex (dim), dimmalex@gmail.com
 * 	   Company:  ASHYELF
 */
#include "land/skin.h"
#include "sline.h"



int main( int argc, const char **argv )
{
	int i;
	int sline_stat;
	struct stat st;
	char buf[65535];
	const char *entry;

	sline_hist_entry_size = 64;
	if ( sline_setup() < 0 )
	{
		fprintf(stderr, "sline: %s\n", sline_errmsg());
		return -1;
	}
	if ( stat( "/etc/banner.he", &st ) == 0 )
	{
		shell( "cat /etc/banner.he" );
	}

	i = 0;
	while ( feof(stdin) == 0 )
	{
		sline_set_prompt( "# " );
		if ( ( sline_stat = sline( buf, 65535, NULL ) ) < 0 )
		{
			goto exit;
		}
		if ( strncmp( buf, "hist", 4 ) == 0 )
		{
			printf("History contains:\n");
			for ( i = 0; ( entry = sline_history_get(i) ) != NULL; ++i )
			{
				if ( strlen( entry ) > 0 ) /* Avoid printing "current blank" */
				{
					printf("%s\n", entry);
				}
			}
			continue;
		}
		else if ( 0 == strncmp( buf, "elf", 3 ) )
		{
			/* open the new sessoin */
			execl( "/bin/ash", "/bin/ash", "--login", NULL );
		}
		i = string_he_command( buf );
	}

exit:
    return i;
}



