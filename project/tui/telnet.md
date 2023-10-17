
***
## Telnet Server Management
Manage gateway telnet server

#### Configuration( tui@telnet )  

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ “disable”, “enable” ]
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

Examples, set the ip 192.168.8.250 can access the telnet server
```shell
tui@telnet:manager/pc3=192.168.8.250
ttrue
```  

Examples, clear the manager allow all ip can access the telnet server
```shell
tui@telnet:manager=
ttrue
```  

