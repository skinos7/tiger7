***
## LTE/NR network management components
Manage LTE/NR networks and 4G/NR basebands. This component must depend on the LTE/NR baseband components and network Management Framework project  
Usually ifname@lte is the first LTE/NR network and module. If there are multiple LTE/NR modules in the system, ifname@lte2 will be the second LTE/NR network and module, and increase by degress

#### **configuration( ifname@lte )**
**ifname@lte** is first LTE network
**ifname@lte2** is second LTE network

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ enable, disable ]

    // IPv4
    "tid":"table identify number",         // [ number ] exclusive route table ID, only for multiple WAN
    "metric":"default route metric",       // [ number  ]
    "mode":"IPV4 address mode",            // [ dhcpc ] for DHCP, [ static ] for manual setting, [ ppp ] for PPP dial
    "static":                                       // detial configure for "mode" is [ static ]
    {
        "ip":"IPv4 address",                        // < IPv4 address >
        "mask":"IPv4 netmask",                      // < IPv4 etmask >
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
    "ppp":                                               // detial configure for "mode" is [ ppp ]
    {
        "mtu":"Maximum transmission unit",               // [ number ], The unit is in bytes
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ disable, enable ]
        "dns":"Custom DNS1",                             // [ IP address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2",                            // [ IP address ], This is valid when custom_dns is [ enable ]
        "txqueuelen":"tx queue len"
    },
    "masq":"out stream share the interface IPv4 address to access the Internet",  // [ disable, enable ]

    // IPv6
    "method":"IPv6 address mode",                       // [ disable ] is not use ipv6
                                                        // [ manual ] for manual setting
                                                        // [ automatic ] for DHCPv6
                                                        // [ slaac ] for Stateless address autoconfiguration
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
    "masquerade":"out stream share the interface IPv6 address to access the Internet",   // [ disable, enable ]

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
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds

        },
        "recv":                                                                                          // detial configure for type is [ recv ]
        {
            "timeout":"How many seconds did not receive a packet considered a failure",                  // [ number ], The unit is in seconds
            "packets":"How many packets",                                                                // [ number ]
            "failed":"failed times"                                                                      // [ number ]
        }
    },

    // TTL settings
    "ttl":"ttl number",                   // default no modify

    // For LTE/NR baseband Settings, the parameters are the same as modem@lte
    // ##### For details, see configuration of modem@lte  #####

}
```
Example, show LTE all configure
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
    }
}
```
Example, modify the keeplive to icmp for LTE1
```shell
ifname@lte:keeplive/type=icmp
ttrue
```
Example, modify the keeplive to icmp for LTE2
```shell
ifname@lte2:keeplive/type=icmp
ttrue
```
Example, modify the icmp keeplive destination address for LTE2
```shell
ifname@lte2:keeplive/icmp/dest/test=8.8.8.8            # modify the icmp keeplive first destination address to 8.8.8.8
ttrue
ifname@lte2:keeplive/icmp/dest/test2=8.8.4.4           # modify the icmp keeplive second destination address to 8.8.4.4 
ttrue
ifname@lte2:keeplive/icmp/dest/test3=114.114.114.114   # modify the icmp keeplive third destination address to 114.114.114.114
ttrue
# You can also use one command to complete the operation of the above three command
ifname@lte2:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```
Example, disable the LTE2 network
```shell
ifname@lte2:status=disable
ttrue
```

#### **Methods**
**ifname@lte** is first LTE network
**ifname@lte2** is second LTE/NR network

+ `status[]` **get the LTE network infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current state",        // [ setup, register, ready, nodevice, reset, down, up ]
                                             // setup for setup the modem
                                             // register for register the network
                                             // ready for ready to connect to internet, hint signal/network/simcard all ok
                                             // nodevice for the corresponding module could not be found
                                             // reset for reset the modem
                                             // down for the network is down
                                             // up for the network is connect succeed
        "mode":"IPV4 address mode",     // [ dhcpc ] for DHCP, [ static ] for manual setting， [ ppp ] for PPP dial
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "delay":"delay time",           // [ failed, 0-10000 ], failed for icmp failed
        "livetime":"online time",       // hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ MAC address ]
        "method":"IPv6 address mode",   // [ disable ] is not use ipv6, [ manual ] for manual setting， [ automatic ] for DHCPv6, [ slaac ] for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ IPv6 address ]
        // For LTE/NR baseband Status, the parameters are the same as modem@lte.status
        // ##### For details, see status of modem@lte  #####
    }
    ```
    ```shell
    # examples, get the first lte network infomation
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
    ```shell
    # examples, get the first LTE network netdev
    ifname@lte.netdev
    usb0
    ```

+ `ifdev[]` **get the ifdev**, *succeed return ifdev, failed return NULL, error return terror*
    ```shell
    # examples, get the first LTE network ifdev
    ifname@lte.ifdev
    modem@lte
    ```

+ `shut[]` **shutdown the modem network**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, shutdown the frist LTE network
    ifname@lte.shut
    ttrue
    ```

+ `setup[]` **setup the modem network**, *succeed return tttrue, failed return tfalse, error return terror*
    ```shell
    # examples, setup the second LTE network
    ifname@lte2.setup
    ttrue
    ```

