***
## 通过网页获到设备信息
以下介绍通过设备的网页接口免登录获取设备信息, 通过网页访问/pub下的指定json可以以JSON格式获取各种设备信息, 如:
- 设备基本信息, 包括设备型号, MAC地址, 名称, 工作模式, 配置版本, 设备当前时间, 开机时长, 软件版本等
- LAN相关的信息, 设备IP地址及流量等信息
- LTE/WAN/WISP等联网相关的信息, LTE的状态, IP地址, 流量, 信号, 运营商信息, WAN口的IP地址, 连接状态, WISP对端的SSID名称, 信道等
- GPS相关的信息, 经纬度, 海拨, 速度等信息, *要求打开GPS功能且设备属于具备GPS功能的型号才可获到取*
- SSID相关的状态信息, SSID的名称, 收发字节, 信道号等
- 客户端列表, 客户端的名称, IP地址, MAC地址, 发收字节数等


#### **获取设备常用信息**

通过访问http://设备IP地址/pub/basic_status.json获取设备常用的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/basic_status.json
```
设备返回JSON数据介绍如下: 
```
{   
    // 设备的基本设置
    "land@machine":
    {
        // main attributes
        "mode":"gateway operator mode",                 // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                        // "ap": access point
                                                        // "wisp": 2.4G Wireless Internet Service Provider connection
                                                        // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wirless baseband)
                                                        // "gateway": wire WAN gateway
                                                        // "dgateway": Dual wire WAN gateway
                                                        // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                        // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                        // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                        // "mix": custom mix connection from multiple internet connection
        "name":"gateway name",                          // < string >, The name cannot contain spaces
        "mac":"gateway MAC address",                    // < mac address >
        "macid":"gateway MAC identify or serial id",    // < string >
        "language":"gateway language",                  // [ "cn", "en", "jp", ... ], language code in two letter, "cn" for Chinese, “en" for English
        // other attributes
        "cfgversion":"gateway configure version",       // [ string ]
        "gpversion":"gateway group configure version",  // [ string ]


        // 示列
        "mode":"nmisp",                              # gateway operator mode Next Mobile(LTE/NR) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                     # gateway name is ASHYELF-12AAD0
        "mac":"00:03:7F:12:AA:D0",                   # gateway MAC is 88:12:4E:20:46:B0
        "macid":"00037F12AAD0",                      # gateway serial id is 88124E2046B0
        "language":"cn",                             # gateway language is chinese
        "cfgversion":"1"                             # gateway configure version is 1(hint modify one times)

    },



    // 设备基本状态
    "land@machine.status":
    {
        "mode":"gateway operator mode",              // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                         // "ap": access point
                                                         // "wisp": 2.4G Wireless Internet Service Provider connection
                                                         // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wirless baseband)
                                                         // "gateway": wire WAN gateway
                                                         // "dgateway": Dual wire WAN gateway
                                                         // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                         // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                         // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                         // "mix": custom mix connection from multiple internet connection
        "name":"gateway name",
        "platform":"gateway platform identify",
        "hardware":"gateway hardware identify",
        "custom":"gateway custom identify",
        "scope":"gateway scope identify",
        "version":"gateway version",
        "livetime":"system running time",                // hour:minute:second:day
        "current":"current date",                        // hour:minute:second:month:day:year
        "mac":"gateway MAC address",
        "macid":"gateway MAC identify or serial id",
        "model":"gateway model",
        "magic":"gateway magic identify",


        // 示列
        "mode":"nmisp",                                   # gateway operator mode Next Mobile(NR/LTE) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                          # gateway name is ASHYELF-12AAD0
        "platform":"smtk2",                               # gateway platform identify is smtk2( hint the basic sdk is second release of smtk )
        "hardware":"mt7621",                              # gateway hardware identify is mt7621( hint the chip is MT7621 )
        "custom":"d228",                                  # gateway custom identify is d228( hint the product name is D228 )
        "scope":"std",                                    # gateway scope identify is std( hint this is a standand release for D228 )
        "version":"tiger7-20220218",                      # gateway version is tiger7-20220218( hint the version publish at 2022.02.18 )
        "livetime":"00:06:35:0",                          # gateway It has been running for 6 minutes and 35 seconds
        "current":"14:54:30:05:04:2022",                  # current date is 2022.05.04, It's 14:54 and 30 seconds
        "mac":"00:03:7F:12:AA:D0",                        # gateway MAC is 00:03:7F:12:AA:D0
        "macid":"00037F12AAD0",                           # gateway serial id is 00037F12AAD0
        "model":"5228",                                   # gateway model is 5228
        "magic":"870E2935E4605D02"                        # gateway magic is 870E2935E4605D02  
    },



    // LAN相关的信息
    "ifname@lan.status":
    {
        "status":"Current status",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the ifname is down
                                             // "up" for the network is connect succeed
        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "method":"IPv6 address mode",   // [ "disable" ] is not use ipv6, [ "manual" ] for manual setting, [ "automatic" ] for DHCPv6, [ "slaac" ] for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ]
        "addr2":"IPv6 address2",        // [ ipv6 address ]
        "addr3":"IPv6 address3",        // [ ipv6 address ]


        // 示例
        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"lan",                    # netdev is lan
        "status":"up",                     # connect is succeed
        "ip":"192.168.1.1",                # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0"      # local IPv6 address is fe80::50:f4ff:fe00:0
    },



    // LTE相关的信息
    "ifname@lte.status":
    {
        // 属性介绍
        "status":"Current state",        // [ "setup", "register", "ready", "nodevice", "reset", "down", "up" ]
                                             // "setup" for setup the modem
                                             // "register" for register the network
                                             // "ready" for ready to connect to internet, hint signal/network/simcard all ok
                                             // "nodevice" for the corresponding module could not be found
                                             // "reset" for reset the modem
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed
        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "ppp" ] for PPP dial
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "delay":"delay time",           // [ "failed", number ], "failed" for icmp failed
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "method":"IPv6 address mode",   // [ "disable" ] is not use ipv6, [ "manual" ] for manual setting, [ "automatic" ] for DHCPv6, [ "slaac" ] for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ]
        "state":"Current state",        // [ "setup", "register", "ready", "connecting", "connected", "reset" ]
                                             // "setup" for setup the modem
                                             // "register" for register the network
                                             // "ready" for ready to connect to internet, hint signal/network/simcard all ok
                                             // "connecting" for connect to the internet
                                             // "connected" for connect internet succeed
                                             // "reset" for reset the modem
        "mversion":"Modem version",     // [ string ]
        "imei":"IMEI numer",            // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                // number for iccid
                                                // "nosim" for cannot found the simcard
                                                // "pin" for the simcard need PIN code
                                                // "puk" for the simcard pin error

        "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                // number for MCC and MNC
                                                // "noreg" for cannot register to opeartor
                                                // "unreg" for cannot register to opeartor
                                                // "dereg" for register to operator be refused
        "netera":"technology Generation",// [ "2G", "3G", "4G", "5G" ], Optional
        "nettype":"network type",        // The format varies depending on the module
                                         // 2G usually shows GSM, GPRS, EDGE, CDMA
                                         // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G usually shows LTE, FDD, TDD

        
        "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
        "csq":"CSQ number",              // [ number ]
        "rssi":"signal intensity",       // [ number ], the unit is dBm
        "rsrp":"RSRP value",             // Optional, The format varies depending on the module
        "rsrq":"RSRQ value",             // Optional, The format varies depending on the module
        "sinr":"sinr value",             // Optional, The format varies depending on the module 
        "band":"current band",           // Optional, The format varies depending on the module
        "pcid":"Physical Cell ID",       // [ number ], Optional

        "operator":"operator name",      // [ string ]
        "operator_advise":               // Recommended profile for PLMN
        {
            "name":"operator name",               
            "dial":"dial number",                     // [ number ]
            "cid":"dial CID",                         // [ number ], default is 1
            "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
            "apn":"APN name",                         // [ string ]
            "user":"user name",                       // [ string ]
            "passwd":"user password"                  // [ string ]
        },


        // 示例
        "mode":"dhcpc",                    # IPv4 connect mode is DHCP
        "netdev":"usb0",                   # netdev is usb0
        "gw":"10.137.89.118",              # gateway is 10.137.89.118
        "dns":"120.80.80.80",              # dns is 120.80.80.80
        "dns2":"221.5.88.88",              # backup dns is 221.5.88.88
        "status":"up",                     # connect is succeed
        "ip":"10.137.89.117",              # ip address is 10.137.89.117
        "mask":"255.255.255.252",          # network mask is 255.255.255.252
        "livetime":"00:15:50:0",           # already online 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0",     # local IPv6 address is fe80::50:f4ff:fe00:0
        "imei":"867160040494084",          # imei is 867160040494084
        "imsi":"460015356123463",          # imei is 460015356123463
        "iccid":"89860121801097564807",    # imei is 89860121801097564807
        "csq":"3",                         # CSQ nubmer is 3
        "signal":"3",                      # signal level is 3
        "state":"connect",                 # state is connect to the internet
        "plmn":"46001",                    # plmn is 46001
        "nettype":"WCDMA",                 # nettype is WCDMA
        "rssi":"-107",                     # signal intensity is -107
        "operator":"中国联通",             # operator name is 中国联通
        "operator_advise":                 # recommended profile for 中国联通
        {
            "name":"中国联通",                 # name is 中国联通
            "dial":"*99#",                     # dial number is *99#
            "apn":"3gnet"                      # APN is 3gnet
        }
    },

    // GPS相关的信息
    "gnss@nmea.info":
    {
        // 属性介绍
        "step":"step of location",                  // [ "setup", "search", "located" ]
        "utc":"UTC date",                           // [ string ], format is hour:minute:second:month:day:year
        "lon":"longitude",                          // [ float ], nmea format
        "lat":"latitude",                           // [ float ], nmea format
        "longitude":"longitude",                    // [ float ]
        "latitude":"latitude",                      // [ float ]
        "speed":"speed",                            // [ float ]
        "direction":"direction",
        "declination":"declination",
        "elv":"Either altitude height",             // [ float ], the unit is meter
        "inview":"Number of visible satellites",    // [ number ]
        "inuse":"Number of satellites in use",      // [ nubmer ]     

        // 示例
        "step":"located",                           # already located
        "utc":"7:55:22:7:12:2021",                  # UTC
        "lon":"11356.56400",
        "lat":"2240.80119",
        "longitude":"113.94273",                    # longitude is 113.94
        "latitude":"22.68001",                      # latitude is 22.68
        "speed":"0.34",
        "elv":"77.90",
        "direction":"",
        "declination":"",
        "inuse":"8"                                 # 8 sat in use
    }
}
```  


#### **也可只获取设备基本信息**
通过访问http://设备IP地址/pub/machine_status.json获取设备基本信息, 减少返回不必要的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/machine_status.json
```
设备返回JSON数据介绍如下: 
```
{
    // 设备的基本设置
    "land@machine":
    {
        // main attributes
        "mode":"gateway operator mode",                 // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                        // "ap": access point
                                                        // "wisp": 2.4G Wireless Internet Service Provider connection
                                                        // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wirless baseband)
                                                        // "gateway": wire WAN gateway
                                                        // "dgateway": Dual wire WAN gateway
                                                        // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                        // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                        // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                        // "mix": custom mix connection from multiple internet connection
        "name":"gateway name",                          // < string >, The name cannot contain spaces
        "mac":"gateway MAC address",                    // < mac address >
        "macid":"gateway MAC identify or serial id",    // < string >
        "language":"gateway language",                  // [ "cn", "en", "jp", ... ], language code in two letter, "cn" for Chinese, “en" for English
        // other attributes
        "cfgversion":"gateway configure version",       // [ string ]
        "gpversion":"gateway group configure version",  // [ string ]


        // 示列
        "mode":"nmisp",                              # gateway operator mode Next Mobile(LTE/NR) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                     # gateway name is ASHYELF-12AAD0
        "mac":"00:03:7F:12:AA:D0",                   # gateway MAC is 88:12:4E:20:46:B0
        "macid":"00037F12AAD0",                      # gateway serial id is 88124E2046B0
        "language":"cn",                             # gateway language is chinese
        "cfgversion":"1"                             # gateway configure version is 1(hint modify one times)

    },


    // 设备基本状态
    "land@machine.status":
    {
        "mode":"gateway operator mode",              // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                         // "ap": access point
                                                         // "wisp": 2.4G Wireless Internet Service Provider connection
                                                         // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wirless baseband)
                                                         // "gateway": wire WAN gateway
                                                         // "dgateway": Dual wire WAN gateway
                                                         // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                         // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                         // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                         // "mix": custom mix connection from multiple internet connection
        "name":"gateway name",
        "platform":"gateway platform identify",
        "hardware":"gateway hardware identify",
        "custom":"gateway custom identify",
        "scope":"gateway scope identify",
        "version":"gateway version",
        "livetime":"system running time",                // hour:minute:second:day
        "current":"current date",                        // hour:minute:second:month:day:year
        "mac":"gateway MAC address",
        "macid":"gateway MAC identify or serial id",
        "model":"gateway model",
        "magic":"gateway magic identify"


        // 示列
        "mode":"nmisp",                                   # gateway operator mode Next Mobile(NR/LTE) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                          # gateway name is ASHYELF-12AAD0
        "platform":"smtk2",                               # gateway platform identify is smtk2( hint the basic sdk is second release of smtk )
        "hardware":"mt7621",                              # gateway hardware identify is mt7621( hint the chip is MT7621 )
        "custom":"d228",                                  # gateway custom identify is d228( hint the product name is D228 )
        "scope":"std",                                    # gateway scope identify is std( hint this is a standand release for D228 )
        "version":"tiger7-20220218",                      # gateway version is tiger7-20220218( hint the version publish at 2022.02.18 )
        "livetime":"00:06:35:0",                          # gateway It has been running for 6 minutes and 35 seconds
        "current":"14:54:30:05:04:2022",                  # current date is 2022.05.04, It's 14:54 and 30 seconds
        "mac":"00:03:7F:12:AA:D0",                        # gateway MAC is 00:03:7F:12:AA:D0
        "macid":"00037F12AAD0",                           # gateway serial id is 00037F12AAD0
        "model":"5228",                                   # gateway model is 5228
        "magic":"870E2935E4605D02"                        # gateway magic is 870E2935E4605D02  
    }
}
```


