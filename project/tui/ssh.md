
***
##  SSH server component
Manage equipment SSH server

#### Configuration( tui@ssh )

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ]
    "port":"service port",                  // [ number ], 1-65535, default is 22
    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
        // "...":"..." You can configure multiple host who can access
    }
}
// Examples
{
    "status":"enable",             // start this service at system startup
    "port":"22",                   // service port 22
    "manager":                     // only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  



## 管理SSH服务器组件
设备的SSH服务器

#### **配置( tui@ssh )** 

```json
// 属性介绍
{
    "status":"是否开机启动",                          // disable为关闭, enable为启动
    "port":"服务端口",                                // 服务端口, 默认为22
    "manager":                                        // 只允许指定的IP地址或MAC地址访问
    {
        // "...":"..." 可以定义很多个IP地址或MAC地址
        "名称1":"IP地址或MAC地址",
        "名称2":"IP地址或MAC地址"
    }
}

// 示例
{
    "status":"enable",             // 启用管理网页服务器
    "port":"22",                   // 端口为22
    "manager":                     // 只允许IP地址为192.168.8.111或MAC地址为00:03:7F:12:AA:B0的主机访问
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

