


function able2boole ( able )
{
    if ( !able )
    {
        return false;
    }
    return able !== 'disable';
}
function boole2able( val )
{
    return val ? 'enable' : 'disable'
}
function json2array(obj, defaults, keyName) {
    var ret = []
    var srcObj = $.extend(true, {}, obj)
    var srcDefaults = $.extend(true, {}, defaults)

    for (var key in srcObj) {
      var tmp = {}
      tmp[keyName || 'name'] = key
      if (typeof srcObj[key] === 'string') {
        tmp.value = srcObj[key]
      } else {
        $.extend(true, tmp, srcObj[key])
      }
      tmp = $.extend(true, {}, srcDefaults, tmp)
      ret.push(tmp)
    }
    return ret
}
function json_empty( obj )
{  
    for( var key in obj )
    {  
        return false  
    };  
    return true  
};  
/**
 * 根据ip和掩码得到网络地址的数组形式
 * 
 * @param string ip IP地址
 * @param string mask 子网掩码
 * @returns 网络地址的数组形式,eg: [192, 168, 2, 0]
 */
function ipadd2array( ip, mask )
{
    var ipArr = ip.split('.')
    var maskArr = mask.split('.')
    var netWorkAddrArr = []
  
    for( var i = 0; i < 4; i++ )
    {
        var tmp = parseInt(ipArr[i] & maskArr[i])
        netWorkAddrArr.push((typeof tmp === 'number') ? tmp : 0)
    }
    return netWorkAddrArr;
}
function ocompare( a, b )
{
    if( typeof a != typeof b )
    {
        return false;
    }    
    if( typeof a == 'object' )
    {
        if ( Object.keys(a).length != Object.keys(b).length )
        {
            return false;
        }
        for( var k in a )
        {
            if ( typeof b[k] == 'undefined' )
            {
                return false;
            }
            if ( ocompare( a[k], b[k] ) == false )
            {
                return false;
            }
        }
    }
    else
    {
        return a === b;
    }
    return true;
}
/**
 * 将13:28:36:03:02:2018转换为 01:02:04 (03/03/2018)
 * 
 * @param {string} date 时间字符串 13:28:36:03:02:2018
 * @returns 转换后的结果 01:02:04 (03/03/2018)
 */
function date2string(date)
{
    var ret;
    var tmp;

    if (!date)
    {
        return '';
    }

    var tmp = date.split(':')
    if (tmp.length < 6)
    {
        ret = ''
    }
    else
    {
        ret = tmp[0] + ':' + tmp[1] + ':' + tmp[2] + ' (' + tmp[3] + '/' + tmp[4] + '/' + tmp[5] + ')'
    }
    return ret
}

/**
 * 将 hour:min:sec:day 转换为 year:hour:min:sec
 * 
 * @param {string} time 时间字符串 hour:min:sec:day
 * @param {object} option 
 * option.noSec 是否显示秒
 */
function time2string( time, option )
{
    var tmp;
    var ret = '';

    if ( !time )
    {
        return '';
    }
    tmp = time.split(':')
    if (tmp.length < 4 )
    {
        return '';
    }
    var day = '';
    var hour = tmp[0];
    var min = tmp[1];
    var sec = tmp[2];
    if (tmp[3] )
    {
        day = parseInt(tmp[3], 10);
        if ( day <= 0 )
        {
            day = '';
        }
    }
    else
    {
        day = '';
    }

    if ( day )
    {
        if (day <= 1)
        {
            ret = day + 'Day ';
        }
        else
        {
            ret = day + 'Days ';
        }
    }
    ret += (hour + ':' + min);
    if ( !option || (option && !option.noSec ) )
    {
        ret = ret + ':' + sec;
    }
    return ret;
}
/**
 * 将 字节转换为人类可读
 * 
 * @param {number} size 字节数
 */
function byte2readable( size )
{ 
    var sizes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB']; 
    for ( var i = 1; i < sizes.length; i++ )
    { 
        if ( size < Math.pow( 1024, i ) )
        {
            return ( Math.round((size / Math.pow( 1024, i - 1) ) * 100) / 100) + sizes[i - 1];
        }
    } 
    return size; 
}



