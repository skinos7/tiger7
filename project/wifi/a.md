***
## 5.8G Radio Management   
Manage 5.8G Radio

#### Configuration( wifi@a )   

```json
// Attribute introduction
{
    "mode":"radio mode",                 // [ "a", "anac", "ac" ]
                                                // "a" for 11A
                                                // "anac" for 11A/AC
                                                // "ac" for 11AC
    
    "bandwidth":"radio bandwidth",       // [ "40", "80", "160" ]
                                                // "40" for 40M
                                                // "80" for 80M
                                                // "160" for 160M

    "channel":"radio channel",           // [ 0, 36-165 ], "0" for auto select the channel
    "beacon":"beacon interval",          // [ number ]
    "country":"country code",            // [ "cn", "en", "jp", ... ]

    "dtim":"radio dtim",                 // [ number ]
    "ldpc":"use LDPC or not",            // [ "disable", "enable" ]
    "shortgi":"short GI",                // [ "disable", "enable" ]
    "stbc":"radio STBC"                  // [ "disable", "enable" ]
}
```

Example, show 5.8G Radio all configure
```shell
wifi@a
{
    "mode":"n",                # 5.8G Radio is 11N
    "bandwidth":"40",          # 5.8G bandwidth 40M
    "channel":"165",           # 5.8G channel is 165

    "beacon":"100",            # 5.8G beacon interval is 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```  

Example, modify the 5.8G Radio channel to auto
```shell
wifi@a:channel=0
ttrue
```

Example, modify the 5.8G Radio channel to 36
```shell
wifi@a:channel=36
ttrue
```


#### **Methods**   

+ `chlist[]` **get the 5.8G radio channel list**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "channel number":{}                // [ number ]:{}
        // more channel number
    }
    ```

    ```shell
    # examples, get the 5.8G radio channel list
    wifi@a.chlist
    {
        "36":{},
        "40":{},
        "44":{},
        "48":{},
        "52":{},
        "56":{},
        "60":{},
        "64":{},
        "149":{},
        "153":{},
        "157":{},
        "161":{},
        "165":{}
    }
    ```

+ `stalist[]` get list of clients on 5.8G radio, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*   
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
    # examples, get the 5.8G radio's client list
    wifi@a.stalist
    {
        "78:11:DC:92:D3:9E":                  // client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "88:C3:97:75:1B:C0":                 // client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "40:31:3C:4D:78:35":                 // client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@assid"
        }
    }
    ```

+ `stabeat[ MAC address ]` disconnect the client, *succeed return tttrue, failed return tfalse, error return terror*   
    ```shell
    # examples, disconnect the client 00:03:7F:13:BD:30 from 5.8G Radio
    wifi@a.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

