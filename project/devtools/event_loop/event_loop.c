/*
 * Description:  event loop exmple
 *      Author:  dimmalex (dim), dimmalex@gmail.com
 *     Company:  ASHYELF
 */

#include "land/skin.h"



talk_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;

	/* 获到配置 */
    cfg = config_sget( COM_IDPATH, NULL );
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
    {
		talk_free( cfg );
		return tfalse;
    }

	/* 启动服务 */
	service_start( COM_IDPATH, COM_IDPATH, "service", NULL );
    talk_free( cfg );
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
	/* 关闭服务 */
    service_stop( COM_IDPATH );
    return ttrue;
}



/* 与控制端交互处理 */
talk_t control_accept( const char *cmd, talk_t value, evloop_t evl )
{
	talk_t ret;
	const char *order;

	if ( 0 == strcmp( cmd, "status" ) )
	{
		if ( evl->object_data )
		{
			ret = talk_dup( evl->object_data );
		}
		else
		{
			ret = json_create( NULL );
			json_set_string( ret, "object", evl->object );
		}
	}
	else if ( 0 == strcmp( order, "set_status" ) )
	{
		if ( value != NULL )
		{
			evl->object_data = talk_dup( value );
		}
		else
		{
			talk_free( evl->object_data );
			evl->object_data = NULL;
		}
		ret = ttrue;
	}

	return ret;
}
talk_t _service( obj_t this, param_t param )
{
	talk_t cfg;
	evloop_t evl;
	const char *object;

	/* 获取配置 */
	object = obj_combine( this );
	cfg = config_sget( this, NULL );

	/* 初始化evloop */
	evl = evloop_create( object, NULL );
	if ( evl != NULL )
	{
		/* 收到SIGTERM退出dispatch */
		evloop_signal_break( evl, SIGTERM );
		/* 收到SIGINT退出dispatch */
		evloop_signal_break( evl, SIGINT );

		/* 注意针对控制端的处理函数 */
		evloop_control_create( evl, control_accept );

	    /* 进入循环 */
	    evloop_loop( evl );

		/* 退出 */
		evloop_free( evl );
	}
	else
	{
		fault( "%s evloop_init error", object );
	}

	/* 退出 */
	warn( "%s exit", object );
	talk_free( cfg );
    return tfalse;
}
talk_t _status( obj_t this, param_t param )
{
	/* 向service发送状态请求 */
	return evloop_control( this, "status", NULL, 3 );
}
talk_t _set_status( obj_t this, param_t param )
{
	talk_t ask;
	talk_t ret;

	/* 获取配置 */
	ask = json_create( NULL );
	json_set_string( ask, "test", "testvalue" );
	/* 向service发送设置的状态请求 */
	ret = evloop_control( this, "set_status", ask, 3 );
	talk_free( ask );
	/* 返回 */
	return ret;
}



