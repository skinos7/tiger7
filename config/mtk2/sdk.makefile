#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.

boot: boot_dep
	if [ -d ${gpBOOT_DIR} ]; then \
		unset STRIPTOOL OBJCOPY CONFIG_CROSS_COMPILER_PATH STRIP CONFIGURE_HOST CROSS_COMPILE AR AS RIPTOOL CROSS CXX CC RANLIB LD;echo "E Y" | make -C ${gpBOOT_DIR} menuconfig;make -C ${gpBOOT_DIR}; \
	fi
	cd ${gBUILD_DIR} && firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.bb ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot
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
boot_install:
	if [ -d ${gpBOOT_DIR} ]; then \
		cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.bb; \
	fi
boot_clean:
	if [ -d ${gpBOOT_DIR} ]; then \
		make -C ${gpBOOT_DIR} clean; \
	fi
boot_distclean: boot_clean
	if [ -d ${gpBOOT_DIR} ]; then \
		make -C ${gpBOOT_DIR} distclean; \
	fi
.PHONY: boot boot_dep boot_menuconfig boot_install boot_clean boot_distclean

kernel: kernel_dep
	if [ "X${COMPILE_PROJECT}" != "X" ]; then \
		cd ${gSDK_DIR};make V=s ${COMPILE_PROJECT}; \
	else \
		cd ${gSDK_DIR};make V=s; \
	fi
	cp ${gpUPGRADE_IMAGE} ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade
	cd ${gBUILD_DIR}; \
	if [ -e ${gOEM_DIR} ]; then \
		firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gOEM_SH} ${gOEM_CONFIG}; \
	else \
		firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade; \
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
kernel_menuconfig: kernel_dep
	cd ${gSDK_DIR};make V=s kernel_menuconfig
	if [ -f ${gpKERNEL_CFGFILE} ]; then \
		if [ -f ${gpKERNEL_SDK_CFGFILE} ]; then \
			cp ${gpKERNEL_SDK_CFGFILE} ${gpKERNEL_CFGFILE}; \
		fi \
	fi
kernel_install:
	cd ${gBUILD_DIR} && fpk-indexed ${gSTORE_DIR} ${gSTORE_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.store;
	cd ${gBUILD_DIR} && change-log ${gVERSION} ${gCUSTOM} ${gSCOPE} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ${gTOP_DIR}/changelog.json ${gPLATFORM_DIR}/changelog.json ${gHARDWARE_DIR}/changelog.json ${gCUSTOM_DIR}/changelog.json ${gSCOPE_DIR}/changelog.json
kernel_clean:
	cd ${gSDK_DIR};make V=s clean
kernel_distclean: kernel_clean
.PHONY: kernel kernel_dep kernel_menuconfig kernel_install kernel_clean kernel_distclean

app: app_dep
app_dep:
app_menuconfig: app_dep
app_install:
app_clean:
app_distclean: app_clean
.PHONY: app app_dep app_menuconfig app_install app_clean app_distclean



OPENWRT_DL_NAME:=dl.tar.xz
OPENWRT_FEED_NAME:=feeds.tar.gz
OPENWRT_SDK_NAME:=openwrt.tar.xz
sdk_update:
	# 下载或更新底层SDK
	if [ ! -d ${gSDK_DIR} ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_SDK_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update mtk2 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_SDK_NAME}; \
		fi; \
		cd ${gTOP_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_SDK_NAME}; mv openwrt ${gPLATFORM};\
	fi
	# 更新dl目录, 避免每次一个一个下载
	if [ ! -d ${gSDK_DIR}/dl ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_DL_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update mtk2 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_DL_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/${OPENWRT_DL_NAME}; \
	fi
	# 更新并安装所有的菜单项
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update mtk2 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar zxvf ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME}; \
	fi
	# 更新fpk
	if [ -e ${gPLATFORM_DIR} ]; then \
		cd ${gPLATFORM_DIR}; rm -fr *.fpk*; \
		cd ${gPLATFORM_DIR}; sdk-update mtk2 ${gHARDWARE} ${gCUSTOM} fpk; \
	fi
sdk_adjust:
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch.sh; \
	fi
sdk_menu:
	# 更新并安装所有的菜单项
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		if [ ! -e ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME} ]; then \
			cd ${gPLATFORM_DIR}; sdk-update mtk2 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME}; \
		fi; \
		cd ${gSDK_DIR}; tar zxvf ${gPLATFORM_DIR}/${OPENWRT_FEED_NAME}; \
	fi
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch.sh; \
	fi
	#cd ${gSDK_DIR};./scripts/feeds update -a
	cd ${gSDK_DIR};./scripts/feeds update project
	cd ${gSDK_DIR};./scripts/feeds update rice
	cd ${gSDK_DIR};./scripts/feeds install -a
sdk_menuconfig: kernel_dep
	# 显示菜单供用户配置
	cd ${gSDK_DIR};make menuconfig
	if [ -f ${gSDK_DIR}/.config ]; then \
		cp ${gSDK_DIR}/.config ${gSDK_CFGFILE}; \
	fi
sdk_clean:
	# 清除所有菜单安装
	cd ${gSDK_DIR};./scripts/feeds uninstall -a
	cd ${gSDK_DIR};./scripts/feeds clean -a
	# distclean整个SDK
	cd ${gSDK_DIR};make V=s distclean
	rm -fr ${gSDK_DIR}/dl 
sdk_distclean: sdk_clean
	# 清除所有download
	rm -fr ${gPLATFORM_DIR}/openwrt.*
	rm -fr ${gPLATFORM_DIR}/dl.*
	rm -fr ${gPLATFORM_DIR}/feeds.*
.PHONY: sdk_update sdk_adjust sdk_menu sdk_menuconfig sdk_clean sdk_distclean



sdk_sz:
	# 通过xmodem协议发送固件到本地
	cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.zz
	#cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.upgrade
	#cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.txt
sdk_tar:
	if [ "X${gPACK_MAKEFILE}" != "X" ]; then \
		if [ -f ${gPACK_MAKEFILE} ]; then \
			make -f ${gPACK_MAKEFILE}; \
		fi; \
	fi
	cd ${gBUILD_DIR}; \
	if [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.bb ] && [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ]; then \
		rm -fr ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
		tar jcvf ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*; \
		sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
	fi
sdk_zzb:
	cd ${gBUILD_DIR}; \
	if [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot ] && [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ]; then \
		rm -fr ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb; \
		if [ -e ${gOEM_DIR} ]; then \
			firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zzb ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot ${gOEM_SH} ${gOEM_CONFIG}; \
			sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zzb; \
			mv ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zzb ~/tftp; \
		else \
			firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.uboot; \
			sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb; \
			mv ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zzb ~/tftp; \
		fi; \
	fi
.PHONY: sdk_sz sdk_zzb sdk_tar



sdk_ftp:
	# 上传到FTP目录
	if [ -e ${gOEM_DIR} ]; then \
		firmware-upload mtk2 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz; \
	else \
		firmware-upload mtk2 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz; \
		if [ -f ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ]; then\
			firmware-upload mtk2 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt; \
		fi \
	fi
sdk_repo:
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then \
		firmware-upload mtk2 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: sdk_ftp sdk_repo



