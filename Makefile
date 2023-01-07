# Toplevel Makefile
# Copyright 2009-2020, dimmalex
# All Rights Reserved.
#
# Macro command rule:
# 1. Prefix is small g for System global macros
# 2. Prefix is small gp for Platform-specific global macros
# 2. Prefix is small gh for Chip-specific global macros
# 3. Prefix is small gc for PCB-specific global macros
# 4. Suffix is _DIR is directory
# 5. Suffix is _CFGFILE is configure file
# 6. Prefix is small gos for information about running system's rootfs
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
gZZID := $(word 5,${gBOARDIDS})
ifeq (${gSCOPE},)
gSCOPE := std
endif 
ifeq (${gZZID},)
gZZID := default
endif 
# Get the date
#gPUBLISH ?= $(shell date +%m%d%y)
gPUBLISH ?= v7.2.1231
#gPUBLISH ?= v7.2.1020
gVERSION ?= $(gPUBLISH)
# Name for compiler
gMAKER ?= dimmalex@gmail.com
export gNPROC gZZID gPLATFORM gHARDWARE gCUSTOM gSCOPE gVERSION gPUBLISH gMAKER

#####################################
######### Do not modify #############
#####################################
# Top directory
gTOP_DIR := $(shell pwd)
# Directory for Making ROM or temporary file during compilation
gBUILD_DIR := ${gTOP_DIR}/build
# Compile temporary installation directory
gINSTALL_DIR := ${gBUILD_DIR}/install
# Runing-system's rootfs directory
gosROOT_DIR := ${gBUILD_DIR}/rootfs
# Project directory name
gosPRJ_NAME := prj
# Runing-system's project directory
gosPRJ_DIR := ${gosROOT_DIR}/${gosPRJ_NAME}
# Platform-specific configure directory
gPLATFORM_DIR := ${gTOP_DIR}/config/${gPLATFORM}
# Chip-specific configure directory
gHARDWARE_DIR := ${gPLATFORM_DIR}/${gHARDWARE}
# Compile FPK temporary directory
gSTORE_DIR := ${gBUILD_DIR}/store
# PCB-specific configure directory
gCUSTOM_DIR := ${gHARDWARE_DIR}/${gCUSTOM}
# Custom configure directory
gSCOPE_DIR := ${gCUSTOM_DIR}/${gSCOPE}
gZZID_DIR := ${gCUSTOM_DIR}/${gZZID}
gZZID_SH := ${gZZID}.sh
gZZID_SH_FILE := ${gZZID_DIR}/${gZZID}.sh
gZZID_CONFIG := ${gZZID}.dtar
gZZID_CONFIG_DIR := ${gZZID_DIR}/rootfs/prj/
# Markdown directory for project component
gCOMFACE_DIR := ${gTOP_DIR}/doc/com
# Tools directory for compile
gTOOLS_DIR := ${gTOP_DIR}/tools
# firmware upgrade setup shell, you can execute some commend at the firmware upgrade
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
gFLASH_MAKEFILE := ${gPLATFORM_DIR}/flash.makefile
# Export macro
export gTOP_DIR gBUILD_DIR gINSTALL_DIR
export gosROOT_DIR gosPRJ_NAME gosPRJ_DIR
export gPLATFORM_DIR gHARDWARE_DIR gSTORE_DIR gCUSTOM_DIR gSCOPE_DIR gCOMFACE_DIR gTOOLS_DIR gPROJECT_DIR gRICE_DIR
export gZZID_DIR gZZID_SH gZZID_SH_FILE gZZID_CONFIG gZZID_CONFIG_DIR
export gPROJECT_INF gMAKEFILE_CFGFILE
export gSDK_DIR
export gSDK_MAKEFILE gLAY_MAKEFILE gDIR_MAKEFILE gFPK_MAKEFILE gCOM_MAKEFILE gLIB_MAKEFILE gEXE_MAKEFILE gFLASH_MAKEFILE
# add the tools directory to PATH
PATH := $(gTOOLS_DIR):$(PATH)
export PATH

