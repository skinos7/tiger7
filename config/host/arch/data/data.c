/*
 *    Description:  x86 host platform configure management
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/farm.h"



talk_t _setup( obj_t this, param_t param )
{
	struct stat st;

    /* create the project tmp directory */
    if ( access( PROJECT_TMP_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_TMP_DIR, 0777 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project var directory */
    if ( access( PROJECT_VAR_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_VAR_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project register directory */
    if ( access( PROJECT_REG_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_REG_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project config directory */
    if ( access( PROJECT_CFG_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_CFG_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project com directory */
    if ( access( PROJECT_COM_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_COM_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project libarary directory */
    if ( access( PROJECT_LIB_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_LIB_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project execute directory */
    if ( access( PROJECT_BIN_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_BIN_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project service directory */
    if ( access( PROJECT_SERV_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_SERV_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the project internal directory */
    if ( access( PROJECT_INTERNAL_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_INTERNAL_DIR, 0775 ) != 0 )
        {
            return tfalse;
        }
    }

    /* globe value setttings */
    reg_set( NULL, "platform", gPLATFORM, sizeof(gPLATFORM) );
    reg_set( NULL, "hardware", gHARDWARE, sizeof(gHARDWARE) );
    reg_set( NULL, "custom", gCUSTOM, sizeof(gCUSTOM) );
    reg_set( NULL, "scope", gSCOPE, sizeof(gSCOPE) );
    reg_set( NULL, "version", gVERSION, sizeof(gVERSION) );
    reg_set( NULL, "publish", gPUBLISH, sizeof(gPUBLISH) );

    /* mount the mtd dir of config */
	if ( stat( PROJECT_CFG_DIR"/"FORMAT_FLAG_FILE, &st ) == 0 )
    {
        ainfo( "clear the config partition" );
		shell( "rm -fr	%s/*", PROJECT_CFG_DIR );
		shell( "rm -fr	%s/.*", PROJECT_CFG_DIR );
    }
	if ( stat( PROJECT_INTERNAL_DIR"/"FORMAT_FLAG_FILE, &st ) == 0 )
	{
		ainfo( "clear the interval partition" );
		shell( "rm -fr	%s/*", PROJECT_INTERNAL_DIR );
		shell( "rm -fr	%s/.*", PROJECT_CFG_DIR );
	}

    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    shell( "sync" );
    return ttrue;
}



talk_t _get( obj_t this, path_t path )
{
    int lv;
    talk_t ret;
    talk_t cfg;
    char buffer[IDENTIFY_SIZE];

    ret = NULL;
    lv = path_level( path );
    if ( lv == 1 )
    {
        cfg = _get( this, NULL );
        ret = talk_cut( cfg, path );
        talk_free( cfg );
        return ret;
    }

    ret = json_create();
    /* cfgversion */
    memset( buffer, 0, sizeof(buffer) );
    read_string( PROJECT_CFG_DIR"/version", buffer, sizeof(buffer) );
    if ( *buffer != '\0' )
    {
        json_set_string( ret, "cfgversion", buffer );
    }
    /* group version */
    memset( buffer, 0, sizeof(buffer) );
    read_string( PROJECT_CFG_DIR"/gpversion", buffer, sizeof(buffer) );
    if ( *buffer != '\0' )
    {
        json_set_string( ret, "gpversion", buffer );
    }
    /* magic */
    memset( buffer, 0, sizeof(buffer) );
    read_string( PROJECT_DIR"/magic", buffer, sizeof(buffer) );
    if ( *buffer != '\0' )
    {
        json_set_string( ret, "magic", buffer );
    }
    /* language */
    memset( buffer, 0, sizeof(buffer) );
    read_string( PROJECT_DIR"/language", buffer, sizeof(buffer) );
    if ( *buffer != '\0' )
    {
        json_set_string( ret, "language", buffer );
    }
	else
	{
        json_set_string( ret, "language", "cn" );
	}
    /* model */
    memset( buffer, 0, sizeof(buffer) );
    read_string( PROJECT_DIR"/model", buffer, sizeof(buffer) );
    if ( *buffer != '\0' )
    {
        json_set_string( ret, "model", buffer );
    }
	else
	{
        strncpy( buffer, gCUSTOM, sizeof(buffer) );
        lowtoupp( buffer );
        json_set_string( ret, "model", buffer );
	}
 
    return ret;
}
boole _set( obj_t this, path_t path, talk_t v )
{
    talk_t cfg;
    boole ret;
    boole def;
    const char *ptr;
    const char *old;

    if ( path_level( path ) == 1 )
    {
        cfg = json_create();
        json_set_value( cfg, path_get( path, 1 ), talk_dup(v) );
        ret = _set( this, NULL, cfg );
        talk_free( cfg );
        return ret;
    }

    ret = false;
    def = false;
    cfg = _get( this, NULL );
    /* cfgversion */
    ptr = json_get_string( v, "cfgversion" );
    if ( ptr != NULL )
    {
		old = json_get_string( cfg, "cfgversion" );
        if ( *ptr == '\0' && old != NULL )
        {
            unlink( PROJECT_CFG_DIR"/version" );
            ret = true;
        }
        else if ( old == NULL || ( ptr != NULL && 0 != strcmp( old, ptr ) ) )
        {
			ret = true;
            write_string( PROJECT_CFG_DIR"/version", ptr );
        }
    }
    /* group version */
    ptr = json_get_string( v, "gpversion" );
    if ( ptr != NULL )
    {
		old = json_get_string( cfg, "gpversion" );
        if ( *ptr == '\0' && old != NULL )
        {
            unlink( PROJECT_CFG_DIR"/gpversion" );
            ret = true;
        }
        else if ( old == NULL || ( ptr != NULL && 0 != strcmp( old, ptr ) ) )
        {
			ret = true;
            write_string( PROJECT_CFG_DIR"/gpversion", ptr );
        }
    }
    /* magic */
    ptr = json_get_string( v, "magic" );
    if ( ptr != NULL )
    {
		old = json_get_string( cfg, "magic" );
        if ( *ptr == '\0' && old != NULL )
        {
            unlink( PROJECT_DIR"/magic" );
            ret = true;
        }
        else if ( old == NULL || ( ptr != NULL && 0 != strcmp( old, ptr ) ) )
        {
			ret = true;
            write_string( PROJECT_DIR"/magic", ptr );
        }
    }
    /* language */
    ptr = json_get_string( v, "language" );
    if ( ptr != NULL )
    {
		old = json_get_string( cfg, "language" );
        if ( *ptr == '\0' && old != NULL )
        {
            unlink( PROJECT_DIR"/language" );
            ret = true;
        }
        else if ( old == NULL || ( ptr != NULL && 0 != strcmp( old, ptr ) ) )
        {
			ret = true;
            write_string( PROJECT_DIR"/language", ptr );
        }
    }
    /* model */
    ptr = json_get_string( v, "model" );
    if ( ptr != NULL )
    {
		old = json_get_string( cfg, "model" );
        if ( *ptr == '\0' && old != NULL )
        {
            unlink( PROJECT_DIR"/model" );
            ret = true;
        }
        else if ( old == NULL || ( ptr != NULL && 0 != strcmp( old, ptr ) ) )
        {
			ret = true;
            write_string( PROJECT_DIR"/model", ptr );
        }
		def = true;
    }


    if ( def == true )
    {
        ainfo( "data default now in passing" );
		shell( "touch %s", PROJECT_CFG_DIR"/"FORMAT_FLAG_FILE );
    }
    talk_free( cfg );

    return ret;
}



talk_t _custom( obj_t this, param_t param )
{
    talk_t v;
    talk_t axp;
    talk_t data;
    talk_t custom;
    const char *ptr;
    struct stat st;
    const char *model;
    const char *lang;
    boole defaultstart;
    int old_config_right;
    static int *config_right;
    char buffer[IDENTIFY_SIZE];

    /* get the data */
    data = _get( this, NULL );
    if ( data == NULL )
    {
        return tfalse;
    }
    model = json_get_string( data, "model" );
    lang = json_get_string( data, "language" );
    reg_set( NULL, "model", model, strlen(model)+1 );

    /* set the config_right to 1 for config work ok */
    config_right = reg_get( NULL, "config_right" );
    if ( config_right != NULL )
    {
        old_config_right = *config_right;
        *config_right = 1;
    }
    else
    {
        old_config_right = 1;
        config_right = reg_set( NULL, "config_right", &old_config_right, sizeof(old_config_right) );
		old_config_right = 0;
    }

    /* fix the hostname and ssid when fix not run */
    defaultstart = false;
    if ( stat( PROJECT_CFG_DIR"/"CONFIG_CUSTOM_FILE, &st ) != 0 )
    {
        defaultstart = true;
        write_string( PROJECT_CFG_DIR"/"CONFIG_CUSTOM_FILE, "setup" );

        /* device model ability custom */
        {
            v = config_sget( ABILITY_CONFIG, NULL );
            custom = json_get_value( v, model );
			if ( custom )
			{
	            json_modify( custom, v );
	            config_sset( ABILITY_CONFIG, NULL, v );
			}
            talk_free( v );
        }

        /* device model custom for release*/
        {
            talk_t x;
			snprintf( buffer, sizeof(buffer), "arch@%s", model );
            v = config_sget( buffer, NULL );
            axp = NULL;
            while( NULL != ( axp = json_each( v, axp )  ) )
            {
                ptr = axp_get_attr( axp );
                custom = axp_get_value( axp );
                if ( phrase_check( custom ) == true )
                {
                    config_sset( ptr, NULL, NULL );
                }
                else
                {
                    x = config_sget( ptr, NULL );
                    if ( x == NULL )
                    {
                        x = json_create();
                    }
                    json_modify( custom, x );
                    config_sset( ptr, NULL, x );
                    talk_free( x );
                }
            }
            talk_free( v );
        }
        /* device model custom for add */
    	if ( stat( PROJECT_DEFCONFIG_DIR, &st ) == 0 )
		{
			shell( "cp -r %s/* %s/", PROJECT_DEFCONFIG_DIR, PROJECT_CFG_DIR );
		}

         /* lang set */
        if ( lang != NULL )
        {
			v = config_sget( "clock@date", NULL );
            if ( v != NULL )
            {
                ptr = config_sgets( "clock@date", "timezone", NULL, 0 );
                if ( 0 == strcmp( lang, "cn") )
                {
                    if ( ptr == NULL || *ptr == '\0' )
                    {
                        config_ssets( "clock@date", "timezone", "8" );
                    }
                }
                else if ( 0 == strcmp( lang, "th" ) )
				{
                    if ( ptr == NULL || *ptr == '\0' )
                    {
                        config_ssets( "clock@date", "timezone", "7" );
                    }
				}
				else
                {
                    if ( ptr == NULL || *ptr == '\0' )
                    {
                        config_ssets( "clock@date", "timezone", "-4" );
                    }
                }
                talk_free( v );
            }
        }
    }

    /* default the configure version if fix work */
    if ( defaultstart == true )
    {
        unlink( PROJECT_CFG_DIR"/version" );
    }

    /* set the config_right to 0 for crack default */
    if ( config_right != NULL )
    {
        *config_right = old_config_right;
    }
    talk_free( data );
    return ttrue;
}
talk_t _default( obj_t this, param_t param )
{
	talk_t lockcfg;
	const char *ptr;

	/* lock default work */
	lockcfg = config_sget( LOCK_COM, NULL );
	ptr = json_get_string( lockcfg, "default" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		talk_free( lockcfg );
		awarn( "cannot default beacuse the default function is locked" );
		errno = EPERM;
		return tfalse;
	}
	talk_free( lockcfg );
    ainfo( "data default now" );
	shell( "touch %s", PROJECT_CFG_DIR"/"FORMAT_FLAG_FILE );
	shell( "rm -fr %s", PROJECT_CFG_DIR"/.bornv6" );   /* old 4.3.3 version */
	shell( "rm -fr %s", PROJECT_CFG_DIR"/fix" );       /* old 4.3.2 version */
    shell( "sync" );
    return ttrue;
}
talk_t _backup( obj_t this, param_t param )
{
	obj_t obj;
    talk_t ret;
	talk_t axp;
	talk_t nobackup;
	talk_t lockcfg;
	const char *ptr;
    char file[PATHNAME_SIZE];
    char filepath[PATHNAME_SIZE];
	const char *tmpbackup = "/tmp/.backup";

	/* lock default work */
	lockcfg = config_sget( LOCK_COM, NULL );
	ptr = json_get_string( lockcfg, "backup" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		talk_free( lockcfg );
		awarn( "cannot backup beacuse the backup function is locked" );
		errno = EPERM;
		return NULL;
	}
	talk_free( lockcfg );
    /* cd PROJECT_TMP_DIR/config; tar -c * -f ../ss */
    ret = NULL;
	shell( "rm -fr %s", tmpbackup );
	shell( "cp -r %s %s", PROJECT_CFG_DIR, tmpbackup );
    if ( chdir( tmpbackup ) == 0 )
    {
    	axp = NULL;
		nobackup = config_sget( CONFIG_COM, "nobackup" );
		while ( NULL != ( axp = json_each( nobackup, axp ) ) )
		{
			ptr = axp_get_attr( axp );
			obj = obj_create( ptr );
			if ( obj == NULL )
			{
				continue;
			}
			snprintf( file, sizeof(file), "%s/%s/%s%s", tmpbackup, obj_prj(obj), obj_name(obj), CFGFILE_POSTFIX );
			unlink( file );
			obj_free( obj );
		}
		talk_free( nobackup );
		ainfo( "data backup now" );
		snprintf( filepath, sizeof(filepath), PROJECT_TMP_DIR"/config_backup_%d", getpid() );
        if ( 0 == shell( "tar -c * -f %s", filepath ) )
		{
			ret = json_create();
			json_set_string( ret, "path", filepath );
		}
    }
    return ret;
}
talk_t _restore( obj_t this, param_t param )
{
    talk_t ret;
    struct stat st;
	talk_t lockcfg;
	const char *ptr;
    const char *filepath;

	/* lock default work */
	lockcfg = config_sget( LOCK_COM, NULL );
	ptr = json_get_string( lockcfg, "restore" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		talk_free( lockcfg );
		awarn( "cannot restore beacuse the restore function is locked" );
		errno = EPERM;
		return tfalse;
	}
	talk_free( lockcfg );
	ret = tfalse;
    /* cd PROJECT_TMP_DIR/config; tar -x  -f ../ss */
    filepath = param_option( param, 1 );
    if ( filepath == NULL )
    {
		errno = EINVAL;
        return tfalse;
    }
    if ( stat( filepath, &st ) != 0 )
    {
		errno = EINVAL;
        return tfalse;
    }
    if ( chdir( PROJECT_CFG_DIR ) != 0 )
    {
        return tfalse;
    }
    ainfo( "data restore now" );
    if ( shell( "tar -x -f %s", filepath ) == 0 )
    {
    	ret = ttrue;
    }
    shell( "sync" );
    return ret;
}
talk_t _factory( obj_t this, param_t param )
{
    struct stat st;

    ainfo( "data factory now" );
    if ( stat( PROJECT_INTERNAL_DIR, &st ) == 0 )
    {
        shell( "touch %s", PROJECT_INTERNAL_DIR"/"FORMAT_FLAG_FILE );
    }
    return _default( this, param );
}



