# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 定义项目基本信息
PKG_NAME:=tmptools
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

# 包含Openwrt的一些有关包处理的函数
include $(INCLUDE_DIR)/package.mk
include $(gSKIN_MKFILE)
# 生成项目通用定义
$(eval $(call Package/Define))

# menuconfig的选项的位置及说明
define Package/$(PKG_NAME)
  CATEGORY:=Skin system
  TITLE:=$(INTRO_STR)
  DEPENDS:=+PACKAGE_arch:arch +PACKAGE_land:land +PACKAGE_network:network
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef

# 安装到系统运行目录中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/Collect,$(1))
	# More installation instructions can be added here
	$(call Build/Install/Fpk,$(1),$(gSTORE_DIR))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

