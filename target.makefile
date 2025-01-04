
# include the target makefile configure
include ${gMAKEFILE_CFGFILE}

# export the COMPILE_PROJECT
COMPILE_PROJECT := ${COMPILE_PROJECT}
export COMPILE_PROJECT

# SDK complie
boot:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
boot_%:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
kernel:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
kernel_%:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
app:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
app_%:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
sdk_%:
	if [ -d ${gSDK_DIR} ]; then \
		make -f ${gSDK_MAKEFILE} -C ${gSDK_DIR} $@; \
	else \
		make -f ${gSDK_MAKEFILE} -C ${gPLATFORM_DIR} $@; \
	fi