#### **也可只获取设备LAN口信息**
通过访问http://设备IP地址/pub/lan_status.json获取设备LAN口信息, 减少返回不必要的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/lan_status.json
```
设备返回JSON数据介绍如下: 
```
{
    // LAN相关的信息
    "ifname@lan.status":
    {
        "status":"Current status",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the ifname is down
                                             // "up" for the network is connect succeed
        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "method":"IPv6 address mode",   // [ "disable" ] is not use ipv6, [ "manual" ] for manual setting, [ "automatic" ] for DHCPv6, [ "slaac" ] for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ]
        "addr2":"IPv6 address2",        // [ ipv6 address ]
        "addr3":"IPv6 address3",        // [ ipv6 address ]


        // 示例
        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"lan",                    # netdev is lan
        "status":"up",                     # connect is succeed
        "ip":"192.168.1.1",                # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0"      # local IPv6 address is fe80::50:f4ff:fe00:0
    }

}
```


#### **也可只获取设备联网信息(LTE/WAN/WISP)信息**
通过访问http://设备IP地址/pub/internet_status.json只获取设备联网信息, 减少返回不必要的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/internet_status.json
```
设备返回JSON数据介绍如下: 
```
{
    // LTE相关的信息
    "ifname@lte.status":
    {
        // 见 获取设备常用信息 中返回值有关 ifname@lte.status 的介绍
    }
}
```


