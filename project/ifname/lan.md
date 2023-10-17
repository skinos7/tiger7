***
## Local/LAN Network Management
Manage local network. This component must depend on local network interface or network switch(SOC) and network management framework project  
Usually ifname@lan is the first local network. If there are multiple local network in the system, ifname@lan2 will be the second local network, and increase by degress

#### **configuration( ifname@lan )**   
**ifname@lan** is first local network   
**ifname@lan2** is second local network   

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ "enable", "disable" ]

    // IPv4
    "mode":"IPV4 address mode",            // [ "dhcpc", "static" ] "dhcpc" for DHCP, "static" for manual setting
    "static":                                       // detial configure for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 netmask >

        "ip2":"IPv4 address 2",                     // < ipv4 address >
        "mask2":"IPv4 netmask 2",                   // < ipv4 netmask >

        "ip3":"IPv4 address 3",                     // < ipv4 address >
        "mask3":"IPv4 netmask 3",                   // < ipv4 netmask >
        
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                               // detial configure for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ]
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ]
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], This is valid when "custom_dns" is "enable"
    },
    "dhcps":                                               // detial configure for "mode" is "ppp"
    {
        "status":"Whether to start the DHCP service",                      // [ "disable", "enable" ]
        "startip":"The start address within the IPv4 allocation pool",     // [ ipv4 address ]
        "endip":"IPv4 assigns the end address within the pool",            // [ ipv4 address ]
        "mask":"IPv4 assigns a subnet mask within a pool",                 // [ ipv4 netmask ]
        "lease":"lease time for assigns",                                  // [ number ], the unit is second
        "gw":"Specifies the IPv4 gateway",                                 // [ ipv4 address ], default is local network IP address
        "dns":"Specifies the IPv4 DNS",                                    // [ ipv4 address ], default is local network IP address
        "dns2":"Specifies the IPv4 backup dns",                            // [ ipv4 address ]
        "options":"dnsmasq original options"                               // [ string ], multiple options are separated by semicolons
    },

    // IPv6
    "method":"IPv6 address mode",             // [ "disable", "manual", "automatic", "slaac", "relay" ]
                                                    // "disable" is not use ipv6
                                                    // "manual" for manual setting
                                                    // "automatic" for DHCPv6
                                                    // "slaac" for Stateless address autoconfiguration
                                                    // "relay" for relay the extern network ipv6 address
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
    "addrpool":
    {
        "status":"Whether to start the DHCPv6 service",                      // [ "disable", "enable" ]
        "startaddr":"The start address within the IPv6 allocation pool",     // [ ipv6 address ]
        "endaddr":"IPv6 assigns the end address within the pool",            // [ ipv4 address ]
        "prefix":"IPv6 assigns a subnet mask within a pool",                 // [ ipv4 netmask ]
        "leasetime":"lease time for assigns",                                // [ number ], the unit is second
        "hop":"Specifies the IPv4 gateway",                                  // [ ipv4 address ], default is local network IP address
        "resolve":"Specifies the IPv4 DNS",                                  // [ ipv4 address ], default is local network IP address
        "resolve2":"Specifies the IPv4 backup dns"                           // [ ipv4 address ]
    }

}
```

Example, show all first local network configure
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

Example, modify the first local network ip address, Take effect after restart
```shell
ifname@lan:static/ip=192.168.2.1
ttrue
```

Example, disable the first local network dhcp server, Take effect after restart
```shell
ifname@lan:dhcps/status=disable
ttrue
```

Example, modify the first local network dhcp pool, start ip 192.168.2.100, end ip 192.168.2.200
```shell
ifname@lan:dhcps|{"startip":"192.168.2.100","endip":"192.168.2.200"}
ttrue
```


#### **Methods**
**ifname@lan** is first local network   
**ifname@lan2** is second local network   

+ `status[]` **get the local network infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
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
        "addr3":"IPv6 address3"         // [ ipv6 address ]
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
    # examples, shutdown the first local network
    ifname@lan.shut
    ttrue
    # examples, shutdown the second local network
    ifname@lan2.shut
    ttrue
    ```

+ `setup[]` **setup the local network**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, setup the frist local network
    ifname@lan.setup
    ttrue
    # examples, setup the second local network
    ifname@lan2.setup
    ttrue
    ```


