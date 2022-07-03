
###################################################################
###################################################################
################## Rom Library Function ###########################
###################################################################
###################################################################

# Lay目录下的文件安装, 在openwrt/include/image.mk中调用
define Lay/install
	cd ${gTOP_DIR}; make lay_install
endef
# Build目录下的文件的安装, 在openwrt/include/image.mk中调用
define Build/install
	cp -fdRp ${gosROOT_DIR}/* $(1); \
	cd $(1);if [ -f needless.sh ]; then \
		chmod a+rwx needless.sh;./needless.sh; \
	fi
endef

