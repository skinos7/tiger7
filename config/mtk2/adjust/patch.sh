#!/bin/bash

# get the ubuntu release
UB_CODE=`lsb_release -s -r`
UB_REL=${UB_CODE:0:2}
echo "Current Ubuntu ${UB_CODE} Release: ${UB_REL}"


# include the config/smtk2 to build
rm -fr ${gSDK_DIR}/package/${gPLATFORM}
if [ ! -e ${gSDK_DIR}/package/${gPLATFORM} ]; then
    ln -s ${gPLATFORM_DIR}  ${gSDK_DIR}/package/${gPLATFORM}
fi


# feeds modify
cp ${gPLATFORM_DIR}/adjust/patch/feeds.conf  ${gSDK_DIR}/feeds.conf
# modify the Makefile system to ignore duplicate creation of ./tmp errors
cp ${gPLATFORM_DIR}/adjust/patch/makefile/toplevel.mk  ${gSDK_DIR}/include/toplevel.mk
# patch the Makefile system package the gosROOT_DIR to image
cp ${gPLATFORM_DIR}/adjust/patch/makefile/image.mk  ${gSDK_DIR}/include/image.mk
# patch the Makefile system ignore the libraries dependencies failed
cp ${gPLATFORM_DIR}/adjust/patch/makefile/package-ipkg.mk  ${gSDK_DIR}/include/package-ipkg.mk
# patch the Makefile system call the kernel/custom.sh to override the kernel source code before kernel compile
cp ${gPLATFORM_DIR}/adjust/patch/makefile/kernel-defaults.mk  ${gSDK_DIR}/include/kernel-defaults.mk
# patch the Makefile system supoort cmake give a subdir
cp ${gPLATFORM_DIR}/adjust/patch/makefile/cmake.mk  ${gSDK_DIR}/include/cmake.mk
# patch the Makefile system ignore the opkg dependencies failed
cp ${gPLATFORM_DIR}/adjust/patch/makefile/package-Makefile  ${gSDK_DIR}/package/Makefile
# patch the Makefile system to support 16m image for MT7628
cp ${gPLATFORM_DIR}/adjust/patch/makefile/image-Makefile ${gSDK_DIR}/target/linux/ramips/image/Makefile
# path th modules.mk system to support i2c-mtk control driver for menuconfig
cp ${gPLATFORM_DIR}/adjust/patch/makefile/ramips-modules.mk ${gSDK_DIR}/target/linux/ramips/modules.mk


# patch to Build tools system fix pkg-config complie error
if [ ! -d ${gSDK_DIR}/tools/pkg-config/patches ]; then
    mkdir ${gSDK_DIR}/tools/pkg-config/patches
