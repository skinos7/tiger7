#!/bin/bash


# link the arch directory to openwrt package
rm -fr ${gSDK_DIR}/package/${gPLATFORM}
if [ ! -e ${gSDK_DIR}/package/${gPLATFORM} ]; then
    ln -s ${gPLATFORM_DIR}  ${gSDK_DIR}/package/${gPLATFORM}
fi


# patch the feeds.conf
cp ${gPLATFORM_DIR}/adjust/feeds.conf  ${gSDK_DIR}/

# patch the Makefile system package the gosROOT_DIR to image
cp ${gPLATFORM_DIR}/adjust/makefile/image.mk  ${gSDK_DIR}/include/image.mk
# patch the Makefile system ignore the libraries dependencies failed
cp ${gPLATFORM_DIR}/adjust/makefile/package-ipkg.mk  ${gSDK_DIR}/include/package-ipkg.mk
# patch the Makefile system call the kernel/custom.sh to override the kernel source code before kernel compile
cp ${gPLATFORM_DIR}/adjust/makefile/kernel-defaults.mk  ${gSDK_DIR}/include/kernel-defaults.mk
# patch the Makefile system supoort cmake give a subdir
cp ${gPLATFORM_DIR}/adjust/makefile/cmake.mk  ${gSDK_DIR}/include/cmake.mk
# patch the Makefile system ignore the opkg dependencies failed
cp ${gPLATFORM_DIR}/adjust/makefile/rootfs.mk  ${gSDK_DIR}/include/rootfs.mk
# patch the Makefile system exit when image to big
cp ${gPLATFORM_DIR}/adjust/makefile/image-commands.mk  ${gSDK_DIR}/include/image-commands.mk


# patch the Kernel to support ashyelf dst
cp ${gPLATFORM_DIR}/adjust/dts/*.dtsi  ${gSDK_DIR}/target/linux/ramips/dts/
cp ${gPLATFORM_DIR}/adjust/dts/*.dts  ${gSDK_DIR}/target/linux/ramips/dts/
# patch the Kernel to support 7628 16m general product
cp ${gPLATFORM_DIR}/adjust/dts/mt76x8.mk  ${gSDK_DIR}/target/linux/ramips/image/mt76x8.mk
# patch the Kernel to support 7628 16m general product
cp ${gPLATFORM_DIR}/adjust/dts/mt7621.mk  ${gSDK_DIR}/target/linux/ramips/image/mt7621.mk


# patch the base-files to don't need the netifd
cp ${gPLATFORM_DIR}/adjust/package/base-files-makefile  ${gSDK_DIR}/package/base-files/Makefile
# patch the base-files to don't need warning no password
cp ${gPLATFORM_DIR}/adjust/package/base-files-profile  ${gSDK_DIR}/package/base-files/files/etc/profile
# patch the miniupnpd to support custom the verdor
cp ${gPLATFORM_DIR}/adjust/miniupnpd/package-makefile  ${gSDK_DIR}/package/feeds/packages/miniupnpd/Makefile
# patch the proftpd to support skinos
cp -r ${gPLATFORM_DIR}/adjust/proftpd ${gSDK_DIR}/package/network/services/
mv ${gSDK_DIR}/package/network/services/proftpd/package.make  ${gSDK_DIR}/package/network/services/proftpd/Makefile


# patch the busybox udhcp client support backupip and ifnameid down to script and renewip failed to exit
cp ${gPLATFORM_DIR}/adjust/package/900-udhcpc_renewexit_backupip_ifnameid.patch  ${gSDK_DIR}/package/utils/busybox/patches/
# patch the busybox syslogd remote sync the local
cp ${gPLATFORM_DIR}/adjust/package/busybox-syslogd.patch  ${gSDK_DIR}/package/utils/busybox/patches/
# patch the busybox to supoort mount nfs( the need tirpc support )
cp ${gPLATFORM_DIR}/adjust/package/busybox-makefile  ${gSDK_DIR}/package/utils/busybox/Makefile


# patch the smstools block forever
cp ${gPLATFORM_DIR}/adjust/package/smstools3-fix-block.patch  ${gSDK_DIR}/package/feeds/packages/smstools3/patches/010-fix-block.patch

# delete the io
rm -fr ${gSDK_DIR}/feeds/packages/utils/io

