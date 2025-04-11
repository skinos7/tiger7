# 定义通用的编译函数, 参考openwrt/include/package-defaults.mk
#
# 定义项目头部通用定义函数
define Package/Define
  LIB_LIST:=$(shell prj-read lib)
  COM_LIST:=$(shell prj-read com)
  CMD_LIST:=$(shell prj-read cmd)
  EXE_LIST:=$(shell prj-read exe)
  OSC_LIST:=$(shell prj-read osc)
  KO_LIST:=$(shell prj-read ko)
  RES_LIST:=$(shell prj-read res)
  SH_LIST:=$(wildcard *.sh *.ash *.lua)
  MKD_LIST:=$(wildcard *.md)
  PNG_LIST:=$(wildcard *.png *.jpg)
  MISC_LIST:=$(wildcard *.json *.cfg *.html)
  FPK_BUILD_DIR:=$(PKG_BUILD_DIR)/.fpk
  FPK_LIB_DIR:=$(PKG_BUILD_DIR)/.fpk/lib
  FPK_BIN_DIR:=$(PKG_BUILD_DIR)/.fpk/bin
  FPK_ETC_DIR:=$(PKG_BUILD_DIR)/.fpk/etc
  FPK_INT_DIR:=$(PKG_BUILD_DIR)/.fpk/internal
  FPK_ROOTFS_DIR:=$(PKG_BUILD_DIR)/.fpk/rootfs
  PROJECT_ID?=${PKG_NAME}
  VERSION_ID?=${PKG_VERSION}
  export PROJECT_ID VERSION_ID
endef


# 定义项目预处理函数, 将源代码拷贝到PKG_BUILD_DIR
# $(call Build/Prepare/Default, NeedBeCopySubdirList);
#     确认有项目目录下prj.json文件, prj.json中要有VERSION_ID
#     拷贝NeedBeCopySubdirList指定的子目录列表, 如未给出默认拷贝${LIB_LIST} ${COM_LIST} ${CMD_LIST} ${EXE_LIST} ${OSC_LIST} ${KO_LIST}
define Build/Prepare/Default
	@if [ "X" == "X${VERSION_ID}" ]; then \
		echo "project ${PROJECT_ID} version cannot find, maybe ${gPROJECT_INF} broken"; \
		exit -1; \
	fi
	@mkdir -p $(PKG_BUILD_DIR)
	if [ -d ./lib ]; then \
		$(CP) ./lib $(PKG_BUILD_DIR); \
	fi
	@if [ "X" == "X$(1)" ]; then \
		for i in ${LIB_LIST} ${COM_LIST} ${CMD_LIST} ${EXE_LIST} ${OSC_LIST} ${KO_LIST} ;do \
			if [ -e $$i ]; then \
				$(CP) $$i $(PKG_BUILD_DIR); \
			fi; \
		done; \
	else \
		for i in $(1) ;do \
			if [ -e $$i ]; then \
				$(CP) $$i $(PKG_BUILD_DIR); \
			fi; \
		done; \
	fi
endef


# 定义项目配置函数
# $(call Build/Configure/Default, ConfigureArgs, ConfigureArgs, Subdir);
#     确认要配置的目录中有configure文件
#     ConfigureArgs可指定调用configure时给出参数
#     Subdir可指定配置的子目录
# $(call Build/Prepare/Farm, NeedBeConfigureSubdirList);
#     确认要配置的目录中有configure文件
#     NeedBeConfigureSubdirList指定要配置的子目录列表, 如未给出默认只配置${OSC_LIST}


# 定义编译程序编译函数
# $(call Build/Compile/FarmBin, NeedBeCompileSubdirList, MakefilePath, CompileTarget);
#     NeedBeCompileSubdirList指定要编译的子目录列表
#     如果对应的子目录下无Makefile, 使用MakefilePath为Makefile
#     CompileTarget给出编译的目标
define Build/Compile/FarmBin
	+$(MAKE_VARS); \
	for i in $(1) ;do \
		if [ -f $(PKG_BUILD_DIR)/$$i/Makefile ]; then \
			$(MAKE) LINUX_DIR=$(LINUX_DIR) STAGING_DIR=$(STAGING_DIR) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$$i $(MAKE_FLAGS) $(3) || exit -1; \
		else \
			$(MAKE) LINUX_DIR=$(LINUX_DIR) STAGING_DIR=$(STAGING_DIR) -f $(2) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$$i $(MAKE_FLAGS) $(3) || exit -1; \
		fi \
	done
