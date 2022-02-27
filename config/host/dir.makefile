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
	@for i in ${PROJECTS} ;do \
		if [ -d $$i ]; then \
			if [ ! -d ${gBUILD_DIR}/$$i ]; then \
				cp -Lr $$i ${gBUILD_DIR}; \
			fi; \
			if [ -f ${gBUILD_DIR}/$$i/${gPROJECT_INF} ];then \
				if [ -f ${gBUILD_DIR}/$$i/Makefile.prj ]; then \
					echo "[`pwd`] make -f ${gBUILD_DIR/}$$i/Makefile.prj -C $$i $@"; \
					make -f ${gBUILD_DIR}/$$i/Makefile.prj -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				elif [ -f ${gPRJ_MAKEFILE} ]; then\
					echo "[`pwd`] make -f ${gPRJ_MAKEFILE} -C $$i $@"; \
					make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		fi; \
	done 
install:
	@for i in ${PROJECTS} ;do \
		if [ -d $$i ]; then \
			if [ ! -d ${gBUILD_DIR}/$$i ]; then \
				cp -Lr $$i ${gBUILD_DIR}; \
			fi; \
			if [ -f ${gBUILD_DIR}/$$i/${gPROJECT_INF} ];then \
				if [ -f ${gBUILD_DIR}/$$i/Makefile.prj ]; then \
					echo "[`pwd`] make -f ${gBUILD_DIR}/$$i/Makefile.prj -C $$i $@"; \
					make -f ${gBUILD_DIR}/$$i/Makefile.prj -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				elif [ -f ${gPRJ_MAKEFILE} ]; then\
					echo "[`pwd`] make -f ${gPRJ_MAKEFILE} -C $$i $@"; \
					make -f ${gPRJ_MAKEFILE} -C ${gBUILD_DIR}/$$i $@ || exit $?; \
				fi; \
			fi; \
		fi; \
	done 
clean distclean:
	@for i in ${subdirs} ;do \
		if [ -d ${gBUILD_DIR}/$$i ]; then \
			rm -fr ${gBUILD_DIR}/$$i; \
		fi; \
	done 
.PHONY: dep all install clean distclean

