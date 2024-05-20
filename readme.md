---
title: "Readme"
author: dimmalex@gmail.com
date: March 22, 2022
output:
    word_document:
        path: D:/tmp/Readme.docx
---


# SkinOS system framework

- SkinOS encapsulates the manageable objects of Linux through the **component**, providing the interface(API) to the upper layer
- The objects that can be managed in SkinOS are all individual components
- Multiple related or similar functional components make up a **project**   

![avatar](./frame.jpg)


# SkinOS project introduction   
- Applications in SkinOS is **project**
- **project** There is a corresponding directory in the SDK, which stores the source code and various resource files related to the project
- **project** corresponding directory all are stored in the **[tiger7/project](https://github.com/skinos7/tiger7/tree/master/project)** directory
- **project** will generate **FPK** after compiling, which is used to install into SkinOS
- The following is structure **project**   


![avatar](./project.png)

**[prj.json](https://github.com/skinos7/doc/blob/master/dev/prj.json.md)** Describe all of the project's resources in JSON format   
- Guide the compilation process at compiling
- Guide the installation process during installation
- The boot option is registered during SkinOS startup
- Register the shutdown item when the SkinOS shuts down
- Register event handlers to SkinOS
- Register the web menu(page) to SkinOS   

**Component** Provide a unified management interface to the SkinOS or users   
**Default Configure** Default setting   
**HTML** The web page file corresponding to the web menu   
**Shell** Shell script file   
**Open Source Program** Open source application   
**Linux Executable Program** executable program   
**Shared Library**    
**Kernel Module**   


# SkinOS project framework

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


SkinOS system components manage the project through **Skin Layer**

![avatar](./project3.png)





# SkinOS component interface

### Common component API   
The following figure describes common project and component interfaces of gateway

![avatar](./component.png)

**land** project, SkinOS core project, manage gateway basic information
- machine component, name it **land@machine**, The alias is **machine**, **[Management of Basic Infomation](https://github.com/skinos7/doc/blob/master/com/land/machine.md)**   
- service component, name it **land@service**, The alias is **service**, **[Management of System Service](https://github.com/skinos7/doc/blob/master/com/land/service.md)**   
- auth component, name it **land@auth**, The alias is **auth**, **[Username/Password and Permission Management](https://github.com/skinos7/doc/blob/master/com/land/auth.md)**   
- syslog component, name it **land@syslog**, The alias is **log**, **[Syslog Management](https://github.com/skinos7/doc/blob/master/com/land/syslog.md)**   

**client** project, Manage all clients   
- global component, name it **client@global**,  **[Management of Global Access](https://github.com/skinos7/doc/blob/master/com/client/global.md)**   
- station component, name it **client@station**,  **[Management of Client Access](https://github.com/skinos7/doc/blob/master/com/client/station.md)**   

**forward** project, Manage forward functions   
- alg component, name it **forward@alg**, **[Management of Application Layer Gateway](https://github.com/skinos7/doc/blob/master/com/forward/alg.md)**   
- route component, name it **forward@route**, **[System Route Table Management](https://github.com/skinos7/doc/blob/master/com/forward/route.md)**   
- firewall component, name it **forward@firewall**, **[Management of System Firewall](https://github.com/skinos7/doc/blob/master/com/forward/firewall.md)**   
- nat component, name it **forward@nat**, **[Management of System NAT/DMZ](https://github.com/skinos7/doc/blob/master/com/forward/nat.md)**   

**ifname** project, Manage all connections of gateway   
- lte connection component, first LTE name it **ifname@lte**, second LTE name it **ifname@lte2**, **[LTE/NR Network Management](https://github.com/skinos7/doc/blob/master/com/ifname/lte.md)**   
- wan connection component, first WAN name it **ifname@wan**, second WAN name it **ifname@wan2**, **[WAN Network Management](https://github.com/skinos7/doc/blob/master/com/ifname/wan.md)**   
- lan connection component, first LAN name it **ifname@lan**, second LAN name it **ifname@lan2**, **[LAN Network Management](https://github.com/skinos7/doc/blob/master/com/ifname/lan.md)**   
- wisp connection component, first WISP name it **ifname@wisp**, second WISP name it **ifname@wisp2**, **[WISP Network Management](https://github.com/skinos7/doc/blob/master/com/ifname/wisp.md)**   

**clock** project, time management   
- date component, name it **clock@date**, **[Manage System Date](https://github.com/skinos7/doc/blob/master/com/clock/date.md)**   
- restart component, name it **clock@restart**, **[System restart Plan Management](https://github.com/skinos7/doc/blob/master/com/clock/restart.md)**   
- ntps component, name it **clock@ntps**, **[NTP Server Management](https://github.com/skinos7/doc/blob/master/com/clock/ntps.md)**   

**wui** project, web server for user   
- admin component, name it **wui@admin**, **[Administrator WEB Server Management](https://github.com/skinos7/doc/blob/master/com/wui/admin.md)**   

**tui** project, terminal server for user   
- ssh component, name it **tui@ssh**, **[SSH Server Management](https://github.com/skinos7/doc/blob/master/com/tui/ssh.md)**   
- telnet component, name it **tui@telnet**, **[Telnet Server Management](https://gitee.com/tiger7/doc/blob/master/com/tui/telnet.md)**   

**wifi** project, all the 802.11abgn/ac interface   
- nssid component, first SSID(2.4G) name it **wifi@nssid**, second SSID(2.4G) name it **wifi@nssid2**, **[2.4G SSID Management](https://github.com/skinos7/doc/blob/master/com/wifi/nssid.md)**   
- assid component, first SSID(5.8G) name it **wifi@assid**, second SSID(5.8G) name it **wifi@assid2**, **[5.8G SSID Management](https://github.com/skinos7/doc/blob/master/com/wifi/assid.md)**   
- nsta component, name it **wifi@nsta**, **[2.4G Station Management](https://github.com/skinos7/doc/blob/master/com/wifi/nsta.md)**   
- asta component, name it **wifi@asta**, **[5.8G Station Management](https://github.com/skinos7/doc/blob/master/com/wifi/asta.md)**   
- n component, name it **wifi@n**, **[2.4G Radio Management](https://github.com/skinos7/doc/blob/master/com/wifi/n.md)**   
- a component, name it **wifi@a**, **[5.8G Radio Management](https://github.com/skinos7/doc/blob/master/com/wifi/a.md)**   

**modem** project, lte/nr modem   
- lte modem component, first LTE/NR modem name it **modem@lte**, second LTE/NR modem name it **modem@lte2**, **[LTE/NR Modem Management](https://github.com/skinos7/doc/blob/master/com/modem/lte.md)**   

**io** project, io management   
- agent component, name it **io@agent**, **[IO Management Agent](https://github.com/skinos7/doc/blob/master/com/io/agent.md)**   

**gnss** project, gps management   
- nmea component, name it **gnss@nmea**, **[GNSS NEMA Protocol Management](https://github.com/skinos7/doc/blob/master/com/gnss/nmea.md)**   

**uart** project, uart management   
- serial component, first serial name it **uart@serial**, second serial name it **uart@serial2**, **[UART Management](https://github.com/skinos7/doc/blob/master/com/uart/serial.md)**   

**arch** project, Hardware related cross-platform components   
- gpio component, name it **arch@gpio**, **[Hardware GPIO Management](https://github.com/skinos7/doc/blob/master/com/arch/gpio.md)**   
- ethernet component, name it **arch@ethernet**, **[Hardware Switch/Ethernet Management](https://github.com/skinos7/doc/blob/master/com/arch/ethernet.md)**   
- pci component, name it **arch@pci**, **[Hardware PCI Management](https://github.com/skinos7/doc/blob/master/com/arch/pci.md)**   
- usb component, name it **arch@usb**, **[Hardware USB Management](https://github.com/skinos7/doc/blob/master/com/arch/usb.md)**   
- data component, name it **arch@data**, **[Hardware Data/EEPROM Management](https://github.com/skinos7/doc/blob/master/com/arch/data.md)**   
- firmware component, name it **arch@firmware**, **[Firmware Management](hhttps://github.com/skinos7/doc/blob/master/com/arch/firmware.md)**   



# Communicate with SkinOS

## HE Command calls component API management gateway

#### 1. Login   
Login to the gateway using TELNET or SSH or UART   
*The account is the same as that on the WEB management page*   

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



#### 2. HE Command Calling component API, Example Query the configuration/status of component land@machine   

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
# land@machine.status                  # HE Command show the gateway basic status
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

**See this document for more command lines [HE command](https://github.com/skinos7/doc/blob/master/use/he_command_en.md)**


## TCP(JSON) calls component API management gateway

Send commands after connecting to TCP port 22220, Example Query the configuration of component land@machine   

```json

{
    "cmd1":"land@machine"            /* Query the gateway basic configure {"cmd1":"land@machine"} */
}
```

Receive   

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

Send commands after connecting to TCP port 22220, Example Query the status of component land@machine   

```json
{
    "cmd1":"land@machine.status"     /* Query the gateway basic status {"cmd1":"land@machine.status"} */
}
```

Receive   

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

**See this document for more JSON-TCP protocol [TCP(JSON) Protocol](https://github.com/skinos7/doc/blob/master/protocol/localport_protocol_en.md)**   



## HTTP(JSON) calls component API management gateway

#### 1. Login   
Method: POST
URL: http://GatewayIP:GatewayPORT/action/login
Content: username=XXXXXXX&password=XXXXXXX   
Use username: admin password:admin to login and get the cookies   

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/login -d'username=admin&password=admin' -c ./gateway.cookies
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

Receive on success   

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/index.html">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

Receive on failure   

```
<html><head></head><body>
        This document has moved to a new <a href="http://192.168.8.1/login.html?warn=loginfailed">location</a>.
        Please update your documents to reflect the new location.
</body></html>
```

#### 2. POST the HE command( Calling component API )
POST the HE command to /action/he, the Content format is:

```
&he=UrlEncode(Base64(HE Command))&he2=UrlEncode(Base64(HE Command2))&&he2=UrlEncode(Base64(HE Command3))...
```


#### 3. Example Query the configuration of component land@machine   
Method: POST
URL: http://GatewayIP:GatewayPORT/action/he
Content: &he=UrlEncode(Base64(land@machine))   

```
dimmalex@HMS:~$ curl -XPOST http://192.168.8.1/action/he -d'&he=bGFuZEBtYWNoaW5l' -b ./gateway.cookies
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=
dimmalex@HMS:~$

```

Receive   

```
eyJoZSI6eyJtb2RlIjoibWlzcCIsIm5hbWUiOiJMVEUmTlItR2F0ZXdheSIsIm1hYyI6IjAwOjAzOjdGOjEyOjAwOjA4IiwibWFjaWQiOiIwMDAzN0YxMjAwMDgiLCJsYW5ndWFnZSI6ImVuIiwiY2ZndmVyc2lvbiI6IjgifX0=通
```

Receive and base64 decode   

```
{"he":{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}}
```

Got this   

```
{"mode":"misp","name":"LTE&NR-Gateway","mac":"00:03:7F:12:00:08","macid":"00037F120008","language":"en","cfgversion":"8"}
```

**See this document for more command lines [HE command](https://github.com/skinos7/doc/blob/master/use/he_command.md)**



---



# SkinOS SDK download and compile

#### 1. Development environment download   
Under Ubuntu ( 20.04 or 18.04 recommended ) run the following command to download the development environment ( please install git and make first )   

```shell
git clone https://github.com/skinos7/tiger7.git
```

*Or download it from gitee*
> *或者从gitee上下载*

```shell
git clone https://gitee.com/tiger7/tiger7.git
```

#### 2. Install the necessary development tools   

```shell
cd tiger7
make ubuntu_preset
```

#### 3. Specify the product model to be developed   

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

#### 4. Download the SDK corresponding to the product model   

```shell
make update
```

#### 5. Update the application menu   

```shell
make menu
make menuconfig
```  
***Note: Save and exit directly when show the menu***

#### 6. Compile gateway firmware   

```shell
make dep
make
```
After 2 to 5 hours, the gateway firmware upgrade package ending in **.zz** will be generated in the tiger7/build directory   



#### 7. Upgrade the firmware to the gateway   
In the gateway web page management interface, **System => Software => Upgrade** Click the firmware that ending in **.zz** to upgrading   

![avatar](./upgrade.png)

# *[中文说明入口](./readme-cn.md)*