fi
cp ${gPLATFORM_DIR}/adjust/patch/buildtools/pkg-config-001-glib-gdate-suppress-string-format-literal-warning.patch   ${gSDK_DIR}/tools/pkg-config/patches/001-glib-gdate-suppress-string-format-literal-warning.patch
# patch to Build tools system fix automake complie error
cp ${gPLATFORM_DIR}/adjust/patch/buildtools/automake-300-perl-style-modify-to-support.patch  ${gSDK_DIR}/tools/automake/patches/300-perl-style-modify-to-support.patch
# patch to Build tools system fix mkimage complie error
cp ${gPLATFORM_DIR}/adjust/patch/buildtools/mkimage-210-openssl-1.1.x-compat.patch  ${gSDK_DIR}/tools/mkimage/patches/210-openssl-1.1.x-compat.patch
cp ${gPLATFORM_DIR}/adjust/patch/buildtools/mkimage-300-ubuntu18-gcc-header-notfound-error.patch  ${gSDK_DIR}/tools/mkimage/patches/300-ubuntu18-gcc-header-notfound-error.patch
# patch to Build tools system fix gcc complie error
cp ${gPLATFORM_DIR}/adjust/patch/buildtools/gcc-960-gcc-inline-error.patch  ${gSDK_DIR}/toolchain/gcc/patches/4.8-linaro/960-gcc-inline-error.patch
if [ "X${UB_REL}" = "X22" ]  ; then
    echo "Custom Ubuntu ${UB_CODE} Patch"
    # patch to Build tools system fix m4 ubuntu22 complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/m4-ubuntu22-fix-sigstksz.patch ${gSDK_DIR}/tools/m4/patches/210-fix-sigstksz.patch
    # patch to Build tools system fix mklibs ubuntu22 complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/mklibs-ubuntu22.Makefile ${gSDK_DIR}/tools/mklibs/Makefile
    # patch to Build tools system fix cmake
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/cmake-ubuntu-cpp-const-fix.patch ${gSDK_DIR}/tools/cmake/patches/
fi
if [ "X${UB_REL}" = "X20" ] || [ "X${UB_REL}" = "X22" ]  ; then
    echo "Custom Ubuntu ${UB_CODE} Patch"
    # patch to Build tools system fix m4 ubuntu20 complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/m4-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/m4/patches/200-gnulib-compile-fix.patch
    # patch to Build tools system fix mkimage complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/mkimage-301-ubuntu20-gcc-header-notfound-error.patch  ${gSDK_DIR}/tools/mkimage/patches/301-ubuntu20-gcc-header-notfound-error.patch
    # patch to Build tools system fix bison complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/bison-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/bison/patches/200-bison-ubuntu20-compile-fix.patch
    # patch to Build tools system fix findutils complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/findutils-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/findutils/patches/200-findutils-ubuntu20-compile-fix.patch
    # patch to Build tools system fix e2fsprogs complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/e2fsprogs-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/e2fsprogs/patches/200-e2fsprogs-ubuntu20-compile-fix.patch
    # patch to Build tools system fix make-ext4fs complie error
    if [ ! -d ${gSDK_DIR}/tools/make-ext4fs/patches ]; then
        mkdir ${gSDK_DIR}/tools/make-ext4fs/patches
    fi
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/make-ext4fs-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/make-ext4fs/patches/200-make-ext4fs-ubuntu20-compile-fix.patch
    # patch to Build tools system fix mtd-utils complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/mtd-utils-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/mtd-utils/patches/400-mtd-utils-ubuntu20-compile-fix.patch
    # patch to Build tools system fix squashfs4 complie error
    cp ${gPLATFORM_DIR}/adjust/patch/buildtools/squashfs-ubuntu20-compile-fix.patch ${gSDK_DIR}/tools/squashfs4/patches/200-squashfs-ubuntu20-compile-fix.patch
fi


# patch the ath10k support led control
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mac80211-990-backport-include-creds.patch ${gSDK_DIR}/package/kernel/mac80211/patches/990-backport-include-creds.patch 
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mac80211-992-add_led_support.patch ${gSDK_DIR}/package/kernel/mac80211/patches/992-add_led_support.patch
# patch the nfs compile error
cp ${gPLATFORM_DIR}/adjust/patch/kernel/fs.mk  ${gSDK_DIR}/package/kernel/linux/modules/
# try to add the atheros wifi to suuport accelerate
#cp ${gPLATFORM_DIR}/adjust/patch/kernel/mac80211-Makefile  ${gSDK_DIR}/package/kernel/mac80211/Makefile

# patch to kernel mtk wifi dir
cp ${gPLATFORM_DIR}/adjust/patch/kernel/119-wifi-path.patch ${gSDK_DIR}/target/linux/ramips/patches