#### **也可只获取设备GPS信息**
通过访问http://设备IP地址/pub/gnss_status.json只获取设备GPS信息, 减少返回其它不必要的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/gnss_status.json
```
设备返回JSON数据介绍如下: 
```
{
    // GPS相关的信息
    "gnss@nmea.info":
    {
       // 见 获取设备常用信息 中返回值有关 gnss@nmea.info 的介绍
    }
}
```



#### **获取设备客户端信息**
通过访问http://设备IP地址/pub/client_list.json获取设备客户端的信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/client_list.json
```
设备返回JSON数据介绍如下: 
```
{
    "client@station.list":
    {
        // 属性介绍
        "client mac address":               // [ MAC address ]
        {
            "ip":"ip address",                    // [ IP address ]
            "ifname":"connected ifname",          // [ "ifname@lan", "ifname@lan2", ... ]
            "name":"client name",                 // [ string ]
            "tx_bytes":"current sent of byte",    // [ number ]
            "rx_bytes":"current recived of byte", // [ number ]
            "livetime":"connected time",          // [ string ], format is hour:minute:second:day
        },
        // ... more client

        // 示例
        "04:CF:8C:39:91:7A":            # first client
        {
            "ip":"192.168.31.140",                        # ip is 192.168.31.140
            "name":"xiaomi-aircondition-ma2_mibt917A",    # hostname is xiaomi-aircondition-ma2_mibt917A
            "tx_bytes":"1779693",                         # sent 1779693 byte
            "rx_bytes":"1375610",                         # recived 1375610 byte
            "livetime":"14:39:34:1"                       # livetime is 1 day 14 hour 39 minute 34 second
        },
        "40:31:3C:B5:6D:4C":            # second client
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C",
            "livetime":"14:39:26:1"
        },
        "14:13:46:C9:97:C7":            # third client
        {
            "ip":"192.168.31.9",
            "name":"",
            "livetime":"14:39:27:1"
        }
    }
}
```


