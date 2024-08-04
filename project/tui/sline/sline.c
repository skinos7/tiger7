/*
 *    Description:  command component call depend on sline
 * 		Author:  dimmalex (dim), dimmalex@gmail.com
 * 	   Company:  ASHYELF
 */
#include "land/skin.h"
#include <readline/readline.h>
#include <readline/history.h>



int main( int argc, const char **argv )
{
	char *line;
	struct stat st;

	if ( stat( "/etc/banner.he", &st ) == 0 )
	{
		shell( "cat /etc/banner.he" );
	}
	while (1)
	{
		#if defined (gSCOPE__sx) || defined (gSCOPE__sr) || defined (gSCOPE__gg) || defined (gSCOPE__gs) || defined (gSCOPE__ok) || defined (gSCOPE__oled) || defined (gSCOPE__base) || defined (gSCOPE__rover)
		line = readline("# ");
		#else
		line = readline("$ ");
		#endif
		if ( line == NULL )
		{
			continue;
		}
		if ( isprint( *line ) == 0 || *line == '\0' || *line == '\r' || *line == '\n' )
		{
			free( line );
			continue;
		}
		if ( 0 == strncmp( line, "exit", 4 ) )
		{
			free( line );
			break;
		}
		else if ( 0 == strncmp( line, "ashy", 4 ) )
		{
			free( line );
			execl( "/bin/ash", "/bin/ash", "--login", NULL ); // open the new sessoin
			break;
		}
		else if ( 0 == strncmp( line, "set ", 4 ) )
		{
			talk_t v;
			talk_t cfg;
			char *gap;
			const char *attr;
			const char *value;
			const char *object;
			char prompt[NAME_MAX];

			object = line+4;
			if ( object == '\0' )
			{
				free( line );
				continue;
			}
			cfg = sget( object, NULL );
			talk_print( cfg );
			add_history( line );
			snprintf( prompt, sizeof(prompt), "%s: ", object );
			while (1)
			{
				line = readline( prompt );
				if ( line == NULL )
				{
					continue;
				}
				if ( isprint( *line ) == 0 || *line == '\0' || *line == '\r' || *line == '\n' )
				{
					free( line );
					continue;
				}
				gap = strstr( line, "=" );
				if ( gap == NULL )
				{
					if ( 0 == strcmp( line, "e" ) )      // exit
					{
						break;
					}
					else if ( 0 == strcmp( line, "s" ) ) // set
					{
						if ( sset( object, cfg, NULL ) == true )
						{
							printf( "ttrue\n" );
						}
						else
						{
							printf( "tfalse\n" );
						}
						talk_free( cfg );
						break;
					}
					else if ( 0 == strcmp( line, "g" ) ) // get
					{
						talk_print( cfg );
					}
					else
					{
						v = attr_gets( cfg, line );
						if ( v != NULL )
						{
							talk_print( v );
						}
					}
				}
				else
				{
					attr = line;
					value = gap+1;
					*gap = '\0';
					if ( *value == '\0' )
					{
						value = NULL;
					}
					attr_sets_string( cfg, value, attr );
				}
				add_history( line );
				free( line );
			}
		}
		else if ( 0 == strncmp( line, "arp ", 4 )
			|| 0 == strncmp( line, "ping ", 5 )
			|| 0 == strncmp( line, "traceroute", 10 ) 
			|| 0 == strncmp( line, "ifconfig", 8 )
			|| 0 == strncmp( line, "route", 5 )
			|| 0 == strncmp( line, "netstat", 7 )
			|| 0 == strncmp( line, "iperf", 5 )
			|| 0 == strncmp( line, "tcpdump", 7 )
			|| 0 == strncmp( line, "mkdir ", 6 )
			|| 0 == strncmp( line, "telnet ", 7 )
			|| 0 == strncmp( line, "wg ", 3 )
			)
		{
			add_history( line );
			shell( line );
		}
		else
		{
			add_history( line );
			string_he_command( line );
		}
		free( line );
	}

	return 0;
}



