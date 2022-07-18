# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt76x2
PKG_VERSION:=V3.0.4.0_P3_DPA_20161201
PKG_SOURCE:=MT76x2E_MT7620_LinuxAP_V3.0.4.0_P3_DPA_20161201.tar.bz2
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt76x2
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  MENU:=1
  TITLE:=MT76x2 wifi driver
  DEPENDS:=+mt76x2-scripts +wifi-l1profile +wifi_services
endef

define KernelPackage/mt76x2-in
  HIDDEN:=1
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MT76x2e BUILD_IN
  DEPENDS:=+mt76x2-scripts +wifi-l1profile +mtk-base-files +wifi_services
  FILES:=
  AUTOLOAD:=
  KCONFIG:= \
    CONFIG_WIFI_MT76x2E=y \
    CONFIG_MT76x2E_RALINK_MT76x2E=y \
    CONFIG_MT76x2E_RLT_WIFI=y \
    CONFIG_MT76x2E_MT_MAC=y \
    CONFIG_MT76x2E_RLT_AP_SUPPORT=y
endef

define KernelPackage/mt76x2-ko
  HIDDEN:=1
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MT76x2e BUILD_KO
  DEPENDS:=+mt76x2-scripts
  FILES:=$(LINUX_DIR)/drivers/net/wireless/mtk/mt76x2/rlt_wifi_ap/mt76x2.ko
  AUTOLOAD:=$(call AutoProbe,mt76x2)
  KCONFIG:= \
    CONFIG_WIFI_MT76x2E=y \
    CONFIG_MT76x2E_RALINK_MT76x2E=y \
    CONFIG_MT76x2E_RLT_WIFI=y \
    CONFIG_MT76x2E_MT_MAC=y \
    CONFIG_MT76x2E_RLT_AP_SUPPORT=m
endef

define KernelPackage/mt76x2/config
	if PACKAGE_kmod-mt76x2
	source "$(SOURCE)/config.in"
	endif
endef

define Build/Prepare
	@echo "this package is built with kernel"
endef

define Build/Compile
	@echo "this package is built with kernel"
endef

define Package/mt76x2-scripts
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=mt76x2 scripts
  DEPENDS:=+mtk-base-files
  HIDDEN:=1
endef

define Package/mt76x2-scripts/install
	$(INSTALL_DIR) $(1)/lib/wifi/
	-if [ "$$(CONFIG_WIFI_SCRIPT_LUA)" = "y" ]; then \
	$(INSTALL_BIN) ./files/mt7612e.lua $(1)/lib/wifi/; \
	else \
	$(INSTALL_BIN) ./files/mt7612e.sh $(1)/lib/wifi/; \
	fi
	$(INSTALL_DIR) $(1)/etc/wireless/mt7612e/
	if [ "$$(CONFIG_PACKAGE_libmapd)" = "y" ]; then \
	$(INSTALL_BIN) ./files/mt7612.map.dat $(1)/etc/wireless/mt7612e/mt7612e.dat; \
	else \
	$(INSTALL_BIN) ./files/mt7612e*.dat $(1)/etc/wireless/mt7612e/ ; \
	fi
	-$(INSTALL_BIN) ./files/mt7612e*.bin $(1)/etc/wireless/mt7612e/
	echo $(PKG_VERSION) > $(1)/etc/wireless/mt7612e/version
endef

define Package/mt76x2-scripts/conffiles
/etc/wireless/mt7612e/
endef

$(eval $(call KernelPackage,mt76x2))
$(eval $(call KernelPackage,mt76x2-ko))
$(eval $(call KernelPackage,mt76x2-in))
$(eval $(call BuildPackage,mt76x2-scripts))