/**
*
*  page runtime infomation here
*  http://www.ashyelf.com/
*
**/
var page =
{
    // get the page paramter
    param: function( paramName, key )
    {
        var str = key || window.location.search;
        var oregex = new RegExp('[\?&]' + paramName + '=([^&]+)', 'i');
        var oMatch = oregex.exec( str );

        if ( oMatch && oMatch.length > 1 )
        {
            return decodeURIComponent( oMatch[ 1 ] );
        }
        else
        {
            return '';
        }
    },
    lang: function( name )
    {
        var lango = {};
        if ( name.indexOf('/') == 0 )
        {
            lango[ window.lang ] = name;
        }
        else
        {
            lango[ window.lang ] = "/lang/"+name+"-"+window.lang+".json";
        }
        return lango;
    },

    password: function( inputID, iconID, cb )
    {
        $('#' + iconID).on( ace.click_event, function (){
            var $inputOld = $( '#' + inputID );
            var $inputNew;
            var type = $inputOld.attr( 'type' );
            if ( type === 'password' )
            {
                $inputNew = $( '<input type="text" />' );
            }
            else
            {
                $inputNew = $( '<input autocomplete="new-password" type="password" />' );
            }
            // xopy the attr
            var attrs = [ 'id', 'maxlength', 'class', 'name' ];
            for ( var key in attrs )
            {
                var attr = attrs[key];
                var attrValue = $inputOld.attr( attr );
                if (attrValue)
                {
                    $inputNew.attr( attr, attrValue );
                }
            }
            // value
            $inputNew.val( $inputOld.val() );
            // swap
            $inputOld.replaceWith( $inputNew );
            var $icon = $( '#' + iconID );
            if ($icon.hasClass('fa-eye-slash'))
            { // show the password
                $icon.removeClass('fa-eye-slash');
                $icon.addClass('fa-eye second-color');
            }
            else
            { // hide the password
                $icon.removeClass('fa-eye second-color');
                $icon.addClass('fa-eye-slash');
            }
            // callback
            cb && cb();
        });
    },
    // args: { message: title, callback:function, confirm:apply, cancel:cannel }
    confirm: function( args )
    {
        var dfd = $.Deferred();
        args = args || {};
        bootbox.confirm({
            message: args.message || ' ',
            buttons:
            {
                confirm:
                {
                    label: args.confirm || $.i18n('Confirm'),
                    className: 'btn btn-main'
                },
                cancel:
                {
                    label: args.cancel || $.i18n('Cancel'),
                    className: 'btn btn-second'
                }
            },
            callback: args.callback||function (result)
            {
                if (!result)
                {
                    dfd.resolve(false);
                }
                else
                {
                    dfd.resolve(true);
                }
            }
        });
        return dfd.promise();
    },
    // args: { message: title, callback:function, ok:button }
    alert: function( args )
    {
        var dfd = $.Deferred();
        bootbox.alert({
            message: args.message || ' ',
            buttons:
            {
                ok:
                {
                    label: args.ok || $.i18n('OK'),
                    className: 'btn btn-main'
                }
            },
            callback: args.callback||function()
            {
                dfd.resolve();
            }
        });
        return dfd.promise();
    },
    // args: { message: title, callback:function }
    prompt: function( args )
    {
        var dfd = $.Deferred();
        bootbox.prompt({
            title: args.message || ' ',
            value: args.value,
            callback: args.callback||function()
            {
                dfd.resolve();
            },
            buttons:
            {
                confirm:
                {
                    label: args.confirm || $.i18n('Confirm'),
                    className: 'btn btn-main'
                },
                cancel:
                {
                    label: args.cancel || $.i18n('Cancel'),
                    className: 'btn btn-second'
                }
            }
        });
        return dfd.promise();
    },

    overlay2text: function( text )
    {
        $('#overlay-loading-text').text( text || '' );
    },
    overlay2hide: function()
    {
        var el = $('#overlay-loading');
        el.spin(false);
        el.hide();
    },
    overlay: function( text )
    {
        var el = $('#overlay-loading');
        this.overlay2text( text );
        el.show();
        $('#overlay-loading-spin').spin('large', '#fff');
    },

    /*
     * 显示全屏的进度条
     * @param {any} progress 
     * progress.title 进度条的标题
     */
    progress: function( progress )
    {
        pregress = progress || {};
        var $title = $('#overlay-progress-title');  // 进度条的标题
        var $progress = $('#overlay-progress .progress'); // 整条进度条
        var $progressBar = $('#overlay-progress .progress-bar'); // 当前进度
        var percent = 0; // 进度条的比例 0 到 100
        var totalMs = (progress.sec || 50) * 1000; // 总时长
        var interval = totalMs / 100; // 进度条百分比加一的间隔
        $title.text(progress.title || ''); // 设置进度条的标题
        $('#overlay-progress').show(); // 显示进度条
        // 进度条开始
        var timer = setInterval( function(){
            if (++percent > 100) { // 进度到100
                clearInterval(timer); // 清除定时器
                $('#overlay-progress').hide(); // 隐藏进度条
                progress.callback && progress.callback(); // 执行回调
                return;
            }
            $progress.attr('data-percent', percent + '%'); // 设置进度条上的百分比
            $progressBar.css( { width: percent + '%' } ); // 设置进度
        }, interval );
    },
    /*
     * 提示设置成功
     * @param {string} title 提示的文字
     */
    hint2succeed: function( title )
    {
        $.gritter.add( { title:title||$.i18n('Set successfully'), class_name:'gritter-success', time:2000 } );
    },
    /*
     * 提示设置失败
     * @param {string} 提示的文字 
     */
    hint2warning: function( title )
    {
        $.gritter.add( { title: title || $.i18n('Set Failure'), class_name: 'gritter-warning', time: 2000 } );
    },
    /**
     * 设置页面当中的定时器, 当离开当前页面时，定时器自动销毁
     * 
     * @param {object} args 
     * args.refresh 定时执行的函数
     * args.interval 时间间隔
     */
    timing: function( args )
    {
        var timer = setInterval(function () {args.refresh();}, args.interval || 5000 );
        // 离开当前页面时, 需要清除定时器
        $('.page-content-area').one('ajaxloadstart', function(e, params) {
          clearInterval(timer);
        });
        return timer;
    }
    
}







