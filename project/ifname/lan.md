***
## Local network management components
Manage local network. This component must depend on local network interface or network switch(SOC) and network Management Framework project  
Usually ifname@lan is the first local network. If there are multiple local network in the system, ifname@lan2 will be the second local network, and increase by degress

#### **configuration( ifname@lan )**
**ifname@lan** is first local network
**ifname@lan2** is second local network

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ enable, disable ]

    // IPv4
    "mode":"IPV4 address mode",            // [ dhcpc ] for DHCP, [ static ] for manual setting
    "static":                                       // detial configure for "mode" is [ static ]
    {
        "ip":"IPv4 address",                        // < IPv4 address >
        "mask":"IPv4 netmask",                      // < IPv4 netmask >

        "ip2":"IPv4 address 2",                     // < IPv4 address >
        "mask2":"IPv4 netmask 2",                   // < IPv4 netmask >

        "ip3":"IPv4 address 3",                     // < IPv4 address >
        "mask3":"IPv4 netmask 3",                   // < IPv4 netmask >
        
        "gw":"IPv4 gateway",                        // [ IPv4 address ]
        "dns":"IPv4 DNS",                           // [ IPv4 address ]
        "dns2":"IPv4 DNS"                           // [ IPv4 address ]
    },
    "dhcpc":                                                       // detial configure for "mode" is [ dhcpc ]
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ disable, enable ]
        "routeopt":"dhcp option static route",                     // [ disable, enable ]
        "custom_dns":"Custom DNS",                                 // [ disable, enable ]
        "dns":"Custom DNS1",                                       // [ IP address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2"                                       // [ IP address ], This is valid when custom_dns is [ enable ]
    },
    "dhcps":                                               // detial configure for "mode" is [ ppp ]
    {
        "status":"Whether to start the DHCP service",                      // [ disable, enable ]
        "startip":"The start address within the IPv4 allocation pool",     // [ IPv4 address ]
        "endip":"IPv4 assigns the end address within the pool",            // [ IPv4 address ]
        "mask":"IPv4 assigns a subnet mask within a pool",                 // [ IPv4 netmask ]
        "lease":"lease time for assigns",                                  // [ number ], the unit is second
        "gw":"Specifies the IPv4 gateway",                                 // [ IPv4 address ], default is local network IP address
        "dns":"Specifies the IPv4 DNS",                                    // [ IPv4 address ], default is local network IP address
        "dns2":"Specifies the IPv4 backup dns"                             // [ IPv4 address ]
    },

    // IPv6
    "method":"IPv6 address mode",                   // [ disable ] is not use ipv6
                                                    // [ manual ] for manual setting
                                                    // [ automatic ] for DHCPv6
                                                    // [ slaac ] for Stateless address autoconfiguration
                                                    // [ relay ] for relay the extern network ipv6 address
    "manual":                             // detial configure for "method" is [ manual ]
    {
        "addr":"IPv6 address",            // < IPv6 address >
        "prefix":"IPv6 prefix",           // < number >, 1-128
        "hop":"IPv6 gateway",             // [ IPv6 address ]
        "resolve":"IPv6 DNS",             // [ IPv6 address ]
        "resolve2":"IPv6 DNS2"            // [ IPv6 address ]
    },
    "automatic":                                     // detial configure for "method" is [ automatic ]
    {
        "custom_dns":"Custom DNS",                   // [ disable, enable ]
        "dns":"Custom DNS1",                         // [ IPv6 address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2"                         // [ IPv6 address ], This is valid when custom_dns is [ enable ]
    },
    "addrpool":
    {
        "status":"Whether to start the DHCPv6 service",      // [ disable, enable ]
        "startaddr":"The start address within the IPv6 allocation pool",     // [ IPv6 address ]
        "endaddr":"IPv6 assigns the end address within the pool",            // [ IPv4 address ]
        "prefix":"IPv6 assigns a subnet mask within a pool",                 // [ IPv4 netmask ]
        "leasetime":"lease time for assigns",                                // [ number ], the unit is second
        "hop":"Specifies the IPv4 gateway",                                  // [ IPv4 address ], default is local network IP address
        "resolve":"Specifies the IPv4 DNS",                                  // [ IPv4 address ], default is local network IP address
        "resolve2":"Specifies the IPv4 backup dns"                           // [ IPv4 address ]
    },

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ disable ] for disable the keeplive, [ icmp ] for ping keeplive, [ recv ] for count receive packet to keeplive
        "icmp":                                                                           // detial configure for type is [ icmp ]
        {
            "dest":                                                                       // destination address for ICMP keeplive
            {
                // "...":"..." You can configure multiple destination IP addresses. If only one PING echo packet is returned, the detection succeeds. If no PING echo packet is returned, the detection fails  
                "destination identify2":"destination address1", // [ string ]:[ IP address ]
                "destination identify2":"destination address2"  // [ string ]:[ IP address ]
            },
            "timeout":"Maximum time to wait for the return of a PING echo packet",     // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection",                        // [ number ], The unit is in seconds

            // Line shake calculation  
            "ill_range":"Total number of counts",                                      // [ number], Total number of counts
            "ill_delay":"Delay threshold",                                             // [ number], The unit is in millisecond
            "ill_time":"Maximum time to warnning",                                     // [ number]
        },
        "recv":                                                                                          // detial configure for type is [ recv ]
        {
            "timeout":"How many seconds did not receive a packet considered a failure",                  // [ number ], The unit is in seconds
            "packets":"How many packets",                                                                // [ number ]
            "failed":"failed times"                                                                      // [ number ]
        }
    }
}
```
Example, show all local network configure
```shell
ifname@lan
{
    "mode":"static",                                 # mode is static
    "static":
    {
        "ip":"192.168.1.1",                          # IPv4 address is 192.168.1.1
        "mask":"255.255.255.0"                       # IPv4 netmask is 255.255.255.0
    },
    "method":"relay",                                # IPv6 address mode is relay

    "dhcps":
    {
        "status":"enable",           # enable the DHCP server
        "startip":"192.168.1.2",     # IPv4 pool start with 192.168.1.2
        "endip":"192.168.1.100",     # IPv4 pool end with 192.168.1.100
        "mask":"255.255.255.0",      # netmask is 255.255.255.0
        "lease":"86400",             # lease is 86400
        "gw":"",                     # not configure the gateway, default assigns 192.168.1.1
        "dns":""                     # not configure the dns, default assigns 192.168.1.1
    }    
}
```
Example, modify the local network ip address
```shell
ifname@lan:static/ip=192.168.2.1
ttrue
```
Example, disable the local network dhcp server
```shell
ifname@lan:dhcps/status=disable
ttrue
```

#### **Methods**
**ifname@lan** is first local network
**ifname@lan2** is second local network

+ `status[]` **get the local network infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current status",        // [ uping, down, up ]
                                             // uping for connecting
                                             // down for the network is down
                                             // up for the network is connect succeed
        "mode":"IPV4 address mode",     // [ dhcpc ] for DHCP, [ static ] for manual setting
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "livetime":"online time",       // hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ MAC address ]
        "method":"IPv6 address mode",   // [ disable ] is not use ipv6, [ manual ] for manual setting， [ automatic ] for DHCPv6, [ slaac ] for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ IPv6 address ]
        "addr2":"IPv6 address2",        // [ IPv6 address ]
        "addr3":"IPv6 address3"         // [ IPv6 address ]
    }
    ```
    ```shell
    # examples, get the first local network infomation
    ifname@lan.status
    {
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
    ```

+ `netdev[]` **get the netdev**, *succeed return netdev, failed return NULL, error return terror*
    ```shell
    # examples, get the first local network netdev
    ifname@lan.netdev
    lan
    ```

+ `shut[]` **shutdown the local network**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, shutdown the second local network
    ifname@lan2.shut
    ttrue
    ```

+ `setup[]` **setup the local network**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, setup the frist local network
    ifname@lan.setup
    ttrue
    ```


