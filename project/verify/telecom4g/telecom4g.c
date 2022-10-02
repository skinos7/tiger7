/*
 *    Description:  component template
 *         Author:  devtools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "land/skin.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX       Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
COM_ID                String, component name, Name of the directory where this component resides
COM_IDPATH            String, Full name of a component in the system, PROJECT_ID@COM_ID
*/

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h				implementation of general linker list
syslog.h			log call implementation
talk.h				implementation of common communication data types
param.h 			implementation of parameter structure and related functions
path.h				implementation of structure and related functions for object path and attribute path
misc.h				miscellaneous function implementation
register.h			global register variable implementation
config.h			implementation function to get/set/list the config
project.h			provide unified project information operation interface for the system
com.h				implementation communication to other component function use talk structure or parameter structure
he2com.h			invokes the function implementation
reactor.h           libevent reactor assist functions
skinapi.h			define all the general component api
*/

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



#include "remotetalk/remotetalk.h"
/* 发起注册 */
boole telecom_cpe_register( const char *remote, const char *encrypt, talk_t lte )
{
	talk_t v;
	talk_t ack;
	const char *ptr;
	char buffer[NAME_MAX];
	talk_t machine_status;

	/* 创建注册用的JSON */
	v = json_create( NULL );
	#if 0
	json_set_string( v, "REGVER", "8.0" );
	json_set_string( v, "UETYPE", "23" );
	json_set_string( v, "MODEL", "ZYY-ES M9" );
	json_set_string( v, "OSVER", "FarmOS6.0" );
	json_set_string( v, "ROM", "8M" );
	json_set_string( v, "RAM", "64M" );
	json_set_string( v, "SWVER", " 4.3.3n" );
	json_set_string( v, "MACID", "4C:BC:98:01:06:98" );
	json_set_string( v, "IMEI1", "863078036821124" );
	json_set_string( v, "SIM1ICCID", "89861120147330291660" );
	json_set_string( v, "SIM1CDMAIMSI", "460115372165490" );
	json_set_string( v, "SIM1CELLID", "219769344" );
	#else
	/* 加入一些指定的属性 */
	json_set_string( v, "REGVER", "8.0" );
	json_set_string( v, "UETYPE", "23" );
	json_set_string( v, "MODEL", "ZYY-ES M9" );
	json_set_string( v, "OSVER", "SkinOS-7.0" );
	json_set_string( v, "ROM", "8M" );
	json_set_string( v, "RAM", "64M" );
	/* 调用land@machine的status接口获取设备信息, 并将version及mac加入到注册JSON中 */
	machine_status = scall( MACHINE_COM, "status", NULL );
	ptr = json_string( machine_status, "version" );
	json_set_string( v, "SWVER", ptr );
	ptr = json_string( machine_status, "mac" );
	json_set_string( v, "MACID", ptr );
	talk_free( machine_status );
	/* 将LTE相关的信息imei, iccid, imsi, cell加入到注册JSON中 */
	ptr = json_string( lte, "imei" );
	json_set_string( v, "IMEI1", ptr );
	ptr = json_string( lte, "iccid" );
	json_set_string( v, "SIM1ICCID", ptr );
	ptr = json_string( lte, "imsi" );
	json_set_string( v, "SIM1LTEIMSI", ptr );
	ptr = json_string( lte, "cell" );
	json_set_string( v, "SIM1CELLID", ptr );
	#endif
	/* 记录时间  */
    if ( system( "date '+%Y-%m-%d %H:%M:%S'>/tmp/.verify_telecom4g_regdate 2>/dev/null" ) == 0 )
    {
        ptr = file2string( "/tmp/.verify_telecom4g_regdate", buffer, sizeof(buffer) );
        if ( ptr != NULL && *ptr != '\0' )
        {
            buffer[ strlen(buffer)-1 ] = '\0';
			json_set_string( v, "REGDATE", buffer );
        }
    }

	/* 使用remotetalk库中的函数发起POST请求, 因此注意在组件的mconfig文件中加入remotetalk的链接, 并且也要加入curl库的链接(因为remotetalk用到了curl库) */
	ack = easy_rtalk_type( remote, "Content-Type:application/encrypted-json", encrypt, v, 30, 30, PROJECT_TMP_DIR"/.verify_telecom4g_cookie" );
	if ( ack == tfalse )
	{
		talk_free( v );
		return false;
	}
	talk_free( v );
	/* 确认注册是否成功 */
	ptr = json_string( ack, "resultCode" );
	if ( ptr == NULL || 0 != strcmp( ptr, "0" ) )
	{
		return false;
	}

	talk_free( v );
	return true;
}



