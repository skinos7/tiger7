***
## WISP Network Management
Manage WISP network. This component must depend on wireless station interface and network Management Framework project  
Usually ifname@wisp is the first WISP(2.4G) network. If there are multiple WISP network in the system, ifname@wisp2 will be the second WISP(5.8G) network, and increase by degress

#### **configuration( ifname@wisp )**   
**ifname@wisp** is first WISP(2.4G) network   
**ifname@wisp2** is second WISP(5.8G) network   

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ "enable", "disable" ]
    "mac":"custom the MAC address",        // [ mac address ]

    // wireless connect
    "peer":"SSID to connect",              // [ string ]
    "peermac":"BSSID to connect",          // [ mac address ]
    "peermode":"mode of connection",       // [ "hidden" ] Indicates that the peer end does not broadcast SSID. In hidden mode, channel must not be empty  
    "channel":"wireless channel",          // [ number ], 0-165, 0 for auto
    "secure":"mode of security",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] for no securiyt
                                                            // [ wpapsk ]  for WPAPSK
                                                            // [ wpa2psk ]  for WPA2PSK
                                                            // [ wpapskwpa2psk ] for WPA Mix
    "wpa_encrypt":"WAP encrypt",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] for AES
                                                            // [ tkip ] for TKIP
                                                            // [ tkipaes ] for auto
    "wpa_key":"WPA key",                   // [ string ], The value is a string of at least 8 characters. This parameter is mandatory if the "secure" is wpapsk/wpa2psk/wpapskwpa2psk
    "ssid_disable":"disable the ssid",     // [ "disable", "enable" ], disable the local ssid when connected

    // IPv4
    "tid":"table identify number",         // [ number ] exclusive route table ID, only for multiple WAN
    "mode":"IPV4 address mode",            // [ "dhcpc", "static", "pppoe" ], "dhcpc" for DHCP, "static" for manual setting, "pppoe" for PPPOE dial
    "static":                                       // detial configure for "mode" is [ static ]
    {
        "ip":"IPv4 address",                        // < IPv4 address >
        "mask":"IPv4 netmask",                      // < IPv4 netmask >
        "gw":"IPv4 gateway",                        // [ IPv4 address ]
        "dns":"IPv4 DNS",                           // [ IPv4 address ]
        "dns2":"IPv4 DNS"                           // [ IPv4 address ]
    },
    "dhcpc":                                                       // detial configure for mode is [ dhcpc ]
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "enable", "disable" ]
        "routeopt":"dhcp option static route",                     // [ "enable", "disable" ]
        "custom_dns":"Custom DNS",                                 // [ "enable", "disable" ]
        "dns":"Custom DNS1",                                       // [ IP address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2"                                       // [ IP address ], This is valid when custom_dns is [ enable ]
    },
    "pppoe":                                             // detial configure for "mode" is [ pppoe ]
    {
        "username":"PPPOE username",                     // [ string ]
        "password":"PPPOE password",                     // [ string ]
        "service":"service name",                        // [ string ], default accept all service
        "txqueuelen":"tx queue szie",                    // [ number ]
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ disable, enable ]
        "dns":"Custom DNS1",                             // [ IP address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2"                             // [ IP address ], This is valid when custom_dns is [ enable ]
    },
    "masq":"out stream share the interface IPv4 address to access the Internet",  // [ "disable", "enable" ]
    "mtu":"Maximum transmission unit",                                            // [ number ], The unit is in bytes

    // IPv6
    "method":"IPv6 address mode",         [ "disable", "manual", "automatic", "slaac" ]
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
        "custom_dns":"Custom DNS",                   // [ "disable", "enable" ]
        "dns":"Custom DNS1",                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
    },
    "masquerade":"out stream share the interface IPv6 address to access the Internet",   // [ "disable", "enable" ]

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ "disable" ] for disable the keeplive, [ "icmp" ] for ping keeplive, [ "recv" ] for count receive packet to keeplive
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
        "recv":                                                   // detial configure for "type" is "recv"
        {
            "timeout":"How many seconds did not receive a packet considered a failure",// [ number ], The unit is in seconds
            "packets":"How many packets",                                              // [ number ]
            "failed":"failed times"                                                    // [ number ]
        }
    },
    // configure connect failed to action
    "failed_timeout":"connect timeout",                                                // [ number ], the unit is second
    "failed_threshold":"first failed to reset time",                                   // [ number ]
    "failed_threshold2":"second failed to reset time",                                 // [ number ]
    "failed_everytime":"every failed to reset time"                                    // [ number ]
}
```

Example, show first WISP(2.4G) all configure
```shell
{
    "tid":"3",                                       # exclusive route table ID is 2

    # WIFI peer
    "peer":"V520-D21D20",    # connect V520-D21D20
    "secure":"wpapsk",       # security is WPAPSK
    "wpa_encrypt":"aes",     # encrypt use AES
    "wpa_key":"87654321",    # password 87654321

    "mode":"pppoe",                                  # mode is PPPOE
    "pppoe":
    {
        "username":"1923221@gd.com",                # IPv4 address is 192.168.1.1
        "password":"FDAED13E"                       # IPv4 netmask is 255.255.255.0
    },
    "method":"slaac",                                # IPv6 address mode is slaac
    "masq":"enable",                                 # out stream share the interface IPv4 address to access the Internet
    "keeplive":                                      # keeplive mechanism configure save here
    {
        "type":"icmp",                               # use ICMP to keeplive
        "icmp":                                      
        {
            "dest":                                             # ping the 8.8.8.8 and 114.114.114.114
            {
                "test":"8.8.8.8",
                "test2":"114.114.114.114"
            },
            "timeout":"10",                                     # The timeout exceeded 10 seconds for 5 consecutive times, the link is considered unavailable
            "failed":"5",
            "interval":"5"
        }
    }
}
```

Example, modify the SSID for first WISP(2.4G) connect
```shell
ifname@wisp:peer=Myhotpot
ttrue
ifname@wisp:secure=wpapsk
ttrue
ifname@wisp:wpa_key=88888888
ttrue
# You can also use one command to complete the operation of the above three command
ifname@wisp|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```

Example, disable the first WISP(2.4G) keeplive function
```shell
ifname@wisp:keeplive=disable
ttrue
```

Example, disable the first WISP(2.4G)
```shell
ifname@wisp:status=disable
ttrue
```

Example, enable the first WISP(2.4G)
```shell
ifname@wisp:status=enable
ttrue
```

Example, enable the second WISP(5.8G)
```shell
ifname@wisp2:status=enable
ttrue
```



#### **Methods**   
**ifname@wisp** is first WISP network   
**ifname@wisp2** is second WISP network   

+ `status[]` **get the WISP infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current state",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed

        "peer":"Peer SSID",              // [ string ]
        "peermac":"Peer BSSID",          // [ MAC address ]
        "channel":"Peer channel",        // [ 1- 165 ]
        "rate":"connect rate",           // [ number ], the unit is M
        "rssi":"Peer RSSI",              // [ number ], the unit is dBm
        "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal

        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
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
        "addr2":"IPv6 address2",        // [ ipv6 address ]
        "addr3":"IPv6 address3"         // [ ipv6 address ]
    }
    ```

    ```shell
    # examples, get the first WISP network infomation
    ifname@wisp.status
    {

        "status":"up",                     # connect is succeed

        "peer":"TP-link-2231",            # peer is TP-link-2231
        "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
        "channel":"10",                   # channel is 10
        "rate":"270",                     # rate is 270M
        "rssi":"-41",                     # rssi is -41dBm
        "signal":"3",                     # signal level is 3

        "mode":"dhcpc",                    # IPv4 connect mode is DHCP
        "netdev":"ath11",                   # netdev is ath11
        "ip":"192.168.10.1",               # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "gw":"192.168.10.254",             # gateway is 192.168.10.254
        "dns":"114.114.114.114",           # dns is 114.114.114.114
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0"      # local IPv6 address is fe80::50:f4ff:fe00:0
    }
    ```

