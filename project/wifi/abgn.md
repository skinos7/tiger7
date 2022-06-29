
***
## Wireless radio driver component
Common 11BGN/11AN/11AC/11AX wireless radio match driver
Match appropriate management components for SDIO/USB/PCI devices

#### **Configuration( arch@abgn )**

```json
// Attributes introduction 
{
    "vendor identify":
    {
        "device identify":
        {
            "options":
            {
                "mode":
                {
                }
            },
            "hostapd":
            {
                "options":
                {
                },
                "value":
                {

                }
            }
        }

    }
    // "...": { ... }    // You can map multiple vendor identify match
}
// Examples
{
    "02d0":                       // vendor identify is 02d0
    {
        "aae8":                   // device identify is aae8
        {
            "options":            // options for the device 
            {
                "mode":                // this device have three mode: n, a, ac. you need provide the opitns to user to select
                {
                    "n":"",
                    "a":               // this device have two bandwitch: 20M, 40M at the mode a
                    {
                        "20":"",
                        "40":""
                    },
                    "ac":              // this device have three bandwitch: 20M, 40M, 80M at the mode ac
                    {
                        "20":"",
                        "40":"",
                        "80":""
                    },
                    "acs":"disable"    // not support channel auto
                },
                "txpower":             // this device have 6 tx power level to select: 17dBm, 18dBm, 19dBm, 20 dBm, 21dBm, 22dBm
                {
                    "17":"",
                    "18":"",
                    "19":"",
                    "20":"",
                    "21":"",
                    "22":""
                }
            },
            "hostapd":            // hostapd related configurations
            {
                "options":              // options for hostapd configure to driver the device
                {
                    "n":                        // add the "supported_rates", "basic_rates", "ht_coex options" to hostapd configure at 11n mode
                    {
                        "supported_rates":"60 90 120 180 240 360 480 540",
                        "basic_rates":"60 120 240",
                        "ht_coex":"0"
                    },
                    "a":                        // add the "tx_queue_data2_burst", "ht_coex" options to hostapd configure at 11a mode
                    {
                        "tx_queue_data2_burst":"2.0",
                        "ht_coex":"0"
                    },
                    "ac":                       // add the "tx_queue_data2_burst", "ht_coex" options to hostapd configure at 11ac mode
                    {
                        "tx_queue_data2_burst":"2.0",
                        "ht_coex":"0"
                    },
                    "ht_capab":"[DSSS_CCK-40]",                                                                         // ht_capab options patch
                    "vht_capab":"[SU-BEAMFORMER][SU-BEAMFORMEE][MAX-MPDU-11454][MAX-A-MPDU-LEN-EXP7][VHT-LINK-ADAPT-3]" // vht_capab options patch
                },
                "value":               // value patch to the hostapd ht_capab and vht_capab
                {
                    "ht_capab":                   // value patch to ht_capab
                    {
                        "ldpc":"",
                        "stbc":""
                    },
                    "vht_capab":                  // value patch to vht_capab
                    {
                        "ldpc":"[RXLDPC]",             // patch to [RXLDPC] to vht_capab when ldpc is enable
                        "stbc":""
                    }
                }
            }
        }
    }
}
``` 


