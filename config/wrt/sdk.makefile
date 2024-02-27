#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.



OPENWRT_SDK_NAME:=openwrt-v22.03.3-221fbfa2d854ccb6cd003c065ec308fbc0651b11.tar.xz
OPENWRT_DL_NAME:=openwrt-v22.03.3-221fbfa2d854ccb6cd003c065ec308fbc0651b11-dl.tar.xz
OPENWRT_FEED_NAME:=openwrt-v22.03.3-221fbfa2d854ccb6cd003c065ec308fbc0651b11-feeds.tar.xz
# Ubuntu编译环境安装
update:
	# 下载或更新底层SDK
	if [ ! -d ${gSDK_DIR} ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_SDK_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update wrt ${gHARDWARE} ${gCUSTOM} ${OPENWRT_SDK_NAME}; \
		fi; \
		cd ${gTOP_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_SDK_NAME}; mv openwrt ${gPLATFORM};\
	fi
	# 更新dl目录, 避免每次一个一个下载
	if [ ! -d ${gSDK_DIR}/dl ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_DL_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update wrt ${gHARDWARE} ${gCUSTOM} ${OPENWRT_DL_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_DL_NAME}; \
	fi
	# 更新并安装所有的菜单项
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update wrt ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME}; \
	fi
	# 更新fpk
	if [ -e ${gCUSTOM_DIR} ]; then \
		cd ${gCUSTOM_DIR}; rm -fr *.fpk*; \
		cd ${gPLATFORM_DIR}; sdk-update wrt ${gHARDWARE} ${gCUSTOM} fpk; \
	fi
adjust:
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch.sh; \
	fi
menu:
	# 更新并安装所有的菜单项
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update wrt ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME}; \
	fi
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch.sh; \
	fi
	#cd ${gSDK_DIR};./scripts/feeds update -a
	cd ${gSDK_DIR};./scripts/feeds update project
	cd ${gSDK_DIR};./scripts/feeds update rice
	cd ${gSDK_DIR};./scripts/feeds install -a
menuconfig: kernel_dep
	# 显示菜单供用户配置
	cd ${gSDK_DIR};make menuconfig
	if [ -f ${gSDK_DIR}/.config ]; then \
		cp ${gSDK_DIR}/.config ${gSDK_CFGFILE}; \
	fi
menuclean:
	# 清除所有菜单安装
	cd ${gSDK_DIR};./scripts/feeds uninstall -a
	cd ${gSDK_DIR};./scripts/feeds clean -a
bufclean:
	# distclean整个SDK
	cd ${gSDK_DIR};make V=s distclean
	rm -fr ${gSDK_DIR}/dl 
sdkclean: bufclean
	rm -fr ${gPLATFORM_DIR}/openwrt.*
	rm -fr ${gPLATFORM_DIR}/dl.*
	rm -fr ${gPLATFORM_DIR}/feeds.*
.PHONY: update adjust menu menuconfig menuclean bufclean sdkclean



sz:
	# 通过xmodem协议发送固件到本地
	cd ${gBUILD_DIR} && sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.zz
	#cd ${gBUILD_DIR} && sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.zz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.upgrade ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.txt
zzb:
	cd ${gBUILD_DIR}; \
	if [ -e ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot ] && [ -e ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ]; then \
		rm -fr ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb; \
		if [ -e ${gOEM_DIR} ]; then \
			firmware-encode ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zzb ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot ${gOEM_SH} ${gOEM_CONFIG}; \
			sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gZZID}.zzb; \
		else \
			firmware-encode ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot; \
			sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb; \
		fi; \
	fi
tar:
	cd ${gBUILD_DIR}; \
	if [ -e ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.bb ] && [ -e ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ]; then \
		rm -fr ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
		tar jcvf ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*; \
		sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
	fi
ftp:
	# 上传到FTP目录
	if [ -e ${gOEM_DIR} ]; then \
		firmware-upload wrt ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz; \
	else \
		firmware-upload wrt ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz; \
		if [ -f ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ]; then\
			firmware-upload wrt ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt; \
		fi \
	fi
repo:
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then\
		firmware-upload wrt ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: sz tar ftp repo



boot_menuconfig: boot_dep
	if [ -d ${gpBOOT_DIR} ]; then \
		make -C ${gpBOOT_DIR} menuconfig V=s; \
		if [ -f ${gpBOOT_DIR}/.config ]; then \
			cp ${gpBOOT_DIR}/.config ${gpBOOT_CFGFILE}; \
		fi \
	fi
