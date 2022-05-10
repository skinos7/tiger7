#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    # mkdir basic directory
    VROOT=/var/skin
    TROOT=/tmp/skin
    mkdir -p $TROOT
    mkdir -p $VROOT
    mkdir -p $VROOT/.reg
    mkdir -p $VROOT/.ser
    mkdir -p $VROOT/.com
    mkdir -p $VROOT/.cfg
    mkdir -p $VROOT/mnt
    mkdir -p $VROOT/mnt/int
    mkdir -p $VROOT/mnt/int/prj
    mkdir -p $VROOT/mnt/int/def
    # load the basic ko
    if [ -e /prj/pdriver/crackid.ko ]; then
        insmod /prj/pdriver/crackid.ko
    fi
    # make the register value default
    he land@register.set_int[land,config_ready,1]
    he land@register.set_string[land,platform,$gPLATFORM]
    he land@register.set_string[land,hardware,$gHARDWARE]
    he land@register.set_string[land,custom,$gCUSTOM]
    he land@register.set_string[land,scope,$gSCOPE]
    he land@register.set_string[land,version,$gVERSION]
    he land@register.set_string[land,local_ifname,ifname@lan]
    he land@register.set_string[land,local_netdev,enp89s0]
    # default the configure if order
    if [ $VROOT/.cfg/.erasev6 ]; then
        rm -fr $VROOT/.cfg/*
        rm -fr $VROOT/.cfg/.erase*
    fi
    if [ $VROOT/mnt/int/.erasev6 ]; then
        rm -fr $VROOT/mnt/int/*
        rm -fr $VROOT/mnt/int/.erase*
    fi
    creturn ttrue
}

shut()
{
    sync
    creturn ttrue
}

custom()
{
    NAME=`hostname`
    he arch@device:name=$NAME
    creturn ttrue
}


# call the method, you cannot delete this
cend

