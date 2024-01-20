/*
 *    Description:  project operation
 *          Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  HP
 */

#include "land/farm.h"
#include "land/project.h"



const char *_intro( obj_t this, param_t param )
{
    const char *str = \
"{"\
"\n    \"create[ project name ]\":\"create a project template\","\
"\n    \"delete[ project name ]\":\"delete the project\","\
"\n    \"add_init[ project name, init level, call ]\":\"register a component api at system init call\","\
"\n    \"add_uninit[ project name, uninit level, call ]\":\"register a component api at system shutdown call\","\
"\n    \"add_joint[ project name, joint level, call ]\":\"register a component api at joint cast\","\
"\n    \"add_object[ project name, joint object, com ]\":\"create a dynamic component depend on exist component\","\
"\n    \"add_wui[ project name, wui name ]\":\"create a ace web page template for project\","\
"\n    \"check[ project name ]\":\"check the a project json format\","\
"\n    \"pack[ project name ]\":\"pack the project into fpk\""\
\
"\n    \"list[ [project name] ]\":\"list project infomation\","\
"\n    \"wui_list\":\"list all the project webpage\","\
"\n    \"md_list\":\"list all the project com document\""\
"\n}\n";
    return str;
}
talk_t _create( obj_t this, param_t param )
{
	const char *name;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	if ( project_create( name, NULL ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _delete( obj_t this, param_t param )
{
	const char *name;
	char path[PATHNAME_SIZE];
	
	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	if ( project_path( name, path, sizeof(path) ) == NULL )
	{
		fprintf( stderr, "error: %s no exist\n", name );
		return tfalse;
	}
	shell( "rm -fr %s", path );
	project_flash();
	return ttrue;
}
talk_t _add_init( obj_t this, param_t param )
{
	const char *name;
	const char *level;
	const char *call;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	level = param_option( param, 2 );
	if ( level == NULL )
	{
		return tfalse;
	}
	call = param_option( param, 3 );
	if ( call == NULL )
	{
		return tfalse;
	}
	if ( project_add_init( name, NULL, level, call ) == false )
	{
		return tfalse;
	}
	return ttrue;
}

talk_t _add_uninit( obj_t this, param_t param )
{
	const char *name;
	const char *level;
	const char *call;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	level = param_option( param, 2 );
	if ( level == NULL )
	{
		return tfalse;
	}
	call = param_option( param, 3 );
	if ( call == NULL )
	{
		return tfalse;
	}
	if ( project_add_uninit( name, NULL, level, call ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _add_joint( obj_t this, param_t param )
{
	const char *name;
	const char *level;
	const char *call;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	level = param_option( param, 2 );
	if ( level == NULL )
	{
		return tfalse;
	}
	call = param_option( param, 3 );
	if ( call == NULL )
	{
		return tfalse;
	}
	if ( project_add_joint( name, NULL, level, call ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _add_object( obj_t this, param_t param )
{
	const char *name;
	const char *object;
	const char *com;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	object = param_option( param, 2 );
	if ( object == NULL )
	{
		return tfalse;
	}
	com = param_option( param, 3 );
	if ( com == NULL )
	{
		return tfalse;
	}
	if ( project_add_object( name, NULL, object, com ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _add_wui( obj_t this, param_t param )
{
	const char *name;
	const char *wuiname;
	char tpath[PATHNAME_SIZE];

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	if ( project_path( "template", tpath, sizeof(tpath) ) == NULL )
	{
		fprintf( stderr, "error: template no exist\n" );
		return tfalse;
	}
	wuiname = param_option( param, 2 );
	if ( wuiname == NULL )
	{
		return tfalse;
	}
	if ( project_add_wui( name, NULL, wuiname, tpath ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _check( obj_t this, param_t param )
{
	const char *name;

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	if ( project_check( name, NULL ) == false )
	{
		return tfalse;
	}
	return ttrue;
}
talk_t _pack( obj_t this, param_t param )
{
	const char *name;
	char cwd[PATHNAME_SIZE];
	char path[PATHNAME_SIZE];
	char fpkname[PATHNAME_SIZE];
    static const char *hardware; // register

	name = param_option( param, 1 );
	if ( name == NULL )
	{
		return tfalse;
	}
	if ( project_path( name, path, sizeof(path) ) == NULL )
	{
		fprintf( stderr, "error: %s no exist\n", name );
		return tfalse;
	}
	if ( strncmp( path, PROJECT_DIR, strlen(PROJECT_DIR) ) == 0 )
	{
		fprintf( stderr, "error: %s no exist\n", name );
		return tfalse;
	}
	/* set by arch/data.setup */
	if ( hardware == NULL )
	{
		hardware = reg_get( NULL, "hardware" );
	}
	cwd[0] = '\0';
	getcwd( cwd, sizeof(cwd) );
	if ( chdir( path ) != 0 )
	{
		fprintf( stderr, "error: cannot enter to project %s directory \n", name );
		return tfalse;
	}
	snprintf( fpkname, sizeof(fpkname), "%s/%s-%s-%s.fpk", PROJECT_TMP_DIR, name, PROJECT_DEFAULT_VERSION, hardware );
	shell( "tar zcf %s *", fpkname );
	if ( cwd[0] != '\0' )
	{
		chdir( cwd );
	}
	printf( "The packaging is located in this %s\n", fpkname );
	return ttrue;
}



talk_t _list( obj_t this, param_t param )
{
    int bytes;
    talk_t ret;
    talk_t axp;
    talk_t plist;
    talk_t value;
    talk_t prjinfo;
    const char *name;
    const char *path;
	const char *project;
    char tmp[PATHNAME_SIZE];

	project = param_option( param, 1 );
	if ( project != NULL )
	{
		char tpath[PATHNAME_SIZE];
		if ( project_path( project, tpath, sizeof(tpath) ) == NULL )
		{
			return NULL;
		}
		snprintf( tmp, sizeof(tmp), "%s/"PROJECT_INFOFILE, tpath );
		ret = json_load( tmp );
        json_set_string( ret, "path", tpath );
        bytes = size_get( tpath );
        json_set_number( ret, "size", bytes );
		return ret;
	}
    plist = project_list();
    ret = json_create();
    axp = NULL;
    while( NULL != ( axp = json_each( plist, axp ) ) )
    {
        name = axp_get_attr( axp );
        path = axp_get_string( axp );
        value = json_create();
        json_set_value( ret, name, value );
        json_set_string( value, "path", path );
        bytes = size_get( path );
        json_set_number( value, "size", bytes );
        /* get the project infomation */
        snprintf( tmp, sizeof(tmp), "%s/"PROJECT_INFOFILE, path );
        prjinfo = json_load( tmp );
        if ( prjinfo == NULL )
        {
            continue;
        }
        json_set_string( value, "intro", json_get_string( prjinfo, "intro" ) );
        json_set_string( value, "version", json_get_string( prjinfo, "version" ) );
        json_set_string( value, "author", json_get_string( prjinfo, "author" ) );
        talk_free( prjinfo );
    }
    talk_free( plist );

    return ret;
}
talk_t _wui_list( obj_t this, param_t param )
{
    talk_t ret;
    talk_t list;
    talk_t axp;
    talk_t info;
    talk_t cfg;
    const char *prjname;
    const char *prjpath;
    talk_t wui;
    talk_t wuiaxp;
    const char *appname;
    talk_t appvalue;
    talk_t lang;
    talk_t langaxp;
    const char *ptr;
    char tmp[PATHNAME_SIZE];
    const char *type;
    talk_t app;

    type = param_option( param, 1 );
    if ( type == NULL )
    {
        type = "wui";
    }
    app = config_sget( ABILITY_CONFIG, "project" );
    /* get the prj list */
    ret = json_create();
    list = project_list();
    axp = NULL;
    while( NULL != ( axp = json_each( list, axp ) ) )
    {
        prjname = axp_get_attr( axp );
        ptr = json_get_string( app, prjname );
        if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
        {
            continue;
        }
        prjpath = axp_get_string( axp );
        /* get the project infomation */
        snprintf( tmp, sizeof(tmp), "%s/%s", prjpath, PROJECT_INFOFILE );
        info = json_load( tmp );
        if ( info == NULL )
        {
            continue;
        }
        /* get the prj wui */
        wui = json_get_value(info, type );
        if ( wui == NULL )
        {
            talk_free( info );
            continue;
        }
        wuiaxp = NULL;
        while ( NULL != ( wuiaxp = json_each( wui, wuiaxp ) ) )
        {
            appname = axp_get_attr( wuiaxp );
            appvalue = axp_cut_value( wuiaxp );
            /* skip the need cfg */
            ptr = json_get_string( appvalue, "needcfg" );
            if ( ptr != NULL && *ptr != '\0' )
            {
                cfg = config_sget( ptr, NULL );
                if ( cfg == NULL )
                {
                    talk_free( appvalue );
                    continue;
                }
                talk_free( cfg );
            }
            /* make the app name */
            snprintf( tmp, sizeof(tmp), "%s_%s", prjname, appname );
            json_set_value( ret, tmp, appvalue );
            /* make the app href path */
            ptr = json_get_string( appvalue, "page" );
            if ( ptr != NULL && *ptr != '\0' )
            {
                snprintf( tmp, sizeof(tmp), "%s/%s", prjpath, ptr );
                json_set_string( appvalue, "page" , tmp );
            }
            /* make the lang json path */
            lang = json_get_value( appvalue, "lang" );
            while( NULL != ( langaxp = json_each( lang, langaxp ) ) )
            {
                ptr = axp_get_string( langaxp );
                snprintf( tmp, sizeof(tmp), "%s/%s", prjpath, ptr );
                axp_set_string( langaxp, tmp );
            }
        }
        talk_free( info );
    }
    talk_free( list );
    talk_free( app );
    return ret;
}
talk_t _md_list( obj_t this, param_t param )
{
    DIR *pdir;
    talk_t ret;
    talk_t app;
    talk_t list;
    talk_t axp;
    talk_t info;
	char *tok;
    const char *ptr;
    const char *prjname;
    const char *prjpath;
    char path[PATHNAME_SIZE];
    char name[PATHNAME_SIZE];
    struct dirent *pent;

    app = config_sget( ABILITY_CONFIG, "project" );
    /* get the prj list */
    ret = json_create();
    list = project_list();
    axp = NULL;
    while( NULL != ( axp = json_each( list, axp ) ) )
    {
        prjname = axp_get_attr( axp );
        ptr = json_get_string( app, prjname );
        if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
        {
            continue;
        }
        prjpath = axp_get_string( axp );
        /* get the project infomation */
        snprintf( path, sizeof(path), "%s/%s", prjpath, PROJECT_INFOFILE );
        info = json_load( path );
        if ( info == NULL )
        {
            continue;
        }
		talk_free( info );

		pdir = opendir( prjpath );
		if ( pdir != NULL )
		{
			while( NULL != ( pent = readdir( pdir ) ) )
			{
				if ( pent->d_name == NULL || *pent->d_name == '\0' || *pent->d_name == '.' )
				{
					continue;
				}
				tok = strstr( pent->d_name, ".md" );
				if ( tok == NULL || strlen( tok ) != 3 )
				{
					continue;
				}
				/* ok, add to ret talk */
				snprintf( path, sizeof(path), "%s%c%s", prjpath, '/', pent->d_name );
				snprintf( name, sizeof(name), "%s%c%s", prjname, OBJECT_GAP_ID, pent->d_name );
				tok = strstr( name, ".md" );
				*tok = '\0';
				json_set_string( ret, name, path );
			}
			closedir( pdir );
		}
    }
    talk_free( list );
    talk_free( app );
    return ret;
}