endef
# 定义编译内核驱动
# $(call Build/Compile/FarmKo, MakeArgs);
#     MakeArgs给出Make编译时的参数
define Build/Compile/FarmKo
    +$(MAKE_VARS); \
    for i in $(1) ;do \
        $(MAKE) $(2) LINUX_DIR=$(LINUX_DIR) STAGING_DIR=$(STAGING_DIR) -C $(LINUX_DIR) SUBDIRS="$(PKG_BUILD_DIR)/$$i" ARCH="$(LINUX_KARCH)" CROSS_COMPILE="$(TARGET_CROSS)" modules; \
	done
endef
# 定义项目编译函数
# $(call Build/Compile/Default, NeedBeCompileOSCList, OSCMakefilePath); 
#     编译${LIB_LIST} ${COM_LIST} ${CMD_LIST} ${EXE_LIST} ${KO_LIST}, 并安装${LIB_LIST}
#     NeedBeCompileOSCList给出测编译此子目录列表, 如未给出测编译${OSC_LIST}
#     编译$(OSC_LIST)时如果对应的子目录下无Makefile, 使用OSCMakefilePath为Makefile
define Build/Compile/Default
	$(FIND) $(PKG_BUILD_DIR) -name \*.o -or -name \*.a | $(XARGS) rm -f
	$(call Build/Compile/FarmBin,${LIB_LIST},${gLIB_MAKEFILE})
	$(call Build/Compile/FarmBin,${LIB_LIST},${gLIB_MAKEFILE},install)
	$(call Build/Compile/FarmBin,${COM_LIST},${gCOM_MAKEFILE})
	$(call Build/Compile/FarmBin,${CMD_LIST},${gEXE_MAKEFILE})
	$(call Build/Compile/FarmBin,${EXE_LIST},${gEXE_MAKEFILE})
	$(call Build/Compile/FarmKo,${KO_LIST})
	if [ "X" == "X$(1)" ]; then \
		$(call Build/Compile/FarmBin,${OSC_LIST},$(2)); \
	fi
	if [ "X" != "X$(1)" ]; then \
		$(call Build/Compile/FarmBin,$(1),$(2)); \
	fi
endef


# 定义项目安装函数
# $(call Build/Install/Default, NeedBeInstallSubdirList, MakefilePath);
#     NeedBeCompileOSCList给出测安装此子目录列表, 如未给出测编译${OSC_LIST}
#     安装$(OSC_LIST)时如果对应的子目录下无Makefile, 使用OSCMakefilePath为Makefile
define Build/Install/Default
	@if [ "X" == "X$(1)" ]; then \
		$(call Build/Compile/FarmBin,${OSC_LIST},$(2),install); \
	fi
	@if [ "X" != "X$(1)" ]; then \
		$(call Build/Compile/FarmBin,$(1),$(2),install); \
	fi
endef



