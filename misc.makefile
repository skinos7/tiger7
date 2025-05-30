
# Ubuntu compile tools install
preset:
	sudo apt-get update
	sudo apt-get install -y vim git subversion pandoc aptitude
	sudo apt-get install -y gcc g++ binutils patch bzip2 flex bison make autoconf gettext texinfo unzip sharutils gawk
	sudo apt-get install -y python2 libtool shtool curl lrzsz zlib1g-dev automake automake1.11
	sudo apt-get install -y lib32z1 libssl-dev device-tree-compiler libevent-dev libreadline-dev
	sudo apt-get install -y snapd
	sudo snap install ttyd --classic
	-sudo apt-get install -y lib32ncurses5
	-sudo apt-get install -y libncurses5-dev
	-sudo apt-get install -y libncursesw5-dev
	# Disable Check the SSL certificate of the server
	git config --global http.sslVerify false
.PHONY: preset

# SDK Download
update:
	# Update the sdk
	git pull
	# Update the Platform-specific configure repositories
	if [ -d ${gPLATFORM_DIR} ]; then \
		echo "cd ${gPLATFORM_DIR}; git pull"; \
	else \
		echo "cd ${gTOP_DIR}/config; git clone git@gitee.com:ashyelf/${gPLATFORM}"; \
	fi
	# Forward to Platform-specific SDK Makefile update
	make -f target.makefile sdk_update
	# Adjust the Platform-specific SDK
	make -f target.makefile sdk_adjust
	# Refresh the menu
	make -f target.makefile sdk_menu

# SDK rebuild for HOST
rebuild:
	make sdk_stop;make;make sdk_install;make sdk_start
# SDK help for smtk2/smtk3
menu:
	make sdk_menu
menuconfig:
	make sdk_menuconfig

tftp:
	make sdk_tftp
ftp:
	make sdk_ftp
sz:
	make sdk_sz

stop:
	make sdk_stop
start:
	make sdk_start
install:
	make sdk_install

.PHONY: update rebuild menu menuconfig tftp ftp sz stop start install