#####################################
######### Do not modify #############
#####################################
# Compile default action
all:
	if [ "X${obj}" != "X" ]; then \
		make -f Makefile.target kernel COMPILE_PROJECT=package/${obj}/compile; \
		make -f Makefile.target app COMPILE_PROJECT=package/${obj}/compile; \
	else \
		make lay; \
		make kernel||exit -1; \
		make app||exit -1; \
		make kernel_install||exit-1; \
		make app_install||exit-1; \
		cp -r ${gINSTALL_DIR}/include  ${gTOP_DIR}/doc/dev; \
	fi
dep: lay
	make boot_dep kernel_dep

lay:
	rm -rf ${gINSTALL_DIR} ${gBUILD_DIR}
	install -d ${gBUILD_DIR} ${gINSTALL_DIR} ${gINSTALL_DIR}/lib ${gINSTALL_DIR}/include ${gosROOT_DIR} ${gSTORE_DIR}
	if [ -d ${gPLATFORM_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gPLATFORM_DIR}; fi
	if [ -d ${gHARDWARE_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gHARDWARE_DIR}; fi
	if [ -d ${gCUSTOM_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gCUSTOM_DIR}; fi
	if [ -d ${gSCOPE_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gSCOPE_DIR}; fi
	make -f ${gLAY_MAKEFILE} fpk_distinct
	make -f ${gLAY_MAKEFILE} fpk_install
lay_install:
	if [ -d ${gPLATFORM_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gPLATFORM_DIR} rootfs_install; fi
	if [ -d ${gHARDWARE_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gHARDWARE_DIR} rootfs_install; fi
	if [ -d ${gCUSTOM_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gCUSTOM_DIR} rootfs_install; fi
	if [ -d ${gSCOPE_DIR} ]; then make -f ${gLAY_MAKEFILE} -C ${gSCOPE_DIR} rootfs_install; fi
	if [ -e ${gZZID_SH_FILE} ]; then \
		cp ${gZZID_SH_FILE} ${gBUILD_DIR}; \
	fi
	if [ -e ${gZZID_CONFIG_DIR} ]; then \
		cd ${gZZID_CONFIG_DIR}; \
		tar -c * -f ${gBUILD_DIR}/${gZZID_CONFIG}; \
	fi
clean:
	if [ "X${obj}" != "X" ]; then \
		make -f Makefile.target kernel COMPILE_PROJECT=package/${obj}/clean; \
		make -f Makefile.target app COMPILE_PROJECT=package/${obj}/clean; \
	else \
		make kernel_clean; \
		rm -rf ${gINSTALL_DIR} ${gBUILD_DIR}; \
	fi
distclean: kernel_distclean boot_distclean
.PHONY: all dep lay lay_install clean distclean

#####################################
######### Do not modify #############
#####################################
# compile for the tools
tools:
	make -C ${gTOOLS_DIR}
tools_install:
	make -C ${gTOOLS_DIR} install
tools_clean:
	make -C ${gTOOLS_DIR} clean
menu menuclean menuconfig:
	make -f Makefile.target $@
# make kernel obj=package/network/compile
kernel_menuconfig kernel_dep kernel kernel_install kernel_clean kernel_distclean:
	make -f Makefile.target $@
app_menuconfig app_dep app app_install app_clean app_distclean:
	make -f Makefile.target $@
boot_menuconfig boot_dep boot boot_install boot_clean boot_distclean:
	make -f Makefile.target $@
.PHONY: tools tools_install tools_clean
.PHONY: menu menuclean menuconfig
.PHONY: boot_menuconfig boot_dep boot boot_install boot_clean boot_distclean
.PHONY: kernel_menuconfig kernel_dep kernel kernel_install kernel_clean kernel_distclean
.PHONY: app_menuconfig app_dep app app_install app_clean app_distclean


sinclude Makefile.misc

