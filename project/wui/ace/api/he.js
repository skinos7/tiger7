


/**
*
*  communication to system API
*  http://www.ashyelf.com/
*
**/
var he =
{
    /* excute cmd api */
    cmd:function ( a, args, func )
    {
        var i;
        var t;
        var ret;
        var uri;
        var hekey;
        var heindex;
        var htmlobj;
        var callbak;
        var timeout;
        var paramter;

        if ( a == null )
        {
            return null;
        }
        // timeout
        if ( args && args.timeout )
        {
            timeout = args.timeout;
        }
        else
        {
            timeout = 0;
        }
        /* get the paramter */
        t = 0;
        heindex = {};
        paramter = "";
        if ( a instanceof Array )               // array
        {
            for ( i in a )
            {
                if ( t == 0 )
                {
                    hekey = "he";
                }
                else
                {
                    hekey = "he" + t;
                }
                heindex[ hekey ] = i;
                paramter += ( "&" + hekey + "=" + url.encode( base64.encode( a[ i ] ) ) );
                // paramter += ("&" + hekey + "=" + base64.encode( a[ i ] ) ); have a bug
                t++;
            }
        }
        else if ( a instanceof Object )         // object
        {
            for ( i in a )
            {
                if ( t == 0 )
                {
                    hekey = "he";
                }
                else
                {
                    hekey = "he" + t;
                }
                heindex[ hekey ] = i;
                paramter += ( "&" + hekey + "=" + url.encode( base64.encode( a[ i ] ) ) );
                // paramter += ("&" + hekey + "=" + base64.encode( a[ i ] ) ); have a bug
                t++;
            }
        }
        if ( paramter == "" )
        {
            return null;
        }
        /* get the uri */
        uri = "/action/he?rand=" + Math.random();
        /* show the loading */
        if ( args && args.loading )
        {
            page.overlay( args.loading );
        }
        /* get the async */
        if ( func != null )
        {
            $.ajax({
                'url':uri, 'type':'POST', 'timeout':timeout, 'async':true, 'contentType':'application/x-www-form-urlencoded', 'data':paramter,
                'complete': function ( x, s )
                {
                    // return while in rebooting
                    if ( window.rebooting )
                    {
                        return;
                    }
                    // relogin while rebooting or no permision
                    if ( ( !x.getAllResponseHeaders() ) || x.getResponseHeader( 'content-type' ) === 'text/html' )
                    {
                        // redirect to login.html
                        window.location.href = 'login.html';
                        return;
                    }
					//console.log( "Server Response1: "+x.responseText );
                    paramter = base64.decode( x.responseText  );
					//console.log( "Server Response3: "+paramter );
                    // string for Exception
                    if ( paramter.indexOf("{" ) < 0 )
                    {
                        console.log( "Server Response String: "+paramter );
                        func( paramter );
                    }
                    else
                    {
						callbak = $.parseJSON( paramter );
                        //callbak = eval( "(" + paramter + ")" );
                        if ( callbak == null )
                        {
                            console.log( "Server Response Not JSON: "+paramter );
                            return;
                        }
                        var value = new Object();
                        for ( i in heindex )
                        {
                            value[ heindex[ i ] ] = callbak[ i ];
                        }
                        ret = value;
                        func( ret );
                    }
                    if ( args && args.loading )
                    {
                        page.overlay2hide();
                    }
                }
            });
        }
        else
        {
            htmlobj = $.ajax( { 'url':uri, 'type':'POST', 'timeout':timeout, 'async':false, 'contentType':'application/x-www-form-urlencoded', 'data':paramter } );
            // return while in rebooting
            if ( window.rebooting )
            {
                return null;
            }
            // relogin while rebooting or no permision
            if ( ( !htmlobj.getAllResponseHeaders() ) || htmlobj.getResponseHeader( 'content-type' ) === 'text/html' )
            {
                window.location.href = 'login.html';
                return null;
            }
			//console.log( "Server Response1: "+htmlobj.responseText );
			paramter = base64.decode( htmlobj.responseText  );
			//console.log( "Server Response3: "+paramter );
            // string for Exception
            if ( paramter.indexOf("{" ) < 0 )
            {
                console.log( "Server Response String: "+paramter );
                ret = paramter;
            }
            else
            {
				callbak = $.parseJSON( paramter );
                //callbak = eval( "(" + paramter + ")" );
                if ( callbak == null )
                {
                    console.log( "Server Response Not JSON: "+paramter );
                    ret = null;
                }
                else
                {
                    var value = new Object();
                    for ( i in heindex )
                    {
                        value[ heindex[ i ] ] = callbak[ i ];
                    }
                    ret = value;
                }
            }
            if ( args && args.loading )
            {
                page.overlay2hide();
            }
            return ret;
        }
    },

    load:function( a, loading )
    {
        var dfd = $.Deferred();
        page.overlay( loading|| $.i18n('Loading') );
        this.cmd( a, null, function (v) {
            page.overlay2hide();
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    bkload:function( a )
    {
        var dfd = $.Deferred();
        this.cmd( a, null, function (v) {
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    save:function( a, loading )
    {
        var dfd = $.Deferred();
        page.overlay( loading||$.i18n('Saving') );
        this.cmd( a, null, function (v) {
            page.overlay2hide();
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    bkexec:function( a )
    {
        var dfd = $.Deferred();
        this.cmd( a, null, function (v) {
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    exec:function( a, loading )
    {
        var dfd = $.Deferred();
        page.overlay( loading||$.i18n('Running') );
        this.cmd( a, null, function (v) {
            page.overlay2hide();
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    
    /*
     * 重启路由器，屏显示进度条
     * @param {any} args 
     * args.title
     * args.restartTime
     * args.href
     */
    reboot: function( args )
    {
        var timeout;
        var arg = args || {};

        if ( arg.restartTime )
        {
            timeout = arg.restartTime;
        }
        else if ( window.custom && window.custom.restart_time > 0 )
        {
            timeout = window.custom.restart_time;
        }
        else
        {
            timeout = 60;
        }
        // 显示进度条
        page.progress({
            title: arg.title || $.i18n('Rebooting...'),
            sec: timeout,
            callback: function ()
            {
                page.alert( {message:arg.hint||$.i18n('Make sure that the device is reconnected')} ).then(function () {
                    if ( arg.href )
                    {
                        window.location.href = arg.href;
                    }
                    else
                    {
                        // 进度条读完之后, 刷新页面
                        window.location.reload(true);
                    }
                })
            }
        });
        // 执行cmds中的命令
        var cmds = arg.cmds || [];
        cmds.push('machine.restart');
        // 设置正在重启的标志位
        window.rebooting = true;
        he.cmd(cmds, null, function () {});
    },
 
    /*
     * 升级后重启路由器，屏显示进度条
     * @param {any} args 
     * args.title
     * args.restartTime
     * args.href
     */
    upgrade_reboot: function( args )
    {
        var timeout;
        var arg = args || {};

        if ( arg.restartTime )
        {
            timeout = arg.restartTime;
        }
        else if ( window.custom && window.custom.upgrade_wait > 0 )
        {
            timeout = window.custom.upgrade_wait;
        }
        else
        {
            timeout = 150;
        }
        // 显示进度条
        page.progress({
            title: arg.title || $.i18n('Restarting...'),
            sec: timeout,
            callback: function ()
            {
                page.alert( {message:arg.hint||$.i18n('Make sure that the device is reconnected')} ).then(function () {
                    if ( arg.href )
                    {
                        window.location.href = arg.href;
                    }
                    else
                    {
                        // 进度条读完之后, 刷新页面
                        window.location.reload(true);
                    }
                })
            }
        });
        // 执行cmds中的命令
        var cmds = arg.cmds || [];
        cmds.push('machine.restart');
        // 设置正在重启的标志位
        window.rebooting = true;
        he.cmd(cmds, null, function () {});
    }
  

}


