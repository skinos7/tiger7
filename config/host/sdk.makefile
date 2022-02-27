#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.
sinclude ${gSDK_MKFILE}



update:
adjust:
menu:
menuconfig: kernel_dep
menuclean:
bufclean:
sdkclean: bufclean
.PHONY: update adjust menu menuconfig menuclean bufclean sdkclean



install_local:
	if [ -f /prj/shut.sh ]; then \
		/prj/shut.sh; \
	fi
	sudo rm -fr /tmp/.farm
	sudo rm -fr /prj
	sudo cp -ar ${gosROOT_DIR}/bin/* /bin
	sudo cp -ar ${gosROOT_DIR}/lib/* /lib
	sudo cp -ar ${gosROOT_DIR}/prj /
local_run:
	if [ -f /prj/setup.sh ]; then \
		/prj/setup.sh; \
	fi
sz:
	# 通过xmodem协议发送固件到本地
	cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2
tar:
ftp:
	# 未定义了FTP目录不工作
	if [ "X${gpFTP_PUB_DIR}" = "X" ]; then\
		return;\
	fi
	# 上传到FTP目录
	if [ -f ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 ]; then \
		curl -u ${gpFTP_PUB_PASSWORD} -T ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 ${gpFTP_PUB_DIR}; \
	fi
.PHONY: sz tar ftp



boot_menuconfig: boot_dep
boot_dep:
boot: boot_dep
boot_install:
boot_clean:
boot_distclean: boot_clean
.PHONY: boot_menuconfig boot_dep boot boot_install boot_clean boot_distclean

kernel_menuconfig: kernel_dep
kernel_dep:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} dep
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} dep; \
	fi
	if [ ! -d ${gBUILD_DIR}/arch ]; then \
		cp -Lr ${gPLATFORM_DIR}/arch ${gBUILD_DIR}; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gPLATFORM} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gPLATFORM}/* ${gBUILD_DIR}/arch; \
		fi; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gHARDWARE} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gHARDWARE}/* ${gBUILD_DIR}/arch; \
		fi; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gCUSTOM} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/* ${gBUILD_DIR}/arch; \
		fi; \
	fi
	make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/arch dep
kernel: kernel_dep
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR}
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR}; \
	fi
	make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/arch
kernel_install:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} install
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} install; \
	fi
	make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/arch install
	make -C ${gTOP_DIR} lay_install
	cd ${gBUILD_DIR} && fpk-store ${gSTORE_DIR} ${gSTORE_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.store;
	cd ${gBUILD_DIR} && rm -fr ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2
	cd ${gosROOT_DIR} && tar jcvf ../${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 *
kernel_clean:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} clean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} clean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/arch clean; \
	fi
kernel_distclean: kernel_clean
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} distclean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} distclean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/arch distclean; \
	fi
.PHONY: kernel_menuconfig kernel_dep kernel kernel_install kernel_clean kernel_distclean

app_menuconfig app_dep app app_install app_clean app_distclean:
	echo -n ""
.PHONY: app_menuconfig app_dep app app_install app_clean app_distclean