# 定义收集项目所有编译后的文件到FPK_BUILD_DIR准备打包为FPK包
# $(call Build/Install/Collect);
define Build/Install/Collect
	# make the fpk dir
	$(INSTALL_DIR) $(FPK_BUILD_DIR)
	$(INSTALL_DIR) $(FPK_LIB_DIR)
	$(INSTALL_DIR) $(FPK_BIN_DIR)
	if [ -d ./lib ]; then \
		$(CP) ./lib/*.so* $(FPK_LIB_DIR); \
	fi
	if [ -d ./bin ]; then \
		$(CP) ./bin/* $(FPK_BIN_DIR); \
	fi
	if [ -d ./etc ]; then \
		$(INSTALL_DIR) $(FPK_ETC_DIR); \
		$(CP) ./etc/* $(FPK_ETC_DIR); \
	fi
	if [ -d ./internal ]; then \
		$(INSTALL_DIR) $(FPK_INT_DIR); \
		$(CP) ./internal/* $(FPK_INT_DIR); \
	fi
	if [ -d ./rootfs ]; then \
		$(INSTALL_DIR) $(FPK_ROOTFS_DIR); \
		$(CP) ./rootfs/* $(FPK_ROOTFS_DIR); \
	fi
	for c in $(PNG_LIST) $(MISC_LIST) ${RES_LIST}; do \
		if [ -e ./$$c ]; then \
			$(CP) $$c $(FPK_BUILD_DIR); \
		fi; \
	done
	if [ "X" != "$(SH_LIST)"  ]; then \
		for c in $(SH_LIST); do \
			if [ -e ./$$c ]; then \
				$(CP) $$c $(FPK_BUILD_DIR); \
				chmod a+x $(FPK_BUILD_DIR)/$$c; \
			fi; \
		done; \
	fi
	for i in ${COM_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			$(CP) $(PKG_BUILD_DIR)/$$i/$$i.com $(FPK_BUILD_DIR); \
		fi; \
	done
	for i in ${OSC_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			if [ -e $(PKG_BUILD_DIR)/$$i/$$i ]; then \
				$(INSTALL_BIN) $(PKG_BUILD_DIR)/$$i/$$i $(FPK_BUILD_DIR); \
			fi; \
		fi; \
	done
	for i in ${LIB_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			$(CP) $(PKG_BUILD_DIR)/$$i/lib$$i.so $(FPK_LIB_DIR); \
			$(LN) lib$$i.so $(FPK_LIB_DIR)/lib$$i.so.0; \
		fi; \
	done
	for i in ${CMD_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			if [ -e $(PKG_BUILD_DIR)/$$i/$$i ]; then \
				$(INSTALL_BIN) $(PKG_BUILD_DIR)/$$i/$$i $(FPK_BIN_DIR); \
			fi; \
		fi; \
	done
	for i in ${EXE_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			if [ -e $(PKG_BUILD_DIR)/$$i/$$i ]; then \
				$(INSTALL_BIN) $(PKG_BUILD_DIR)/$$i/$$i $(FPK_BUILD_DIR); \
			fi; \
		fi; \
	done
	for i in ${KO_LIST};do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			$(INSTALL_BIN) $(PKG_BUILD_DIR)/$$i/*.ko $(FPK_BUILD_DIR); \
		fi; \
	done
	#
	$(INSTALL_DIR) $(gCOMFACE_DIR)/$(PROJECT_ID)
	for i in ${MKD_LIST};do \
        if [ -e ./$$i ]; then \
            $(CP) $$i $(gCOMFACE_DIR)/$(PROJECT_ID); \
        fi; \
	done
	for i in ${PNG_LIST};do \
        if [ -e ./$$i ]; then \
            $(CP) $$i $(gCOMFACE_DIR)/$(PROJECT_ID); \
        fi; \
	done
endef

# 定义项目生成FPK包及安装FPK包
# $(call Build/Install/Fpk, TargetRootfs, FpkDir);
#     TargetRootfs给出目标系统的文件根目录
#     给出FpkDir时将fpk拷贝至此目录
define Build/Install/Fpk
	# install the develop link lib and head file to fpk
	$(INSTALL_DIR) ${FPK_BUILD_DIR}/install
	$(INSTALL_DIR) ${FPK_BUILD_DIR}/install/include
	for i in ${LIB_LIST} ;do \
		if [ -d $(PKG_BUILD_DIR)/$$i ]; then \
			$(INSTALL_DIR) ${FPK_BUILD_DIR}/install/include/$$i; \
			$(CP) $(PKG_BUILD_DIR)/$$i/*.h ${FPK_BUILD_DIR}/install/include/$$i; \
			$(INSTALL_DIR) ${FPK_BUILD_DIR}/install/lib; \
			$(CP) $(PKG_BUILD_DIR)/$$i/lib*.so ${FPK_BUILD_DIR}/install/lib; \
			$(LN) lib$$i.so ${FPK_BUILD_DIR}/install/lib/lib$$i.so.0; \
		fi; \
	done
	cd ${FPK_BUILD_DIR};tar zcf $(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk *
	fpk-install $(1) ${gINSTALL_DIR} $(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk
	if [ "X" != "X$(2)" ]; then \
		mv $(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk $(2); \
	fi
endef


