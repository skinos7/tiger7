#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.



update:
	# 更新fpk
	-if [ -e ${gCUSTOM_DIR} ]; then \
		cd ${gCUSTOM_DIR}; rm -fr *.fpk*; \
		cd ${gCUSTOM_DIR}; wget --ftp-user=dl --ftp-password=tiger7@ASHYELF ${gpFTP_PUB_SDK}/${gHARDWARE}/${gCUSTOM}/*.fpk; \
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
	if [ -f /prj/shut.sh ]; then \
		/prj/shut.sh; \
	fi
	sudo rm -fr /tmp/skin
	sudo rm -fr /var/skin
	sudo rm -fr /prj
	sudo cp -ar ${gosROOT_DIR}/bin/* /usr/local/bin
	sudo cp -ar ${gosROOT_DIR}/lib/* /usr/local/lib
	sudo cp -ar ${gosROOT_DIR}/prj /
	sudo ldconfig
	if [ -f /prj/setup.sh ]; then \
		/prj/setup.sh; \
	fi
sz:
tar:
ftp:
repo:
	# 未定义了FTP库不工作
	if [ "X${gpFTP_PUB_REPO}" = "X" ]; then\
		sleep 10000;\
	fi
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then\
		for i in `ls ${gSTORE_DIR}`; do \
			curl -u ${gpFTP_PUB_PASSWORD} -T ${gSTORE_DIR}/$${i} ${gpFTP_PUB_REPO}/; \
		done \
	fi
.PHONY: local sz tar ftp repo



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



