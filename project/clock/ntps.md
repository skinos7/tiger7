***
## NTP Server management
The management NTP Server

#### Configuration( clock@ntps )
```json
// attribute introduction
{
    "status":"NTP server state"       // [ "disable", "enable"]
}
```
Example, show the configure
```shell
clock@ntps
{
    "status":"enable"           # NTP server enable
}
```
Example, disable the NTP server
```shell
clock@ntps:status=disable
ttrue
```

## NTP服务器管理
管理NTP服务器

#### **配置( clock@ntps )** 
```json
// 属性介绍
{
    "status":"是否启用NTP服务器"           // [ "disable", "enable" ]
}
```
示例， 显示配置
```shell
clock@ntps
{
    "mode":"enable"             # NTP服务器启用
}
```
示例， 禁用NTP服务器
```shell
clock@ntps:status=disable
ttrue
```

