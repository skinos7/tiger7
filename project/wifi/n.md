***
## 2.4G Radio Management   
Manage 2.4G Radio

#### Configuration( wifi@n )   

```json
// Attribute introduction
{
    "mode":"radio mode",                 // [ "n", "b", "bg" ]
                                                // "n" for 11N
                                                // "b" for 11B
                                                // "bg" for 11BG
    
    "bandwidth":"radio bandwidth",       // [ "20", "2040", "40" ]
                                                // "20" for 20M
                                                // "2040" for 20M and 40M
                                                // "40" for 40M

    "channel":"radio channel",           // [ 0-14 ], "0" for auto select the channel
    "beacon":"beacon interval",          // [ number ]
    "country":"country code",            // [ "cn", "en", "jp", ... ]

    "dtim":"radio dtim",                 // [ number ]
    "ldpc":"use LDPC or not",            // [ "disable", "enable" ]
    "shortgi":"short GI",                // [ "disable", "enable" ]
    "stbc":"radio STBC"                  // [ "disable", "enable" ]
}
```

Example, show 2.4G SSID all configure
```shell
wifi@n
{
    "mode":"n",                # 2.4G Radio is 11N
    "bandwidth":"40",          # 2.4G bandwidth 40M
    "channel":"11",            # 2.4G channel is 11

    "beacon":"100",            # 2.4G beacon interval is 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```  

Example, modify the 2.4G radio channel to auto
```shell
wifi@n:channel=0
ttrue
```

Example, modify the 2.4G radio channel to 11
```shell
wifi@n:channel=11
ttrue
```


#### **Methods**   

+ `chlist[]` **get the 2.4G radio channel list**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "channel number":{}                // [ number ]:{}
        // more channel number
    }
    ```

    ```shell
    # examples, get the 2.4G radio channel list
    wifi@n.chlist
    {
        "1":{},
        "2":{},
        "3":{},
        "4":{},
        "5":{},
        "6":{},
        "7":{},
        "8":{},
        "9":{},
        "10":{},
        "11":{},
        "12":{},
        "13":{}
    }
    ```

+ `stalist[]` get list of clients on 2.4G radio, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "client MAC address":              // [ MAC address ]:{}
        {
            "livetime":"online time",               // [ hour:minute:second:day ]
            "rssi":"signal strength",               // [ number ], the unit maybe dBm or %
        }
        // ... more client
    }
    ```

    ```shell
    # examples, get the 2.4G radio's client list
    wifi@n.stalist
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
    # examples, disconnect the client 00:03:7F:13:BD:30 from 2.4G Radio
    wifi@n.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

