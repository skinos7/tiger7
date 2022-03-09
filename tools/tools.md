
### Introduction
Some read/install/build operations of FPK are provided during the compilation or development process


### fpk-install
```
fpk-instlal [root filesystem] [install directory] <FPK package[...]> 
```
Install the FPK package
- **root filesystem** is the root directory of the running system
- **install directory** is the installation directory of the header file and library  

### fpk-indexed
```
fpk-indexed <FPK directory> <index file for repo>
```
Make the software repo index file
- **FPK directory** is the directory full of FPK file
- **index file for repo** will be create

### prj-read
```
prj-read <fpk information key> [fpk pathname]
```
To read the contents of the project configuration file, 
- **FPK information Key** nodes in the configuration file
- **FPK pathname** is the path of the configuration file  

