
***
## System restart plan management
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
```


## 自动重启管理组件
管理系统自动重启， 它将在指定的时间重启系统

#### **配置( clock@restart )** 
```json
// 属性介绍
{
    "mode":"重启模式",              // [ "disable", "age", "point", "idel" ]
                                        // disable为禁用重启功能
                                        // age为达到最大运行时长时重启
                                        // point为定点重启
                                        // idle为空闲重启

    "age":"最大运行时长",            // [ 数字 ], 单位为秒, 重启模式为"age"(最大运行时长)时有效

    "point_age":"系统运行最大时长",  // [ 数字 ], 单位为秒, 重启模式为"point"(定点重启)时有效, 在系统运行最大时长到达此秒数时立即重启
    "point_hour":"指定重启的时",     // [ 0-23 ], 时, 重启模式为"point"(定点重启)时有效, 需要系统获到到正确的时间才启效
    "point_minute":"指定重启的分",   // [ 0-59 ], 分, 重启模式为"point"(定点重启)时有效, 需要系统获到到正确的时间才启效

    "idle_start":"开始启动",         // [ 数字 ], 单位为秒, 重启模式为"idle"(空闲重启)时有效, 在系统运行最大时长到达时这个时长后空间重启机制
    "idle_time":"空闭时长",          // [ 数字 ], 单位为秒, 重启模式为"idle"(空闲重启)时有效, 无客户端时长达到此值即重启(秒为单位)
    "idle_age":"系统运行最大时长",    // [ 数字 ], 单位为秒, 重启模式为"idle"(空闲重启)时有效, 在系统运行最大时长到达此秒数时立即重启
}
```
示例, 显示整个组件配置
```shell
clock@date
{
    "mode":"age",    // 重启模式为最大运行时长
    "age":"2880",    // 运行时长达到2880为秒即重启
}
```
示例, 禁用自动重启
```shell
clock@restart:mode=disable
ttrue
```