/**
*
*  menus runtime infomation here
*  http://www.ashyelf.com/
*
**/
var menu =
{
    /* menu structure
        [ 
            {
                state: true,
                active: true,
                title: 2.4G WIFI,
                hash: myhtml,
                iconClass: menu-icon fa fa-tachometer,
                submenus:
                [
                    {  active:false, title:SSID, hash:ssid }
                ],...
            },...
        ]
    */
    // add the menu;
    add: function ( stat, menus, name, link, img )
    {
    	var hashstr = "";

    	if ( window.identify )
		{
			if ( link.indexOf("?") >= 0 )
			{
				hashstr = link+"&"+window.identify;
			}
			else
			{
				hashstr = link+"?"+window.identify;
			}
		}
		else
		{
    		hashstr = link;
		}
        menus.push( { state:stat, active:false, title:name, hash:hashstr, iconClass:img, submenus:null } );
    },
    // add the menu;
    insert: function ( stat, menus, name, link, img )
    {
    	var hashstr = "";

    	if ( window.identify )
		{
			if ( link.indexOf("?") >= 0 )
			{
				hashstr = link+"&"+window.identify;
			}
			else
			{
				hashstr = link+"?"+window.identify;
			}
		}
		else
		{
    		hashstr = link;
		}
        menus.unshift( { state:stat, active:false, title:name, hash:hashstr, iconClass:img, submenus:null } );
    },
    // add the menu link
    addlink: function ( menus, name, linkname, link )
    {
        var m;
    	var hashstr = "";

    	if ( window.identify )
		{
			if ( link.indexOf("?") >= 0 )
			{
				hashstr = link+"&"+window.identify;
			}
			else
			{
				hashstr = link+"?"+window.identify;
			}
		}
		else
		{
    		hashstr = link;
		}
        for( var key in menus )
        {
            m = menus[ key ];
            if ( m.title == name )
            {
                m.state = true;
                if ( !m.submenus )
                {
                    m.submenus = [];
                }
                m.submenus.push( { active:false, title:linkname, hash:hashstr } );
                break;
            }
        }
    },
    // active the menu or menu link;
    mark: function ( menus, name, linkname )
    {
        var m;
        var submenu;
        for( var key in menus )
        {
            m = menus[ key ];
            if ( m.title == name )
            {
                if ( !linkname )
                {
                    m.active = true;
                    return;
                }
                /* sub menu */
                if ( m.submenus && m.submenus.length > 0 )
                {
                    for ( var skey in m.submenus )
                    {
                        submenu = m.submenus[ skey ];
                        if ( submenu.title == linkname )
                        {
                            submenu.active = true;
                            return;
                        }
                    }
                }
            }
        }
    },
    // show the menu;
    display: function( position, menus )
    {
        var m;
        var submenu;
        var str = '';
        for ( var key in menus )
        {
            var m = menus[ key ];
            /* is disable */
            if ( m.state == false )
            {
                continue;
            }
            /* is active */
            if ( m.active == true )
            {
                str += '<li class="active">';
            }
            else
            {
                str += '<li class="">';
            }
            /* have the sub menus */
            if ( m.submenus && m.submenus.length > 0 )
            {
                str += '  <a data-url="' + m.hash + '" href="#' + m.hash + '" class="dropdown-toggle">';
            }
            else
            {
                str += '  <a data-url="' + m.hash  + '" href="#' + m.hash  + '">';
            }
            /* menu body */
            str += '    <i class="' + m.iconClass + '"></i>';
            str += '    <span class="menu-text">' + m.title + '</span>'
            str += '    <b class="arrow"></b>';
            str += '  </a>';
            /* sub menu */
            if ( m.submenus && m.submenus.length > 0 )
            {
                str += '<ul class="submenu">';
                for ( var skey in m.submenus )
                {
                    var submenu = m.submenus[ skey ];
                    /* is active */
                    if ( submenu.active == true )
                    {
                        str += '<li class="active">';
                    }
                    else
                    {
                        str += '<li class="">';
                    }
                    str += '  <a data-url="' + submenu.hash + '" href="#' + submenu.hash + '">';
                    str += '    <i class="menu-icon fa fa-caret-right"></i>';
                    str += submenu.title;
                    str += '  </a>';
                    str += '  <b class="arrow"></b>';
                    str += '</li>';
                }
                str += '</ul>';
            }
            str += '</li>';
        }
        $(position).append(str);
    },
    
}





