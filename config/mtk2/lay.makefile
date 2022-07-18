### Common Actions
all:
	@mkdir -p ${gosROOT_DIR}
	@if [ -f mkdir.sh ]; then \
		./mkdir.sh ${gosROOT_DIR}; \
	fi
	@if [ -f mkdev.sh ]; then \
		./mkdev.sh ${gosROOT_DIR}; \
	fi
	# copy a fpk to build
	@tmpls=`find ./ -maxdepth 1 -name "*.fpk"`; \
	if [ "X$${tmpls}" != "X" ]; then \
		cp *.fpk $(gBUILD_DIR); \
	fi

fpk_distinct:
	# clear the exsit project fpk for core
	cd ${gPROJECT_DIR}; \
	list=`ls`; \
	for i in $${list}; do \
		if [ -f ${gPROJECT_DIR}/$$i/${gPROJECT_INF} ]; then \
			rm -fr $(gBUILD_DIR)/$$i-*.fpk; \
		fi \
	done;
	if [ -d ${gRICE_DIR} ]; then \
		cd ${gRICE_DIR}; \
		list=`ls`; \
		for i in $${list}; do \
			if [ -f ${gRICE_DIR}/$$i/prj.json ]; then \
				rm -fr $(gBUILD_DIR)/$$i-*.fpk; \
			fi \
		done; \
	fi
	# clear the exsit project fpk for platform
	cd ${gPLATFORM_DIR}; \
	list="arch pdriver"; \
	for i in $${list}; do \
		if [ -f ${gPLATFORM_DIR}/$$i/Makefile ]; then \
			rm -fr $(gBUILD_DIR)/$$i-*.fpk; \
		fi \
	done

fpk_install:
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
	@if [ -d rootfs ]; then \
		cp -fdRp rootfs/* ${gosROOT_DIR}; \
	fi
	@if [ -d ${gosROOT_DIR} ]; then \
		find ${gosROOT_DIR} -type d -name ".svn"|xargs rm -rf; \
		find ${gosROOT_DIR} -type d -name ".git"|xargs rm -rf; \
	fi

### Phony Target Declare
.PHONY: all fpk_distinct fpk_install rootfs_install