/* usually used to initialize or deploy the component, is also usually registered in the project information file to be called at startup
 * This function can be called by the user at the he terminal, project@component.setup to call this function */
boole_t _setup( obj_t this, param_t param )
{
	/* 默认根据组件配置的status属性来决定是否启动_service服务函数 */
	talk_t cfg;
	const char *ptr;
	const char *object;

	/* 获取组件名全称 */
	object = obj_combine(this);
	/* 获取组件配置 */
	cfg = config_get( this, NULL );
	/* 得到组件配置的status属性值 */
	ptr = json_string( cfg, "status" );
	/* 判断值是否为enable */
	if ( ptr != NULL && 0 == strcmp( ptr, "enable") )
	{
		/* 将本组件的_service函数以服务的形式运行. 服务名即为当前的组件全称verify@telecom4g */
		sstart( object, "service", NULL, object );
	}

	/* 释放组件配置 */
	talk_free( cfg );
	/* 退出, 记住一定要运行后立即退出, 如果不退出可能导致整个系统启动卡死在这里 */
    return ttrue;
}

/* Typically used to shut down or exit this component, this method is called when the system shuts down, which can be registered in the project information file
 * This function can be called by the user at the he terminal, project@component.shut to call this function */
boole_t _shut( obj_t this, param_t param )
{
	const char *object;

	/* 获取组件名全称 */
	object = obj_combine(this);
	/* 停止服务 */
	sdelete( object );
	/* 退出, 记住一定要运行后立即退出, 如果不退出可能导致整个系统启动卡死在这里 */
	return ttrue;

}

/* Usually it is started as a service process in other functions, so it will always run, and if it exits the system it will restart it */
boole_t _service( obj_t this, param_t param )
{
	int i;
	boole r;
	int *iptr;
	talk_t ret;
	talk_t cfg;
	talk_t ifnamest;
	const char *object;
	const char *ifname;
	const char *remote;
	const char *encrypt;
	const char *cur_iccid;
	const char *save_iccid;

	/* 获取组件名全称 */
	object = obj_combine( this );
	/* 查看是否存在默认路由来确定数据业务是否可用 */
    do
    {
        if ( route_info( "0.0.0.0", NULL, NULL, NULL ) == true )
        {
            break;
        }
        if ( routes_info( DEFAULT_TABLE_NAME, "0.0.0.0", NULL, NULL, NULL ) > 0 )
        {
            break;
        }
		/* 如数据业务不可用, 休眠15秒后再试 */
        sleep( 5 );
    }while(1);
	/* 获取组件所有配置 */
	cfg = config_get( this, NULL );
	save_iccid = json_string( cfg, "iccid" );
	/* 获取 读哪一个LTE模块 的信息, 如无配置则默认为第一个LTE模块 */
	ifname = json_string( cfg, "lte" );
	if ( ifname == NULL || *ifname == '\0' )
	{
		ifname = LTE_COM;
	}
	/* 调用LTE模块的status接口来获取它的ICCID */
	ifnamest = scall( ifname, "status", NULL );
	cur_iccid = json_string( ifnamest, "iccid" );
	if ( cur_iccid == NULL || *cur_iccid == '\0' )
	{
		/* 如获到不到LTE的ICCID, 休眠10秒后退出, 退出后系统会重新运行此函数 */
		talk_free( ifnamest );
		sleep( 10 );
		return tfalse;
	}
	if ( save_iccid != NULL && 0 == strcmp( save_iccid, cur_iccid ) )
	{
		/* 不满足上报自注册信息触发条件,      注销系统服务并退出*/
		return terror;
	}

	ret = tfalse;
	/* 发起注册 */
	remote = json_string( cfg, "remote" );
	encrypt = json_string( cfg, "encrypt" );
	r = telecom_cpe_register( remote, encrypt, ifnamest );
	if ( r == false )
	{
		/* 对注册失败做计数 */
		iptr = register_pointer( object, "regfailed" );
		if ( iptr == NULL )
		{
			/* 初始化计数为1 */
			i = 1;
			iptr = register_set( object, "regfailed", &i, sizeof(i), 0 );
		}
		else
		{
			i = *iptr;
			/* 如果注册失败次数超过10次结束本次开机注册 */
			if ( i >= 10 )
			{
				ret = terror;
			}
			else
			{
				/* 计数加1 */
				i++;
				*iptr = i;
				/* 等待一个小时后再发起注册 */
				sleep( 3600 );
			}
		}
	}
	else
	{
		/* 注册成功后记录iccid, 并结束 */
		ret = ttrue;
		config_ssets_string( object, cur_iccid, "iccid" );
	}

	/* 释放空间并退回 */
	talk_free( cfg );
	talk_free( ifnamest );
	return ret;
}

