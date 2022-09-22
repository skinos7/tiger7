
***
## Management of web page component
Administration of equipment Management web page

#### Configuration( wui@admin )

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ]
    "login":"authentication is required",   // [ enable, disable ], WARNING: disable will not require login
    "port":"service port",                  // [ number ], 1-65535, default is 80
    "sslport":"https port",                 // [ number ], 1-65535, default is 443
    "termport":"Terminal port",             // [ number ], 1-65535, default is 81
    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        // "...":"..." You can configure multiple host who can access
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    },

    // custom the webpage frame
    "css_file":"CSS file path",                   // Fill in the file name must be in located on /prj/wui/admin/assets/css
    "logo_color":"Title bar of logo color",       // title color
    "logo_title":"Text in the middle of a web page",
    "logo_file":"LOGO file path",                 // Fill in the file name must be in located on /prj/wui/admin/assets/css 
    "logo_width":"LOGO width",
    "logo_height":"LOGO height",
    "logo_align":"center",                        // [ center, right ]
    "logo_model":"show or not",                   // [ enable, disable ]
    "model_bar":"show or not",                    // [ enable, disable ]

    // custom the webpage show
    "firmware_id":"show or not",                  // [ enable, disable ]
    "upgrade_online":"show or not",               // [ enable, disable ]
    "repo_online":"show or not",                  // [ enable, disable ]

    // custom the web menu
    "menu":
    {
        "wan":"show or not",                      // [ enable, disable  ]
        "wan2":"show or not",                     // [ enable, disable  ]
        "wisp":"show or not",                     // [ enable, disable  ]
        "wisp2":"show or not",                    // [ enable, disable  ]
        "lte":"show or not",                      // [ enable, disable  ]
        "lte2":"show or not",                     // [ enable, disable  ]
        "lan":"show or not",                      // [ enable, disable  ]
        "sta":"show or not",                      // [ enable, disable  ]
        "connection":"show or not",               // [ enable, disable  ]
        "opmode":"show or not",                   // [ enable, disable  ]
        "language":"show or not",                 // [ enable, disable  ]

        "terminal":"show or not",                  // [ enable, disable  ]
        "development":"show or not",               // [ enable, disable  ]
    }
    
}
```
***The modification takes effect only after a restart***

Examples, show all the configure
```shell
wui@admin
{
    "status":"enable",             # start this service at system startup
    "login":"disable",             # you can access to webpage with no login
    "port":"80",                   # service port 80
    "sslport":"443",               # https port 443
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
Examples, modify the port of web page server
```shell
wui@admin:port=2222
true
```  
Examples, disable the web page server
```shell
wui@admin:status=disable
true
```  




## 管理网页服务器组件
设备的管理网页服务器

#### **配置( wui@admin )** 

```json
// 属性介绍
{
    "status":"是否开机启动",                          // disable为关闭, enable为启动
    "login":"是否无认证即可访问",                     // disable或其它表示需要登录, enable表示不需要登陆即可直接进入管理界面
    "port":"服务端口",                                // 服务端口, 默认为80
    "termport":"终端端口",                            // 终端端口, 默认为81    
    "manager":                                        // 只允许指定的IP地址或MAC地址访问
    {
        // "...":"..." 可以定义很多个IP地址或MAC地址
        "名称1":"IP地址或MAC地址",
        "名称2":"IP地址或MAC地址"
    },
    "css_file":"CSS文件",                             // 空表示使用默认的, 填入的文件名必须位于/prj/wui/admin/assets/css中
    "logo_title":"网页中间文字",
    "logo_file":"LOGO文件",                           // 空表示无LOGO, 填入的文件名必须位于/prj/wui/admin/assets/css中 
    "logo_width":"LOGO宽度",
    "logo_height":"LOGO高度",
    "logo_align":"center"                             // center表示居中, right表示靠左
}
```  
***修改此组件配置需要重启后生效***

示例, 显示所有的配置
```shell
wui@admin
{
    "status":"enable",             # 启用管理网页服务器
    "login":"disable",             # 不需要认证即可访问
    "port":"80",                   # 端口为80
    "manager":                     # 只允许IP地址为192.168.8.111或MAC地址为00:03:7F:12:AA:B0的主机访问
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
示例, 修改管理网页服务器的端口为8080
```shell
wui@admin:port=8080
true
```  
示例, 禁用管理网页服务器
```shell
wui@admin:status=disable
true
```  





