---
title: "Readme-cn"
author: dimmalex@gmail.com
date: March 22, 2022
output:
    word_document:
        path: D:/tmp/Readme-cn.docx
---



# SkinOS系统的来由

- Linux, Ubuntu, Openwrt未统一或未完全统一开发者与管理员之间的接口, 导致对管理员及开发者重复学习及重复开发
- SkinOS的最重要的目标是向开发者与管理员之间提供一套 **统一的接口** 规范
- 开发者以提供 **统一的接口** 的要求开发应用, 并给出统一规范的接口文档
- 管理员及使用者通过 **统一的接口** 对应用进行管理
- 管理员也可通过 **统一的接口** 基于应用进一步开发应用或脚本实现定制化
- 基于 **统一的接口** 在网络层面实现远程调用, 实现更统一更智能的 **物联网**
- 基于 **统一的接口** 实现跨平台

![avatar](./idea.png)


# SkinOS API简介

![avatar](./interface.png)

- 以组件的方式对象化系统中所有硬件及功能, 向上提供 **接口** 及 **配置**

    - 组件的配置, 为功能项或硬件的配置, 一个JSON格式的对象, 掉电保存
        - 管理者可 **命令行** , **远程** , **编程** 查询组件的配置
        - 管理者可 **命令行** , **远程** , **编程** 修改组件的配置

    - 组件接口(API), 功能项或硬件向外提供的接口, 完成指定的功能或信息的交互
        - 管理者可 **命令行** 调用组件的接口
        - 管理者可 **远程** 调用组件的接口
        - 管理者可 **编程** 调用组件的接口

- **管理员和使用者** 对 **硬件或功能项** 以上面介绍的两种方式对其管理和交互


# SkinOS系统架构

- SkinOS中通过 **组件** 对Linux的硬件及功能封装，向上层提供调用接口
- SkinOS中的 **硬件或功能项** 都是一个个 **组件**
- 多个相关硬件或功能项的组件组成一个 **项目**
- **组件** 又可通过 **SkinOS API** 相互调用及依赖, 实现复用

![avatar](./frame.jpg)



# 常用的SkinOS API

下图为网关常用的 **功能项及硬件** 对应的组件

![avatar](./component.png)

