
***
## Manage system date
Manage system date


#### Configuration( clock@date )

```json
// Attributes introduction
{
    "timezone":"time zone",                             // [ -12 to 12 ], West 12 to East 12
    "ntpclient":"whether to start the NTP client",      // [ disable, enable ]
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

#### **Methods**

+ `status[]` **get the date infomation**, *succeed return talk to describes, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "current":"current date",                         // hour:minuti:second:month:day:year
        "livetime":"system live time",                    // hour:minute:second:day
        "uptime":"system uptime in second",               // [ number ]
        "source":"The source of the time",                // [ ntp, set, lte, gps ]
                                                             // ntp: indicates that it originated from NTP, which has the highest NTP priority, and NTP synchronization success covers all other times
                                                             // set: indicates time set by manually
                                                             // rtc: indicates source RTC time  
                                                             // lte: indicates source LTE time 
                                                             // gps: indicates source GPS time 
                                                             // An empty or none of this node indicates that it has not been set
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

+ `current[ <current date> ]` **set current date**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, set 11:12:23, On July 8th, in 2019
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```

+ `ntpsync[ [NTP Server] ]` **sync the time with NTP server**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, sync the time with time.window.com
    clock@date.ntpsync[ time.window.com ]
    ttrue
    # examples, sync the time with NTP server in the configure
    clock@date.ntpsync
    ttrue
    ```




## 时间管理组件
管理系统时间

#### **配置( clock@date )** 
```json
// 属性介绍
{
    "timezone":"时区",                // [ -12至12], 即西12到东12区
    "ntpclient":"是否启用NTP客户端",  // [ disable, enable ]
    "ntpserver":"NTP服务器",          // [ string ]
    "ntpserver2":"NTP服务器2",        // [ string ]
    "ntpserver3":"NTP服务器3",        // [ string ]
    "ntpinterval":"同步间隔"          // [ number ], 即与NTP服务器同步时间的间隔(秒为单位)
}
```
示例查询所有配置
```shell
clock@date
{
    "timezone":"8",                   # 时区为东八区
    "ntpclient":"enable",             # 开启NTP对时
    "ntpserver":"ntp1.aliyun.com",    # ntp1.aliyun.com, ntp2.aliyun.com, ntp3.aliyun.com依次尝试直到成功
    "ntpserver2":"ntp2.aliyun.com",
    "ntpserver3":"ntp3.aliyun.com",
    "ntpinterval":"86400"             # 每间隔86400秒同步一次NTP时间
}
```  
示例修改时区为西5区
```shell
clock@date:timezone=-5
ttrue
```  

#### **接口** 

+ `status[]` **显示时间相关的信息**, *成功返回JSON描述时间信息, 失败返回NULL, 出错返回terror*
    ```json
    // 接口返回信息属性介绍
    {
        "timezone":"时区",              // [ -12至12], 从-12至12, 即西12到东12区
        "current":"当前时间",           // 时:分:秒:月:日:年
        "livetime":"运行时长",          // 时:分:秒:天
        "uptime":"运行时长",            // 从启动后累计秒数
        "source":"当前时间来源"         // [ ntp, set, lte, gps ]
                                            // ntp表示时间来源于NTP对时, NTP对时优先级最高, NTP对时成功即覆盖所有其它时间
                                            // set表示被手动设置时间
                                            // rtc表示来源RTC时间 
                                            // lte表示来源LTE基带时间, LTE基带时间最低, 只在时间未设置过才会使用LTE基带时间
                                            // gps表示来源GPS时间 
                                            // 空或无此节点表示未设置过
    }
    ```
    ```shell
    # 示例
    clock@date.status
    {
        "timezone":"8",                   # 时区为东八区
        "source":"ntp",                   # 当前时间使用的是NTP对时的时间
        "current":"01:41:15:01:02:2016",  # 当前时间为2016年1月2号1点41分15秒
        "livetime":"01:41:30:0",          # 运行了1个小时41分30秒
        "uptime":"6090"                   # 累计运行了6090秒
    }
    ```

+ `current[ 时间 ]` **设置当前时间**, 格式为 时:分:秒:月:日:年, *成功返回ttrue, 失败返回tfalse, 出错返回terror*
    ```shell
    # 示例,  把设备时间设置为2019年7月8日的11点12分23秒
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```

+ `ntpsync[ [NTP服务器] ]` **与指定的NTP服务器同步时间**, 参数中不给出NTP服务器将从组件配置的ntpserver属性获取NTP服务器来同步时间, *成功返回ttrue, 失败返回tfalse, 出错返回terror*
    ```shell
    # 示例, 与times.windows.com使用NTP协议同步时间
    clock@date.ntpsync[ times.windows.com ]
    ttrue
    ```

