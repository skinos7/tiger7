***
## 项目管理组件（land@prj） 
项目相关的管理

#### 以下介绍项目概念 
* 项目是针对完成特定需求而开发的一个程序集合， 相当于windows的应用程序, 安卓中的APP
* 每个项目都有对应的目录, 称为项目目录，存放于SDK中的project目录下, 项目相关的源代码及资源文件等的都存放在项目目录下
* 所有的开发及编程都应以项目为中心
* 每一个项目目录中都必须包括： 
    - 项目信息文件, 即prj.json
    - 项目编译Makefile, 如使用Openwrt的编译体系时Makefile使用Openwrt的格式
* 每一个项目目录中都可包含:
    - 可执行程序或可执行程序源代码(在SDK中会是可执行程序的源代码, 而在安装到设备中只会有可执行程序)
    - 库或库源代码(在SDK中会是库的源代码, 而在安装到设备中只会有库的二进制文件) 
    - 内核驱动或其源代码(在SDK中会是驱动源代码, 而在安装到设备中只会有驱动的二进制文件)  
    - 组件源代码(在SDK中会组件源代码, 而在安装到设备中只会有组件的二进制文件)  
    - 默认配置文件
    - 界面文件
    - 语言文件
    - 脚本文件及其它资源文件

#### 以下介绍FPK概念
* 项目开发完成后将编译成FPK安装包安装到系统中
* FPK安装包即相当于windows中的软件安装包， 安卓中的apk文件
* 项目在编译后会生产以fpk结尾的FPK安装包
* FPK安装包可以通过系统界面安装到系统中
* 每一个FPK安装包中都必须包括： 
	- 项目信息文件, 即prj.json
* 每一个FPK安装包都可包含:
	- lib下库
	- bin下可执行文件
	- drv下的驱动文件
	- .com结尾的组件文件
	- .cfg结尾的配置文件
	- .html结尾的界面文件
	- .json结尾的语言文件
	- .sh结尾的SHELL脚本文件
	- 其它的资源文件
	- install/include下开发用的库头文件(SDK编译时用到)
	- install/lib下开发用的库(SDK编译时用到)