# patch to mt7603 to read the eeprom from flash
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7603/files/mt7603.dat   ${gSDK_DIR}/package/mtk/drivers/mt7603/files/
# patch to mt7603 for makefile or otheres
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7603/patches/001-driver-path.patch   ${gSDK_DIR}/package/mtk/drivers/mt7603/patches/001-driver-path.patch
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7603/patches/mt7603_no_wps_compile_fix.patch   ${gSDK_DIR}/package/mtk/drivers/mt7603/patches/020-wps_compile_fix.patch
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7603/patches/mt7603_no_wpa3_compile_fix.patch   ${gSDK_DIR}/package/mtk/drivers/mt7603/patches/021-wpa3_compile_fix.patch
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7603/patches/111-utf8-ssid-fix.patch   ${gSDK_DIR}/package/mtk/drivers/mt7603/patches/
# patch to mt76x2 chip to platform
rm -fr ${gSDK_DIR}/package/mtk/drivers/mt76x2/
cp -r ${gPLATFORM_DIR}/adjust/patch/kernel/mt76x2 ${gSDK_DIR}/package/mtk/drivers/mt76x2
mv ${gSDK_DIR}/package/mtk/drivers/mt76x2/package.make ${gSDK_DIR}/package/mtk/drivers/mt76x2/Makefile
# patch to mt7663 to read the eeprom from flash
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7663/files/mt7663.1.dat ${gSDK_DIR}/package/mtk/drivers/mt7663/files/
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7663/files/mt7663.2.dat ${gSDK_DIR}/package/mtk/drivers/mt7663/files/
# patch to mt7663  chip new drivers to platform
cp -r ${gPLATFORM_DIR}/adjust/patch/kernel/mt7663/patches/001-driver-path.patch ${gSDK_DIR}/package/mtk/drivers/mt7663/patches/001-driver-path.patch

# patch to mt7663 ver6.0.3  chip new drivers to platform
rm -fr ${gSDK_DIR}/package/mtk/drivers/mt7663_603/
cp -r ${gPLATFORM_DIR}/adjust/patch/kernel/mt7663_603 ${gSDK_DIR}/package/mtk/drivers/mt7663_603
mv ${gSDK_DIR}/package/mtk/drivers/mt7663_603/package.make ${gSDK_DIR}/package/mtk/drivers/mt7663_603/Makefile
# patch to mt7628 for .dat file
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7628/files/mt7628.dat ${gSDK_DIR}/package/mtk/drivers/mt7628/files/
# patch to mt7628 to compile apcli no error
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7628/patches/mt7628_100_compile_error.patch ${gSDK_DIR}/package/mtk/drivers/mt7628/patches/100_compile_error.patch
cp ${gPLATFORM_DIR}/adjust/patch/kernel/mt7628/patches/mt7628_101_nowsc_fix.patch ${gSDK_DIR}/package/mtk/drivers/mt7628/patches/101_nowsc_fix.patch


# patch to busybox for udhcpc support ifnameid and exit when renewip failed
cp ${gPLATFORM_DIR}/adjust/patch/busybox/busybox-900-udhcpc_renewexit_backupip_ifnameid.patch ${gSDK_DIR}/package/utils/busybox/patches/900-udhcpc_renewexit_backupip_ifnameid.patch
# patch to busybox for no log interference
cp ${gPLATFORM_DIR}/adjust/patch/busybox/busybox-901-nolog_interference.patch ${gSDK_DIR}/package/utils/busybox/patches/901-nolog_interference.patch

# fix the base-file do not need the netifd
cp ${gPLATFORM_DIR}/adjust/patch/package/base-files-Makefile ${gSDK_DIR}/package/base-files/Makefile
# fix the base-file do support ipv6 forwarding
cp ${gPLATFORM_DIR}/adjust/patch/package/sysctl.conf ${gSDK_DIR}/package/base-files/files/etc/
# fix the dropbear cannot run the hetui
cp ${gPLATFORM_DIR}/adjust/patch/package/shells ${gSDK_DIR}/package/base-files/files/etc/
# remove the down the wifi at shutdown, this patch causes the reboot instruction to malfunction
rm -fr ${gSDK_DIR}/package/utils/busybox/patches/311-reboot-down-wifi.patch
# patch to mtd for no return error when complie
cp ${gPLATFORM_DIR}/adjust/patch/package/mtd.c ${gSDK_DIR}/package/system/mtd/src/
# patch to ated for down the lte and change interface from br0 to lan
cp ${gPLATFORM_DIR}/adjust/patch/package/ate.c ${gSDK_DIR}/package/mtk/applications/ated/src/
# patch to ated for down the lte and change interface from br-lan to lan
cp ${gPLATFORM_DIR}/adjust/patch/package/ate-ext.c ${gSDK_DIR}/package/mtk/applications/ated/src/
# patch to ated for down the lte and change interface from br-lan to lan
cp ${gPLATFORM_DIR}/adjust/patch/package/ated_ext-9ccd2fb9.tar.bz2 ${gSDK_DIR}/dl/
# patch to ipset for compile error
cp ${gPLATFORM_DIR}/adjust/patch/package/ipset-makefile ${gSDK_DIR}/package/network/utils/ipset/Makefile
if [ ! -d ${gSDK_DIR}/package/network/utils/ipset/patches ]; then
    mkdir ${gSDK_DIR}/package/network/utils/ipset/patches