boot_dep:
	if [ -d ${gpBOOT_DIR} ]; then \
		if [ -f ${gpBOOT_CFGFILE} ]; then \
			if [ -f ${gpBOOT_DIR}/.config ]; then \
				tmpdiff=`diff ${gpBOOT_CFGFILE} ${gpBOOT_DIR}/.config`; \
				if [ "X$${tmpdiff}" != "X" ]; then \
					cp ${gpBOOT_CFGFILE} ${gpBOOT_DIR}/.config; \
				fi \
			else \
				cp ${gpBOOT_CFGFILE} ${gpBOOT_DIR}/.config; \
			fi \
		fi; \
		make -C ${gpBOOT_DIR} dep; \
	fi
boot: boot_dep
	if [ -d ${gpBOOT_DIR} ]; then \
		unset STRIPTOOL OBJCOPY CONFIG_CROSS_COMPILER_PATH STRIP CONFIGURE_HOST CROSS_COMPILE AR AS RIPTOOL CROSS CXX CC RANLIB LD;echo "E Y" | make -C ${gpBOOT_DIR} menuconfig;make -C ${gpBOOT_DIR}; \
	fi
	cd ${gBUILD_DIR} && firmware-encode ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.bb ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot
boot_install:
	if [ -d ${gpBOOT_DIR} ]; then \
		cd ${gBUILD_DIR} && sz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.bb; \
	fi
boot_clean:
	if [ -d ${gpBOOT_DIR} ]; then \
		make -C ${gpBOOT_DIR} clean; \
	fi
boot_distclean: boot_clean
	if [ -d ${gpBOOT_DIR} ]; then \
		make -C ${gpBOOT_DIR} distclean; \
	fi
.PHONY: boot_menuconfig boot_dep boot boot_install boot_clean boot_distclean

kernel_menuconfig: kernel_dep
	cd ${gSDK_DIR};make V=s kernel_menuconfig
	if [ -f ${gpKERNEL_CFGFILE} ]; then \
		if [ -f ${gpKERNEL_SDK_CFGFILE} ]; then \
			cp ${gpKERNEL_SDK_CFGFILE} ${gpKERNEL_CFGFILE}; \
		fi \
	fi
kernel_dep:
	if [ -f ${gSDK_CFGFILE} ]; then \
		if [ -f ${gSDK_DIR}/.config ]; then \
			tmpdiff=`diff ${gSDK_CFGFILE} ${gSDK_DIR}/.config`; \
			if [ "X$${tmpdiff}" != "X" ]; then \
				cp ${gSDK_CFGFILE} ${gSDK_DIR}/.config; \
			fi \
		else \
			cp ${gSDK_CFGFILE} ${gSDK_DIR}/.config; \
		fi \
	fi
	if [ -f ${gpKERNEL_CFGFILE} ]; then \
		if [ -f ${gpKERNEL_SDK_CFGFILE} ]; then \
			tmpdiff=`diff ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}`; \
			if [ "X$${tmpdiff}" != "X" ]; then \
				cp ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}; \
			fi \
		else \
			cp ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}; \
		fi \
	fi
kernel: kernel_dep
	if [ "X${COMPILE_PROJECT}" != "X" ]; then \
		cd ${gSDK_DIR};make V=s ${COMPILE_PROJECT}; \
	else \
		cd ${gSDK_DIR};make V=s; \
	fi
	cp ${gpUPGRADE_IMAGE} ${gBUILD_DIR}/${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade
	cd ${gBUILD_DIR}; \
	if [ -e ${gOEM_DIR} ]; then \
		firmware-encode ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gOEM_SH} ${gOEM_CONFIG}; \
	else \
		firmware-encode ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade; \
	fi
kernel_install:
	cd ${gBUILD_DIR} && fpk-indexed ${gSTORE_DIR} ${gSTORE_DIR}/${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.store;
	cd ${gBUILD_DIR} && change-log ${gVERSION} ${gCUSTOM} ${gSCOPE} ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gPLATFORM}_${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ${gTOP_DIR}/changelog.json ${gPLATFORM_DIR}/changelog.json ${gHARDWARE_DIR}/changelog.json ${gCUSTOM_DIR}/changelog.json ${gSCOPE_DIR}/changelog.json;
kernel_clean:
	cd ${gSDK_DIR};make V=s clean
kernel_distclean: kernel_clean
.PHONY: kernel_menuconfig kernel_dep kernel kernel_install kernel_clean kernel_distclean

app_menuconfig:
app_dep:
app: app_dep
app_install:
app_clean:
app_distclean: app_clean
.PHONY: app_menuconfig app_dep app app_install app_clean app_distclean



