#
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odhcp6c
PKG_RELEASE:=20

# modify by dimmalex for from dl
PKG_SOURCE:=odhcp6c-2023.05.12~bcd28363.tar.xz
PKG_BUILD_DIR:=$(BUILD_DIR)/odhcp6c-2023.05.12~bcd28363
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL=$(PROJECT_GIT)/project/odhcp6c.git
#PKG_SOURCE_DATE:=2023-05-12
#PKG_SOURCE_VERSION:=bcd283632ac13391aac3ebdd074d1fd832d76fa3
#PKG_MIRROR_HASH:=21aac76fcf4099287868afb52924ed2219e8e846ad82b96b7e1753417a8dde87
#PKG_MAINTAINER:=Hans Dedecker <dedeckeh@gmail.com>
#PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += \
	-DUSE_LIBUBOX=on

ifneq ($(CONFIG_PACKAGE_odhcp6c_ext_cer_id),0)
  CMAKE_OPTIONS += -DEXT_CER_ID=$(CONFIG_PACKAGE_odhcp6c_ext_cer_id)
endif

define Package/odhcp6c
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Embedded DHCPv6-client for OpenWrt
  DEPENDS:=@IPV6 +libubox +libnl-tiny 
endef

define Package/odhcp6c/config
  config PACKAGE_odhcp6c_ext_cer_id
    int "CER-ID Extension ID (0 = disabled)"
    depends on PACKAGE_odhcp6c
    default 0
endef

define Package/odhcp6c/conffiles
/etc/odhcp6c.user
/etc/odhcp6c.user.d/
endef

define Package/odhcp6c/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/odhcp6c $(1)/usr/sbin/
	# comment by dimmalex
	#$(INSTALL_DIR) $(1)/lib/netifd/proto
	#$(INSTALL_BIN) ./files/dhcpv6.sh $(1)/lib/netifd/proto/dhcpv6.sh
	#$(INSTALL_BIN) ./files/dhcpv6.script $(1)/lib/netifd/
	#$(INSTALL_DIR) $(1)/etc/odhcp6c.user.d/
	#$(INSTALL_CONF) ./files/odhcp6c.user $(1)/etc/
endef

$(eval $(call BuildPackage,odhcp6c))