#### prj.json: 即项目信息文件, 描述整个项目的信息, 在项目及FPK安装包中都必须包含, 介绍如下 
```json
// 属性介绍
{
    "name":"项目名",                 // 在整个系统中必须唯一
    "intro":"项目的简介",
    "desc":"项目进一步的介绍",
    "type":"类型",                   // root表示必须有root权限
    "version":"版本",
    "author":"作者",
    "osc":                           // 指出项目包含的开源程序
    {
        "开源程序名":"开源程序简介", // 开源程序在项目目录下有相同名的源代码目录
        // ...                       // 其它开源程序
    },
    "ko":                            // 指出项目包含的内核模块名
    {
        "内核模块名":"简介",         // 内核模块名必须在整个系统中唯一, 内核模块也会在项目目录下有相同名的源代码目录
        // ...                       // 其它库
    },
    "lib":                           // 指出项目包含的库, 此节点下的库可以被其它项目调用, 编译后会在项目的lib目录下生产对应的库文件
    {
        "库名":"库的简介",           // 库名必须在整个系统中唯一, 库名也会在项目目录下有相同名的源代码目录
        // ...                       // 其它库
    },
    "exe":                           // 指出项目包含的可执行程序, 编译后会在项目的bin目录下生产同名的可执行程序
    {
        "可执行程序名":"简介",       // 可执行程序在项目目录下有相同名的源代码目录
        // ...                       // 其它可执行程序
    },
    "com":                           // 指出项目包含的组件
    {
        "组件名":"组件介绍",         // 项目通常将自已的功能包装成组件的形态供用户或其它的应用来调用, 组件在项目目录下有相同名的源代码目录 
        // ...                       // 其它组件
    },
    "res":                           // 指出项目包含的一些其它的资源文件
    {
        "文件名":"简介",
        // ...                       // 其它资源文件
    },
    "obj":                           // 指出项目中的动态组件, 动态组件是组件的一个扩展或别名, 动态组件与组件的关系相当于对象与类的关系
    {
        "动态组件名":"原始组件",
        // ...                       // 其它动态组件
    },
    "init":                          // 项目所有的启动项, 具体可参看init.md
    {
        "启动级别":"启动时调用的接口",
        // ...                       // 其它启动项
    },
    "uninit":                        // 项目所有的关机项, 具体可参看init.md
    {
        "关机级别":"关机时调用的接口",
        // ...                       // 其它关机项
    },
    "joint":                         // 项目所有的事件处理, 具体可参看joint.md
    {
        "事件名称":"事件发生时调用的接口",
        // ...                       // 其它事件处理
    }
}

// 示例
{
    "name":"arch",                              // arch项目为系统底层项目
    "intro":"mtk platform layer for farm os",
    "desc":"This project enables the system to run on the core of MTK, and the details of its packaged MTK chip will provide a unified management or use interface to the upper layerthe proejct",
    "type":"root",                              // 必须有root权限
    "version":"6.0.0",                          // 版本为6.0.0
    "author":"dimmalex@gmail.com",              // 作者为dimmalex@gmail.com
    "osc":
    {
        "ntpclient":"ntp client"                // 开源程序有ntpclient, 为NTP客户端程序
    },
    "lib":
    {
        "land":"core library"                   // 项目含有land核心库
    },
    "exe":
    {
        "daemon":"service daemon",              // 项目含有demaon可执行程序
        "he":"tools for call all component",    // 项目含有he可执行程序
        "hetui":"tools for terminal"            // 项目含有hetui可执行程序
    },
    "com":
    {
        "device":"device infomation",             // 项目含有device组件, 此组件的全名为arch@device, 即项目名加上@再加组件名
        "data":"data management",                 // 项目含有data组件
        "firmware":"firmware management",         // 项目含有firmware组件
        "reggpio":"register and gpio management", // 项目含有reggpio组件
        "test":"test the device management",      // 项目含有test组件
        "ethernet":"ethernet switch management",  // 项目含有ethernet组件
        "mt7628":"802.11n wireless management"    // 项目含有mt7628组件
    },
    "res":
    {
        "testpage.py":"only test"                 // 项目含有testpage.py文件
    },
    "obj":
    {
        "wifi@nradio":"mt7628",    // 动态组件名为wifi@nradio, 对应的实际组件为本项目的mt7628组件, 即操作wifi@nradio最终由本项目的mt7628组件处理
        "test":"test"              // 动态组件名为test, 对应的实际组件为本项目的test组件, 即操作test最终由本项目的test组件处理
    }
    "init":
    {
        "ethernet":"arch@ethernet.setup",           // 启动级别为ethernet时调用arch@ethernet.setup初始化交换机
        "nradio":"wifi@nradio.setup",               // 启动级别为nradio时调用wifi@nradio.setup初始化2.4G无线
        "aradio":"wifi@aradio.setup"                // 启动级别为aradio时调用wifi@aradio.setup初始化5.8G无线
    },
    "uninit":
    {
        "nradio":"wifi@nradio.shut",                // 关机级别为nradio时调用wifi@nradio.shut关闭2.4G无线
        "aradio":"wifi@aradio.shut"                 // 关机级别为aradio时调用wifi@aradio.shut关闭5.8G无线
    },

    "joint":
    {
        "firmware/upgrading":"arch@reggpio.event",  // 系统发生firmware/upgrading事件时调用arch@reggpio.event来处理LED灯指示
        "firmware/upgraded":"arch@reggpio.event",   // 系统发生firmware/upgraded事件时调用arch@reggpio.event来处理LED灯指示
        "network/arise":"arch@reggpio.event",       // 以下事件都将调用arch@reggpio.event来处理LED指示灯
        "network/ready":"arch@reggpio.event",
        "network/lining":"arch@reggpio.event",
        "network/online":"arch@reggpio.event",
        "network/offline":"arch@reggpio.event",
        "modem/poweron":"arch@reggpio.event",
        "modem/poweroff":"arch@reggpio.event",
        "modem/msim":"arch@reggpio.event",
        "modem/bsim":"arch@reggpio.event",
        "signal/flash":"arch@reggpio.event",
        "nssid/up":"arch@reggpio.event",
        "nssid/down":"arch@reggpio.event"
    }    
}
```  


#### **接口** 

+ `create[ 项目名称 ]` 创建一个项目框架, 此指令将提示需要输入项目简介后将在可写介质的prj目录下创建项目框架

>*成功返回如下ttrue, 失败返回tfalse*

+ `add_init[ 项目名称, 启动级别, 调用接口 ]`  为项目注册开机启动项

>*成功返回如下ttrue, 失败返回tfalse*

+ `add_uninit[ 项目名称, 关机级别, 调用接口 ]`  为项目注册关机项

>*成功返回如下ttrue, 失败返回tfalse*

+ `add_joint[ 项目名称, 事件名称, 调用接口 ]` 为项目注册事件处理

>*成功返回如下ttrue, 失败返回tfalse*

+ `add_wui[ 项目名称, 网页名称 ]` 为项目注册网页菜单, 此指令将提示需要输入中英文菜单名称

>*成功返回如下ttrue, 失败返回tfalse*

+ `check[ 项目名称 ]` 检查指定项目的所有json格式是否错误

>*无错误返回如下ttrue, 有错误返回tfalse*

+ `pack[ 项目名称 ]` 将项目打包成一个FPK安装包, 成功后会在终端提示打包好的FPK安装包位置

>*成功返回如下ttrue, 失败返回tfalse*

+ `delete[ 项目名称 ]` 删除一个项目

>*成功返回如下ttrue, 失败返回tfalse*

+ `list[ [项目名称] ]` 不给出指定项目名称将列出系统中所有的项目信息

>*成功返回JSON, 以下为JSON介绍; 失败返回NULL*

