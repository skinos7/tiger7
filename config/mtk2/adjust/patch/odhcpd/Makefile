#
# Copyright (C) 2013-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odhcpd
PKG_RELEASE:=1

# modify by dimmalex for from dl
PKG_SOURCE:=odhcpd-2023.10.24~d8118f6e.tar.xz
PKG_BUILD_DIR:=$(BUILD_DIR)/odhcpd-2023.10.24~d8118f6e
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL=$(PROJECT_GIT)/project/odhcpd.git
#PKG_MIRROR_HASH:=59d04d80175a115d04031291195dcb2ce858301c78bd37f18aeabfa40f750ef8
#PKG_SOURCE_DATE:=2023-10-24
#PKG_SOURCE_VERSION:=d8118f6e76e5519881f9a37137c3a06b3cb60fd2
#PKG_MAINTAINER:=Hans Dedecker <dedeckeh@gmail.com>
#PKG_LICENSE:=GPL-2.0

PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_odhcpd_$(BUILD_VARIANT)_ext_cer_id
PKG_ASLR_PIE_REGULAR:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/odhcpd/default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt DHCPv6(-PD)/RA Server & Relay
  DEPENDS:=+libubox +libuci +libubus +libnl-tiny
endef

define Package/odhcpd/default/description
 odhcpd is a daemon for serving and relaying IP management protocols to
 configure clients and downstream routers. It tries to follow the RFC 6204
 requirements for IPv6 home routers.
endef

define Package/odhcpd/default/config
menu "Configuration"
	depends on PACKAGE_$(1)

config PACKAGE_odhcpd_$(2)_ext_cer_id
	int
	default 0
	prompt "CER-ID Extension ID (0 = disabled)"
endmenu
endef

define Package/odhcpd
  $(call Package/odhcpd/default)
  TITLE += and DHCPv4 server
  VARIANT:=full
endef

Package/odhcpd/config=$(call Package/odhcpd/default/config,odhcpd,full)

define Package/odhcpd/description
 $(call Package/odhcpd/default/description)

 This is a variant providing server services for DHCPv4, RA, stateless and
 stateful DHCPv6,  prefix delegation and can be used to relay RA, DHCPv6 and
 NDP between routed (non-bridged) interfaces in case no delegated prefixes
 are available.
endef

define Package/odhcpd-ipv6only
  $(call Package/odhcpd/default)
  VARIANT:=ipv6only
  DEPENDS+= @IPV6
endef

Package/odhcpd-ipv6only/config=$(call Package/odhcpd/default/config,odhcpd-ipv6only,ipv6only)

define Package/odhcpd-ipv6only/description
 $(call Package/odhcpd/default/description)

 This is a variant providing server services for RA, stateless and stateful
 DHCPv6,  prefix delegation and can be used to relay RA, DHCPv6 and NDP between
 routed (non-bridged) interfaces in case no delegated prefixes are available.
endef

# comment by dimmalex for no ubus
#CMAKE_OPTIONS += -DUBUS=1

ifeq ($(BUILD_VARIANT),full)
  CMAKE_OPTIONS += -DDHCPV4_SUPPORT=1
endif

ifneq ($(CONFIG_PACKAGE_odhcpd_$(BUILD_VARIANT)_ext_cer_id),0)
  CMAKE_OPTIONS += -DEXT_CER_ID=$(CONFIG_PACKAGE_odhcpd_$(BUILD_VARIANT)_ext_cer_id)
endif

define Package/odhcpd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/odhcpd $(1)/usr/sbin/
	# comment by dimmalex
	#$(INSTALL_BIN) ./files/odhcpd-update $(1)/usr/sbin/
	#$(INSTALL_DIR) $(1)/etc/init.d
	#$(INSTALL_BIN) ./files/odhcpd.init $(1)/etc/init.d/odhcpd
	#$(INSTALL_DIR) $(1)/etc/uci-defaults
	#$(INSTALL_BIN) ./files/odhcpd.defaults $(1)/etc/uci-defaults/15_odhcpd
endef

Package/odhcpd-ipv6only/install = $(Package/odhcpd/install)

$(eval $(call BuildPackage,odhcpd))
$(eval $(call BuildPackage,odhcpd-ipv6only))