/**
*
*  table runtime infomation here
*  http://www.ashyelf.com/
*
**/
var jqtable =
{
    // 表格第一列的删除和修改选项
    actionOptions:
    {
        name:'myac',
        width:80,
        fixed:true,
        sortable:false,
        resize:false,
        formatter:'actions', 
        formatoptions:
        { 
            keys:true, // 绑定回车
            delOptions:
            {
                recreateForm: true,
                beforeShowForm: function(e){jqtable.style_delete_form(e)}
            },
            // 以表单形式显示编辑框
            editformbutton: true,
            editOptions:
            {
                closeAfterEdit: true,
                recreateForm: true, 
                reloadAfterSubmit: false,
                beforeShowForm: function(e){jqtable.style_edit_form(e)},
                beforeCheckValues: function ()
                {
                    // 触发resize，调节弹框高度
                    $(window).triggerHandler('resize.jqGrid');
                }
            }
        }
    },
    // jqGrid导航按钮选项
    navOptions:
    {
        edit: true,
        editicon : 'ace-icon fa fa-pencil blue',
        add: true,
        addicon : 'ace-icon fa fa-plus-circle purple',
        del: true,
        delicon : 'ace-icon fa fa-trash-o red',
        search: false,
        refresh: false,
        view: true,
        viewicon: 'ace-icon fa fa-search-plus grey'
    },
    // 编辑选项
    editOptions:
    {
        closeAfterEdit: true,
        recreateForm: true, 
        reloadAfterSubmit: false,
        beforeShowForm: function(e){jqtable.style_edit_form(e)},
        beforeCheckValues: function ()
        {
            // 触发resize，调节弹框高度
            $(window).triggerHandler('resize.jqGrid');
        }
    },
    // 添加选项
    addOptions:
    {
        closeAfterAdd: true,
        recreateForm: true,
        reloadAfterSubmit: false,
        beforeShowForm : function(e){jqtable.style_edit_form(e)},
        beforeCheckValues: function ()
        {
            // 触发resize，调节弹框高度
            $(window).triggerHandler('resize.jqGrid');
        }
    },
    // 删除选项
    deleteOptions:
    {
        recreateForm: true,
        beforeShowForm : function(e){jqtable.style_delete_form(e)}
    },
    // 查看选项
    viewOptions:
    {
        beforeShowForm : function(e){jqtable.style_view_form(e)}
    },



    /* create the tables
     * @param {any} grid_selector 
     * @param {any} pager_selector 
     * @param {any} custom 自定义参数
     * @returns 
    */
    create: function( grid_selector, pager_selector, custom )
    {
        var defaults = 
        {
            datatype: 'local', editurl: 'clientArray', viewrecords : true, height: 'auto',
            rowNum:10000, rowList:[],
            hideRowList: true, pgbuttons:false, pginput:false,
            pager: pager_selector,
            multiselect: true, multiboxonly: true, shrinkToFit:false, // 禁止列自适应
            loadComplete: function()
            {
                var t = this;
                setTimeout( function (){ jqtable.updatePagerIcons(t); jqtable.enableTooltips(t); }, 0 );
            }
        };
        if ( custom && custom.hideRowList )
        {
            delete defaults.rowList;
        }
        // 添加水平滚动条
        $( grid_selector ).wrap( '<div style="overflow-x:auto;"></div>' );
        // 结合自定义的参数初始化一个表格
        var item =  $(grid_selector).jqGrid( $.extend( true, defaults, custom ) );
        // 添加响应式布局的功能
        jqtable.resize( grid_selector );
        return item;
    },
    /*
     * 实现jqGrid的自适应宽度
     * @param {String} grid_selector 
     */
    resize: function( grid_selector )
    {
        // 获取当前表格的容器
        function getParentColumn( grid_selector )
        {
            var parent_column = $(grid_selector).closest('[class*="col-"]');
            return parent_column;
        }
    
        //resize to fit page size
        $(window).on( 'resize.jqGrid', function(){
            // 计算自适应元素的top
            function getTop( el, elH )
            {
                var windowH = $(window).height();
                var top = (windowH - elH) / 2 * 0.7;
                return top > 10 ? top : 10;
            }
            // 计算自适应元素的left
            function getLeft( el )
            {
                var windowW = $(window).width();
                var left = (windowW - el.width()) / 2;
                return left;
            }
            // 计算自适应元素的高度
            function getHeight( el )
            {
                // 上下间隔分别为10
                var windowH = $(window).height() - 20;
                // 元素的高度
                var elH = el.outerHeight(true);
                // 没有添加校验结果的高度，并且校验结果没有隐藏
                if (el.data('added-height') !== 'added' && !el.find('.FormError.ui-state-error').is(':hidden')) {
                    // 加上校验结果的高度
                    elH += $('.FormError.ui-state-error').outerHeight(true);
                    // 标记为已添加
                    el.data('added-height', 'added')
                }
                // 返回最小值
                return Math.min(windowH, elH);
            }
            function resizeModal( el, maxWidth )
            {
                el.css( { overflow: 'auto', position: 'fixed', width: $('body').width() - 30, 'max-width': maxWidth || 767 } );
                // 等待dom渲染完毕，因为需要el的宽高
                // 将对话框移动到中间
                setTimeout(function () {
                    // 先计算高度
                    var elH = getHeight(el);
                    el.css({ // 宽度生效后, 才能计算left
                        left: getLeft(el),
                        top: getTop(el, elH),
                        height: elH
                    });
                }, 20);
            }
            // 获取当前表格的容器
            var parent_column = getParentColumn(grid_selector);
            // table的宽度
            // $(grid_selector).jqGrid( 'setGridWidth', parent_column.width() );
            // 添加纪录弹框宽度的自适应
            resizeModal($('[id^="editmod"]'));
            // jqGrid alert 的自适应
            resizeModal($('[id^="alertmod_"]'), 200);
            // jqGrid delete 的自适应
            resizeModal($('[id^="delmod"]'), 200);
            // jqGrid view 的自适应
            resizeModal($('[id^="viewmod"]'));
        });
        //resize on sidebar collapse/expand
        $(document).on( 'settings.ace.jqGrid' , function( ev, event_name, collapsed ){
            var parent_column = getParentColumn(grid_selector);
            if ( event_name === 'sidebar_collapsed' || event_name === 'main_container_fixed' )
            {
                //setTimeout is for webkit only to give time for DOM changes and then redraw!!!
                setTimeout(function(){ $(grid_selector).jqGrid( 'setGridWidth', parent_column.width() ); }, 20);
            }
        });
        // 监听 jqGrid 的 reloadGrid
        $(grid_selector).on('reloadGrid', function () { $(window).triggerHandler('resize.jqGrid'); });
        // 初始化时,需要手动触发一次resize
        $(window).triggerHandler('resize.jqGrid');
    },
    //replace icons with FontAwesome icons like above
    updatePagerIcons: function( table )
    {
        var replacement = 
        {
            'ui-icon-seek-first' : 'ace-icon fa fa-angle-double-left bigger-140',
            'ui-icon-seek-prev' : 'ace-icon fa fa-angle-left bigger-140',
            'ui-icon-seek-next' : 'ace-icon fa fa-angle-right bigger-140',
            'ui-icon-seek-end' : 'ace-icon fa fa-angle-double-right bigger-140'
        };
        $('.ui-pg-table:not(.navtable) > tbody > tr > .ui-pg-button > .ui-icon').each(function(){
            var icon = $(this);
            var $class = $.trim(icon.attr('class').replace('ui-icon', ''));
            if ( $class in replacement )
            {
                icon.attr('class', 'ui-icon '+ replacement[$class] );
            }
        });
    },
    enableTooltips: function( table )
    {
        $('.navtable .ui-pg-button').tooltip({container:'body'});
        $(table).find('.ui-pg-div').tooltip({container:'body'});
    },
    /*
     * 获取滚动条的当前位置
     * @returns 滚动条的位置
     */
    getScrollPos: function()
    {
        var scrollPos;
        if (typeof window.pageYOffset != 'undefined')
        {
            scrollPos = window.pageYOffset;
        }
        else if ( typeof document.compatMode != 'undefined' && document.compatMode != 'BackCompat')
        {
            scrollPos = document.documentElement.scrollTop;
        }
        else if (typeof document.body != 'undefined')
        {
            scrollPos = document.body.scrollTop;
        }
        return scrollPos;
    },
    /*
     * 设置滚动条的位置
     * @param {Number} scrollPos 滚动条的位置
     */
    setScrollPos: function( scrollPos )
    {
        document.documentElement.scrollTop = scrollPos;
        document.body.scrollTop = scrollPos;
    },
    style_delete_form: function( e )
    {
        var form = $(e[0]);
    
        // if(form.data('styled')) return false;
        form.closest('.ui-jqdialog').find('.ui-jqdialog-titlebar').wrapInner('<div class="widget-header" />')
        // 为了调整对话框的宽度, 所以触发resize
        $(window).triggerHandler('resize.jqGrid');
        var buttons = form.next().find('.EditButton .fm-button');
        buttons.addClass('btn btn-sm btn-round').find('[class*="-icon"]').hide();//ui-icon, s-icon
        buttons.eq(0).addClass('btn-main');
        buttons.eq(1).addClass('btn-second');
        // form.data('styled', true);
    },
    style_edit_form: function( e )
    {
        var form = $(e[0]);
        form.closest('.ui-jqdialog').find('.ui-jqdialog-titlebar').wrapInner('<div class="widget-header" />');
        // 为了调整对话框的宽度, 所以触发resize
        $(window).triggerHandler('resize.jqGrid');
        //update buttons classes
        var buttons = form.next().find('.EditButton .fm-button');
        buttons.addClass('btn btn-sm').find('[class*="-icon"]').hide();//ui-icon, s-icon
        buttons.eq(0).addClass('btn-main');
        buttons.eq(1).addClass('btn-second');
    },
    style_view_form: function( e )
    {
        $(window).triggerHandler('resize.jqGrid');
    },


    /**
     * 创建星期复选框
     * @returns 星期复选框的字符串形式
     */
    weekday_select_make: function( e )
    {
        var weekdays =
        [  
            {id: 'weekday1',label: $.i18n('Monday')},
            {id: 'weekday2',label: $.i18n('Tuesday')},
            {id: 'weekday3',label: $.i18n('Wednesday')},
            {id: 'weekday4',label: $.i18n('Thursday')},
            {id: 'weekday5',label: $.i18n('Friday')},
            {id: 'weekday6',label: $.i18n('Saturday')},
            {id: 'weekday7',label: $.i18n('Sunday')}
        ];
    
        var str = '';
        for ( var i in weekdays )
        {
            var weekday = weekdays[i];
            str += ' \
              <div class="checkbox-red" style="display: inline-block; margin-left: 6px; margin-top: 4px;"> \
                <label> \
                  <input id="' + weekday.id + '" name="form-field-checkbox" type="checkbox" class="ace" /> \
                  <span class="lbl">' + weekday.label + '</span> \
                </label> \
              </div>';
        }
        return str;
    },
    
    /**
     * 创建时间选择器
     * @param {String} prefix id的前缀
     * @returns 时间选择器的字符串形式
     */
    timer_select_make: function( prefix  )
    {
        var str = '';
        for ( var i =  0; i < 3; i++ )
        {
            var id = prefix + '-'
            switch (i)
            {
                case 0:
                    id += 'hour'
                    break;
                case 1:
                    id += 'min'
                    break;
                case 2:
                    id += 'sec'
                    break;
                default:
                    break;
            }
            str += '<select id="' + id + '">';
            for ( var j = 0; j < 60; j++ )
            {
                // 如果是小时，到24则跳出
                if ( i === 0 && j === 24 )
                {
                    break;
                }
                // 个位数时，前面补一个零
                var val = j < 10 ? '0' + j : j
                str += '<option value="' + val + '">' + val + '</option>';
            }
            str += '</select>';
        }
        return str;
    }
    
    
}





