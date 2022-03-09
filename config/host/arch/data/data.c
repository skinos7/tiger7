/*
 *    Description:  platform configure management
 *         Author:  dimmalex (dim), dimmalex@gmail.com
 *        Company:  ASHYELF
 */
#include "land/farm.h"



boole_t _setup( obj_t this, param_t param )
{
	int i;
	int fd;
	struct stat st;
	unsigned long randi;
    char path[PATH_MAX+1];

    /* create the root directory */
	if ( strlen(PROJECT_ROT_DIR) > 1 )
	{
		if ( access( PROJECT_ROT_DIR, R_OK ) != 0 )
		{
			if ( mkdir( PROJECT_TMP_DIR, REGULAR_DIR_MODE ) != 0 )
			{
				return tfalse;
			}
		}
	}
    /* create the tmp directory */
    if ( access( PROJECT_TMP_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_TMP_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the var directory */
    if ( access( PROJECT_VAR_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_VAR_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the register directory */
    if ( access( PROJECT_REG_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_REG_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the service directory */
    if ( access( PROJECT_SER_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_SER_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the com directory */
    if ( access( PROJECT_COM_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_COM_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the config directory */
    if ( access( PROJECT_CFG_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_CFG_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }

    /* create the mount directory */
    if ( access( PROJECT_MNT_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_MNT_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }
    /* create the mount internal directory */
    if ( access( PROJECT_INT_DIR, R_OK ) != 0 )
    {
        if ( mkdir( PROJECT_INT_DIR, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
    }

	/* seed the random */
	randi = 0;
	srandom(time(NULL));
	fd = open( "/dev/urandom", O_RDONLY );
	if ( fd >= 0 )
	{
		read( fd, &randi, sizeof(randi) );
		string2file( RANDOM_FILENAME, "%lu", randi );
		close( fd );
	}
	else
	{
		randi = random();
		string2file( RANDOM_FILENAME, "%u", randi );
	}

    /* insmod crackid */
    project_ko_path( path, sizeof(path), PDRIVER_PROJECT, "crackid.ko" );
    if ( access( path, R_OK ) == 0 )
    {
        execute( 0, true, "insmod %s", path );
    }

    /* globe value setttings */
	i = SYSLOG_INFO|SYSLOG_WARN|SYSLOG_FAULT;
	i |= SYSLOG_LAND|SYSLOG_ARCH|SYSLOG_DEFAULT;
    register_set( NULL, "syslog_mask", &i, sizeof(i) );
	i = 1;
    register_set( NULL, "config_ready", &i, sizeof(i) );
    register_set( NULL, "platform", gPLATFORM, sizeof(gPLATFORM) );
    register_set( NULL, "hardware", gHARDWARE, sizeof(gHARDWARE) );
    register_set( NULL, "custom", gCUSTOM, sizeof(gCUSTOM) );
    register_set( NULL, "scope", gSCOPE, sizeof(gSCOPE) );
    register_set( NULL, "version", gVERSION, sizeof(gVERSION) );
    register_set( NULL, "publish", gPUBLISH, sizeof(gPUBLISH) );

    /* earse the config partition */
	if ( stat( PROJECT_CFG_DIR"/"ERASE_FLAG_FILE, &st ) == 0 )
	{
        /* if the order for deafult, need to format to clear all the garbage */
        warn( "erase the config partition by requirement" );
        ainfo( "clear the config partition" );
		shell( "rm -fr	%s/*", PROJECT_CFG_DIR );
		shell( "rm -fr	%s/.*", PROJECT_CFG_DIR );
	}	
    /* earse the internal partition */
	if ( stat( PROJECT_INT_DIR"/"ERASE_FLAG_FILE, &st ) == 0 )
    {
        /* if the order for factory, need to format to clear all the garbage */
        warn( "erase the interval partition by requirement" );
		shell( "rm -fr	%s/*", PROJECT_INT_DIR );
		shell( "rm -fr	%s/.*", PROJECT_CFG_DIR );
    }	

    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    execute( 0, true, "sync" );
    return ttrue;
}



