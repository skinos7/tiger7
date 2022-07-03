#!/bin/bash

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

