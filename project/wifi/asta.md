***
## 5.8G Station Management   
Manage 5.8G Station


#### Configuration( wifi@asta )   

```json
// Attribute introduction
{
    "status":"enable or  disable the function",     // [ "enable", "disable" ]

    // wireless connect
    "peer":"SSID to connect",              // [ string ]
    "peermac":"BSSID to connect",          // [ mac address ]
    "peermode":"mode of connection",       // [ "hidden" ] Indicates that the peer end does not broadcast SSID. In hidden mode, channel must not be empty  
    "channel":"wireless channel",          // [ number ], 0, 36-165, 0 for auto
    "secure":"mode of security",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] for no securiyt
                                                            // [ wpapsk ]  for WPAPSK
                                                            // [ wpa2psk ]  for WPA2PSK
                                                            // [ wpapskwpa2psk ] for WPA Mix
    "wpa_encrypt":"WAP encrypt",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] for AES
                                                            // [ tkip ] for TKIP
                                                            // [ tkipaes ] for auto
    "wpa_key":"WPA key"                    // [ string ], The value is a string of at least 8 characters. This parameter is mandatory if the "secure" be "wpapsk" or "wpa2psk" or "wpapskwpa2psk"
    "ssid_disable":"disable the ssid",     // [ "disable", "enable" ], disable the local ssid when connected

}
```

Example, show 5.8G Station all configure
```shell
{
    # WIFI peer
    "peer":"V520-D21D20-5G",    # connect V520-D21D20-5G
    "secure":"wpapsk",       # security is WPAPSK
    "wpa_encrypt":"aes",     # encrypt use AES
    "wpa_key":"87654321",    # password 87654321
}
```

Example, modify the SSID for 5.8G Station connect
```shell
wifi@asta:peer=Myhotpot
ttrue
wifi@asta:secure=wpapsk
ttrue
wifi@asta:wpa_key=88888888
ttrue
# You can also use one command to complete the operation of the above three command
iwifi@asta|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```

Example, disable the 5.8G Station connect
```shell
wifi@asta:status=disable
ttrue
```

Example, enable the 5.8G Station connect
```shell
wifi@asta:status=enable
ttrue
```



#### **Methods**   

+ `status[]` **get the 5.8G Station infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current state",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed

        "peer":"Peer SSID",              // [ string ]
        "peermac":"Peer BSSID",          // [ MAC address ]
        "channel":"Peer channel",        // [ 0, 36-165 ]
        "rate":"connect rate",           // [ number ], the unit is M
        "rssi":"Peer RSSI",              // [ number ], the unit is dBm
        "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
    }
    ```

    ```shell
    # examples, get the 5.8G Station infomation
    wifi@asta.status
    {

        "status":"up",                     # connect is succeed
        "peer":"TP-link-2231-5G",            # peer is TP-link-2231-5G
        "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
        "channel":"10",                   # channel is 10
        "rate":"270",                     # rate is 270M
        "rssi":"-41",                     # rssi is -41dBm
        "signal":"3"                      # signal level is 3
    }
    ```

+ `netdev[]` **get the 5.8G Station netdev**, *succeed return netdev, failed return NULL, error return terror*   
    ```shell
    # examples, get the 5.8G Station netdev
    wifi@asta.netdev
    ath11
    ```


+ `aplist[]` **use the 5.8G Station scan the surrounding AP**, *succeed return talk to describes infomation, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID name",                           // [ string ]
            "channel":"channel number",                   // [ number ], 0,36-165, 0 for auto
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
    # examples, get the surrounding AP from 5.8G Station scan
    wifi@asta.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # frist AP SSID
            "channel":"36",                                # first AP channel
            "secure":"wpapskwpa2psk",                      # secure mode is WPA mix
            "wpa_encrypt":"aes",                           # encrypt type is AES
            "sig":"70",                                    # signal is 70%
            "signal":"3",                                  # signal level is 3, range is 0-4
            "chext":"below",                               # extern channel is below
            "mode":"11a/n/ac ABOVE"
        },
        "B4:82:C5:80:22:81":                    # second AP by scanning
        {
            "ssid":"dimmalex-work",
            "channel":"42",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11a/n/ac"
        },
        "8C:74:A0:D6:68:B0":                    # third AP by scanning
        {
            "ssid":"CMCC-ktfK",
            "channel":"165",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11a/n/ac"
        }
    }
    ```

+ `shut[]` **shutdown the 5.8G Station**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, shutdown the 5.8G Station
    wifi@asta.shut
    ttrue
    ```

+ `setup[]` **setup the 5.8G Station**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, setup the 5.8G Station
    wifi@asta.setup
    ttrue
    ```

