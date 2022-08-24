#!/bin/bash

# delete the needless init
lookdir="./etc/init.d"
exception="sysfixtime boot system sysctl done hwnat umount set-irq-affinity land"
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

# link the mnt to /tmp
rm -fr ./mnt
ln -s /tmp/mnt/ ./mnt

# delete the needless etc/*
rm -fr ./etc/banner.failsafe
rm -fr ./etc/banner.mtk
rm -fr ./etc/device_info
#rm -fr ./etc/diag.sh
rm -fr ./etc/kernel.config
rm -fr ./etc/openwrt.config
rm -fr ./etc/opkg
rm -fr ./etc/openwrt_release
rm -fr ./etc/openwrt_version
rm -fr ./etc/sdk.version
#rm -fr ./etc/protocols
#rm -fr ./etc/services
rm -fr ./etc/sysupgrade.conf
rm -fr ./etc/udhcpd.config
rm -fr ./etc/udhcpc*
rm -fr ./etc/board.d
rm -fr ./etc/capabilities
rm -fr ./etc/config
rm -fr ./etc/ethers
rm -fr ./etc/shinit
rm -fr ./etc/uci-defaults

# delete the needless preinit
rm -fr ./lib/preinit/07_set_preinit_iface_ramips
rm -fr ./lib/preinit/80_mount_root
rm -fr ./lib/preinit/30_failsafe_wait


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

rm -fr ./usr/sbin/hwnat-disable.sh
rm -fr ./usr/sbin/hwnat-enable.sh
rm -fr ./usr/sbin/mac
rm -fr ./usr/sbin/setsmp.sh
rm -fr ./prj/pdriver/fibocom-log
rm -fr ./prj/pdriver/longsung_gobinet.ko
rm -fr ./prj/pdriver/quectel-log
rm -fr ./prj/pdriver/*.config

# delete self
rm -fr ./needless.sh


