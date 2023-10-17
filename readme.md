---
title: "Readme"
author: dimmalex@gmail.com
date: March 22, 2022
output:
    word_document:
        path: D:/tmp/Readme.docx
---



# SkinOS system framework( 系统框架 )

- SkinOS encapsulates the manageable objects of Linux through the **component**, providing the interface(API) to the upper layer
- The objects that can be managed in SkinOS are all individual components
- Multiple related or similar functional components make up a **project**
- SkinOS中通过 **组件** 对Linux的可管理对象封装，向上层提供调用接口
- 在SkinOS中可管理的对象都是一个个 **组件**
- 多个相关或相类功能的组件组成一个项目

![avatar](./frame.jpg)



# SkinOS project introduction( 项目介绍 )

- Applications in SkinOS is **project**
- **project** There is a corresponding directory in the SDK, which stores the source code and various resource files related to the project
- **project** corresponding directory all are stored in the **[tiger7/project](./project/)** directory
- **project** will generate **FPK** after compiling, which is used to install into SkinOS
- The following is structure **project**
- SkinOS中的应用程序是 **项目**
- **项目** 在SDK中有对应的目录，目录中存放项目相关的源代码及各种资源文件
- **项目** 对应的目录都存放在都在 **[tiger7/project](./project/)** 目录下
- **项目** 编译后会生成 **FPK安装包**, 用于安装到SkinOS中
- 以下是 **项目** 结构

![avatar](./project.png)



