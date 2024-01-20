/*
 * Description:  make project
 * 		Author:  dimmalex (dim), dimmalex@gmail.com
 * 	   Company:  ASHYELF
 */
#include "land/skin.h"
#define TEMPLATE_PATH "/prj/tmptools"



static boole project_create_json( const char *name, const char *apppath )
{
	FILE *fp;
	talk_t info;
	char intro[LINE_MAX+1];
	char prjpath[PATH_MAX+1];
	char prjinfo[PATH_MAX+1];

	if ( name == NULL )
	{
		fprintf( stderr, "error: argument lack\n" );
		return false;
	}
	/* create the app directory */
	if ( apppath == NULL )
	{
		apppath = PROJECT_APP_DIR;
	}
	if ( access( apppath, R_OK ) != 0 )
	{
		if ( mkdir( apppath, REGULAR_DIR_MODE ) != 0 )
		{
			fprintf( stderr, "error: cannot mkdir the %s\n", apppath );
			return false;
		}
	}
	/* create the app directory */
	snprintf( prjpath, sizeof(prjpath), "%s/%s", apppath, name );
	if ( access( prjpath, R_OK ) != 0 )
	{
		if ( mkdir( prjpath, REGULAR_DIR_MODE ) != 0 )
		{
			fprintf( stderr, "error: cannot mkdir the %s\n", prjpath );
			return false;
		}
	}
	/* create the project json */
	snprintf( prjinfo, sizeof(prjinfo), "%s/%s/%s", apppath, name, PROJECT_INFOFILE );
    /* get the intro */
    printf( "project %s introduction: ", name );
    fflush( stdout );
    fgets( intro, sizeof(intro), stdin );
    intro[strlen(intro)-1] ='\0';
	/* create the project info */
	info = file2talk( prjinfo );
	if ( info == NULL )
	{
		info = json_create( NULL );
	}
	json_set_string( info, "name", name );
	json_set_string( info, "version", PROJECT_DEFAULT_VERSION );
	json_set_string( info, "author", "tmptools" );
	json_set_string( info, "intro", intro );
	// open file
	if ( NULL == ( fp = fopen( prjinfo, "w+" ) ) )
	{
		fprintf( stderr, "error: cannot fopen the %s\n", prjinfo );
		talk_free( info );
		return false;
	}
	talk_fprint( fp, info );
	fclose( fp );

	talk_free( info );
	project_dirty();
	return true;
}

static boole project_create_wui( const char *name, const char *prjpath, const char *wuiname )
{
	talk_t cfg;
	talk_t info;
	talk_t app;
	talk_t lang;
	char path[PATH_MAX];
	char page[PATH_MAX];
	char cnjson[PATH_MAX];
	char enjson[PATH_MAX];

    /* get the prj info */
	snprintf( path, sizeof(path), "%s/"PROJECT_INFOFILE, prjpath );
	cfg = file2talk( path );
	if ( cfg == NULL )
	{
		fprintf( stderr, "error: %s json format error\n", path );
		return false;
	}
    /* create */
	snprintf( page, sizeof(page), "%s.html", wuiname );
	snprintf( cnjson, sizeof(cnjson), "%s-cn.json", wuiname );
	snprintf( enjson, sizeof(enjson), "%s-en.json", wuiname );
	info = json_value( cfg, "wui" );
	if ( info == NULL )
	{
		info = json_create( NULL );
		json_set_value( cfg, "wui", info );
	}
	/* add the wui */
	json_delete_axp( info, name );
	app = json_create( NULL );
	json_set_value( info, name, app );
	/* html */
	json_set_string( app, "page", page );
	/* lang */
	lang = json_create( NULL );
	json_set_value( app, "lang", lang );
	json_set_string( lang, "cn", cnjson );
	json_set_string( lang, "en", enjson );

	/* make the html file */
	snprintf( page, sizeof(page), "%s/%s.html", prjpath, wuiname );
	{
		int i;
		FILE *fp;
		char src[PATH_MAX];
		char buf[LINE_MAX];

		unlink( page );
		snprintf( src, sizeof(src), "%s/page.html", TEMPLATE_PATH );
		buf[LINE_MAX-1] = '\0';
		fp = fopen( src, "r" );
		if ( fp != NULL )
		{
			i = 0;
			while( NULL != fgets( buf, sizeof(buf)-1, fp ) )
			{
				if ( NULL != strstr( buf, "templat@com" ) )
				{
					string3file( page, "\tvar comname = \"%s@%s\"\n", name, wuiname );
				}
				else
				{
					string3file( page, buf );
				}
				i++;
			}
			fclose( fp );
		}
	}

	/* make the lang file */
	shell( "cp %s/en.json %s/%s", TEMPLATE_PATH, prjpath, enjson );
	shell( "cp %s/cn.json %s/%s", TEMPLATE_PATH, prjpath, cnjson );

	/* get the chinese menu title */
	printf( "web menu title(Chinese): " );
	fflush( stdout );
	fgets( page, sizeof(page), stdin );
	page[strlen(page)-1] ='\0';
	json_set_string( app, "cn", page );
	/* get the english menu title */
	printf( "web menu name(English): " );
	fflush( stdout );
	fgets( page, sizeof(page), stdin );
	page[strlen(page)-1] ='\0';
	json_set_string( app, "en", page );

	/* write the PROJECT_INFOFILE */
	talk_save( cfg, path );
	talk_print( app );

	talk_free( cfg );
	return true;
}