**land** 项目, SkinOS核心项目, 管理网关的基本功能   
- **[设备基本信息](https://gitee.com/tiger7/doc/blob/master/com/land/machine.md)** , machine组件, 命名为 **land@machine**, 别名为 **machine**   
- **[系统服务](https://gitee.com/tiger7/doc/blob/master/com/land/service.md)**, service组件, 命名为 **land@service**, 别名为 **service**   
- **[用户名密码及权限](https://gitee.com/tiger7/doc/blob/master/com/land/auth.md)**, auth组件, 命名为 **land@auth**, 别名为 **auth**   
- **[日志管理](https://gitee.com/tiger7/doc/blob/master/com/land/syslog.md)**, syslog组件, 命名为 **land@syslog**, 别名为 **log**   

**client** 项目, 管理所有客户端   
- **[访问控制](https://gitee.com/tiger7/doc/blob/master/com/client/global.md)**, global组件, 命名为 **client@global**   
- **[终端管理](https://gitee.com/tiger7/doc/blob/master/com/client/station.md)**, station组件, 命名为 **client@station**   

**forward** 项目, 管理路由转发功能   
- **[应用层网关](https://gitee.com/tiger7/doc/blob/master/com/forward/alg.md)**, alg组件, 命名为 **forward@alg**  
- **[默认路由表](https://gitee.com/tiger7/doc/blob/master/com/forward/route.md)**, route组件, 命名为 **forward@route**   
- **[防火墙](https://gitee.com/tiger7/doc/blob/master/com/forward/firewall.md)**, firewall组件, 命名为 **forward@firewall**   
- **[端口映射](https://gitee.com/tiger7/doc/blob/master/com/forward/nat.md)**, nat组件, 命名为 **forward@nat**   

**ifname** 项目, 管理网关所有的网络连接   
- **[LTE/NR(4G/5G)网络](https://gitee.com/tiger7/doc/blob/master/com/ifname/lte.md)**, lte连接组件, 第一个LTE命名为 **ifname@lte**, 第二个LTE命名为 **ifname@lte2**   
- **[有线宽带(WAN)](https://gitee.com/tiger7/doc/blob/master/com/ifname/wan.md)**, wan连接组件, 第一个WAN命名为 **ifname@wan**, 第二个WAN命名为 **ifname@wan2**   
- **[本地网络(LAN)](https://gitee.com/tiger7/doc/blob/master/com/ifname/lan.md)**, lan连接组件, 第一个LAN命名为 **ifname@lan**, 第二个LAN命名为 **ifname@lan2**   
- **[无线连网(WISP)](https://gitee.com/tiger7/doc/blob/master/com/ifname/wisp.md)**, wisp连接组件, 第一个WISP命名为 **ifname@wisp**, 第二个WISP命名为 **ifname@wisp2**   

**clock** 项目, 时间相关的管理   
- **[系统时间](https://gitee.com/tiger7/doc/blob/master/com/clock/date.md)**, date组件, 命名为 **clock@date**   
- **[自动重启](https://gitee.com/tiger7/doc/blob/master/com/clock/restart.md)**, restart组件, 命名为 **clock@restart**   
- **[NTP服务器](https://gitee.com/tiger7/doc/blob/master/com/clock/ntps.md)**, ntps组件, 命名为 **clock@ntps**   

**wui** 项目, WEB服务   
- **[WEB服务器](https://gitee.com/tiger7/doc/blob/master/com/wui/admin.md)**, admin组件, 命名为 **wui@admin**   

**tui** 项目, 终端命令服务   
- **[SSH服务器](https://gitee.com/tiger7/doc/blob/master/com/tui/ssh.md)**, ssh组件, 命名为 **tui@ssh**    
- **[TELNET服务器](https://gitee.com/tiger7/doc/blob/master/com/tui/telnet.md)**, telnet组件, 命名为 **tui@telnet**   

**wifi** 项目, 所有802.11abgn/ac无线接口管理   
- **[2.4G热点](https://gitee.com/tiger7/doc/blob/master/com/wifi/nssid.md)**, nssid组件, 第一个SSID(2.4G)命名为 **wifi@nssid**, 第二个SSID(2.4G)命名为 **wifi@nssid2**   
- **[5.8G热点](https://gitee.com/tiger7/doc/blob/master/com/wifi/assid.md)**, assid组件, 第一个SSID(5.8G)命名为 **wifi@assid**, 第二个SSID(5.8G)命名为 **wifi@assid2**   
- **[2.4G网卡](https://gitee.com/tiger7/doc/blob/master/com/wifi/nsta.md)**, nsta组件, 命名为 **wifi@nsta**   
- **[5.8G网卡](https://gitee.com/tiger7/doc/blob/master/com/wifi/asta.md)**, asta组件, 命名为 **wifi@asta**   
- **[2.4G基带](https://gitee.com/tiger7/doc/blob/master/com/wifi/n.md)**, n组件, 命名为 **wifi@n**   
- **[5.8G基带](https://gitee.com/tiger7/doc/blob/master/com/wifi/a.md)**, a组件, 命名为 **wifi@a**   

**modem** 项目, LTE/NR基带管理   
- **[LTE/NR(4G/5G)基带](https://gitee.com/tiger7/doc/blob/master/com/modem/lte.md)**, lte组件, 第一个LTE/NR基带命名为 **modem@lte**, 第二个LTE/NR基带命名为 **modem@lte2**   

**io** 项目, IO口管理及相关协议的实现   
- **[IO口管理](https://gitee.com/tiger7/doc/blob/master/com/io/agent.md)**, agent组件, 命名为 **io@agent**   

**gnss** 项目, 定位功能及相关的协议实现   
- **[全球定位](https://gitee.com/tiger7/doc/blob/master/com/gnss/nmea.md)**, nmea组件, 命名为 **gnss@nmea**   

**uart** 项目, 串口(UART)相关的功能及协议实现  
- **[串口管理](https://gitee.com/tiger7/doc/blob/master/com/uart/serial.md)**, serial组件, 第一个UART命名为 **uart@serial**, 第二个UART命名为 **uart@serial2**   

**arch** 项目, 底层硬件相关的项目   
- **[GPIO管理](https://gitee.com/tiger7/doc/blob/master/com/arch/gpio.md)**, gpio组件, 命名为 **arch@gpio**   
- **[以太网管理](https://gitee.com/tiger7/doc/blob/master/com/arch/ethernet.md)**, ethernet组件, 命名为 **arch@ethernet**   
- **[PCI设备管理](https://gitee.com/tiger7/doc/blob/master/com/arch/pci.md)**, pci组件, 命名为 **arch@pci**   
- **[USB设备管理](https://gitee.com/tiger7/doc/blob/master/com/arch/usb.md)**, usb组件, 命名为 **arch@usb**   
- **[数据管理](https://gitee.com/tiger7/doc/blob/master/com/arch/data.md)**, data组件, 命名为 **arch@data**   
- **[固件管理](https://gitee.com/tiger7/doc/blob/master/com/arch/firmware.md)**, firmware组件, 命名为 **arch@firmware**   



# 操作SkinOS

## 在终端使用HE指令操作网关

##### 1. 登录
通过Telnet/SSH/串口登陆到网关
*帐号与WEB管理帐号相同*

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


##### 2. 查询配置, 示例查询设备基本配置, 即查询组件land@machine的配置, 命令行中给出组件名即可

```shell
# land@machine                         # HE Command show the gateway basic configure
{
    "mode":"misp",
    "name":"LTE&NR-Gateway",
    "mac":"00:03:7F:12:00:08",
    "macid":"00037F120008",
    "language":"en",
    "cfgversion":"8"
}
```

##### 3. 修改配置, 示例修改设备的名称为MyGateway, 即修改组件land@machine的name的值

```shell
# land@machine:name=MyGateway          # HE Command modify the gateway basic configure
ttrue
```


##### 4. 调用接口, 示例获取设备基本状态, 即调用组件land@machine的status接口, 命令行中给出组件名及接口名称
```
# land@machine.status                  # HE Command show the gateway basic status
{
    "mode":"gateway",
    "name":"MyGateway",
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

**更多命令行介绍见此文档 [HE指令介绍](https://gitee.com/tiger7/doc/blob/master/use/he/he_command_cn.md)**



## 使用TCP(JSON)协议操作网关

##### 连接TCP端口22220后发送, 示例查询设备基本配置, 即查询组件land@machine的配置

```json

{
    "cmd1":"land@machine"            /* Query the gateway basic configure {"cmd1":"land@machine"} */
}
```

接收

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

##### 连接TCP端口22220后发送, 示例修改设备的名称为MyGateway, 即修改组件land@machine的name的值

```json

{
    "cmd1":"land@machine:name=MyGateway"     /* Modify the gateway basic configure {"cmd1":"land@machine:name=MyGateway"} */
}
```

接收

```json
{
    "cmd1":"ttrue"
}
```



##### 连接TCP端口22220后发送, 示例获取设备基本状态, 即调用组件land@machine的status接口

```json
{
    "cmd1":"land@machine.status"     /* Query the gateway basic status {"cmd1":"land@machine.status"} */
}
```

接收

```json
{
    "cmd1":
    {
        "mode":"gateway",
        "name":"MyGateway",
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
**注意此功能默认关闭, 使用此功能需要通过界面打开, 更多TCP(JSON)协议介绍见此文档 [TCP(JSON)协议](https://gitee.com/tiger7/doc/blob/master/protocol/localport/localport_protocol_cn.md)**



## 使用HTTP(JSON)协议操作网关

##### 1. 登录
方法: POST
URL: http://GatewayIP:GatewayPORT/action/login
内容: username=XXXXXXX&password=XXXXXXX
使用用户名admin密码admin登录并获取cookies

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/login -d'username=admin&password=admin' -c ./gateway.cookies
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

成功时收到

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

失败时收到

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/login.html?warn=loginfailed">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

##### 2. 调用接口
POST HE指令 到/action/he, POST格式如下:

```
&he=UrlEncode(Base64(HE Command))&he2=UrlEncode(Base64(HE Command2))&&he2=UrlEncode(Base64(HE Command3))...
```


##### 示例查询设备基本配置, 即查询组件land@machine的配置   

方法: POST
URL: http://GatewayIP:GatewayPORT/action/he
内容: &he=UrlEncode(Base64(land@machine))   

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/he -d'&he=bGFuZEBtYWNoaW5l' -b ./gateway.cookies
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=
dimmalex@HMS:~$
```

收到   

```
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=
```

收到并使用base64解码   

```
{"he":{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}}
```

最终得到   

```
{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}
```


##### 示例修改设备的名称为MyGateway, 即修改组件land@machine的name的值   

方法: POST
URL: http://GatewayIP:GatewayPORT/action/he
内容: &he=UrlEncode(Base64(land@machine:name=MyGateway))   

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/he -d'&he=bGFuZEBtYWNoaW5lOm5hbWU9TXlHYXRld2F5' -b ./gateway.cookies
eyJoZSI6dHJ1ZX0=
dimmalex@HMS:~$
```

收到   

```
eyJoZSI6dHJ1ZX0=
```

收到并使用base64解码   

```
{"he":true}
```

最终得到true表示操作成功

```
true
```

##### 示例获取设备基本状态, 即调用组件land@machine的status接口   

方法: POST
URL: http://GatewayIP:GatewayPORT/action/he
内容: &he=UrlEncode(Base64(land@machine.status))   

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/he -d'&he=bGFuZEBtYWNoaW5lLnN0YXR1cw==' -b ./gateway.cookies
eyJoZSI6eyJtb2RlIjoiZ2F0ZXdheSIsIm5hbWUiOiJNeUdhdGV3YXkiLCJwbGF0Zm9ybSI6InNtdGsyIiwiaGFyZHdhcmUiOiJtdDc2MjEiLCJjdXN0b20iOiJkMjI4Iiwic2NvcGUiOiJzdGQiLCJ2ZXJzaW9uIjoidjcuMy4wOTE2IiwibGl2ZXRpbWUiOiIxOTozOTowNDoyIiwiY3VycmVudCI6IjIzOjEwOjEwOjEwOjA4OjIwMjMiLCJtYWMiOiIwMDowMzo3RjoxMjowMDowOCIsIm1vZGVsIjoiNTIyOCIsImNtb2RlbCI6IiIsImNmZ3ZlcnNpb24iOiI4In19
dimmalex@HMS:~$
```

收到   

```
eyJoZSI6eyJtb2RlIjoiZ2F0ZXdheSIsIm5hbWUiOiJNeUdhdGV3YXkiLCJwbGF0Zm9ybSI6InNtdGsyIiwiaGFyZHdhcmUiOiJtdDc2MjEiLCJjdXN0b20iOiJkMjI4Iiwic2NvcGUiOiJzdGQiLCJ2ZXJzaW9uIjoidjcuMy4wOTE2IiwibGl2ZXRpbWUiOiIxOTozOTowNDoyIiwiY3VycmVudCI6IjIzOjEwOjEwOjEwOjA4OjIwMjMiLCJtYWMiOiIwMDowMzo3RjoxMjowMDowOCIsIm1vZGVsIjoiNTIyOCIsImNtb2RlbCI6IiIsImNmZ3ZlcnNpb24iOiI4In19
```

收到并使用base64解码   

```
{"he":{"mode":"gateway","name":"MyGateway","platform":"smtk2","hardware":"mt7621","custom":"d228","scope":"std","version":"v7.3.0916","livetime":"19:39:04:2","current":"23:10:10:10:08:2023","mac":"00:03:7F:12:00:08","model":"5228","cmodel":"","cfgversion":"8"}}
```

最终得到

```
{"mode":"gateway","name":"MyGateway","platform":"smtk2","hardware":"mt7621","custom":"d228","scope":"std","version":"v7.3.0916","livetime":"19:39:04:2","current":"23:10:10:10:08:2023","mac":"00:03:7F:12:00:08","model":"5228","cmodel":"","cfgversion":"8"}
```


**更多HE指令介绍见此文档 [HE指令介绍](https://gitee.com/tiger7/doc/blob/master/use/he/he_command_cn.md)**



---



