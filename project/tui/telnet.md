
***
## Telnet server component
Manage telnet server

#### Configuration( tui@telnet )

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ]
    "port":"service port",                  // [ number ], 1-65535, default is 23
    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
        // "...":"..." You can configure multiple host who can access
    }
}
```
Examples, show all telnet server configure
```shell
tui@telnet
{
    "status":"enable",             # start this service at system startup
    "port":"23",                   # service port 23
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
Examples, modify the port of telnet server
```shell
tui@telnet:port=2323
ttrue
```  
Examples, disable the telnet server
```shell
tui@telnet:status=disable
ttrue
```  


## 管理telnet服务器组件
设备的telnet服务器

#### **配置( tui@telnet )** 

```json
// 属性介绍
{
    "status":"是否开机启动",                          // [ disable, enable ], disable为关闭, enable为启动
    "port":"服务端口",                                // [ 数字 ] 服务端口, 默认为23
    "manager":                                       // 只允许指定的IP地址或MAC地址访问
    {
        // "...":"..." 可以定义很多个IP地址或MAC地址
        "名称1":"IP地址或MAC地址",
        "名称2":"IP地址或MAC地址"
    }
}
```
示例, 显示所有telnet服务器配置
```shell
tui@telent
{
    "status":"enable",             # 启用
    "port":"23",                   # 端口为23
    "manager":                     # 只允许IP地址为192.168.8.111或MAC地址为00:03:7F:12:AA:B0的主机访问
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
示例, 修改telnet服务器的端口为2323
```shell
tui@telnet:port=2323
ttrue
```  
示例, 禁用telnet服务器
```shell
tui@telnet:status=disable
ttrue
```  