static const char *helpstr = \
"argument error\n"\
"help:\n"\
"      prj create      project_name                       create a project template\n"\
"      prj delete      project_name                       delete the project\n"\
"      prj add_com     project_name com_name              create a shell component template\n"\
"      prj add_init    project_name init_level   com_api  register a component api at system init call\n"\
"      prj add_uninit  project_name uninit_level com_api  register a component api at system shutdown call\n"\
"      prj add_joint   project_name joint_event  com_api  register a component api at joint cast\n"\
"      prj add_object  project_name object_name com_name  create a dynamic component depend on exist component\n"\
"      prj add_wui     project_name wui_name              create a ace web page template for project\n"\
"      prj check       project_name                       check the a project json format\n"\
"      prj pack        project_name                       pack the project into fpk\n";

int main( int argc, const char **argv )
{
    int ret;
    struct stat st;
    const char *ptr;
    const char *type;
    const char *name;
	const char *hardware;
	char cwd[PATH_MAX+1];
    char path[PATH_MAX+1];
    char prjpath[PATH_MAX+1];

    if ( argc < 3 )
    {
        fprintf( stderr, "%s", helpstr );
        return -1;
    }
    ret = 0;
    type = argv[1];
    name = argv[2];
	if ( type == NULL || name == NULL )
	{
        fprintf( stderr, "%s", helpstr );
		return -1;
	}



	/* create the project */
    if ( 0 == strcmp( type, "create" ) )
    {
		if ( project_create_json( name, NULL ) == false )
		{
			return -1;
		}
		if ( project_path( path, sizeof(path), name ) == NULL )
		{
			return -1;
		}
		printf( "The project directory %s\n", path );
    }
    /* delete the project */
    else if ( 0 == strcmp( type, "delete" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		shell( "rm -fr %s", prjpath );
	}
    /* check the project json format */
    else if ( 0 == strcmp( type, "check" ) )
    {
		if ( project_check( name, NULL ) == false )
		{
            return -1;
		}
	}
    /* pack the project to fpk */
    else if ( 0 == strcmp( type, "pack" ) )
    {
		if ( project_path( path, sizeof(path), name ) == NULL )
		{
			fprintf( stderr, "error: %s no exist\n", name );
			return -1;
		}
		if ( strncmp( path, PROJECT_DIR, strlen(PROJECT_DIR) ) == 0 )
		{
			fprintf( stderr, "error: %s no exist\n", name );
			return -1;
		}
		hardware = register_value( LAND_PROJECT, "hardware" );
		cwd[0] = '\0';
		getcwd( cwd, sizeof(cwd) );
		if ( chdir( path ) != 0 )
		{
			fprintf( stderr, "error: cannot enter to project %s directory \n", name );
			return -1;
		}
		snprintf( path, sizeof(path), "%s/%s-%s-%s.fpk", PROJECT_TMP_DIR, name, PROJECT_DEFAULT_VERSION, hardware );
		shell( "tar zcf %s *", path );
		if ( cwd[0] != '\0' )
		{
			chdir( cwd );
		}
		printf( "The packaging is located in this %s\n", path );
	}



    /* create the project com */
    else if ( 0 == strcmp( type, "add_com" ) )
    {
        ptr = argv[3];
        if ( ptr == NULL )
        {
            fprintf( stderr, "%s", helpstr );
            return -1;
        }
		/* create project */
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
        if ( stat( prjpath, &st ) != 0 )
        {
			if ( project_create_json( name, PROJECT_APP_DIR ) == false )
			{
				return -1;
			}
        }
		snprintf( path, sizeof(path), "%s/%s/%s.ash", PROJECT_APP_DIR, name, ptr );
		/* create shell component */
		shell( "cp %s/component.ash %s", TEMPLATE_PATH, path );
		snprintf( cwd, sizeof(cwd), "%s%c%s", name, PRJ_COM_SEP, ptr );
		com_register( cwd, path );
		printf( "The component file %s\n", path );
    }
    /* create the project wui */
    else if ( 0 == strcmp( type, "add_wui" ) )
    {
        ptr = argv[3];
        if ( ptr == NULL )
        {
            fprintf( stderr, "%s", helpstr );
            return -1;
        }
		/* create project */
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
        if ( stat( prjpath, &st ) != 0 )
        {
			if ( project_create_json( name, PROJECT_APP_DIR ) == false )
			{
				return -1;
			}
        }
		/* create wui */
        project_create_wui( name, prjpath, ptr );
		printf( "The web page file %s/%s.html\n", prjpath, ptr );
    }



    /* create the project init */
    else if ( 0 == strcmp( type, "add_init" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_init( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( INIT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project uninit */
    else if ( 0 == strcmp( type, "add_uninit" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_uninit( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( UNINIT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project joint */
    else if ( 0 == strcmp( type, "add_joint" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_joint( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( JOINT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project object */
    else if ( 0 == strcmp( type, "add_object" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_object( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		com_register( argv[3], argv[4] );
	}



    else
    {
        fprintf( stderr, "%s", helpstr );
    }

    return ret;
}



