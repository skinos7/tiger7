***
## Sensor for Noise detector
Manage Noise detector

#### Configuration( sensor@noisedetector )
```json
// Attributes introduction
{
    "status":"work state"                              // [ "disable", "enable" ]
}
```

Examples, show all the configure
```shell
sensor@noisedetector
{
    "status":"enable"                   # Voltameter work
}
```  

Examples, disable the noise detector
```shell
sensor@noisedetector:status=disable
ttrue
```  



#### **Methods**   
+ `status[]` **get the noise detector status**, *succeed return talk to describes, failed return NULL, error return terror*    
    ```json
    // Attributes introduction of talk by the method return
    {
        "noise":"current noise"                // [ number ], unit is DB
    }    
    ```

    ```shell
    # examples, get the current noise detector status
    sensor@noisedetector.status
    {
        "voltage":"71.3"               # current noise 71.3DB
    }  
    ```