+ `netdev[]` **get the WISP netdev**, *succeed return netdev, failed return NULL, error return terror*   
    ```shell
    # examples, get the first WISP netdev
    ifname@wisp.netdev
    ath11
    ```

+ `ifdev[]` **get the ifdev**, *succeed return ifdev, failed return NULL, error return terror*   
    ```shell
    # examples, get the first WISP network ifdev
    ifname@wan.ifdev
    wifi@nsta
    ```

+ `chlist[]` **get the WISP channal list**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "channel number":{}       // [ number ]:{}
        // ... more channel
    }
    ```

    ```shell
    # examples, get the first WISP channel list
    ifname@wisp.chlist
    {
        "1":{},        # channel 1
        "2":{},        # channel 2
        "3":{},        # channel 3
        "4":{},        # channel 4
        "5":{},        # channel 5
        "6":{},        # channel 6
        "7":{},        # channel 7
        "8":{},        # channel 8
        "9":{},        # channel 9
        "10":{},       # channel 10
        "11":{}        # channel 11
    }
    ```

+ `aplist[]` **use the WISP scan the surrounding AP**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID name",                           // [ string ]
            "channel":"channel number",                   // [ number ], 0-165, 0 for auto
            "secure":"mode of security",                  // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                 // "disable" for no securiyt
                                                                 // "wpapsk"  for WPAPSK
                                                                 // "wpa2psk"  for WPA2PSK
                                                                 // "wpapskwpa2psk" for WPA Mix
            "wpa_encrypt":"WAP encrypt",                  // [ "aes", "tkip", "tkipaes" ]
                                                                 // "aes" for AES
                                                                 // "tkip" for TKIP
                                                                 // "tkipaes" for auto
            "sig":"signal level(%)",                      // [ number ]
            "signal":"signal level[0-4]",                 // [ "0", "1", "2", "3", "4" ]
            "chext":"extern channel",                     // [ "none", "below", "above" ]
            "mode":"wireless system"                      // [ string ]
        }
        // ... more AP
    }
    ```

    ```shell
    # examples, get the surrounding AP from first WISP scan
    ifname@wisp.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # frist AP SSID
            "channel":"6",                                 # first AP channel
            "secure":"wpapskwpa2psk",                      # secure mode is WPA mix
            "wpa_encrypt":"aes",                           # encrypt type is AES
            "sig":"70",                                    # signal is 70%
            "signal":"3",                                  # signal level is 3, range is 0-4
            "chext":"below",                               # extern channel is below
            "mode":"11b/g/n"
        },
        "B4:82:C5:80:22:41":                    # second AP by scanning
        {
            "ssid":"dimmalex-work",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11b/g/n"
        },
        "8C:74:A0:D6:68:B0":                    # third AP by scanning
        {
            "ssid":"CMCC-ktfK",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11b/g/n"
        }
    }
    ```

+ `shut[]` **shutdown the WISP network**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, shutdown the frist WISP network
    ifname@wisp.shut
    ttrue
    # examples, shutdown the second WISP network
    ifname@wisp2.shut
    ttrue
    ```

+ `setup[]` **setup the WISP network**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, setup the frist WISP network
    ifname@wisp.setup
    ttrue
    # examples, setup the second WISP network
    ifname@wisp2.setup
    ttrue
    ```

