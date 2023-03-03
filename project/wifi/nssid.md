***
## 2.4G SSID management components
Manage 2.4G SSID
Usually wifi@nssid is the first 2.4G SSID. If there are multiple 2.4G SSID in the system, wifi@nssid2 will be the second 2.4G SSID, and increase by degress

#### **configuration( wifi@nssid )**
**wifi@nssid** is first 2.4G SSID
**wifi@nssid2** is second 2.4G SSID

```json
// Attribute introduction
{
    "status":"SSID status",                 // [ "enable", "disable" ]
    "ssid":"SSID name",                     // [ string ]
    "isolated":"isolated clients",          // [ "enable", "disable" ]
    "broadcast":"broadcast SSID",           // [ "enable", "disable" ]
    "secure":"secure mode",                 // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                // disable for disable the secure
                                                // wpapsk for WPAPSK
                                                // wpa2psk for WPA2PSK
                                                // wpapskwpa2psk for WPA Auto
    "wpa_encrypt":"wpa encrypt mode",       // [ "aes", "tkip", "tkipaes" ]
                                                // aes for AES
                                                // tkip for TKIP
                                                // tkipaes for Auto
    "wpa_key":"WPA password",               // [ string ], the length must be greater than 8
    "wpa_rekey":"WPA key renegotiate time", // [ number ], the unit is second, space for no renegotiate

    "acl":"access control function",        // [ "disable", "accept", "drop" ]
                                                // disable 
                                                // accept for white list
                                                // drop for blacklist
    "acl_table":                            // white list or black list, vaild when acl be "accept" or "drop"
    {
        "MAC Address":"",                   // [ MAC address ]:""
        //... more mac address
    },
    "maxsta":"maximum number of clients",   // [ nubmer ], space for no limit

    "wmm":"WMM state",                      // [ "enable", "disable" ]
    "options":                              // custom attribute list for radio chip
    {
        "custom attribute":"value of custom attribute",
        //... more custom attribute
    }
}

Example, show first 2.4G SSID all configure
```shell
wifi@nssid
{
    "status":"enable",              # enable the SSID
    "ssid":"5228-test-2.4g",        # SSID name is 5228-test-2.4g
    "isolated":"disable",           # clients are not quarantined, client can access echo other
    "broadcast":"enable",           # broadcast the SSID name
    "secure":"wpapskwpa2psk",       # secure mode is WPA Auto
    "wpa_encrypt":"tkipaes",        # WAP encrypt is Auto
    "wpa_key":"22222222",           # WPA encrypt key is 22222222
    "wpa_rekey":"",                 # WPA encrypt key no renegotiate
    "acl":"accept",                 # white list
    "acl_table":                    # white list content, only the MAC address 00:22:33:11:33:22/00:22:33:11:33:23/00:22:33:11:33:24/00:22:33:11:33:EB can access
    {
        "00:22:33:11:33:22":"",
        "00:22:33:11:33:23":"",
        "00:22:33:11:33:24":"",
        "00:22:33:11:33:EB":""
    },
    "maxsta":"64",                  # supports simultaneous access by a maximum of 64 clients
    "wmm":"enable"                  # enable the WMM
}
```  
Example, modify the first 2.4G SSID name be myNewSSID
```shell
wifi@nssid:ssid=myNewSSID
ttrue
```
Example, disable the first 2.4G SSID
```shell
wifi@nssid:status=disable
ttrue
```
Example, enable the first 2.4G SSID
```shell
wifi@nssid:status=enable
ttrue
```
Example, modify the first 2.4G SSID secure mode to WPAPSK and change the wpa key to 88888888
```shell
wifi@nssid:secure=wpapsk            # modify the secure mode to WPAPSK
ttrue
wifi@nssid:wpa_encrypt=tkipaes      # modify the WAP encrypt to auto
ttrue
wifi@nssid:wpa_key=88888888         # modify the WPA encrypt key is 88888888
ttrue
# You can also use one command to complete the operation of the above three command
wifi@nssid|{"secure":"wpapsk", "wpa_encrypt":"tkipaes", "wpa_key":"88888888"}
ttrue
```
Example, disable the second 2.4G SSID
```shell
wifi@nssid2:status=disable
ttrue
```

#### **Methods**
**wifi@nssid** is first 2.4G SSID
**wifi@nssid2** is second 2.4G SSID

+ `status[]` **get the SSID infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
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
        "rate":"current rate"                   // [ number ]
    }
    ```
    ```shell
    # examples, get the first 2.4G SSID status
    wifi@nssid.status
    {
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
    ```

+ `stalist[]` get list of clients, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "client MAC address":              // [ MAC address ]:{}
        {
            "livetime":"online time",               // [ hour:minute:second:day ]
            "rssi":"signal strength",               // [ number ], the unit maybe dBm or %
        },
        // ... more client
    }
    ```
    ```shell
    # examples, get the first 2.4G SSID's client list
    wifi@nssid.list
    {
        "78:11:DC:92:D3:9E":                  // client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "88:C3:97:75:1B:C0":                 // client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "40:31:3C:4D:78:35":                 // client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@nssid"
        }
    }
    ```

+ `stabeat[ MAC address ]` disconnect the client, *succeed return tttrue, failed return tfalse, error return terror*
    ```shell
    # examples, disconnect the client 00:03:7F:13:BD:30 from first 2.4G SSID
    wifi@nssid.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

