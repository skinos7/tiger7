# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 定义项目基本信息
PKG_NAME:=tui
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
  DEPENDS:=+PACKAGE_arch:arch +PACKAGE_land:land +libevent2 +libreadline
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef

# 安装到系统运行目录中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/Collect,$(1))
	# ########## 在此可添加额外的安装指示: 开始 ########### # 
	# add this for hetui
	$(INSTALL_DIR) $(1)/bin
	$(LN) sline ${1}/bin/hetui
	$(INSTALL_DIR) ${FPK_ROOTFS_DIR}/bin
	$(LN) sline ${FPK_ROOTFS_DIR}/bin/hetui
	$(INSTALL_DIR) $(FPK_ROOTFS_DIR)
	$(INSTALL_DIR) $(FPK_ROOTFS_DIR)/etc/dropbear
	$(INSTALL_BIN) ./files/dropbear_dss_host_key $(FPK_ROOTFS_DIR)/etc/dropbear
	$(INSTALL_BIN) ./files/dropbear_rsa_host_key $(FPK_ROOTFS_DIR)/etc/dropbear
	# ########## 在此可添加额外的安装指示: 结束 ########### # 
	$(call Build/Install/Fpk,$(1))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

