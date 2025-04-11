#!/bin/bash

cons="  0 0 662"
disk="  0 0 660"
null="  0 0 666"
urandom="  root root 644"
mkdev () 
{    # usage: mkdev name [bcu] major minor owner group mode
    if [ "$opt_v" ]
    then    
        if [ "$opt_d" ]
        then    
            echo "sudo rm -f $1"
        else
            echo "$1        = $2 $3 $4 $5:$6 $7"
        fi
    fi
    [ ! "$opt_n" ] && sudo rm -f $1 &&
    [ ! "$opt_d" ] && sudo mknod $1 $2 $3 $4 &&
    sudo chown $5:$6 $1 &&
    sudo chmod $7 $1
}

# Miscellaneous steps to prepare the root filesystem

ROOTDIR=${1}
if [ "X${1}" == "X" ];
then
    exit -1;
fi
if [ -d ${ROOTDIR} ];
then
    cd ${ROOTDIR}
else
    mkdir ${ROOTDIR}
    cd ${ROOTDIR}
fi
mkdir -m 0755 -p bin
mkdir -m 0755 -p sbin
mkdir -m 0755 -p lib
mkdir -m 0755 -p prj
mkdir -m 0755 -p usr
mkdir -m 0775 -p tmp
mkdir -m 0755 -p dev
mkdir -m 0755 -p proc
mkdir -m 0755 -p sys
mkdir -m 0755 -p etc


