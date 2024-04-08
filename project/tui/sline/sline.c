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
		line = readline("# ");
		if ( line == NULL || isprint( *line ) == 0 )
		{
			continue;
		}
		if ( *line == '\0' || *line == '\r' || *line == '\n' )
		{
			free( line );
			continue;
		}
		if ( 0 == strncmp( line, "elf", 3 ) )
		{
			/* open the new sessoin */
			execl( "/bin/ash", "/bin/ash", "--login", NULL );
			break;
		}
		else if ( 0 == strncmp( line, "exit", 4 ) )
		{
			break;
		}
		add_history( line );
		string_he_command( line );
		free( line );
	}

	return 0;
}



