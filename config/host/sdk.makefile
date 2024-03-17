#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.



update:
	# 更新fpk
	if [ -e ${gPLATFORM_DIR} ]; then \
		cd ${gPLATFORM_DIR}; rm -fr *.fpk*; \
		cd ${gPLATFORM_DIR}; sdk-update host ${gHARDWARE} ${gCUSTOM} fpk; \
	fi
adjust:
menu:
menuconfig: kernel_dep
menuclean:
bufclean:
sdkclean: bufclean
	cd ${gCUSTOM_DIR}; rm -fr *.fpk *.store
.PHONY: update adjust menu menuconfig menuclean bufclean sdkclean



local:
	sudo rm -fr /tmp/skin
	sudo rm -fr /usr/prj
	sudo cp -ar ${gosROOT_DIR}/usr/local/bin/* /usr/local/bin
	sudo cp -ar ${gosROOT_DIR}/usr/local/lib/* /usr/local/lib
	sudo cp -ar ${gosROOT_DIR}/usr/prj /usr
	sudo ldconfig
start:
	if [ -f /usr/prj/setup.sh ]; then \
		/usr/prj/setup.sh; \
	fi
stop:
	if [ -f /usr/prj/shut.sh ]; then \
		/usr/prj/shut.sh; \
	fi
	sudo rm -fr /tmp/skin
purge:
	sudo rm -fr /var/skin
sz:
	# 通过xmodem协议发送固件到本地
	cd ${gBUILD_DIR} && sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.upgrade ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*.txt
zzb:
tar:
ftp:
	# 上传到FTP目录
	if [ -e ${gOEM_DIR} ]; then \
		firmware-upload host ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.tar.bz2; \
	else \
		firmware-upload host ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
		if [ -f ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ]; then\
			firmware-upload host ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt; \
		fi \
	fi
repo:
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then\
		firmware-upload host ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: local start stop purge sz zzb tar ftp repo



boot_menuconfig: boot_dep
boot_dep:
boot: boot_dep
boot_install:
boot_clean:
boot_distclean: boot_clean
.PHONY: boot_menuconfig boot_dep boot boot_install boot_clean boot_distclean

kernel_menuconfig: kernel_dep
kernel_dep:
kernel: kernel_dep
kernel_install:
kernel_clean:
kernel_distclean: kernel_clean
.PHONY: kernel_menuconfig kernel_dep kernel kernel_install kernel_clean kernel_distclean

app_menuconfig:
app_dep:
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
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/${gSCOPE} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/${gSCOPE}/* ${gBUILD_DIR}/arch; \
		fi; \
	fi
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch dep
app: app_dep
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR}
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR}; \
	fi
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch
app_install:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} install
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} install; \
	fi
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch install
	make -C ${gTOP_DIR} lay_install
	cd ${gosROOT_DIR};if [ -f needless.sh ]; then \
		chmod a+rwx needless.sh;./needless.sh; \
	fi
	cd ${gBUILD_DIR} && fpk-indexed ${gSTORE_DIR} ${gSTORE_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.store;
	cd ${gBUILD_DIR} && rm -fr ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2
	cd ${gosROOT_DIR} && tar jcvf ../${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 *
app_clean:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} clean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} clean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch clean; \
	fi
app_distclean: app_clean
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} distclean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} distclean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch distclean; \
	fi
.PHONY: app_menuconfig app_dep app app_install app_clean app_distclean