#### **获取设备无线SSID状态信息**
通过访问http://设备IP地址/pub/ssid_status.json获取设备无线状态信息
如默认设备的IP地址为192.168.8.1, 即可访问如下地址
```
    http://192.168.8.1/pub/ssid_status.json
```
设备将返回如下
```
{
    // 2.4G无线状态
    "wifi@nssid":
    {
        // 属性介绍
        "state":"current state",                // [ "up", "down" ], "up" for enable, "down" for disable
        "secure":"secure mode",                 // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                        // disable for disable the secure
                                                        // wpapsk for WPAPSK
                                                        // wpa2psk for WPA2PSK
                                                        // wpapskwpa2psk for WPA Auto
        "rx_bytes":"recvice bytes",             // [ nubmer ]
        "rx_packets":"recvice packets",         // [ number ]
        "rx_errs":"recvice errs packets",       // [ number ]
        "rx_drops":"recvice drop packets",      // [ nubmer ]
        "tx_bytes":"send bytes",                // [ nubmer ]
        "tx_packets":"send packets",            // [ nubmer ]
        "tx_errs":"send errors packets",        // [ nubmer ]
        "tx_drops":"send drop packets",         // [ nubmer ]
        "mac":"mac address",                    // [ MAC address ]
        "livetime":"online time",               // [ hour:minute:second:day ]
        "ssid":"SSID name",                     // [ string ]
        "bssid":"BSSID",                        // [ MAC address ]
        "channel":"current channel",            // [ number ]
        "rate":"current rate",                  // [ number ]

        "state":"up",
        "secure":"wpapskwpa2psk",
        "rx_bytes":"767164641",
        "rx_packets":"22258095",
        "rx_errs":"489663",
        "rx_drops":"0",
        "tx_bytes":"369735875",
        "tx_packets":"2036548",
        "tx_errs":"0",
        "tx_drops":"0",
        "mac":"00:03:7F:12:88:70",
        "livetime":"14:45:36:2",
        "ssid":"dimmalex-home",
        "bssid":"00:03:7F:12:88:70",
        "channel":"11",
        "rate":"300"

    }
    // 5.8G无线状态(如果设备支持5.8G才会有此节点)
    "wifi@assid":
    {
        // 属性介绍
        "state":"current state",                // [ "up", "down" ], "up" for enable, "down" for disable
        "secure":"secure mode",                 // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                        // disable for disable the secure
                                                        // wpapsk for WPAPSK
                                                        // wpa2psk for WPA2PSK
                                                        // wpapskwpa2psk for WPA Auto
        "rx_bytes":"recvice bytes",             // [ nubmer ]
        "rx_packets":"recvice packets",         // [ number ]
        "rx_errs":"recvice errs packets",       // [ number ]
        "rx_drops":"recvice drop packets",      // [ nubmer ]
        "tx_bytes":"send bytes",                // [ nubmer ]
        "tx_packets":"send packets",            // [ nubmer ]
        "tx_errs":"send errors packets",        // [ nubmer ]
        "tx_drops":"send drop packets",         // [ nubmer ]
        "mac":"mac address",                    // [ MAC address ]
        "livetime":"online time",               // [ hour:minute:second:day ]
        "ssid":"SSID name",                     // [ string ]
        "bssid":"BSSID",                        // [ MAC address ]
        "channel":"current channel",            // [ number ]
        "rate":"current rate",                  // [ number ]

        "state":"up",
        "secure":"wpapskwpa2psk",
        "rx_bytes":"23211",
        "rx_packets":"221232",
        "rx_errs":"22",
        "rx_drops":"0",
        "tx_bytes":"3232433",
        "tx_packets":"76676776",
        "tx_errs":"0",
        "tx_drops":"0",
        "mac":"00:03:7F:12:88:78",
        "livetime":"14:45:36:2",
        "ssid":"dimmalex-home",
        "bssid":"00:03:7F:12:88:78",
        "channel":"36",
        "rate":"866" 
    }
}
```



