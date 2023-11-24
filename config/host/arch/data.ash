#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    # mkdir basic directory
    VROOT=/var/skin
    TROOT=/tmp/skin
    sudo mkdir -p $TROOT
    sudo mkdir -p $VROOT
    sudo chmod a+rwx $TROOT
    sudo chmod a+rwx $VROOT
    mkdir -p $VROOT/.reg
    mkdir -p $VROOT/.ser
    mkdir -p $VROOT/.com
    mkdir -p $VROOT/.conf
    mkdir -p $VROOT/mnt
    mkdir -p $VROOT/mnt/config
    mkdir -p $VROOT/mnt/internal
    # load the basic ko
    if [ -e /usr/prj/pdriver/crackid.ko ]; then
        insmod /usr/prj/pdriver/crackid.ko
    fi
    # make the register value default
    RAND=`date +%s`
    he land@register.set_int[land,rand,$RAND]
    he land@register.set_int[land,config_ready,1]
    he land@register.set_string[land,platform,$gPLATFORM]
    he land@register.set_string[land,hardware,$gHARDWARE]
    he land@register.set_string[land,custom,$gCUSTOM]
    he land@register.set_string[land,scope,$gSCOPE]
    he land@register.set_string[land,version,$gVERSION]
    he land@register.set_string[land,local_ifname,ifname@lan]
    NETDEV=`he arch@data:local_netdev`
    he land@register.set_string[land,local_netdev,$NETDEV]
    MODEL=`he arch@data:model`
    he land@register.set_string[land,model,$MODEL]
    MAC=`he arch@data:mac`
    he land@register.set_string[land,mac,$MAC]
    # default the configure if order
    if [ -e $VROOT/mnt/config/.customv6 ]; then
    	echo "mount the configure"
    else
    	echo "clear the configure"
        rm -fr $VROOT/mnt/config/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $VROOT/mnt/config/.customv6
    fi
    if [ -e $VROOT/mnt/internal/.customv6 ]; then
    	echo "mount the interval"
	else
    	echo "clear the interval"
		rm -fr $VROOT/mnt/internal/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $VROOT/mnt/internal/.customv6
    fi

    NAME=`hostname`
    he land@machine:name=$NAME
    
    creturn ttrue
}

shut()
{
    sync
    creturn ttrue
}

# call the method, you cannot delete this
cend

