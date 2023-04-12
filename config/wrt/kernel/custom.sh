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


# watchdog custom
src2kernel arch_mips_ralink_makefile ${LINUX_DIR}/arch/mips/ralink/Makefile
src2kernel hard_wdt_timer.c ${LINUX_DIR}/arch/mips/ralink/
src2kernel main.c ${LINUX_DIR}/init/
src2kernel hard_wdt.c ${LINUX_DIR}/drivers/watchdog/
src2kernel drivers_watchdog_makefile ${LINUX_DIR}/drivers/watchdog/Makefile
src2kernel module.c ${LINUX_DIR}/kernel/module.c

# mtd parts parse
src2kernel ofpart_core.c ${LINUX_DIR}/drivers/mtd/parsers/
# winbond W25Q512JV
src2kernel winbond.c ${LINUX_DIR}/drivers/mtd/spi-nor/

# gpio base bus number
src2kernel gpio-mt7621.c ${LINUX_DIR}/drivers/gpio/

# esw phy led configure
src2kernel esw_rt3050.c ${LINUX_DIR}/drivers/net/ethernet/ralink/

# usb
src2kernel serial.h ${LINUX_DIR}/include/linux/usb/
src2kernel generic.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel usb-serial.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel option.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel usb_wwan.c ${LINUX_DIR}/drivers/usb/serial/
src2kernel qmi_wwan.c ${LINUX_DIR}/drivers/net/usb/