#### **编程介绍**

##### 通过socket获取设备信息介绍
1. 发送请求

通过socket编程向设备的WEB服务器端口（通常为192.168.8.1的IP的80端口）发送TCP连接, 成功后发送以下HTTP请求/pub/basic_status.json页面即可:
```
GET /pub/basic_status.json HTTP/1.1
Host: 192.168.8.1
Connection: close
User-Agent: DeviceAPI/6

```
GET /pub/basic_status.json HTTP/1.1为 **HTTP请求头**
后面为**HTTP的头**, 每个请求根据实际情况会有所不同, 但都需要符合HTTP协议
***注意结尾是两个回车换行，即\r\n\r\n***


2. 发送完成后, 通过read函数可以读到如下内容
```
HTTP/1.1 200 OK
Server: GoAhead-http
Date: Tue Feb 23 00:52:51 2021
Transfer-Encoding: chunked
Connection: close
Pragma: no-cache
Cache-Control: no-cache

73e
{"land@machine":{"platform":"smtk","hardware":"mt7628","custom":"d218","scope":"zwzx","cfgversion":"4","language":"cn","macid":"00037F120550","mac":"00:03:7F:12:05:50","name":"LTE-120550","copywrong":"hide","title":"LTE-120550","mode":"misp"},"land@machine.status":{"custom":"d218","version":"4.3.3w","publish":"021721","cfgversion":"4","macid":"00037F120550","mac":"00:03:7F:12:05:50","magic":"4B840F2BDF697881","model":"V518E","name":"LTE-120550","mode":"misp","livetime":"05:22:11:0","current":"08:52:51:02:23:2021"},"ifname@lan.status":{"mode":"static","ifname":"ifname@lan","ifdev":"bridge@lan","device":"lan","ontime":"13","status":"up","ip":"192.168.8.1","dstip":"192.168.8.1","mask":"255.255.255.0","mac":"00:03:7F:12:05:50","rx_bytes":"49506","rx_packets":"426","tx_bytes":"168511","tx_packets":"281","livetime":"05:21:58:0"},"ifname@lte.status":{"mode":"dhcpc","ifname":"ifname@lte","device":"usb0","gw":"10.8.23.76","dns":"202.96.134.33","dns2":"202.96.128.166","ifdev":"modem@lte","ontime":"36","status":"up","ip":"10.8.23.75","dstip":"10.8.23.75","mask":"255.255.255.248","keeplive":"244","mac":"02:50:F4:00:00:00","rx_bytes":"108872","rx_packets":"1268","tx_bytes":"319921","tx_packets":"3781","tid":"1","vid":"2c7c","pid":"0125","syspath":"/sys/bus/usb/devices/1-1","name":"Quectel-EC2X","advise":"dhcpc","step":"online","imei":"862607056478018","imsi":"460115372165490","iccid":"89861120147330291660","plmn":"46011","nettype":"FDD LTE","band":"LTE BAND 1","csq":"19","rssi":"-75","signal":"3","rsrp":"-102","rsrq":"-10","sinr":"-18.0","operator":"中国电信","operator_advise":{"name":"中国电信","dial":"*99#","apn":"ctnet","user":"ctnet@mycdma.cn","passwd":"vnet.mobi"},"livetime":"05:21:35:0"},"uart@gnss.status":{"dev":"modem@lte","tty":"/dev/ttyUSB1","uart":{"rx":"4375214","tx":"0","connect":"ok"},"gnss_local(0.0.0.0)":{"rx":"680","tx":"0","connect":"ok"},"step":"search","utc":"0:52:50:2:23:2021","uptime":"19330","plmn":"46011"}}
0

```
3. 如何解析内容
在2中返数据中会有一个JSON对象， 它以{开头并以}结尾， 只要解析这个JSON对象， 找出其对应的属性即可