fi
cp ${gPLATFORM_DIR}/adjust/patch/package/ipset-010-new_api_compile_error.patch ${gSDK_DIR}/package/network/utils/ipset/patches/010-new_api_compile_error.patch

# patch to switch to support or/and operation
if [ ! -d ${gSDK_DIR}/package/mtk/applications/switch/patches ]; then
    mkdir ${gSDK_DIR}/package/mtk/applications/switch/patches
fi
cp ${gPLATFORM_DIR}/adjust/patch/package/switch-100-support-and-or.patch ${gSDK_DIR}/package/mtk/applications/switch/patches/100-support-and-or.patch
# add the smp to fast network
cp ${gPLATFORM_DIR}/adjust/patch/package/smp.sh ${gSDK_DIR}/package/mtk/misc/mtk-base-files/files/usr/sbin/

# patch to wathdog to no log print
cp ${gPLATFORM_DIR}/adjust/patch/package/watchdog.c ${gSDK_DIR}/package/mtk/applications/watchdog/src/

# replace this directory from smtk/package/network/services/dnsmasq, beacuse this smtk2 release run crack
rm -fr ${gSDK_DIR}/package/network/services/dnsmasq
cp -r ${gPLATFORM_DIR}/adjust/patch/dnsmasq ${gSDK_DIR}/package/network/services/dnsmasq
mv ${gSDK_DIR}/package/network/services/dnsmasq/package.make ${gSDK_DIR}/package/network/services/dnsmasq/Makefile
# replace this directory from openwrt(2d23fedac5709f94184a401667e45ed0d861ac5e) for wpa_supplicant don't suuport -H
rm -fr ${gSDK_DIR}/package/network/services/hostapd
cp -r ${gPLATFORM_DIR}/adjust/patch/hostapd ${gSDK_DIR}/package/network/services/hostapd
mv ${gSDK_DIR}/package/network/services/hostapd/package.make ${gSDK_DIR}/package/network/services/hostapd/Makefile
# replace this directory from openwrt(2d23fedac5709f94184a401667e45ed0d861ac5e) for update
rm -fr ${gSDK_DIR}/package/network/services/relayd
cp -r ${gPLATFORM_DIR}/adjust/patch/relayd ${gSDK_DIR}/package/network/services/relayd
mv ${gSDK_DIR}/package/network/services/relayd/package.make ${gSDK_DIR}/package/network/services/relayd/Makefile
# replace this directory from posc of smtk, only sync to smtk skinos com
rm -fr ${gSDK_DIR}/package/network/services/proftpd
cp -r ${gPLATFORM_DIR}/adjust/patch/proftpd ${gSDK_DIR}/package/network/services/proftpd
mv ${gSDK_DIR}/package/network/services/proftpd/package.make  ${gSDK_DIR}/package/network/services/proftpd/Makefile
# replace this directory from smtk/package/network/services/miniupnpd, only sync to smtk skinos com
rm -fr ${gSDK_DIR}/package/mtk/applications/miniupnpd
rm -fr ${gSDK_DIR}/package/network/services/miniupnpd
cp -r ${gPLATFORM_DIR}/adjust/patch/miniupnpd ${gSDK_DIR}/package/network/services/miniupnpd
mv ${gSDK_DIR}/package/network/services/miniupnpd/package.make ${gSDK_DIR}/package/network/services/miniupnpd/Makefile
# add the ttyd support from the smtk/package/utils
rm -fr ${gSDK_DIR}/package/utils/ttyd
cp -r ${gPLATFORM_DIR}/adjust/patch/ttyd ${gSDK_DIR}/package/utils/ttyd
mv ${gSDK_DIR}/package/utils/ttyd/package.make ${gSDK_DIR}/package/utils/ttyd/Makefile
# add the libwebsockets support from the smtk/package/utils
rm -fr ${gSDK_DIR}/feeds/packages/libs/libwebsockets/
cp -r ${gPLATFORM_DIR}/adjust/patch/libwebsockets ${gSDK_DIR}/feeds/packages/libs/libwebsockets
mv ${gSDK_DIR}/feeds/packages/libs/libwebsockets/package.make ${gSDK_DIR}/feeds/packages/libs/libwebsockets/Makefile
# delete this directory from smtk/package/network/services/igmpproxy, beacuse this smtk2 release complie error
rm -fr ${gSDK_DIR}/package/mtk/applications/igmpproxy
# fix the gawk don't support namespace val
if [ ! -d ${gSDK_DIR}/feeds/packages/libs/libgpg-error/patches ]; then
    mkdir ${gSDK_DIR}/feeds/packages/libs/libgpg-error/patches
