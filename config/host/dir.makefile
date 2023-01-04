exclude_dirs:= .
subdirs:=$(shell find . -maxdepth 1 -type d)
subdirs:=$(patsubst ./%,%,$(subdirs))
subdirs:=$(filter-out $(exclude_dirs),$(subdirs))
ifeq ("X${OBJ}","X")
-include ${gPROJECT_CFGFILE}
else
PROJECTS := ${OBJ}
subdirs := ${OBJ}
endif
all dep:
	for i in ${PROJECTS} ;do \
		if [ -d $$i ]; then \
			if [ ! -d ${gBUILD_DIR}/$$i ]; then \
				mkdir ${gBUILD_DIR}/$$i; \
				cp -fr $$i/* ${gBUILD_DIR}/$$i; \
			fi; \
			if [ -f ${gBUILD_DIR}/$$i/${gPROJECT_INF} ];then \
				if [ -f ${gBUILD_DIR}/$$i/fpk.makefile ]; then \
					echo "[`pwd`] make -f ${gBUILD_DIR/}$$i/fpk.makefile -C $$i $@"; \
					make -f ${gBUILD_DIR}/$$i/fpk.makefile -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				else \
					echo "[`pwd`] make -f ${gFPK_MAKEFILE} -C $$i $@"; \
					make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		fi; \
	done 
install:
	for i in ${PROJECTS} ;do \
		if [ -d $$i ]; then \
			if [ ! -d ${gBUILD_DIR}/$$i ]; then \
				mkdir ${gBUILD_DIR}/$$i; \
				cp -fr $$i/* ${gBUILD_DIR}/$$i; \
			fi; \
			if [ -f ${gBUILD_DIR}/$$i/${gPROJECT_INF} ];then \
				if [ -f ${gBUILD_DIR}/$$i/fpk.makefile ]; then \
					echo "[`pwd`] make -f ${gBUILD_DIR}/$$i/fpk.makefile -C $$i $@"; \
					make -f ${gBUILD_DIR}/$$i/fpk.makefile -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				else \
					echo "[`pwd`] make -f ${gFPK_MAKEFILE} -C $$i $@"; \
					make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		fi; \
	done 
clean distclean:
	for i in ${subdirs} ;do \
		if [ -d ${gBUILD_DIR}/$$i ]; then \
			rm -fr ${gBUILD_DIR}/$$i; \
		fi; \
	done 
.PHONY: dep all install clean distclean

