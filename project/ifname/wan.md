***
## WAN Network Management
Manage WAN network. This component must depend on WAN network interface and network Management Framework project  
Usually ifname@wan is the first WAN network. If there are multiple WAN network in the system, ifname@wan2 will be the second WAN network, and increase by degress

#### **configuration( ifname@wan )**   
**ifname@wan** is first WAN network   
**ifname@wan2** is second WAN network   

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ "enable", "disable" ]
    "mac":"custom the MAC address",        // [ mac address ]

    // IPv4
    "tid":"table identify number",         // [ number ] exclusive route table ID, only for multiple WAN
    "mode":"IPV4 address mode",            // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
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
    "pppoe":                                    // detial configure for "mode" is "pppoe"
    {
        "username":"PPPOE username",                     // [ string ]
        "password":"PPPOE password",                     // [ string ]
        "service":"service name",                        // [ string ], default accept all service
        "txqueuelen":"tx queue szie",                    // [ number ]
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ "disable", "enable" ]
        "dns":"Custom DNS1",                             // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2",                            // [ ip address ], This is valid when "custom_dns" is "enable"
    },
    "masq":"out stream share the interface IPv4 address to access the Internet",  // [ "disable", "enable" ]
    "mtu":"Maximum transmission unit",                                            // [ number ], The unit is in bytes

    // IPv6
    "method":"IPv6 address mode",         [ "disable", "manual", "automatic", "slaac", "relay" ]
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

Examples, show the first WAN all configure
```shell
ifname@wan
{
    "tid":"3",                                       # exclusive route table ID is 2

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

Example, modify the first WAN dial mode to DHCP
```shell
ifname@wan:mode=dhcpc
ttrue
```

Example, modify the first WAN pppoe username and password
```shell
ifname@wan:pppoe/username=dimmalex@ashyelf.com
ttrue
ifname@wan:pppoe/password=123456
ttrue
# You can also use one command to complete the operation of the above three command
ifname@wan:pppoe|{"username":"dimmalex@ashyelf.com", "password":"123456"}
ttrue
```

Example, disable the first WAN
```shell
ifname@wan:status=disable
ttrue
```

Example, disable the second WAN
```shell
ifname@wan2:status=disable
ttrue
```

#### **Methods**
**ifname@wan** is first WAN network
**ifname@wan2** is second WAN network

+ `status[]` **get the local network infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current state",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed
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
    # examples, get the first WAN network infomation
    ifname@wan.status
    {
        "status":"up",                     # connect is succeed

        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"wan",                    # netdev is lan
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

+ `netdev[]` **get the WAN netdev**, *succeed return netdev, failed return NULL, error return terror*   
    ```shell
    # examples, get the first WAN network netdev
    ifname@wan.netdev
    wan
    ```

+ `ifdev[]` **get the ifdev**, *succeed return ifdev, failed return NULL, error return terror*   
    ```shell
    # examples, get the first WAN network ifdev
    ifname@wan.ifdev
    vlan@wan
    ```

+ `shut[]` **shutdown the WAN network**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, shutdown the frist WAN network
    ifname@wan.shut
    ttrue
    # examples, shutdown the second WAN network
    ifname@wan2.shut
    ttrue    
    ```

+ `setup[]` **setup the WAN network**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, setup the frist WAN network
    ifname@wan.setup
    ttrue
    # examples, setup the second WAN network
    ifname@wan2.setup
    ttrue
    ```

