# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 定义项目基本信息
PKG_NAME:=clock
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

# 包含Openwrt的一些有关包处理的函数
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk
include $(gSKIN_MKFILE)
# 生成项目通用定义
$(eval $(call Package/Define))

# menuconfig的选项的位置及说明
define Package/$(PKG_NAME)
  CATEGORY:=Skin system
  TITLE:=$(INTRO_STR)
  DEPENDS:=+PACKAGE_arch:arch +PACKAGE_land:land +libevent2
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef

# 安装到系统运行目录中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/Collect,$(1))
	# ########## 在此可添加额外的安装指示: 开始 ########### # 
	# ########## 在此可添加额外的安装指示: 结束 ########### # 
	$(call Build/Install/Fpk,$(1))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

