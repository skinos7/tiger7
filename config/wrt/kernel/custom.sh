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


#pcie dummy support


#mt7628 ethernet driver fixed


#mt7621 mt7628 gpio base 0 fixed
src2kernel gpio-mt7621.c ${LINUX_DIR}/drivers/gpio

# early ttySx


# usb driver custom



