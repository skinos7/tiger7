#!/bin/bash

src2kernel()
{
    if [ -e "${gSCOPE_DIR}/kernel/${1}" ]; then
        echo "cp ${gSCOPE_DIR}/kernel/${1} ${2}"
        cp -fr ${gSCOPE_DIR}/kernel/${1} ${2}
    elif [ -e "${gCUSTOM_DIR}/kernel/${1}" ]; then
        echo "cp ${gCUSTOM_DIR}/kernel/${1} ${2}"
        cp -fr ${gCUSTOM_DIR}/kernel/${1} ${2}
    elif [ -e "${gHARDWARE_DIR}/kernel/${1}" ]; then
        echo "cp ${gHARDWARE_DIR}/kernel/${1} ${2}"
        cp -fr ${gHARDWARE_DIR}/kernel/${1} ${2}
    elif [ -e "${gPLATFORM_DIR}/kernel/${1}" ]; then
        echo "cp ${gPLATFORM_DIR}/kernel/${1} ${2}"
        cp -fr ${gPLATFORM_DIR}/kernel/${1} ${2}
    fi
}


# Fconfig mtd create
mv ${LINUX_DIR}/drivers/mtd/mtdpart.c ${LINUX_DIR}/drivers/mtd/mtdpart.c.bak
mv ${LINUX_DIR}/drivers/mtd/ralink/ralink_bbu_spi.c ${LINUX_DIR}/drivers/mtd/ralink/ralink_bbu_spi.c.bak
src2kernel mtdpart.c ${LINUX_DIR}/drivers/mtd/
src2kernel ralink_bbu_spi.c ${LINUX_DIR}/drivers/mtd/ralink/

# watchdog
src2kernel ralink_wdt_timer.c ${LINUX_DIR}/arch/mips/ralink/
src2kernel arch_ralink_makefile ${LINUX_DIR}/arch/mips/ralink/Makefile
mv ${LINUX_DIR}/init/main.c ${LINUX_DIR}/init/main.c.bak
src2kernel main.c ${LINUX_DIR}/init/
src2kernel drivers_watchdog_makefile ${LINUX_DIR}/drivers/watchdog/Makefile
src2kernel hardware_wdt.c ${LINUX_DIR}/drivers/watchdog/

# i2c alog bus 
src2kernel dev-i2c-bus.c ${LINUX_DIR}/arch/mips/ralink/
src2kernel i2c-slave.c ${LINUX_DIR}/arch/mips/ralink/

# ralink pci reset don't use uart3 lines
src2kernel pci.c ${LINUX_DIR}/arch/mips/ralink/

# ralink no gpio operation
mv ${LINUX_DIR}/drivers/char/ralink_gpio.c ${LINUX_DIR}/drivers/char/ralink_gpio.c.bak
src2kernel ralink_gpio.c ${LINUX_DIR}/drivers/char/

# hw nat add paramter for usb fix hw_nat for br-lan to lan, eth2 to eth0.2 or usb0
src2kernel ra_nat.c ${LINUX_DIR}/net/nat/hw_nat/

# add the hook for hw_nat
src2kernel usbnet.c ${LINUX_DIR}/drivers/net/usb/

# usb
src2kernel serial.h ${LINUX_DIR}/include/linux/usb/
src2kernel generic.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel usb-serial.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel option.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel usb_wwan.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel qmi_wwan.c ${LINUX_DIR}/drivers/net/usb/

# no print
src2kernel init.c ${LINUX_DIR}/arch/mips/ralink/
src2kernel cmdline.c ${LINUX_DIR}/arch/mips/ralink/

# ralink ethernet comment gpio operator
src2kernel raether.c ${LINUX_DIR}/drivers/net/raeth/

# no ralink gpio driver irq
src2kernel irq.c ${LINUX_DIR}/arch/mips/ralink/

# sdcard gpio mode fix
src2kernel sd.c ${LINUX_DIR}/drivers/mmc/host/mtk-mmc/
src2kernel slot-gpio.c ${LINUX_DIR}/drivers/mmc/core/

# gpiolib and irq domain
src2kernel arch_mips_Kconfig ${LINUX_DIR}/arch/mips/Kconfig

# compat driver 
src2kernel ath9k_platform.h ${LINUX_DIR}/include/linux/

