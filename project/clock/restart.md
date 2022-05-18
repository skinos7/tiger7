
***
## Restart plan component
The management system restarts automatically, and it restarts the system at a specified time

#### Configuration( clock@restart )

```json
// attribute introduction
{
    "mode":"Restart plan mode",                        // [ age, point, idle ]
                                                         // age: maximum runtime to restart
                                                         // point: fixed-point to restart
                                                         // idle: idle specifie period to restart

    "age":"The maximum runtime",                            // [ number ], The unit is seconds, valid for "mode" is age

    "point_age":"The maximum runtime",                         // [ number ], valid for "mode" is point
    "point_hour":"Specifies when hour to restart",             // [ number ], valid for "mode" is point
    "point_minute":"Specifies when minute of hour to restart", // [ number ], valid for "mode" is point

    "idle_start":"Start runtime",                           // [ number ], valid for "mode" is idle
    "idle_wireless_time":"Idle time",                       // [ number ], valid for "mode" is idle
    "idle_age":"The maximum runtime",                       // [ number ], valid for "mode" is idle
}
// example for age mode, the will restart auto at system run 2880 second
{
    "mode":"age",
    "age":"2880",
}
// example2 for age point
{
    "mode":"point",
    "point_hour":"23",
    "point_minute":"45",
    "point_age":"2880"
}
```



## 自动重启管理组件
管理系统自动重启， 它将在指定的时间重启系统


#### **配置( clock@restart )** 
```json
// 属性介绍
{
    "mode":"重启模式",              // 分为:最大运行时长(age)|定点重启(point)|空闲时重启(idle)三种

    "age":"最大运行时长",            // 单位为秒, 模式为 最大运行时长时 有效

    "point_age":"系统运行最大时长",  // 模式为 定点重启 时而时间不正常时到系统运行最大时长时立即重启
    "point_hour":"指定重启的时",     // 模式为 定点重启 时且时间正常时有效
    "point_minute":"指定重启的分",   // 模式为 定点重启 时且时间正常时有效

    "idle_start":"开始启动",         // 从这个时长后<开始启动>空间重启机制(秒为单位), 模式为 空闲时重启 有效
    "idle_wireless_time":"空闭时长", // 达到空闭时长都无客户端接入即重启(秒为单位), 模式为 空闲时重启 有效
    "idle_age":"系统运行最大时长",   // 或达到系统运行最大时长时立即重启, 模式为 空闲时重启 有效
}

// 示例
{
    "mode":"age",    // 重启模式为最大运行时长
    "age":"2880",    // 运行时长达到2880为秒即重启
}
// 示例2
{
    "mode":"point",           // 重启模式为定点重启, 即在获取到时间后会在一个指定的时间点重启系统
    "point_hour":"23",        // 23点即晚上11点
    "point_minute":"45",      // 第45分钟
    "point_age":"2880"        // 如果一直未能获取到时间即在系统运行达到此秒数时重启
                              // 即如果系统能获取到时间则每天的23点45分钟重启, 否则就等待系统运行秒数达到2880时重启系统
}
```