fi
cp -r ${gPLATFORM_DIR}/adjust/patch/package/libgpg-error-namespace-error-fix.patch ${gSDK_DIR}/feeds/packages/libs/libgpg-error/patches

# ddns-script to support skinos
cp -r ${gPLATFORM_DIR}/adjust/patch/ddns/dynamic_dns_functions.sh ${gSDK_DIR}/feeds/packages/net/ddns-scripts/files
cp -r ${gPLATFORM_DIR}/adjust/patch/ddns/dynamic_dns_updater.sh ${gSDK_DIR}/feeds/packages/net/ddns-scripts/files

# net-snmp don't print err
cp -r ${gPLATFORM_DIR}/adjust/patch/package/net-snmpd-5.4.4-fix-err-print.patch ${gSDK_DIR}/feeds/packages/net/net-snmp/patches

# smstools bin reset
cp -r ${gPLATFORM_DIR}/adjust/patch/smstools3/package.make ${gSDK_DIR}/feeds/packages/utils/smstools3/Makefile

# patch to libubox to support ddns-scripts
if [ ! -d ${gSDK_DIR}/package/libs/libubox/patches ]; then
    mkdir ${gSDK_DIR}/package/libs/libubox/patches
fi
cp ${gPLATFORM_DIR}/adjust/patch/libubox/jshn-suuport-loadfile.patch ${gSDK_DIR}/package/libs/libubox/patches/

# update the openvpn
rm -fr ${gSDK_DIR}/package/network/services/openvpn
cp -r ${gPLATFORM_DIR}/adjust/patch/openvpn ${gSDK_DIR}/package/network/services/

# update the dropbear
rm -fr ${gSDK_DIR}/package/network/services/dropbear
cp -r ${gPLATFORM_DIR}/adjust/patch/dropbear ${gSDK_DIR}/package/network/services/

# update the libnl-tiny
cp ${gPLATFORM_DIR}/adjust/patch/libnl-tiny/if_addr.h ${gSDK_DIR}/package/libs/libnl-tiny/src/include/linux/

# update the odhcp6c
rm -fr ${gSDK_DIR}/package/network/ipv6/odhcp6c
cp -r ${gPLATFORM_DIR}/adjust/patch/odhcp6c ${gSDK_DIR}/package/network/ipv6

# update the odhcpd
rm -fr ${gSDK_DIR}/package/network/services/odhcpd
cp -r ${gPLATFORM_DIR}/adjust/patch/odhcpd ${gSDK_DIR}/package/network/services/

