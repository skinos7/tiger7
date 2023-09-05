# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt7663_603
PKG_VERSION:=gerrit
PKG_SOURCE:=MT7663_LinuxAP_6.0.3.0_20200106-775e6a.tar.bz2

include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt7663v603
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  MENU:=1
  TITLE:=MT7663_603 wifi driver
  DEPENDS:=+mt7663-scripts +wifi-l1profile +mtk-base-files +wifi_services
endef

define KernelPackage/mt7663v603-in
  HIDDEN:=1
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MT7663e ver603 BUILD_IN
  DEPENDS:=+mt7663-scripts
  FILES:=
  AUTOLOAD:=
  KCONFIG:= \
    CONFIG_WIFI_MT7663E_603=y \
    CONFIG_MT7663E_MT_MAC=y \
    CONFIG_MT7663E_MT_WIFI=y \
    CONFIG_MT7663E_MT_AP_SUPPORT=y \
    CONFIG_MT7663E_WIFI_MODE_AP=y
endef

define KernelPackage/mt7663v603-ko
  HIDDEN:=1
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MT7663e ver603 BUILD_KO
  DEPENDS:=+mt7663-scripts
  FILES:=$(LINUX_DIR)/drivers/net/wireless/mtk/mt7663_603/mt_wifi_ap/mt7663_603.ko
  AUTOLOAD:=$(call AutoProbe,mt7663_603)
  KCONFIG:= \
    CONFIG_WIFI_MT7663E_603=y \
    CONFIG_MT7663E_MT_MAC=y \
    CONFIG_MT7663E_MT_WIFI=m \
    CONFIG_MT7663E_MT_AP_SUPPORT=m \
    CONFIG_MT7663E_WIFI_MODE_AP=m
endef

define KernelPackage/mt7663v603/config
	if PACKAGE_kmod-mt7663v603
	source "$(SOURCE)/config.in"
	endif
endef

define Build/Prepare
	@echo "this package is built with kernel"
endef

define Build/Compile
	@echo "this package is built with kernel"
endef

define Package/mt7663v603-scripts
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=mt7663 scripts
  DEPENDS:=+mtk-base-files
  HIDDEN:=1
endef

define Package/mt7663v603-scripts/install
	$(INSTALL_DIR) $(1)/lib/wifi/
	$(INSTALL_DIR) $(1)/etc/wireless/mt7663/
	-if [ "$$(CONFIG_WIFI_SCRIPT_LUA)" = "y" ]; then \
	$(INSTALL_BIN) ./files/mt7663.lua $(1)/lib/wifi/; \
	else \
	$(INSTALL_BIN) ./files/mt7663.sh $(1)/lib/wifi/; \
	fi
	echo $(PKG_VERSION) > $(1)/etc/wireless/mt7663/version

	if [ "$$(CONFIG_KCONFIG_FIRST_IF_MT7663E)" = "y" ]; then \
		$(INSTALL_BIN) ./files/mt7663*.bin $(1)/etc/wireless/mt7663/; \
		$(INSTALL_BIN) ./files/mt7663-sku.dat $(1)/etc/wireless/mt7663/; \
		$(INSTALL_BIN) ./files/mt7663-sku-bf.dat $(1)/etc/wireless/mt7663/; \
		if [ "$$(CONFIG_PACKAGE_libmapd)" = "y" ]; then \
			$(INSTALL_BIN) ./files/mt7663.1.map.dat $(1)/etc/wireless/mt7663/mt7663.1.dat; \
		else \
			$(INSTALL_BIN) ./files/mt7663.1.dat $(1)/etc/wireless/mt7663/; \
		fi \
	fi

	if [ "$$(CONFIG_KCONFIG_SECOND_IF_MT7663E)" = "y" ]; then \
		$(INSTALL_BIN) ./files/mt7663*.bin $(1)/etc/wireless/mt7663/; \
		$(INSTALL_BIN) ./files/mt7663-sku.dat $(1)/etc/wireless/mt7663/; \
		$(INSTALL_BIN) ./files/mt7663-sku-bf.dat $(1)/etc/wireless/mt7663/; \
		if [ "$$(CONFIG_PACKAGE_libmapd)" = "y" ]; then \
			$(INSTALL_BIN) ./files/mt7663.2.map.dat $(1)/etc/wireless/mt7663/mt7663.2.dat; \
		else \
			$(INSTALL_BIN) ./files/mt7663.2.dat $(1)/etc/wireless/mt7663/; \
		fi \
	fi
endef

define Package/mt7663-scripts/conffiles
/etc/wireless/mt7663/
endef

$(eval $(call KernelPackage,mt7663v603))
$(eval $(call KernelPackage,mt7663v603-ko))
$(eval $(call KernelPackage,mt7663v603-in))
$(eval $(call BuildPackage,mt7663v603-scripts))
