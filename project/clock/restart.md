
***
## System restart Plan Management   
The management system restarts automatically, and it restarts the system at a specified time

#### Configuration( clock@restart )   
```json
// attribute introduction
{
    "mode":"Restart plan mode",                        // [ "disable", "age", "point", "idle" ]
                                                         // "disable": disable restart auto
                                                         // "age": maximum runtime to restart
                                                         // "point": fixed-point to restart
                                                         // "idle": idle specifie period to restart

    "age":"The maximum runtime",                               // [ number ], The unit is seconds, valid for "mode" is "age"

    "point_age":"The maximum runtime",                         // [ number ], valid for "mode" is "point"
    "point_hour":"Specifies when hour to restart",             // [ number ], valid for "mode" is "point"
    "point_minute":"Specifies when minute of hour to restart", // [ number ], valid for "mode" is "point"

    "idle_start":"Start runtime",                              // [ number ], valid for "mode" is "idle"
    "idle_wireless_time":"Idle time",                          // [ number ], valid for "mode" is "idle"
    "idle_hour":"Specifies when hour to restart",              // [ number ], valid for "mode" is "idle"
    "idle_minute":"Specifies when minute of hour to restart",  // [ number ], valid for "mode" is "idle"
    "idle_age":"The maximum runtime"                           // [ number ], valid for "mode" is "idle"
}
```

Example show the configure that is age mode, the will restart auto at system run 2880 second
```shell
clock@restart
{
    "mode":"age",
    "age":"2880",
}
```

Example show the configure that is point mode, the will restart at 23:45
```shell
clock@restart
{
    "mode":"point",
    "point_hour":"23",
    "point_minute":"45",
    "point_age":"2880"
}
```

Example disable the restart function
```shell
clock@restart:mode=disable
ttrue
```

Example set will restart auto at system run 3600 second
```shell
clock@restart|{"mode":"age","age":"3600"}
ttrue
```

Example set will restart auto at 03:30
```shell
clock@restart|{"mode":"point","point_hour":"03","point_minute":"30"}
ttrue
```

