#!/bin/bash

# delete the needless init
lookdir="./etc/init.d"
exception="sysfixtime boot system sysctl done set-irq-affinity land"
for file in `ls ${lookdir}`
do 
    save=0
    for ext in ${exception}
    do 
        if [ ${file} == ${ext} ]
        then 
            save=1
        fi
    done
    if [ $save != 1 ]
    then
        rm -f ${lookdir}/${file}
    fi
done

# delete the needless etc/*
rm -fr ./etc/banner.failsafe
rm -fr ./etc/device_info
rm -fr ./etc/diag.sh
rm -fr ./etc/opkg
rm -fr ./etc/openwrt_release
rm -fr ./etc/openwrt_version
rm -fr ./etc/sysupgrade.conf
rm -fr ./etc/udhcpd.config
rm -fr ./etc/board.d
rm -fr ./etc/capabilities
rm -fr ./etc/config
rm -fr ./etc/ethers
rm -fr ./etc/shinit
rm -fr ./etc/uci-defaults

# delete the needless other
rm -fr ./rom
rm -fr ./www
rm -fr ./overlay

rm -fr ./bin/ipcalc.sh
rm -fr ./bin/board_detect
rm -fr ./bin/config_generate

rm -fr ./sbin/sysupgrade
rm -fr ./sbin/upgraded
rm -fr ./sbin/validate_data
rm -fr ./sbin/led.sh
rm -fr ./sbin/reload_config
rm -fr ./sbin/wifi

rm -fr ./lib/netifd
rm -fr ./lib/upgrade
rm -fr ./lib/network
rm -fr ./lib/wifi

rm -fr ./usr/lib/opkg
rm -fr ./usr/lib/dnsmasq
rm -fr ./usr/sbin/ntpd-hotplug
rm -fr ./usr/share/acl.d
rm -fr ./usr/share/dnsmasq

# delete self
rm -fr ./needless.sh