**[prj.json](https://gitee.com/tiger7/doc/blob/master/dev/prj.json.md)** Describe all of the project's resources in JSON format( 以JSON格式描述项目所有的资源 )

- Guide the compilation process at compiling( 编译时指导编译过程 )
- Guide the installation process during installation( 安装时指导安装过程 )
- The boot option is registered during SkinOS startup( 系统启动时注册启动项 )
- Register the shutdown item when the SkinOS shuts down( 系统关机时注册关机项 )
- Register event handlers to SkinOS( 向系统注册事件处理项 )
- Register the web menu(page) to SkinOS( 向系统注册网页界面 )


**Component** Provide a unified management interface to the SkinOS or users( 向系统或用户提供统一的管理接口 )

**Default Configure** Default setting( 默认配置 )

**HTML** The web page file corresponding to the web menu( 菜单对应的界面文件 )

**Shell** Shell script file( shell脚本文件 )

**Open Source Program** Open source application( 开源应用程序 )

**Linux Executable Program** executable program( 可执行程序 )

**Shared Library** 

**Kernel Module**


# SkinOS project framework( 项目框架 )

![avatar](./project2.png)

**Skin Layer**, provide a unified management interface to the SkinOS or users
- **Component** provide a unified management interface to the SkinOS or users
- **Default Configure file** provide default setting
- **HTML** provide management UI
- **prj.json** Describe all of the project's resources in JSON format

**Linux System and Application**, Traditional Linux applications, resource files, etc, Generally, no external interface is provided：
- **Executable program**
- **Kernel Module**
- **Shared Library**
- **shell** script
- **Resource file**

SkinOS system components manage the project through **Skin Layer**( SkinOS的系统组件通过**Skin Layer**对项目的管理 )
![avatar](./project3.png)





# SkinOS component interface( 组件接口 )


### Common component API( 常用的组件API )

The following figure describes common project and component interfaces of gateway( 下图介绍网关常用的项目及组件API )

![avatar](./component.png)


**land** project, SkinOS core project, manage gateway basic information

- machine component, name it **land@machine**, The alias is **machine**, **[Management of Basic Infomation](https://gitee.com/tiger7/doc/blob/master/com/land/machine.md)**( 设备基本信息管理 )
- service component, name it **land@service**, The alias is **service**, **[Management of System Service](https://gitee.com/tiger7/doc/blob/master/com/land/service.md)**( 系统服务管理 )
- auth 
- auth component, name it **land@auth**, The alias is **auth**, **[Username/Password and Permission Management](https://gitee.com/tiger7/doc/blob/master/com/land/auth.md)**( 用户名密码及权限管理 )
- syslog component, name it **land@syslog**, The alias is **log**, **[Syslog Management](https://gitee.com/tiger7/doc/blob/master/com/land/syslog.md)**( 日志管理 )

**client** project, Manage all clients

- global component, name it **client@global**,  **[Management of Global Access](https://gitee.com/tiger7/doc/blob/master/com/client/global.md)**( 访问管理 )
- station component, name it **client@station**,  **[Management of Client Access](https://gitee.com/tiger7/doc/blob/master/com/client/station.md)**( 客户端管理 )

**forward** project, Manage forward functions

- alg component, name it **forward@alg**, **[Management of Application Layer Gateway](https://gitee.com/tiger7/doc/blob/master/com/forward/alg.md)**( 应用层网关管理 )
- route component, name it **forward@route**, **[System Route Table Management](https://gitee.com/tiger7/doc/blob/master/com/forward/route.md)**( 路由表管理 )
- firewall component, name it **forward@firewall**, **[Management of System Firewall](https://gitee.com/tiger7/doc/blob/master/com/forward/firewall.md)**( 防火墙管理 )
- nat component, name it **forward@nat**, **[Management of System NAT/DMZ](https://gitee.com/tiger7/doc/blob/master/com/forward/nat.md)**( 端口映射管理 )

**ifname** project, Manage all connections of gateway

- lte connection component, first LTE name it **ifname@lte**, second LTE name it **ifname@lte2**, **[LTE/NR Network Management](https://gitee.com/tiger7/doc/blob/master/com/ifname/lte.md)**( LTE网络管理 )
- wan connection component, first WAN name it **ifname@wan**, second WAN name it **ifname@wan2**, **[WAN Network Management](https://gitee.com/tiger7/doc/blob/master/com/ifname/wan.md)**( WAN口管理 )
- lan connection component, first LAN name it **ifname@lan**, second LAN name it **ifname@lan2**, **[LAN Network Management](https://gitee.com/tiger7/doc/blob/master/com/ifname/lan.md)**( LAN口管理 )
- wisp connection component, first WISP name it **ifname@wisp**, second WISP name it **ifname@wisp2**, **[WISP Network Management](https://gitee.com/tiger7/doc/blob/master/com/ifname/wisp.md)**( 无线互联网管理 )

**clock** project, time management

- date component, name it **clock@date**, **[Manage System Date](https://gitee.com/tiger7/doc/blob/master/com/clock/date.md)**( 时间管理 )
- restart component, name it **clock@restart**, **[System restart Plan Management](https://gitee.com/tiger7/doc/blob/master/com/clock/restart.md)**( 重启计划管理 )

**wui** project, web server for user

- admin component, name it **wui@admin**, **[Administrator WEB Server Management](https://gitee.com/tiger7/doc/blob/master/com/wui/admin.md)**( WEB服务管理 )

**tui** project, terminal server for user

- ssh component, name it **tui@ssh**, **[SSH Server Management](https://gitee.com/tiger7/doc/blob/master/com/tui/ssh.md)**( SSH服务管理 )
- telnet component, name it **tui@telnet**, **[Telnet Server Management](https://gitee.com/tiger7/doc/blob/master/com/tui/telnet.md)**( TELNET服务管理 )

**wifi** project, all the 802.11abgn/ac interface

- nssid component, first SSID(2.4G) name it **wifi@nssid**, second SSID(2.4G) name it **wifi@nssid2**, **[2.4G SSID Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/nssid.md)**( 2.4G无线热点管理 )
- assid component, first SSID(5.8G) name it **wifi@assid**, second SSID(5.8G) name it **wifi@assid2**, **[5.8G SSID Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/assid.md)**( 5.8G无线热点管理 )
- nsta component, name it **wifi@nsta**, **[2.4G Station Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/nsta.md)**( 2.4G网卡管理 )
- asta component, name it **wifi@asta**, **[5.8G Station Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/asta.md)**( 5.8G网卡管理 )
- n component, name it **wifi@n**, **[2.4G Radio Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/n.md)**( 2.4G基带管理 )
- a component, name it **wifi@a**, **[5.8G Radio Management](https://gitee.com/tiger7/doc/blob/master/com/wifi/a.md)**( 5.8G基带管理 )

**modem** project, lte/nr modem

- lte modem component, first LTE/NR modem name it **modem@lte**, second LTE/NR modem name it **modem@lte2**, **[LTE/NR Modem Management](https://gitee.com/tiger7/doc/blob/master/com/modem/lte.md)**( LTE模块管理 )

**io** project, io management

- agent component, name it **io@agent**, **[IO Management Agent](https://gitee.com/tiger7/doc/blob/master/com/io/agent.md)**( IO口管理 )

**gnss** project, gps management

- nmea component, name it **gnss@nmea**, **[GNSS NEMA Protocol Management](https://gitee.com/tiger7/doc/blob/master/com/gnss/nmea.md)**( GNSS定位管理 )

**uart** project, uart management

- serial component, first serial name it **uart@serial**, second serial name it **uart@serial2**, **[UART Management](https://gitee.com/tiger7/doc/blob/master/com/uart/serial.md)**( 串口管理 )

**arch** project, Hardware related cross-platform components

- gpio component, name it **arch@gpio**, **[Hardware GPIO Management](https://gitee.com/tiger7/doc/blob/master/com/arch/gpio.md)**( GPIO管理 )
- ethernet component, name it **arch@ethernet**, **[Hardware Switch/Ethernet Management](https://gitee.com/tiger7/doc/blob/master/com/arch/ethernet.md)**( 以太网管理 )
- pci component, name it **arch@pci**, **[Hardware PCI Management](https://gitee.com/tiger7/doc/blob/master/com/arch/pci.md)**( PCI设备管理 )
- usb component, name it **arch@usb**, **[Hardware USB Management](https://gitee.com/tiger7/doc/blob/master/com/arch/usb.md)**( USB设备管理 )
- data component, name it **arch@data**, **[Hardware Data/EEPROM Management](https://gitee.com/tiger7/doc/blob/master/com/arch/data.md)**( 数据管理 )
- firmware component, name it **arch@firmware**, **[Firmware Management](https://gitee.com/tiger7/doc/blob/master/com/arch/firmware.md)**( 固件管理 )



# Communicate with SkinOS( 与网关交互 )

## HE Command calls component API management gateway( 命令行执行HE指令调用组件API管理网关 )

#### Login(登录)

Login to the gateway using TELNET or SSH or UART( 通过telnet/ssh/串口登陆到网关 )
```
dimmalex@HMS:~$ telnet 192.168.8.1
Trying 192.168.8.1...
Connected to 192.168.8.1.
Escape character is '^]'.

LTE&NR-Gateway login: admin
Password: 

               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
        Ashy   EEEEEEEEE   LLLLLLLLLL   FF

 -----------------------------------------------------------
 Command Help
 -----------------------------------------------------------
 @ ----------------------- List all the project
 @? ---------------------- List all the static component
 @@ ---------------------- List all the dynamic component
 <com> ------------------- Show component configure
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
 -----------------------------------------------------------
#
```
*The account is the same as that on the WEB management page( 帐号与WEB管理帐号相同 )*


#### HE Command Calling component API, Example Query the configuration/status of component land@machine( 调用接口, 示例查询组件land@machine的配置和状态 )

```json
# land@machine                         /* HE Command show the gateway basic configure */
{
    "mode":"misp",
    "name":"LTE&NR-Gateway",
    "mac":"00:03:7F:12:00:08",
    "macid":"00037F120008",
    "language":"en",
    "cfgversion":"8"
}
# land@machine.status                  /* HE Command show the gateway basic status */
{
    "mode":"gateway",
    "name":"LTE&NR-Gateway",
    "platform":"smtk2",
    "hardware":"mt7621",
    "custom":"d228",
    "scope":"std",
    "version":"v7.3.0916",
    "livetime":"19:39:04:2",
    "current":"23:10:10:10:08:2023",
    "mac":"00:03:7F:12:00:08",
    "model":"5228",
    "cmodel":"",
    "cfgversion":"8"
}
# 
```

**See this document for more command lines [HE command](https://gitee.com/tiger7/doc/blob/master/use/he_command.md)**
**更多命令行介绍见此文档 [HE指令介绍](https://gitee.com/tiger7/doc/blob/master/use/he_command.md)**



## TCP(JSON) calls component API management gateway( TCP协议调用组件API管理网关 )

Send commands after connecting to TCP port 22220, Example Query the configuration of component land@machine( 连接TCP端口22220后发送, 示例查询组件land@machine的配置 )

```json

{
    "cmd1":"land@machine"            /* Query the gateway basic configure {"cmd1":"land@machine"} */
}
```

Receive( 接收 )

```json
{
    "cmd1":
    {
        "mode":"misp",
        "name":"LTE&NR-Gateway",
        "mac":"00:03:7F:12:00:08",
        "macid":"00037F120008",
        "language":"en",
        "cfgversion":"8"
    }
}
```

Send commands after connecting to TCP port 22220, Example Query the status of component land@machine( 连接TCP端口22220后发送, 示例查询组件land@machine的状态 )

```json
{
    "cmd1":"land@machine.status"     /* Query the gateway basic status {"cmd1":"land@machine.status"} */
}
```

Receive( 接收 )

```json
{
    "cmd1":
    {
        "mode":"gateway",
        "name":"LTE&NR-Gateway",
        "platform":"smtk2",
        "hardware":"mt7621",
        "custom":"d228",
        "scope":"std",
        "version":"v7.3.0916",
        "livetime":"19:39:04:2",
        "current":"23:10:10:10:08:2023",
        "mac":"00:03:7F:12:00:08",
        "model":"5228",
        "cmodel":"",
        "cfgversion":"8"
    }
}
```


**See this document for more JSON-TCP protocol [TCP(JSON) Protocol](https://gitee.com/tiger7/doc/blob/master/protocol/localport_protocol.md)**
**更多JSON-TCP协议介绍见此文档 [TCP(JSON)协议](https://gitee.com/tiger7/doc/blob/master/protocol/localport_protocol.md)**



## HTTP(JSON) calls component API management gateway( HTTP协议调用组件API管理网关 )

#### Login(登录)

Method: POST
URL: http://GatewayIP:GatewayPORT/action/login
Content: username=XXXXXXX&password=XXXXXXX

1. Use username: admin password:admin to login and get the cookies( 使用用户名admin密码admin登录并获取cookies )

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/login -d'username=admin&password=admin' -c ./gateway.cookies
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

2. Receive on success( 成功时接收 )

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

3. Receive on failure( 失败时接收 )

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/login.html?warn=loginfailed">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

##### POST the HE command( Calling component API )( 调用接口, 示例查询组件land@machine的配置 )

POST the HE command to /action/he, the Content format is:
```
&he=UrlEncode(Base64(HE Command))&he2=UrlEncode(Base64(HE Command2))&&he2=UrlEncode(Base64(HE Command3))...
```


##### Example Query the configuration of component land@machine( 调用接口, 示例查询组件land@machine的配置 )

Method: POST
URL: http://GatewayIP:GatewayPORT/action/he
Content: &he=UrlEncode(Base64(land@machine))

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/he -d'&he=bGFuZEBtYWNoaW5l' -b ./gateway.cookies
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=
dimmalex@HMS:~$

```

Receive( 接收 )

```
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=通
```

Receive( 接收 ) and base64 decode

```
{"he":{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}}

```

Got this( 最终得到 )

```
{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}
```


**See this document for more command lines [HE command](https://gitee.com/tiger7/doc/blob/master/use/he_command.md)**
**更多命令行介绍见此文档 [HE指令介绍](https://gitee.com/tiger7/doc/blob/master/use/he_command.md)**


---



# SkinOS SDK download and compile( SDK下载 )

#### 1. Development environment download( 开发环境下载 )

Under Ubuntu ( 20.04 or 18.04 recommended ) run the following command to download the development environment ( please install git and make first )
在Ubuntu下（ 建议使用20.04或18.04 ）执行以下命令下载开发环境( 请先安装git及make )

```shell
git clone https://gitee.com/tiger7/tiger7.git
```
*Or download it from github*
```shell
git clone https://github.com/skinos7/tiger7.git
```

#### 2. Install the necessary development tools( 安装开发工具 )

```shell
cd tiger7
make ubuntu_preset
```

#### 3. Specify the product model to be developed( 指定产品型号 )

```shell
make pid gBOARDID=<Product complete model>

# For products such as D218 enter the following instructions:
make pid gBOARDID=mtk2-mt7628-d218

# For products such as 2218/3218/5218 enter the following instructions:
make pid gBOARDID=mtk2-mt7628-r600

# For products such as D228 enter the following instructions:
make pid gBOARDID=mtk2-mt7621-d228

# For products such as D228P enter the following instructions:
make pid gBOARDID=mtk2-mt7621-d228p

```

#### 4. Download the SDK corresponding to the product model( 下载产品型号对应的SDK )

```shell
make update
```

#### 5. Update the application menu( 更新软件菜单 )

```shell
make menu
make menuconfig
```  
***Note: Save and exit directly when show the menu***
***注意: 显示菜单后直接退出并保存即可***

#### 6. Compile gateway firmware( 编译网关固件 )

```shell
make dep
make
```
After 2 to 5 hours, the gateway firmware upgrade package ending in **.zz** will be generated in the tiger7/build directory
等待大约2至5个小时后会在tiger7/build目录下生成以 **.zz** 结尾的网关固件升级包



#### 7. Upgrade the firmware to the gateway( 将固件升级到网关 )

In the gateway web page management interface, **System => Software => Upgrade** Click the firmware that ending in **.zz** to upgrading
在网关网页管理界面中的 **系统=>软件管理=>软件更新** 来点选.zz结尾的固件升级包来升级固件

![avatar](./upgrade.png)



---



# 项目的开发指导

- **[项目及组件开发](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)**
介绍及演示SkinSDK **项目(project)** 开发步骤, 并以示例的方式演示如何开发一个功能

- **[移植Openwrt项目到SkinSDK中-简单版](https://gitee.com/tiger7/doc/blob/master/dev/porting_openwrt.md)**
介绍及演示移植Openwrt下的开源项目到SkinSDK中，并使其开机运行

- **[移植Openwrt项目到SkinSDK中-进阶版](https://gitee.com/tiger7/doc/blob/master/dev/porting_openwrt_adv.md)**
介绍及演示移植Openwrt下的开源项目到SkinSDK中, 并为其开发WEB管理界面, 允许用户通过WEB管理界或CLI对其管理


# 项目的开发指导 --- 正在编写中

- [在项目中编写Linux可执行程序](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
演示在SkinSDK **项目(project)** 中开发Linux的可执行程序

- [在项目中编写共享库](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
演示在SkinSDK **项目** 中开发共享动态库

- [在项目中编写内核模块](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
演示在SkinSDK **项目** 中开发内核模块

- [在项目中编写脚本程序](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
演示在SkinSDK **项目** 中编写Shell等脚本程序

- [移植Linux程序到SkinSDK中-简单版](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍及演示移植Linux程序到SkinSDK中，并使其开机运行

- [移植Linux程序到SkinSDK中-进阶版](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍及演示移植Linux程序到SkinSDK中, 并为其开发WEB管理界面, 允许用户通过WEB管理界或CLI对其管理

- [在modemdrv项目下添加LTE/NR模块驱动](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍在如何加入新的LTE/NR模块的驱动, 让SkinSDK支持更多的4G或5G模块的驱动

- [自定义网络连接模式](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍在自定义网络连接策略, 实现更复杂的连接逻辑

- [自定义默认配置](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍在如何自定义产品的默认配置, 允许自定义设备在恢复出厂后的原始配置

- [SkinOS的事件系统](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍SkinOS的事件, 通过注册事件事件来触发操作

- [SkinOS的寄存变量系统](https://gitee.com/tiger7/doc/blob/master/dev/beginner_development.md)
介绍通过寄存变量系统实现简单的组件间交互, 组件生存周期之外保存信息



##### 也可以通过开发接口调用， 具体参见以下两个文件:
- **[编程中调用组件接口](https://gitee.com/tiger7/doc/blob/master/dev/call_component.md)**
- **[编程中管理组件配置](https://gitee.com/tiger7/doc/blob/master/dev/component_config.md)**






