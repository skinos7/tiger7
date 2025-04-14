# Toplevel Makefile
# Copyright 2009-2020, dimmalex
# All Rights Reserved.
#
# Macro command rule:
# 1. Prefix is small g for System global macros
# 2. Prefix is small gp for Platform-specific global macros
# 2. Prefix is small gh for Chip-specific global macros
# 3. Prefix is small gc for PCB-specific global macros
# 3. Prefix is small gs for Scope-specific global macros
# 6. Prefix is small gos for information about running system's rootfs
# 4. Suffix is _DIR is directory
# 5. Suffix is _CFGFILE is configure file
# 7. Suffix is _SH is shell
# 8. Suffix is _CMD is command

ifeq (gBOARDID, $(wildcard gBOARDID))
sinclude gBOARDID 
endif
gBOARDID ?= host-x86-ubuntu

#####################################
######### Do not modify #############
#####################################
gNPROC=`nproc`
empty:=
space:= $(empty) $(empty)
gBOARDIDS := $(subst -,${space},${gBOARDID})
# Platform-specific Identify
gPLATFORM := $(word 1,${gBOARDIDS})
# Chip-specific Identify
gHARDWARE := $(word 2,${gBOARDIDS})
# PCB-specific Identify
gCUSTOM := $(word 3,${gBOARDIDS})
# Custom Identify
gSCOPE := $(word 4,${gBOARDIDS})
# Custom OEM
gOEM := $(word 5,${gBOARDIDS})
ifeq (${gSCOPE},)
gSCOPE := std
endif 
ifeq (${gOEM},)
gOEM := default
endif 
# Get the date
#gPUBLISH ?= $(shell date +%m%d%y)
gPUBLISH ?= v7.4.0915
gVERSION ?= $(gPUBLISH)
# Name for compiler
gMAKER ?= dimmalex@gmail.com
export gNPROC
export gBOARDIDS gPLATFORM gHARDWARE gCUSTOM gSCOPE gOEM
export gVERSION gPUBLISH gMAKER

#####################################
######### Do not modify #############
#####################################
# Top directory
gTOP_DIR := $(shell pwd)
# Directory for Making ROM or temporary file during compilation
gBUILD_DIR := ${gTOP_DIR}/build
# Compile temporary installation directory
gINSTALL_DIR := ${gBUILD_DIR}/install
# Compile FPK temporary directory
gSTORE_DIR := ${gBUILD_DIR}/store
# Markdown directory for project component
gCOMFACE_DIR := ${gTOP_DIR}/doc/com
# Tools directory for compile
gTOOLS_DIR := ${gTOP_DIR}/tools
# Runing-system's rootfs directory
gosROOT_DIR := ${gBUILD_DIR}/rootfs
# Project directory name
# Runing-system's project directory
gosPRJ_DIR := ${gosROOT_DIR}/prj
# Platform-specific configure directory
gPLATFORM_DIR := ${gTOP_DIR}/config/${gPLATFORM}
# Chip-specific configure directory
gHARDWARE_DIR := ${gPLATFORM_DIR}/${gHARDWARE}
# PCB-specific configure directory
gCUSTOM_DIR := ${gHARDWARE_DIR}/${gCUSTOM}
# Custom configure directory
gSCOPE_DIR := ${gCUSTOM_DIR}/${gSCOPE}
# OEM configure directory
gOEM_DIR := ${gCUSTOM_DIR}/${gOEM}
gOEM_SHELL := ${gOEM_DIR}/${gOEM}.sh
# Platform-specific tools directory for compile
gpTOOLS_DIR := ${gPLATFORM_DIR}/tools
# Project source code directory
gPROJECT_DIR := ${gTOP_DIR}/project
# Commercial customer project source code directory
gRICE_DIR := ${gTOP_DIR}/rice
# Project configure json filename
gPROJECT_INF := prj.json
# Makefile configure pathname
gMAKEFILE_CFGFILE := $(shell \
	if [ -f "${gSCOPE_DIR}/makefile.config" ]; \
	then echo ${gSCOPE_DIR}/makefile.config; \
	elif [ -f "${gCUSTOM_DIR}/makefile.config" ]; \
	then echo ${gCUSTOM_DIR}/makefile.config; \
	elif [ -f "${gHARDWARE_DIR}/makefile.config" ]; \
	then echo ${gHARDWARE_DIR}/makefile.config;\
	else echo ${gPLATFORM_DIR}/makefile.config;\
	fi)
