***
## Sensor for Voltage Meter
Manage Voltameter

#### Configuration( sensor@voltameter )   
```json
// Attributes introduction
{
    "status":"work state"                              // [ "disable", "enable" ]
}
```

Examples, show all the configure
```shell
sensor@voltameter
{
    "status":"enable"                   # Voltameter work
}
```  

Examples, disable the Voltameter
```shell
sensor@voltameter:status=disable
ttrue
```  



#### **Methods**   
+ `status[]` **get the Voltameter status**, *succeed return talk to describes, failed return NULL, error return terror*    
    ```json
    // Attributes introduction of talk by the method return
    {
        "state":"the battary state",                // [ "noboard", "charge", "uncharge" ]
                                                             // noboard: indicates cannot find the Voltameter
                                                             // charge: indicates charging
                                                             // uncharge: indicates not charged
        "voltage":"current voltage",                // [ number ], unit is V
        "percent":"current battary power"           // [ nubmer ], unit is %
    }    
    ```

    ```shell
    # examples, get the current Voltameter status
    sensor@voltameter.status
    {
        "state":"uncharge",             # current not charged
        "voltage":"11.9",               # current voltage 11.9V
        "percent":"70"                  # current battary 70%
    }  
    ```

