/*
 *  Description:  loop call back test
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"



talk_t _callfather( obj_t this, param_t param )
{
	printf( "callfather is called\n" );
    if ( com_sexist( "devtools@loopcall", "test" ) == true )
	{
    	printf( "devtools@loopcall.test exist\n" );
		scall( "devtools@loopcall", "test", NULL );
	}
	return NULL;
}

talk_t _test( obj_t this, param_t param )
{
	printf( "child test is called\n" );
	return NULL;
}
talk_t _loop( obj_t this, param_t param )
{
	printf( "child loop is called\n" );
	return NULL;
}
talk_t _loop2( obj_t this, param_t param )
{
	printf( "child loop2 is called\n" );
	return NULL;
}





