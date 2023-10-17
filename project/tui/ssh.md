
***
##  SSH Server Management
Manage gateway SSH server

#### Configuration( tui@ssh )   

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ “disable”, “enable” ]
    "port":"service port",                  // [ number ], 1-65535, default is 22
    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
        // "...":"..." You can configure multiple host who can access
    }
}
```  

Examples, show all SSH server configure
```shell
tui@ssh
{
    "status":"enable",             # start this service at system startup
    "port":"22",                   # service port 22
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access ssh server
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

Examples, modify the port of SSH server
```shell
tui@ssh:port=2222
ttrue
```  

Examples, disable the SSH server
```shell
tui@ssh:status=disable
ttrue
```  

Examples, set the ip 192.168.8.250 can access the SSH server
```shell
tui@ssh:manager/pc3=192.168.8.250
ttrue
```  

Examples, clear the manager allow all ip can access the SSH server
```shell
tui@ssh:manager=
ttrue
```  
