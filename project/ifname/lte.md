***
## LTE/NR Network Management
Manage LTE/NR networks and 4G/NR basebands. This component must depend on the LTE/NR baseband components and network Management Framework project  
Usually ifname@lte is the first LTE/NR network and module. If there are multiple LTE/NR modules in the system, ifname@lte2 will be the second LTE/NR network and module, and increase by degress

#### **Configuration( ifname@lte )**   
**ifname@lte** is first LTE network   
**ifname@lte2** is second LTE network   

```json
// Attribute introduction
{
    // For LTE/NR Network Settings
    // Status for network
    "status":"start at system startup",    // [ "enable", "disable" ]

    // IPv4
    "tid":"table identify number",         // [ number ] exclusive route table ID, only for multiple WAN
    "metric":"default route metric",       // [ number  ]
    "mode":"IPV4 address mode",            // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "ppp" ] for PPP dial
    "static":                                 // detial configure for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 etmask >
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // detial configure for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ]
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ]
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], This is valid when "custom_dns" is "enable"
    },
    "ppp":                                    // detial configure for "mode" is "ppp"
    {
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ "disable", "enable" ]
        "dns":"Custom DNS1",                             // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2",                            // [ ip address ], This is valid when "custom_dns" is "enable"
        "txqueuelen":"tx queue len"
    },
    "masq":"out stream share the interface IPv4 address to access the Internet",  // [ "disable", "enable" ]
    "mtu":"Maximum transmission unit",                                            // [ number ], The unit is in bytes

    // IPv6
    "method":"IPv6 address mode",             // [ "disable", "manual", "automatic", "slaac" ]
                                                    // "disable" is not use ipv6
                                                    // "manual" for manual setting
                                                    // "automatic" for DHCPv6
                                                    // "slaac" for Stateless address autoconfiguration
    "manual":                                 // detial configure for "method" is "manual"
    {
        "addr":"IPv6 address",                      // < ipv6 address >
        "prefix":"IPv6 prefix",                     // < number >, 1-128
        "hop":"IPv6 gateway",                       // [ ipv6 address ]
        "resolve":"IPv6 DNS",                       // [ ipv6 address ]
        "resolve2":"IPv6 DNS2"                      // [ ipv6 address ]
    },
    "automatic":                             // detial configure for "method" is "automatic"
    {
        "custom_resolve":"Custom DNS",                   // [ "disable", "enable" ]
        "resolve":"Custom DNS1",                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
        "resolve2":"Custom DNS2"                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
    },
    "masquerade":"out stream share the interface IPv6 address to access the Internet",   // [ "disable", "enable" ]

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ "disable" ] for disable the keeplive
                                  // [ "icmp" ] for ping keeplive
                                  // [ "dns" ] for test the dns response
                                  // [ "recv" ] for count receive packet to keeplive
                                  // [ "auto" ] for count receive packet to keeplive when test the dns response failed

        "action":"failed to do",  // [ "reboot" ] reboot the system
                                  // [ "reset" ] reset the modem
                                  // [ ] other redial the connetion

        "icmp":                                                   // detial configure for "type" is "icmp"
        {
            "dest":                                                         // destination address for ICMP keeplive
            {
                "destination identify2":"destination address1",                        // [ string ]:[ IP address ]
                // "...":"..." You can configure multiple destination IP addresses. If only one PING echo packet is returned, the detection succeeds. If no PING echo packet is returned, the detection fails  
            },
            "timeout":"Maximum time to wait for the return of a PING echo packet",     // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection",                        // [ number ], The unit is in seconds

            // Line shake calculation  
            "ill_range":"Total number of counts",                                      // [ number], Total number of counts
            "ill_delay":"Delay threshold",                                             // [ number], The unit is in millisecond
            "ill_time":"Maximum time to warnning",                                     // [ number]
        },
        "dns":                                                   // detial configure for "type" is "icmp"
        {
            "timeout":"Maximum time to wait for the return of a dns resolve packet",   // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection",                        // [ number ], The unit is in seconds

            // Line shake calculation  
            "ill_range":"Total number of counts",                                      // [ number], Total number of counts
            "ill_delay":"Delay threshold",                                             // [ number], The unit is in millisecond
            "ill_time":"Maximum time to warnning",                                     // [ number]
        },
        "recv":                                                   // detial configure for "type" is "recv"
        {
            "timeout":"How many seconds did not receive a packet considered a failure",// [ number ], The unit is in seconds
            "packets":"How many packets",                                              // [ number ]
            "failed":"failed times"                                                    // [ number ]
        }
    },

    // Configure connect failed to action
    "failed_timeout":"connect timeout",                                                // [ number ], the unit is second
    "failed_threshold":"first failed to reset time",                                   // [ number ]
    "failed_threshold2":"second failed to reset time",                                 // [ number ]
    "failed_everytime":"every failed to reset time",                                   // [ number ]

    // For LTE/NR baseband Settings, the parameters are the same as modem@lte or modem@lte2
    // lock attributes
    "lock_imei":"lock imei",                   // [ "", "enable", IMEI ], enable: The IMEI detected for the first time will be locked
    "lock_imsi":"lock imsi",                   // [ "", "enable", IMEI ], enable will lock current imei
    "lock_pin":"simcard pin",                  // [ string ]
    "lock_nettype":"network type",             // [ "2g", "3g", "4g", "nsa", "sa" ]
    "lock_band":"lock band",                   // The format varies depending on the module
    "lock_cell":"lock cell",                   // The format varies depending on the module

    // UART other function    
    "gnss":"gps function",                     // [ "disable", "enable" ]
    "atport":"atport function",                // [ "disable", "enable" ]

    // custom at command
    "custom_set":                              // custom at setting list at the modem setup
    {
        "AT command":"AT exeucte result"             // [ string ]:[ string ]
        // ...more AT command
    },
    "custom_watch":                            // custom at watch list at the modem watch
    {
        "AT command":"AT exeucte result"             // [ string ]:[ string ]
        // ...more AT command
    },

    // simcard dial attributes
    "need_simcard":"SIMcard must be detected",                                                 // [ "enable", "disable" ]
    "simcard_failed_threshold":"first failed to reset time",                                   // [ number ]
    "simcard_failed_threshold2":"second failed to reset time",                                 // [ number ]
    "simcard_failed_threshold3":"third failed to reset time",                                  // [ number ]
    "simcard_failed_everytime":"every failed to reset time",                                   // [ number ]

    // signal dial attributes
    "need_plmn":"must register to plmn",                                                       // [ "enable", "disable" ]
    "need_signal":"Signal must be effectivity",                                                // [ "enable", "disable" ]
    "signal_failed_threshold":"first failed to reset time",                                    // [ number ]
    "signal_failed_threshold2":"second failed to reset time",                                  // [ number ]
    "signal_failed_threshold3":"third failed to reset time",                                   // [ number ]
    "signal_failed_everytime":"every failed to reset time",                                    // [ number ]

    // watch attributes
    "watch_interval":"How often do query the modem",  // [ number ], the unit in second

    // profile attributes
    "profile":"custom the profile",            // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile save here, the json be used when "profile" value is enable
    {
        "dial":"dial number",                     // [ number ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // [ "pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password",                 // [ string ]
        "cids":"multi CID settings",              // [ "disable", "enable" ], "enable" for set multi-CID for modem
        "cids_cfg":                               // settings of multi-CID save here, the json be used when "cids" value is enable
        {
            "CID number":                                 // [ number ]
            {
                "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
                "apn":"APN name",                         // [ string ]
                "user":"user name",                       // [ string ]
                "passwd":"user password",                 // [ string ]
            }
            //"CID nubmer ":{ CID profile }     How many CID profile need setting save how many properties
        }
    },

    // backup simcard configure
    "bsim":"backup simcard function",                         // [ "disable", "enable" ]
    "bsim_cfg":                                               // settings of backup simcard save here, the json be used when "bsim" value is enable
    {
        "mode":"Specify the simcard",                               // [ "auto", "bsim", "msim", "detect" ]
                                                                        // "bsim" for backup simcard
                                                                        // "msim" for main simcard
                                                                        // "detect" the IO for auto that need detect IO support
        "signal_failed":"Check the signal failed how many times to switch the simcard",  // [ number ]
        "dial_failed":"connect to internet failed how many times to switch the simcard", // [ number ]
        "failover":"backup simcard usage duration",                                      // [ number ], the unit is second
        "keeplive_switch":"keeplive faeild to switch",                                   // [ "disable", "enable" ]
        // backup simcard lock attributes
        "lock_imei":"lock imei",                   // [ "disable", "enable", IMEI ], "enable": The IMEI detected for the first time will be locked
        "lock_imsi":"lock imsi",                   // [ "disable", "enable", IMEI ], enable will lock current imei
        "lock_pin":"simcard pin",                  // [ string ]
        "lock_netmode":"network type",             // [ "2g", "3g", "4g", "nsa", "sa" ]
        "lock_band":"lock band",                   // The format varies depending on the module
        "lock_cell":"lock cell",                   // The format varies depending on the module
        // backup profile attributes
        "profile":"custom the profile",            // [ "disable", "enable" ]
        "profile_cfg":                             // custom profile save here, the json be used when "profile" value is enable
        {
            "dial":"dial number",                     // [ number ]
            "cid":"dial CID",                         // [ number ], default is 1
            "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
            "apn":"APN name",                         // [ string ]
            "user":"user name",                       // [ string ]
            "passwd":"user password",                 // [ string ]
            "cids":"multi CID settings",              // [ "disable", "enable" ], enable for set multi-CID for modem
            "cids_cfg":                                   // settings of multi-CID save here, the json be used when "cids" value is enable
            {
                "CID number":                                 // [ number ]
                {
                    "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
                    "apn":"APN name",                         // [ string ]
                    "user":"user name",                       // [ string ]
                    "passwd":"user password",                 // [ string ]
                }
                // ... more CID profile
            }
        }
    },

    // soft backup simcard configure
    "ssim":"soft backup simcard function",                         // [ "disable", "enable" ]
    "ssim_cfg":                                                    // settings of soft backup simcard save here, the json be used when "ssim" value is enable
    {
        "mode":"Specify the operator",                                   // [ "signal", "plmn" ]
                                                                            // "signal" for test and use the The best signal operator
                                                                            // "46000" for china mobile
                                                                            // "46001" china union
                                                                            // "46003" for china telecom
        "signal_failed":"Check the signal failed how many times to switch the simcard",  // [ number ]
        "dial_failed":"connect to internet failed how many times to switch the simcard", // [ number ]
    },

    // SMS configure
    "sms":"SMS function enable or disable",
    "sms_cfg":
    {
        "center":"SMS center number",                             // [ number ]
        "he":"enable or disable the he command",                  // [ "disable", "enable" ]
        "he_contact":"set a contact to send the he command",      // [ number ]
        "he_prefix":"set a prefix at the he command"              // [ string ]
    }

}
```   
Example, show first LTE all configure   
```shell
ifname@lte
{
    "tid":"2",                                       # exclusive route table ID is 2
                                                     # mode default is dhcpc

    "ppp":                                           # ppp configure will be used when "mode" is ppp
    {
        "lcp_echo_interval":"10",                         # LCP echo interval is 10 second
        "lcp_echo_failure":"12"                           # LCP echo failure times is 12
    },
    "method":"slaac",                                # IPv6 address mode is slaac
    "masq":"enable",                                 # out stream share the interface IPv4 address to access the Internet
    "keeplive":                                      # keeplive mechanism configure save here
    {
        "type":"recv",                               # use count receive packet to keeplive
        "recv":                                      # If no 1 packet is received within 20X30 second(10 minutes), the link is considered unavailable
        {
            "timeout":"20",
            "failed":"30",
            "packets":"1"
        }
    },
 
    "lock_imei":"enable",              # The IMEI detected for the first time will be locked
    "lock_imsi":"460015356123463",     # the simcard imsi must be 460015356123463

    "gnss":"enable",                   # enbale the GPS function

    "profile":"enable",                # custom the APN profile
    "profile_cfg":
    {
        "dial":"*99#",                     # dial number is *99#
        "type":"ipv4v6",                   # ip address type is ipv4 and ipv6
        "apn":"internet",                  # APN is internet
        "user":"card",                     # username is card
        "passwd":"card"                    # password is card
    }
}
```   
Example, modify the keeplive to icmp for first LTE network  
```shell
ifname@lte:keeplive/type=icmp
ttrue
```   
Example, modify the mode to ppp for first LTE network  
```shell
ifname@lte:mode=ppp
ttrue
```   
Example, modify the icmp keeplive destination address for first LTE network  
```shell
ifname@lte:keeplive/icmp/dest/test=8.8.8.8            # modify the icmp keeplive first destination address to 8.8.8.8
ttrue
ifname@lte:keeplive/icmp/dest/test2=8.8.4.4           # modify the icmp keeplive second destination address to 8.8.4.4 
ttrue
ifname@lte:keeplive/icmp/dest/test3=114.114.114.114   # modify the icmp keeplive third destination address to 114.114.114.114
ttrue
# You can also use one command to complete the operation of the above three command
ifname@lte:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   
Example, disable the first LTE network   
```shell
ifname@lte:status=disable
ttrue
```   
Example, modify the keeplive to icmp for second LTE/NR network  
```shell
ifname@lte2:keeplive/type=icmp
ttrue
```   
Examples, enable the GNSS for first LTE modem   
```shell
ifname@lte:gnss=enable
ttrue
```   

#### **Methods**   
**ifname@lte** is first LTE network   
**ifname@lte2** is second LTE/NR network   

+ `status[]` **get the LTE network infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        // For LTE/NR Network status    
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
        "delay":"delay time",           // [ "failed", number ], "block" for first connect to send icmp test, "failed" for icmp failed
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "method":"IPv6 address mode",   // [ "disable", "manual", "automatic", "slaac" ]
                                            // "disable" is not use ipv6
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ]

        // For LTE/NR baseband Status, the parameters are the same as modem@lte or modem@lte2
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
        }
    }
    ```   
    Example, get the first lte network infomation
    ```shell
    ifname@lte.status
    {
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
    }
    ```   

+ `netdev[]` **get the netdev**, *succeed return netdev, failed return NULL, error return terror*   
    Example, get the first LTE network netdev
    ```shell
    ifname@lte.netdev
    usb0
    ```   

+ `ifdev[]` **get the ifdev**, *succeed return ifdev, failed return NULL, error return terror*   
    Example, get the first LTE network ifdev
    ```shell
    ifname@lte.ifdev
    modem@lte
    ```   

+ `shut[]` **shutdown the modem network**, *succeed return ttrue, failed return tfalse, error return terror*   
    Example, shutdown the frist LTE network
    ```shell
    ifname@lte.shut
    ttrue
    ```   
    Example, shutdown the second LTE network
    ```shell
    ifname@lte2.shut
    ttrue
    ```   

+ `setup[]` **setup the modem network**, *succeed return tttrue, failed return tfalse, error return terror*   
    Example, setup the frist LTE network
    ```shell
    ifname@lte.setup
    ttrue
    ```   
    Example, setup the second LTE network
    ```shell
    ifname@lte2.setup
    ttrue
    ```   