gCONFIGURE_SH := configure.sh
gMAKEFILE_SH := makefile.sh
# Platform-specific SDK directory
gSDK_DIR := ${gTOP_DIR}/${gPLATFORM}
# Makefile for Compile SDK
gSDK_MAKEFILE := ${gPLATFORM_DIR}/sdk.makefile
# Makefile for layout complie temporary directory 
gLAY_MAKEFILE := ${gPLATFORM_DIR}/lay.makefile
# Makefile for fpk project directory
gDIR_MAKEFILE := ${gPLATFORM_DIR}/dir.makefile
# Makefile for fpk project
gFPK_MAKEFILE := ${gPLATFORM_DIR}/fpk.makefile
# Makefile for component
gCOM_MAKEFILE := ${gPLATFORM_DIR}/com.makefile
# Makefile for library
gLIB_MAKEFILE := ${gPLATFORM_DIR}/lib.makefile
# Makefile for execute
gEXE_MAKEFILE := ${gPLATFORM_DIR}/exe.makefile
# Makefile for flash
gPACK_MAKEFILE := ${gPLATFORM_DIR}/pack.makefile
# Export macro
export gTOP_DIR gBUILD_DIR gINSTALL_DIR gSTORE_DIR gCOMFACE_DIR gTOOLS_DIR
export gosROOT_DIR gosPRJ_DIR
export gPLATFORM_DIR gHARDWARE_DIR gCUSTOM_DIR gSCOPE_DIR
export gOEM_DIR gOEM_SHELL
export gpTOOLS_DIR
export gPROJECT_DIR gRICE_DIR gPROJECT_INF gMAKEFILE_CFGFILE gCONFIGURE_SH gMAKEFILE_SH
export gSDK_DIR gSDK_MAKEFILE gLAY_MAKEFILE gDIR_MAKEFILE gFPK_MAKEFILE gCOM_MAKEFILE gLIB_MAKEFILE gEXE_MAKEFILE gPACK_MAKEFILE
# add the tools directory to PATH
PATH := $(gpTOOLS_DIR):$(gTOOLS_DIR):$(PATH)
export PATH

#####################################
######### Start and end #############
#####################################
all:
	if [ "X${obj}" != "X" ]; then \
		make -f target.makefile kernel COMPILE_PROJECT=package/${obj}/compile; \
		make -f target.makefile app COMPILE_PROJECT=package/${obj}/compile; \
	else \
		make dep; \
		make kernel_dep app_dep; \
		make kernel||exit -1; \
		make app||exit -1; \
		make kernel_install||exit-1; \
		make app_install||exit-1; \
		cp -r ${gINSTALL_DIR}/include  ${gTOP_DIR}/doc/dev; \
	fi

pid:
	@echo '\ngBOARDID=${gBOARDID}' >> gBOARDID
	@echo "Switch the Board Identify to ${gBOARDID}"
pidinfo:
	echo 'gBOARDID=${gBOARDID}'

dep:            # prepare gBUILD_DIR gINSTALL_DIR gosROOT_DIR gSTORE_DIR, install fpk to gosROOT_DIR
	./mkdel
	rm -rf ${gINSTALL_DIR} ${gBUILD_DIR}
	install -d ${gBUILD_DIR} ${gINSTALL_DIR} ${gINSTALL_DIR}/lib ${gINSTALL_DIR}/include ${gosROOT_DIR} ${gSTORE_DIR}
	make -f ${gLAY_MAKEFILE} rootfs_prepare
rootfs_install: # call before image, copy all the file of config/xxxx/**rootfs** to gosROOT_DIR
	make -f ${gLAY_MAKEFILE} rootfs_install

clean:
	if [ "X${obj}" != "X" ]; then \
		make -f target.makefile kernel COMPILE_PROJECT=package/${obj}/clean; \
		make -f target.makefile app COMPILE_PROJECT=package/${obj}/clean; \
	else \
		make kernel_clean; \
		make app_clean; \
		rm -rf ${gINSTALL_DIR} ${gBUILD_DIR}; \
	fi

.PHONY: all pid pidinfo dep rootfs_install clean

#####################################
######### Tools target ##############
#####################################
tools:
	make -C ${gTOOLS_DIR}
tools_install:
	make -C ${gTOOLS_DIR} install
tools_clean:
	make -C ${gTOOLS_DIR} clean
.PHONY: tools tools_install tools_clean

#####################################
######## Misc Target ################
#####################################
sinclude misc.makefile

#####################################
######### Forward to SDK ############
#####################################
boot:
	make -f target.makefile $@
boot_%:
	make -f target.makefile $@
kernel:
	make -f target.makefile $@
kernel_%:
	make -f target.makefile $@
app:
	make -f target.makefile $@
app_%:
	make -f target.makefile $@
sdk_%:
	make -f target.makefile $@
