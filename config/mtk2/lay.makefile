### Common Actions
rootfs_prepare:
	# make the rootfs
	mkdir -p ${gosROOT_DIR}
	if [ -f ${gPLATFORM_DIR}/mkrootfs.sh ]; then \
		${gPLATFORM_DIR}/mkrootfs.sh ${gosROOT_DIR}; \
	fi
	if [ -f ${gHARDWARE_DIR}/mkrootfs.sh ]; then \
		${gHARDWARE_DIR}/mkrootfs.sh ${gosROOT_DIR}; \
	fi
	if [ -f ${gCUSTOM_DIR}/mkrootfs.sh ]; then \
		${gCUSTOM_DIR}/mkrootfs.sh ${gosROOT_DIR}; \
	fi
	if [ -f ${gSCOPE_DIR}/mkrootfs.sh ]; then \
		${gSCOPE_DIR}/mkrootfs.sh ${gosROOT_DIR}; \
	fi
	# copy a fpk to build
	tmpls=`find ${gPLATFORM_DIR}/ -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		cp ${gPLATFORM_DIR}/*.fpk $(gBUILD_DIR); \
	fi
	tmpls=`find ${gHARDWARE_DIR}/ -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		cp ${gHARDWARE_DIR}/*.fpk $(gBUILD_DIR); \
	fi
	tmpls=`find ${gCUSTOM_DIR}/ -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		cp ${gCUSTOM_DIR}/*.fpk $(gBUILD_DIR); \
	fi
	tmpls=`find ${gSCOPE_DIR}/ -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		cp ${gSCOPE_DIR}/*.fpk $(gBUILD_DIR); \
	fi
	# clear the exsit project fpk
	for d in ${gPROJECT_DIR} ${gRICE_DIR} ${gPLATFORM_DIR}/cdriver; do \
		cd $${d}; list=`ls`; \
		for i in $${list}; do \
			if [ -f $${d}/$$i/${gPROJECT_INF} ]; then \
				rm -fr $(gBUILD_DIR)/$$i-*.fpk; \
			fi \
		done;
	done
	cd ${gPLATFORM_DIR}; \
	list="arch pdriver"; \
	for i in $${list}; do \
		if [ -f ${gPLATFORM_DIR}/$$i/Makefile ]; then \
			rm -fr $(gBUILD_DIR)/$$i-*.fpk; \
		fi \
	done
	# install the fpk to rootfs
	@tmpls=`find $(gBUILD_DIR) -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		for c in $(gBUILD_DIR)/*.fpk; do \
			if [ -f $$c ]; then \
				fpk-install ${gosROOT_DIR} ${gINSTALL_DIR} $$c; \
			fi; \
		done; \
	fi

rootfs_install:
	if [ -d ${gPLATFORM_DIR}/rootfs ]; then \
		cp -fdRp ${gPLATFORM_DIR}/rootfs/* ${gosROOT_DIR}; \
	fi
	if [ -d ${gHARDWARE_DIR}/rootfs ]; then \
		cp -fdRp ${gHARDWARE_DIR}/rootfs/* ${gosROOT_DIR}; \
	fi
	if [ -d ${gCUSTOM_DIR}/rootfs ]; then \
		cp -fdRp ${gCUSTOM_DIR}/rootfs/* ${gosROOT_DIR}; \
	fi
	if [ -d ${gSCOPE_DIR}/rootfs ]; then \
		cp -fdRp ${gSCOPE_DIR}/rootfs/* ${gosROOT_DIR}; \
	fi
	if [ -d ${gosROOT_DIR} ]; then \
		find ${gosROOT_DIR} -type d -name ".svn"|xargs rm -rf; \
		find ${gosROOT_DIR} -type d -name ".git"|xargs rm -rf; \
	fi

### Phony Target Declare
.PHONY: rootfs_prepare rootfs_install

