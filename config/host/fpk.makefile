
# 包含skin项目的一些有关包处理的函数
include $(gSKIN_MKFILE)
# 项目的注册信息
PKG_NAME:=$(shell prj-read name)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(shell pwd)
# 生成项目通用定义
$(eval $(call Package/Define))
# 包编译时的目录
export PKG_NAME PKG_VERSION PROJECT_ID VERSION_ID PKG_BUILD_DIR FPK_BUILD_DIR FPK_LIB_DIR FPK_BIN_DIR FPK_ROOTFS_DIR
#
all:
	$(call Build/Compile/Default)
	$(call Build/Install/Collect)
install:
	$(call Build/Install/Fpk,$(gosROOT_DIR),$(gSTORE_DIR))
dep:
clean distclean:

.PHONY: all clean distclean dep install