##### 通过其它的HTTP工具获取信息
通过其它的HTTP工具获取网页， 之后再通过查找工具解析对应的变量内容，如通过linux下的命令wget获取
1. 通过wget将http://192.168.8.1/pub/basic_status.json界面返回的JSON存放入info.json中
```
dimmalex@dimmalex-NUC10i7FNH:~/mytry6$ wget http://192.168.8.1/pub/basic_status.json -O info.json
--2021-02-23 08:58:59--  http://192.168.8.1/pub/basic_status.json
Connecting to 192.168.8.1:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: unspecified
Saving to: ‘info.json’

info.json                                                [ <=>                                                                                                                  ]   1.26K  --.-KB/s    in 0s      

2021-02-23 08:58:59 (38.7 MB/s) - ‘info.json’ saved [1293]
```
2. 查看info.json文件的内容, 此文件为一个JSON, 解析此文件即可
```
dimmalex@dimmalex-NUC10i7FNH:~/mytry6$ cat info.json 
{"land@machine":{"platform":"smtk","hardware":"mt7621","custom":"v520","scope":"std","cfgversion":"27","language":"cn","macid":"00037F12CC70","mac":"00:03:7F:12:CC:70","name":"V520-12CC70","title":"V520-12CC70","mode":"gateway"},"land@machine.status":{"custom":"v520","version":"4.3.3w","publish":"021821","cfgversion":"27","macid":"00037F12CC70","mac":"00:03:7F:12:CC:70","magic":"C3440921E4692895","model":"V520","name":"V520-12CC70","mode":"gateway","livetime":"05:28:15:0","current":"08:58:59:02:23:2021"},"ifname@lan.status":{"mode":"static","ifname":"ifname@lan","ifdev":"bridge@lan","device":"lan","ontime":"13","status":"up","ip":"192.168.31.1","dstip":"192.168.31.1","mask":"255.255.255.0","mac":"00:03:7F:12:CC:70","rx_bytes":"102283720","rx_packets":"533292","tx_bytes":"253971036","tx_packets":"463761","livetime":"05:28:02:0"},"ifname@wan.status":{"mode":"dhcpc","ifname":"ifname@wan","device":"eth0.2","gw":"223.212.231.1","dns":"211.148.192.141","dns2":"211.148.192.151","ifdev":"vlan@wan","ontime":"27","status":"up","ip":"223.212.231.16","dstip":"223.212.231.16","mask":"255.255.255.128","mac":"00:03:7F:12:CC:76","rx_bytes":"276227928","rx_packets":"1002981","tx_bytes":"90351607","tx_packets":"495241","tid":"3","livetime":"05:27:48:0"},"uart@gnss.status":{"step":"notty"}}
dimmalex@dimmalex-NUC10i7FNH:~/mytry6$ 

```

##### 通过工具演示收发数据
以下演示通过一个第三方的TCP&UDP客户端发送原始的HTTP请求包, 并收到回复的原始HTTP的回复, 里面包含了设备LAN的状态
![avatar](./pub.md-test.png) 
