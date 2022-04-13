/*
 *  Description:  loop call test
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



talk_t _callchild( obj_t this, param_t param )
{
	printf( "callchild is called\n" );
    if ( com_sexist( "devtools@loopcallback", "callfather" ) == true )
	{
    	printf( "devtools@loopcallback.callfather exist\n" );
		scall( "devtools@loopcallback", "callfather", NULL );
	}
	return NULL;
}


talk_t _test( obj_t this, param_t param )
{
	printf( "test is called\n" );
    if ( com_sexist( "devtools@loopcall", "loop" ) == true )
	{
    	printf( "devtools@loopcall.loop exist\n" );
		scall( "devtools@loopcall", "loop", NULL );
	}
	return NULL;
}
talk_t _loop( obj_t this, param_t param )
{
	printf( "loop is called\n" );
    if ( com_sexist( "devtools@loopcall", "loop2" ) == true )
	{
    	printf( "devtools@loopcall.loop2 exist\n" );
		scall( "devtools@loopcall", "loop2", NULL );
	}
	return NULL;
}
/*
call the child loop2 when comment it 
talk_t _loop2( obj_t this, param_t param )
{
	printf( "loop2 is called\n" );
	return NULL;
}
*/