/**
*
*  input check funciton here
*  http://www.ashyelf.com/
*
**/
var check =
{
    // is number 
     number: function( input )
     {
        var pattern = /^[0-9]+.?[0-9]*$/;
        if ( !pattern.test( input ) )
        {
            return false;
        }
        return true;
    },
    // is port 
     port: function( input )
     {
        var pattern = /^[0-9]+.?[0-9]*$/;
        if ( !pattern.test( input ) )
        {
            return false;
        }
        if ( parseInt( input ) > 65535 || parseInt( input ) <= 0 )
        {
            return false;
        }
        return true;
    },
    // is ip address?
    ip: function ( value )
    {
        var pattern = /^\d{1,3}(\.\d{1,3}){3}$/;
        if ( !pattern.exec( value ) )
        {
            return false;
        }
        var ary = value.split( '.' );
        for ( key in ary )
        {
            if ( parseInt( ary[ key ] ) > 255 )
            {
                return false;
            }
        }
        return true;
    },
    // is ip address?
    ipv6: function ( value )
    {
        return true;
    },
    // is mac
    mac: function( input, loose )
    {
        var pattern = /^[A-Fa-f\d]{2}:[A-Fa-f\d]{2}:[A-Fa-f\d]{2}:[A-Fa-f\d]{2}:[A-Fa-f\d]{2}:[A-Fa-f\d]{2}$/;
        var loosePattern = /^[A-Fa-f\d]{12}$/;
        if ( loose )
        {
            return pattern.test(input) || loosePattern.test(input)
        }
        else
        {
            return pattern.test(input)
        }
    }    
    
}



