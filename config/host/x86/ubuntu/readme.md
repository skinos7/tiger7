---
title: "Readme for Ubuntu to be Server"
author: dimmalex@gmail.com
date: March 22, 2023
output:
    word_document:
        path: D:/tmp/Readme.docx
---


# SkinOS Server SDK download and compile

#### 1. Development environment download   
Under Ubuntu ( 20.04 or 18.04 recommended ) run the following command to download the development environment ( please install git and make first )   
> #### 开发环境下载
> 在Ubuntu下（ 建议使用20.04或18.04 ）执行以下命令下载开发环境( 请先安装git及make )

```shell
git clone https://github.com/skinos7/tiger7.git
```

*Or download it from gitee*
> *或者从gitee上下载*

```shell
git clone https://gitee.com/tiger7/tiger7.git
```

#### 2. Install the necessary development tools   
> #### 安装开发工具

```shell
cd tiger7
make ubuntu_preset
```

#### 3. Specify the product model to be developed   
> #### 指定产品型号

```shell
make pid gBOARDID=<Product complete model>

# For products such as Ubuntu enter the following instructions:
make pid gBOARDID=host-x86-ubuntu
```

#### 4. Download the SDK corresponding to the product model   
> #### 下载产品型号对应的SDK

```shell
make update
```

#### 5. Compile Ubuntu Server   
> #### 编译服务器

```shell
make dep
make
```


#### 5. Install Ubuntu Server   
> #### 安装服务器

```shell
make local
```


#### 5. Run Ubuntu Server   
> #### 运行服务器

```shell
make start
```
