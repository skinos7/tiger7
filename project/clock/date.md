***
## Manage System Date   
Manage system date

#### Configuration( clock@date )   
```json
// Attributes introduction
{
    "timezone":"time zone",                             // [ number ], -12 to 12, West 12 to East 12
    "ntpclient":"whether to start the NTP client",      // [ "disable", "enable" ]
    "ntpserver":"NTP Server",                           // [ string ]
    "ntpserver2":"NTP Server 2",                        // [ string ]
    "ntpserver3":"NTP Server 3",                        // [ string ]
    "ntpinterval":"NTP Synchronization interval"        // [ number ], interval (in seconds) for time synchronization with the NTP server
}
```

Examples, show all the configure
```shell
clock@date
{
    "timezone":"8",                   # time zone is East 8, china
    "ntpclient":"enable",             # enable the NTP client to synchronization with NTP server
    "ntpserver":"ntp1.aliyun.com",    # ntp1.aliyun.com, ntp2.aliyun.com, ntp3.aliyun.com, Try in turn until you succeed
    "ntpserver2":"ntp2.aliyun.com",
    "ntpserver3":"ntp3.aliyun.com",
    "ntpinterval":"86400"             # synchronization every 86400 seconds
}
```  

Examples, modify the time zone to West 5
```shell
clock@date:timezone=-5
ttrue
```  

Examples, disable the NTP client time synchronization
```shell
clock@date:ntpclient=disable
ttrue
```  



#### **Methods**   
+ `status[]` **get the date infomation**, *succeed return talk to describes, failed return NULL, error return terror*    
    ```json
    // Attributes introduction of talk by the method return
    {
        "source":"The source of the time",                // [ "ntp", "set", "lte", "gps" ]
                                                             // ntp: indicates that it originated from NTP, which has the highest NTP priority, and NTP synchronization success covers all other times
                                                             // set: indicates time set by manually
                                                             // rtc: indicates source RTC time  
                                                             // lte: indicates source LTE time 
                                                             // gps: indicates source GPS time 
                                                             // An empty or none of this node indicates that it has not been set
        "current":"current date",                         // [ string ], format is hour:minuti:second:month:day:year
        "livetime":"system live time",                    // [ string ], format is hour:minute:second:day
        "uptime":"system uptime in second"                // [ number ]
    }    
    ```

    ```shell
    # examples, get the current date
    clock@date.status
    {
        "current":"12:29:41:05:10:2022",         # current is 12:29:41, On May 10, in 2022
        "livetime":"00:01:58:0",                 # system run 1 minute and 58 second
        "uptime":"118"                           # system run 118 second
    }  
    ```

+ `current[ current date ]` **set current date for gateway**, *succeed return ttrue, failed return tfalse, error return terror*    
    ```shell
    # examples, set 11:12:23, On July 8th, in 2019
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```

+ `ntpsync[ [NTP Server] ]` **sync the time for gateway with NTP server**, *succeed return ttrue, failed return tfalse, error return terror*    
    ```shell
    # examples, sync the time with time.window.com
    clock@date.ntpsync[ time.window.com ]
    ttrue
    # examples, sync the time with NTP server in the configure
    clock@date.ntpsync
    ttrue
    ```