/* Typically used for show the status */
talk_t _status( obj_t this, param_t param )
{
	int *iptr;
	talk_t ret;
	talk_t cfg;
	const char *ptr;
	const char *object;

	/* 获取组件名全称 */
	object = obj_combine( this );
	/* 创建返回 JSON */
	ret = json_create( NULL );
	/* 获取组件所有配置 */
	cfg = config_get( this, NULL );
	/* 如果在组件配置中发现有iccid的信息即表示已注册过 */
	ptr = json_string( cfg, "iccid" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		json_set_string( ret, "status", "registered" );
		json_set_string( ret, "iccid", ptr );
	}
	else
	{
		json_set_string( ret, "status", "unregistered" );
		/* 获到注册失败次数 */
		iptr = register_pointer( object, "regfailed" );
		json_set_number( ret, "failed", *iptr );
	}

	/* 释放空间并退回 */
	talk_free( cfg );
    return ret;
}

/* Typically used for event process */
talk_t _take( obj_t this, param_t param )
{
	const char *event;
	const char *object;

	/* 获取组件名全称 */
	object = obj_combine( this );
	/* 得到事件名称 */
	event = param_string( param, 1 );
	/* 如果事件名称为network/online(即外网连接上线)则立即重启组件的服务 */
	if ( event != NULL && 0 == strcmp( event, "network/online" ) )
	{
		sreset( object, "service", NULL, object );
		return ttrue;
	}
    return tfalse;
}



/* Functions that are triggered when viewing a component configuration are usually used to obtain the actual configuration and then calibrate and then return
 * This function can be called by the user at the he terminal, project@component to call this function */
talk_t _get( obj_t this, attr_t path )
{
    talk_t cfg;

    /* gets the configuration parameters for the component */
    cfg = config_get( this, path );

	info( "returns the configuration of the %s", COM_IDPATH );
    return cfg;
}
/* When you set a component parameter, you will be triggered to call this function, usually filtered by this function and then stored in the actual configuration
 * This function can be called by the user at the he terminal, project@component= to call this function */
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

    /* directly save the set parameters into the flash */
    ret = config_set( this, v, path );
    /* if the flash is successfully saved, the call is called by calling first _shut closing and then calling the _setup to restart the corresponding service */
    if ( ret == true )
    {
		info( "save the configuration of the %s and reset it", COM_IDPATH );
        _shut( this, NULL );
        _setup( this, NULL );
    }
    return ret;
}



