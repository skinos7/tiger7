/*
  USB Driver for GSM modems

  Copyright (C) 2005  Matthias Urlichs <smurf@smurf.noris.de>

  This driver is free software; you can redistribute it and/or modify
  it under the terms of Version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  Portions copied from the Keyspan driver by Hugh Blemings <hugh@blemings.org>

  History: see the git log.

  Work sponsored by: Sigos GmbH, Germany <info@sigos.de>

  This driver exists because the "normal" serial driver doesn't work too well
  with GSM modems. Issues:
  - data loss -- one single Receive URB is not nearly enough
  - nonstandard flow (Option devices) control
  - controlling the baud rate doesn't make sense

  This driver is named "option" because the most common device it's
  used for is a PC-Card (with an internal OHCI-USB interface, behind
  which the GSM interface sits), made by Option Inc.

  Some of the "one port" devices actually exhibit multiple USB instances
  on the USB bus. This is not a bug, these ports are used for different
  device features.
*/

#define DRIVER_AUTHOR "Matthias Urlichs <smurf@smurf.noris.de>"
#define DRIVER_DESC "USB Driver for GSM modems"

#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/bitops.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include "usb-wwan.h"

/* Function prototypes */
static int  option_probe(struct usb_serial *serial,
			const struct usb_device_id *id);
static int option_attach(struct usb_serial *serial);
static void option_release(struct usb_serial *serial);
static int option_send_setup(struct usb_serial_port *port);
static void option_instat_callback(struct urb *urb);

/* Vendor and product IDs */
#define OPTION_VENDOR_ID			0x0AF0
#define OPTION_PRODUCT_COLT			0x5000
#define OPTION_PRODUCT_RICOLA			0x6000
#define OPTION_PRODUCT_RICOLA_LIGHT		0x6100
#define OPTION_PRODUCT_RICOLA_QUAD		0x6200
#define OPTION_PRODUCT_RICOLA_QUAD_LIGHT	0x6300
#define OPTION_PRODUCT_RICOLA_NDIS		0x6050
#define OPTION_PRODUCT_RICOLA_NDIS_LIGHT	0x6150
#define OPTION_PRODUCT_RICOLA_NDIS_QUAD		0x6250
#define OPTION_PRODUCT_RICOLA_NDIS_QUAD_LIGHT	0x6350
#define OPTION_PRODUCT_COBRA			0x6500
#define OPTION_PRODUCT_COBRA_BUS		0x6501
#define OPTION_PRODUCT_VIPER			0x6600
#define OPTION_PRODUCT_VIPER_BUS		0x6601
#define OPTION_PRODUCT_GT_MAX_READY		0x6701
#define OPTION_PRODUCT_FUJI_MODEM_LIGHT		0x6721
#define OPTION_PRODUCT_FUJI_MODEM_GT		0x6741
#define OPTION_PRODUCT_FUJI_MODEM_EX		0x6761
#define OPTION_PRODUCT_KOI_MODEM		0x6800
#define OPTION_PRODUCT_SCORPION_MODEM		0x6901
#define OPTION_PRODUCT_ETNA_MODEM		0x7001
#define OPTION_PRODUCT_ETNA_MODEM_LITE		0x7021
#define OPTION_PRODUCT_ETNA_MODEM_GT		0x7041
#define OPTION_PRODUCT_ETNA_MODEM_EX		0x7061
#define OPTION_PRODUCT_ETNA_KOI_MODEM		0x7100
#define OPTION_PRODUCT_GTM380_MODEM		0x7201

#define HUAWEI_VENDOR_ID			0x12D1
#define HUAWEI_PRODUCT_E173			0x140C
#define HUAWEI_PRODUCT_E1750			0x1406
#define HUAWEI_PRODUCT_K4505			0x1464
#define HUAWEI_PRODUCT_K3765			0x1465
#define HUAWEI_PRODUCT_K4605			0x14C6
#define HUAWEI_PRODUCT_E173S6			0x1C07

#define QUANTA_VENDOR_ID			0x0408
#define QUANTA_PRODUCT_Q101			0xEA02
#define QUANTA_PRODUCT_Q111			0xEA03
#define QUANTA_PRODUCT_GLX			0xEA04
#define QUANTA_PRODUCT_GKE			0xEA05
#define QUANTA_PRODUCT_GLE			0xEA06

#define NOVATELWIRELESS_VENDOR_ID		0x1410

/* YISO PRODUCTS */

#define YISO_VENDOR_ID				0x0EAB
#define YISO_PRODUCT_U893			0xC893

/*
 * NOVATEL WIRELESS PRODUCTS
 *
 * Note from Novatel Wireless:
 * If your Novatel modem does not work on linux, don't
 * change the option module, but check our website. If
 * that does not help, contact ddeschepper@nvtl.com
*/
/* MERLIN EVDO PRODUCTS */
#define NOVATELWIRELESS_PRODUCT_V640		0x1100
#define NOVATELWIRELESS_PRODUCT_V620		0x1110
#define NOVATELWIRELESS_PRODUCT_V740		0x1120
#define NOVATELWIRELESS_PRODUCT_V720		0x1130

/* MERLIN HSDPA/HSPA PRODUCTS */
#define NOVATELWIRELESS_PRODUCT_U730		0x1400
#define NOVATELWIRELESS_PRODUCT_U740		0x1410
#define NOVATELWIRELESS_PRODUCT_U870		0x1420
#define NOVATELWIRELESS_PRODUCT_XU870		0x1430
#define NOVATELWIRELESS_PRODUCT_X950D		0x1450

/* EXPEDITE PRODUCTS */
#define NOVATELWIRELESS_PRODUCT_EV620		0x2100
#define NOVATELWIRELESS_PRODUCT_ES720		0x2110
#define NOVATELWIRELESS_PRODUCT_E725		0x2120
#define NOVATELWIRELESS_PRODUCT_ES620		0x2130
#define NOVATELWIRELESS_PRODUCT_EU730		0x2400
#define NOVATELWIRELESS_PRODUCT_EU740		0x2410
#define NOVATELWIRELESS_PRODUCT_EU870D		0x2420
/* OVATION PRODUCTS */
#define NOVATELWIRELESS_PRODUCT_MC727		0x4100
#define NOVATELWIRELESS_PRODUCT_MC950D		0x4400
/*
 * Note from Novatel Wireless:
 * All PID in the 5xxx range are currently reserved for
 * auto-install CDROMs, and should not be added to this
 * module.
 *
 * #define NOVATELWIRELESS_PRODUCT_U727		0x5010
 * #define NOVATELWIRELESS_PRODUCT_MC727_NEW	0x5100
*/
#define NOVATELWIRELESS_PRODUCT_OVMC760		0x6002
#define NOVATELWIRELESS_PRODUCT_MC780		0x6010
#define NOVATELWIRELESS_PRODUCT_EVDO_FULLSPEED	0x6000
#define NOVATELWIRELESS_PRODUCT_EVDO_HIGHSPEED	0x6001
#define NOVATELWIRELESS_PRODUCT_HSPA_FULLSPEED	0x7000
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED	0x7001
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED3	0x7003
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED4	0x7004
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED5	0x7005
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED6	0x7006
#define NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED7	0x7007
#define NOVATELWIRELESS_PRODUCT_MC996D		0x7030
#define NOVATELWIRELESS_PRODUCT_MF3470		0x7041
#define NOVATELWIRELESS_PRODUCT_MC547		0x7042
#define NOVATELWIRELESS_PRODUCT_EVDO_EMBEDDED_FULLSPEED	0x8000
#define NOVATELWIRELESS_PRODUCT_EVDO_EMBEDDED_HIGHSPEED	0x8001
#define NOVATELWIRELESS_PRODUCT_HSPA_EMBEDDED_FULLSPEED	0x9000
#define NOVATELWIRELESS_PRODUCT_HSPA_EMBEDDED_HIGHSPEED	0x9001
#define NOVATELWIRELESS_PRODUCT_E362		0x9010
#define NOVATELWIRELESS_PRODUCT_E371		0x9011
#define NOVATELWIRELESS_PRODUCT_U620L		0x9022
#define NOVATELWIRELESS_PRODUCT_G2		0xA010
#define NOVATELWIRELESS_PRODUCT_MC551		0xB001

/* AMOI PRODUCTS */
#define AMOI_VENDOR_ID				0x1614
#define AMOI_PRODUCT_H01			0x0800
#define AMOI_PRODUCT_H01A			0x7002
#define AMOI_PRODUCT_H02			0x0802
#define AMOI_PRODUCT_SKYPEPHONE_S2		0x0407

#define DELL_VENDOR_ID				0x413C

/* Dell modems */
#define DELL_PRODUCT_5700_MINICARD		0x8114
#define DELL_PRODUCT_5500_MINICARD		0x8115
#define DELL_PRODUCT_5505_MINICARD		0x8116
#define DELL_PRODUCT_5700_EXPRESSCARD		0x8117
#define DELL_PRODUCT_5510_EXPRESSCARD		0x8118

#define DELL_PRODUCT_5700_MINICARD_SPRINT	0x8128
#define DELL_PRODUCT_5700_MINICARD_TELUS	0x8129

#define DELL_PRODUCT_5720_MINICARD_VZW		0x8133
#define DELL_PRODUCT_5720_MINICARD_SPRINT	0x8134
#define DELL_PRODUCT_5720_MINICARD_TELUS	0x8135
#define DELL_PRODUCT_5520_MINICARD_CINGULAR	0x8136
#define DELL_PRODUCT_5520_MINICARD_GENERIC_L	0x8137
#define DELL_PRODUCT_5520_MINICARD_GENERIC_I	0x8138

#define DELL_PRODUCT_5730_MINICARD_SPRINT	0x8180
#define DELL_PRODUCT_5730_MINICARD_TELUS	0x8181
#define DELL_PRODUCT_5730_MINICARD_VZW		0x8182

#define DELL_PRODUCT_5800_MINICARD_VZW		0x8195  /* Novatel E362 */
#define DELL_PRODUCT_5800_V2_MINICARD_VZW	0x8196  /* Novatel E362 */
#define DELL_PRODUCT_5804_MINICARD_ATT		0x819b  /* Novatel E371 */

#define KYOCERA_VENDOR_ID			0x0c88
#define KYOCERA_PRODUCT_KPC650			0x17da
#define KYOCERA_PRODUCT_KPC680			0x180a

#define ANYDATA_VENDOR_ID			0x16d5
#define ANYDATA_PRODUCT_ADU_620UW		0x6202
#define ANYDATA_PRODUCT_ADU_E100A		0x6501
#define ANYDATA_PRODUCT_ADU_500A		0x6502

#define AXESSTEL_VENDOR_ID			0x1726
#define AXESSTEL_PRODUCT_MV110H			0x1000

#define BANDRICH_VENDOR_ID			0x1A8D
#define BANDRICH_PRODUCT_C100_1			0x1002
#define BANDRICH_PRODUCT_C100_2			0x1003
#define BANDRICH_PRODUCT_1004			0x1004
#define BANDRICH_PRODUCT_1005			0x1005
#define BANDRICH_PRODUCT_1006			0x1006
#define BANDRICH_PRODUCT_1007			0x1007
#define BANDRICH_PRODUCT_1008			0x1008
#define BANDRICH_PRODUCT_1009			0x1009
#define BANDRICH_PRODUCT_100A			0x100a

#define BANDRICH_PRODUCT_100B			0x100b
#define BANDRICH_PRODUCT_100C			0x100c
#define BANDRICH_PRODUCT_100D			0x100d
#define BANDRICH_PRODUCT_100E			0x100e

#define BANDRICH_PRODUCT_100F			0x100f
#define BANDRICH_PRODUCT_1010			0x1010
#define BANDRICH_PRODUCT_1011			0x1011
#define BANDRICH_PRODUCT_1012			0x1012

#define QUALCOMM_VENDOR_ID			0x05C6

#define CMOTECH_VENDOR_ID			0x16d8
#define CMOTECH_PRODUCT_6001			0x6001
#define CMOTECH_PRODUCT_CMU_300			0x6002
#define CMOTECH_PRODUCT_6003			0x6003
#define CMOTECH_PRODUCT_6004			0x6004
#define CMOTECH_PRODUCT_6005			0x6005
#define CMOTECH_PRODUCT_CGU_628A		0x6006
#define CMOTECH_PRODUCT_CHE_628S		0x6007
#define CMOTECH_PRODUCT_CMU_301			0x6008
#define CMOTECH_PRODUCT_CHU_628			0x6280
#define CMOTECH_PRODUCT_CHU_628S		0x6281
#define CMOTECH_PRODUCT_CDU_680			0x6803
#define CMOTECH_PRODUCT_CDU_685A		0x6804
#define CMOTECH_PRODUCT_CHU_720S		0x7001
#define CMOTECH_PRODUCT_7002			0x7002
#define CMOTECH_PRODUCT_CHU_629K		0x7003
#define CMOTECH_PRODUCT_7004			0x7004
#define CMOTECH_PRODUCT_7005			0x7005
#define CMOTECH_PRODUCT_CGU_629			0x7006
#define CMOTECH_PRODUCT_CHU_629S		0x700a
#define CMOTECH_PRODUCT_CHU_720I		0x7211
#define CMOTECH_PRODUCT_7212			0x7212
#define CMOTECH_PRODUCT_7213			0x7213
#define CMOTECH_PRODUCT_7251			0x7251
#define CMOTECH_PRODUCT_7252			0x7252
#define CMOTECH_PRODUCT_7253			0x7253

#define TELIT_VENDOR_ID				0x1bc7
#define TELIT_PRODUCT_UC864E			0x1003
#define TELIT_PRODUCT_UC864G			0x1004
#define TELIT_PRODUCT_CC864_DUAL		0x1005
#define TELIT_PRODUCT_CC864_SINGLE		0x1006
#define TELIT_PRODUCT_DE910_DUAL		0x1010
#define TELIT_PRODUCT_UE910_V2			0x1012
#define TELIT_PRODUCT_LE922_USBCFG0		0x1042
#define TELIT_PRODUCT_LE922_USBCFG3		0x1043
#define TELIT_PRODUCT_LE922_USBCFG5		0x1045
#define TELIT_PRODUCT_LE920			0x1200
#define TELIT_PRODUCT_LE910			0x1201
#define TELIT_PRODUCT_LE910_USBCFG4		0x1206

/* ZTE PRODUCTS */
#define ZTE_VENDOR_ID				0x19d2
#define ZTE_PRODUCT_MF622			0x0001
#define ZTE_PRODUCT_MF628			0x0015
#define ZTE_PRODUCT_MF626			0x0031
#define ZTE_PRODUCT_ZM8620_X			0x0396
#define ZTE_PRODUCT_ME3620_MBIM			0x0426
#define ZTE_PRODUCT_ME3620_X			0x1432
#define ZTE_PRODUCT_ME3620_L			0x1433
#define ZTE_PRODUCT_AC2726			0xfff1
#define ZTE_PRODUCT_CDMA_TECH			0xfffe
#define ZTE_PRODUCT_AC8710T			0xffff
#define ZTE_PRODUCT_MC2718			0xffe8
#define ZTE_PRODUCT_AD3812			0xffeb
#define ZTE_PRODUCT_MC2716			0xffed

#define BENQ_VENDOR_ID				0x04a5
#define BENQ_PRODUCT_H10			0x4068

#define DLINK_VENDOR_ID				0x1186
#define DLINK_PRODUCT_DWM_652			0x3e04
#define DLINK_PRODUCT_DWM_652_U5		0xce16
#define DLINK_PRODUCT_DWM_652_U5A		0xce1e

#define QISDA_VENDOR_ID				0x1da5
#define QISDA_PRODUCT_H21_4512			0x4512
#define QISDA_PRODUCT_H21_4523			0x4523
#define QISDA_PRODUCT_H20_4515			0x4515
#define QISDA_PRODUCT_H20_4518			0x4518
#define QISDA_PRODUCT_H20_4519			0x4519

/* TLAYTECH PRODUCTS */
#define TLAYTECH_VENDOR_ID			0x20B9
#define TLAYTECH_PRODUCT_TEU800			0x1682

/* TOSHIBA PRODUCTS */
#define TOSHIBA_VENDOR_ID			0x0930
#define TOSHIBA_PRODUCT_HSDPA_MINICARD		0x1302
#define TOSHIBA_PRODUCT_G450			0x0d45

#define ALINK_VENDOR_ID				0x1e0e
#define SIMCOM_PRODUCT_SIM7100E			0x9001 /* Yes, ALINK_VENDOR_ID */
#define ALINK_PRODUCT_PH300			0x9100
#define ALINK_PRODUCT_3GU			0x9200

/* ALCATEL PRODUCTS */
#define ALCATEL_VENDOR_ID			0x1bbb
#define ALCATEL_PRODUCT_X060S_X200		0x0000
#define ALCATEL_PRODUCT_X220_X500D		0x0017
#define ALCATEL_PRODUCT_L100V			0x011e
#define ALCATEL_PRODUCT_L800MA			0x0203

#define PIRELLI_VENDOR_ID			0x1266
#define PIRELLI_PRODUCT_C100_1			0x1002
#define PIRELLI_PRODUCT_C100_2			0x1003
#define PIRELLI_PRODUCT_1004			0x1004
#define PIRELLI_PRODUCT_1005			0x1005
#define PIRELLI_PRODUCT_1006			0x1006
#define PIRELLI_PRODUCT_1007			0x1007
#define PIRELLI_PRODUCT_1008			0x1008
#define PIRELLI_PRODUCT_1009			0x1009
#define PIRELLI_PRODUCT_100A			0x100a
#define PIRELLI_PRODUCT_100B			0x100b
#define PIRELLI_PRODUCT_100C			0x100c
#define PIRELLI_PRODUCT_100D			0x100d
#define PIRELLI_PRODUCT_100E			0x100e
#define PIRELLI_PRODUCT_100F			0x100f
#define PIRELLI_PRODUCT_1011			0x1011
#define PIRELLI_PRODUCT_1012			0x1012

/* Airplus products */
#define AIRPLUS_VENDOR_ID			0x1011
#define AIRPLUS_PRODUCT_MCD650			0x3198

/* Longcheer/Longsung vendor ID; makes whitelabel devices that
 * many other vendors like 4G Systems, Alcatel, ChinaBird,
 * Mobidata, etc sell under their own brand names.
 */
#define LONGCHEER_VENDOR_ID			0x1c9e

/* 4G Systems products */
/* This is the 4G XS Stick W14 a.k.a. Mobilcom Debitel Surf-Stick *
 * It seems to contain a Qualcomm QSC6240/6290 chipset            */
#define FOUR_G_SYSTEMS_PRODUCT_W14		0x9603
#define FOUR_G_SYSTEMS_PRODUCT_W100		0x9b01

/* iBall 3.5G connect wireless modem */
#define IBALL_3_5G_CONNECT			0x9605

/* Zoom */
#define ZOOM_PRODUCT_4597			0x9607

/* SpeedUp SU9800 usb 3g modem */
#define SPEEDUP_PRODUCT_SU9800			0x9800

/* Haier products */
#define HAIER_VENDOR_ID				0x201e
#define HAIER_PRODUCT_CE81B			0x10f8
#define HAIER_PRODUCT_CE100			0x2009

/* Gemalto's Cinterion products (formerly Siemens) */
#define SIEMENS_VENDOR_ID			0x0681
#define CINTERION_VENDOR_ID			0x1e2d
#define CINTERION_PRODUCT_HC25_MDMNET		0x0040
#define CINTERION_PRODUCT_HC25_MDM		0x0047
#define CINTERION_PRODUCT_HC28_MDMNET		0x004A /* same for HC28J */
#define CINTERION_PRODUCT_HC28_MDM		0x004C
#define CINTERION_PRODUCT_EU3_E			0x0051
#define CINTERION_PRODUCT_EU3_P			0x0052
#define CINTERION_PRODUCT_PH8			0x0053
#define CINTERION_PRODUCT_AHXX			0x0055
#define CINTERION_PRODUCT_PLXX			0x0060
#define CINTERION_PRODUCT_PH8_2RMNET		0x0082
#define CINTERION_PRODUCT_PH8_AUDIO		0x0083
#define CINTERION_PRODUCT_AHXX_2RMNET		0x0084
#define CINTERION_PRODUCT_AHXX_AUDIO		0x0085

/* Olivetti products */
#define OLIVETTI_VENDOR_ID			0x0b3c
#define OLIVETTI_PRODUCT_OLICARD100		0xc000
#define OLIVETTI_PRODUCT_OLICARD120		0xc001
#define OLIVETTI_PRODUCT_OLICARD140		0xc002
#define OLIVETTI_PRODUCT_OLICARD145		0xc003
#define OLIVETTI_PRODUCT_OLICARD155		0xc004
#define OLIVETTI_PRODUCT_OLICARD200		0xc005
#define OLIVETTI_PRODUCT_OLICARD160		0xc00a
#define OLIVETTI_PRODUCT_OLICARD500		0xc00b

/* Celot products */
#define CELOT_VENDOR_ID				0x211f
#define CELOT_PRODUCT_CT680M			0x6801

/* Samsung products */
#define SAMSUNG_VENDOR_ID                       0x04e8
#define SAMSUNG_PRODUCT_GT_B3730                0x6889

/* YUGA products  www.yuga-info.com gavin.kx@qq.com */
#define YUGA_VENDOR_ID				0x257A
#define YUGA_PRODUCT_CEM600			0x1601
#define YUGA_PRODUCT_CEM610			0x1602
#define YUGA_PRODUCT_CEM500			0x1603
#define YUGA_PRODUCT_CEM510			0x1604
#define YUGA_PRODUCT_CEM800			0x1605
#define YUGA_PRODUCT_CEM900			0x1606

#define YUGA_PRODUCT_CEU818			0x1607
#define YUGA_PRODUCT_CEU816			0x1608
#define YUGA_PRODUCT_CEU828			0x1609
#define YUGA_PRODUCT_CEU826			0x160A
#define YUGA_PRODUCT_CEU518			0x160B
#define YUGA_PRODUCT_CEU516			0x160C
#define YUGA_PRODUCT_CEU528			0x160D
#define YUGA_PRODUCT_CEU526			0x160F
#define YUGA_PRODUCT_CEU881			0x161F
#define YUGA_PRODUCT_CEU882			0x162F

#define YUGA_PRODUCT_CWM600			0x2601
#define YUGA_PRODUCT_CWM610			0x2602
#define YUGA_PRODUCT_CWM500			0x2603
#define YUGA_PRODUCT_CWM510			0x2604
#define YUGA_PRODUCT_CWM800			0x2605
#define YUGA_PRODUCT_CWM900			0x2606

#define YUGA_PRODUCT_CWU718			0x2607
#define YUGA_PRODUCT_CWU716			0x2608
#define YUGA_PRODUCT_CWU728			0x2609
#define YUGA_PRODUCT_CWU726			0x260A
#define YUGA_PRODUCT_CWU518			0x260B
#define YUGA_PRODUCT_CWU516			0x260C
#define YUGA_PRODUCT_CWU528			0x260D
#define YUGA_PRODUCT_CWU581			0x260E
#define YUGA_PRODUCT_CWU526			0x260F
#define YUGA_PRODUCT_CWU582			0x261F
#define YUGA_PRODUCT_CWU583			0x262F

#define YUGA_PRODUCT_CLM600			0x3601
#define YUGA_PRODUCT_CLM610			0x3602
#define YUGA_PRODUCT_CLM500			0x3603
#define YUGA_PRODUCT_CLM510			0x3604
#define YUGA_PRODUCT_CLM800			0x3605
#define YUGA_PRODUCT_CLM900			0x3606

#define YUGA_PRODUCT_CLU718			0x3607
#define YUGA_PRODUCT_CLU716			0x3608
#define YUGA_PRODUCT_CLU728			0x3609
#define YUGA_PRODUCT_CLU726			0x360A
#define YUGA_PRODUCT_CLU518			0x360B
#define YUGA_PRODUCT_CLU516			0x360C
#define YUGA_PRODUCT_CLU528			0x360D
#define YUGA_PRODUCT_CLU526			0x360F

/* Viettel products */
#define VIETTEL_VENDOR_ID			0x2262
#define VIETTEL_PRODUCT_VT1000			0x0002

/* ZD Incorporated */
#define ZD_VENDOR_ID				0x0685
#define ZD_PRODUCT_7000				0x7000

/* LG products */
#define LG_VENDOR_ID				0x1004
#define LG_PRODUCT_L02C				0x618f

/* MediaTek products */
#define MEDIATEK_VENDOR_ID			0x0e8d
#define MEDIATEK_PRODUCT_DC_1COM		0x00a0
#define MEDIATEK_PRODUCT_DC_4COM		0x00a5
#define MEDIATEK_PRODUCT_DC_4COM2		0x00a7
#define MEDIATEK_PRODUCT_DC_5COM		0x00a4
#define MEDIATEK_PRODUCT_7208_1COM		0x7101
#define MEDIATEK_PRODUCT_7208_2COM		0x7102
#define MEDIATEK_PRODUCT_7103_2COM		0x7103
#define MEDIATEK_PRODUCT_7106_2COM		0x7106
#define MEDIATEK_PRODUCT_FP_1COM		0x0003
#define MEDIATEK_PRODUCT_FP_2COM		0x0023
#define MEDIATEK_PRODUCT_FPDC_1COM		0x0043
#define MEDIATEK_PRODUCT_FPDC_2COM		0x0033

/* Cellient products */
#define CELLIENT_VENDOR_ID			0x2692
#define CELLIENT_PRODUCT_MEN200			0x9005

/* Hyundai Petatel Inc. products */
#define PETATEL_VENDOR_ID			0x1ff4
#define PETATEL_PRODUCT_NP10T_600A		0x600a
#define PETATEL_PRODUCT_NP10T_600E		0x600e

/* TP-LINK Incorporated products */
#define TPLINK_VENDOR_ID			0x2357
#define TPLINK_PRODUCT_MA180			0x0201

/* Changhong products */
#define CHANGHONG_VENDOR_ID			0x2077
#define CHANGHONG_PRODUCT_CH690			0x7001

/* Inovia */
#define INOVIA_VENDOR_ID			0x20a6
#define INOVIA_SEW858				0x1105

/* VIA Telecom */
#define VIATELECOM_VENDOR_ID			0x15eb
#define VIATELECOM_PRODUCT_CDS7			0x0001

/* some devices interfaces need special handling due to a number of reasons */
enum option_blacklist_reason {
		OPTION_BLACKLIST_NONE = 0,
		OPTION_BLACKLIST_SENDSETUP = 1,
		OPTION_BLACKLIST_RESERVED_IF = 2
};

#define MAX_BL_NUM  8
struct option_blacklist_info {
	/* bitfield of interface numbers for OPTION_BLACKLIST_SENDSETUP */
	const unsigned long sendsetup;
	/* bitfield of interface numbers for OPTION_BLACKLIST_RESERVED_IF */
	const unsigned long reserved;
};

static const struct option_blacklist_info four_g_w14_blacklist = {
	.sendsetup = BIT(0) | BIT(1),
};

static const struct option_blacklist_info four_g_w100_blacklist = {
	.sendsetup = BIT(1) | BIT(2),
	.reserved = BIT(3),
};

static const struct option_blacklist_info alcatel_x200_blacklist = {
	.sendsetup = BIT(0) | BIT(1),
	.reserved = BIT(4),
};

static const struct option_blacklist_info zte_0037_blacklist = {
	.sendsetup = BIT(0) | BIT(1),
};

static const struct option_blacklist_info zte_k3765_z_blacklist = {
	.sendsetup = BIT(0) | BIT(1) | BIT(2),
	.reserved = BIT(4),
};

static const struct option_blacklist_info zte_ad3812_z_blacklist = {
	.sendsetup = BIT(0) | BIT(1) | BIT(2),
};

static const struct option_blacklist_info zte_mc2718_z_blacklist = {
	.sendsetup = BIT(1) | BIT(2) | BIT(3) | BIT(4),
};

static const struct option_blacklist_info zte_mc2716_z_blacklist = {
	.sendsetup = BIT(1) | BIT(2) | BIT(3),
};

static const struct option_blacklist_info zte_me3620_mbim_blacklist = {
	.reserved = BIT(2) | BIT(3) | BIT(4),
};

static const struct option_blacklist_info zte_me3620_xl_blacklist = {
	.reserved = BIT(3) | BIT(4) | BIT(5),
};

static const struct option_blacklist_info zte_zm8620_x_blacklist = {
	.reserved = BIT(3) | BIT(4) | BIT(5),
};

static const struct option_blacklist_info huawei_cdc12_blacklist = {
	.reserved = BIT(1) | BIT(2),
};

static const struct option_blacklist_info net_intf0_blacklist = {
	.reserved = BIT(0),
};

static const struct option_blacklist_info net_intf1_blacklist = {
	.reserved = BIT(1),
};

static const struct option_blacklist_info net_intf2_blacklist = {
	.reserved = BIT(2),
};

static const struct option_blacklist_info net_intf3_blacklist = {
	.reserved = BIT(3),
};

static const struct option_blacklist_info net_intf4_blacklist = {
	.reserved = BIT(4),
};

static const struct option_blacklist_info net_intf5_blacklist = {
	.reserved = BIT(5),
};

static const struct option_blacklist_info net_intf6_blacklist = {
	.reserved = BIT(6),
};

static const struct option_blacklist_info zte_mf626_blacklist = {
	.sendsetup = BIT(0) | BIT(1),
	.reserved = BIT(4),
};

static const struct option_blacklist_info zte_1255_blacklist = {
	.reserved = BIT(3) | BIT(4),
};

// comment by dimmalex for add at sim7600 section
//static const struct option_blacklist_info simcom_sim7100e_blacklist = {
//	.reserved = BIT(5) | BIT(6),
//};

static const struct option_blacklist_info telit_le910_blacklist = {
	.sendsetup = BIT(0),
	.reserved = BIT(1) | BIT(2),
};

static const struct option_blacklist_info telit_le920_blacklist = {
	.sendsetup = BIT(0),
	.reserved = BIT(1) | BIT(5),
};

static const struct option_blacklist_info telit_le922_blacklist_usbcfg0 = {
	.sendsetup = BIT(2),
	.reserved = BIT(0) | BIT(1) | BIT(3),
};

static const struct option_blacklist_info telit_le922_blacklist_usbcfg3 = {
	.sendsetup = BIT(0),
	.reserved = BIT(1) | BIT(2) | BIT(3),
};

static const struct option_blacklist_info cinterion_rmnet2_blacklist = {
	.reserved = BIT(4) | BIT(5),
};


static const struct option_blacklist_info blacklist_45 = {
	.reserved = BIT(4)|BIT(5),
};
static const struct option_blacklist_info blacklist_4 = {
	.reserved = BIT(4),
};
static const struct option_blacklist_info blacklist_56 = {
	.reserved = BIT(5)| BIT(6),
};
static const struct option_blacklist_info blacklist_7 = {
	.reserved = BIT(7),
};
static const struct option_blacklist_info blacklist_2345 = {
	.reserved = BIT(2)| BIT(3)| BIT(4)| BIT(5),
};
static const struct option_blacklist_info blacklist_23456 = {
	.reserved = BIT(2)| BIT(3)| BIT(4)| BIT(5)| BIT(6),
};
static const struct option_blacklist_info blacklist_3 = {
	.sendsetup = BIT(3),
};
static const struct option_blacklist_info blacklist_6 = {
	.reserved = BIT(6),
};
static const struct option_blacklist_info blacklist_345 = {
	.reserved = BIT(3)| BIT(4)| BIT(5),
};
static const struct option_blacklist_info blacklist_456 = {
	.reserved = BIT(4)| BIT(5)| BIT(6),
};
static const struct option_blacklist_info blacklist_016 = {
	.reserved = BIT(0)| BIT(1)| BIT(6),
};
static const struct option_blacklist_info blacklist_2 = {
	.reserved = BIT(2),
};
static const struct option_blacklist_info blacklist_012 = {
	.reserved = BIT(0)|BIT(1)|BIT(2),
};
static const struct option_blacklist_info blacklist_014 = {
	.reserved = BIT(0)|BIT(1)|BIT(4),
};
static const struct option_blacklist_info blacklist_015 = {
	.reserved = BIT(0)|BIT(1)|BIT(5),
};
/* add by dimmalex for fibocom l710  */
#define FIBOCOM_USB_VENDOR_AND_INTERFACE_INFO(vend, cl, sc, pr) \
        .match_flags = USB_DEVICE_ID_MATCH_INT_INFO | USB_DEVICE_ID_MATCH_VENDOR, \
        .idVendor = (vend), \
        .bInterfaceClass = (cl), \
        .bInterfaceSubClass = (sc), \
        .bInterfaceProtocol = (pr)



static const struct usb_device_id option_ids[] = {





    /* add by dimmalex for gosuncn GM800 */
    { USB_DEVICE(0x305A, 0x1421), .driver_info = (kernel_ulong_t)&blacklist_345 }, // Gosuncn GM800
    { USB_DEVICE(0x305A, 0x1406), .driver_info = (kernel_ulong_t)&blacklist_345 }, // Gosuncn GM800
    { USB_DEVICE(0x3763, 0x3C93), .driver_info = (kernel_ulong_t)&blacklist_345 }, // Gosuncn GM510



    /* add by dimmalex for meiglink slm730 */
    { USB_DEVICE(0x05C6, 0xF601), .driver_info = (kernel_ulong_t)&blacklist_45 }, // forge slm730



    /* add by dimmalex for yuga clm920/meiglink slm750  */
    { USB_DEVICE(0x05C6, 0x9025), .driver_info = (kernel_ulong_t)&blacklist_4 },


    /* add by dimmalex for xinyi m710a-s  */
    { USB_DEVICE(0x05C6, 0x5013), .driver_info = (kernel_ulong_t)&blacklist_4 },



    /* add by dimmalex for longsung u8300 */
    { USB_DEVICE(0x1C9E, 0x9B05), .driver_info = (kernel_ulong_t)&blacklist_4 },
    { USB_DEVICE(0x1C9E, 0x9B3C), .driver_info = (kernel_ulong_t)&blacklist_4 },



    /* add by dimmalex for simcom sim7600/sim8200 */
    { USB_DEVICE(0x05C6, 0x90DB), .driver_info = (kernel_ulong_t)&blacklist_2345 },
    { USB_DEVICE(0x1E0E, 0x9001), .driver_info = (kernel_ulong_t)&blacklist_56 },
	{ USB_DEVICE_INTERFACE_CLASS( 0x1E0E, 0x9011, 0xff), .driver_info = (kernel_ulong_t)&blacklist_7 },



    /* add by dimmalex for chinamoblie m8321 */
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0532, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0536, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0542, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0573, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0579, 0xff, 0xff, 0xff) },



    /* add by dimmalex for uCloudlink m2 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1782, 0x5D22, 0xff, 0x00, 0x00), .driver_info = (kernel_ulong_t)&blacklist_3 },



    /* add by dimmalex for Innofidei E7B01 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1d53, 0x3d53, 0x2, 0x2, 0x1) },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1d53, 0xd00d, 0x2, 0x2, 0x1) },


    
    /* add by dimmalex for fibocom l710  */
    { FIBOCOM_USB_VENDOR_AND_INTERFACE_INFO(0x2CB7, 0xff, 0xff, 0xff) },
    { FIBOCOM_USB_VENDOR_AND_INTERFACE_INFO(0x2CB7, 0x0a, 0x00, 0xff) },
    { USB_DEVICE_AND_INTERFACE_INFO(0x19D2, 0x0256, 0xff, 0xff, 0xff) },
    // { USB_DEVICE_AND_INTERFACE_INFO(0x19D2, 0x0579, 0xff, 0xff, 0xff) },  comment beacuse already add in chinamoblie m8321



    /* add by dimmalex for fibocom l610  */
    { USB_DEVICE(0x1782, 0x4d11), .driver_info = (kernel_ulong_t)&blacklist_4 },
    { USB_DEVICE(0x1782, 0x4d10), .driver_info = (kernel_ulong_t)&blacklist_4 },



    /* add by dimmalex for ZTE ME3760/thinkwill ML7810 */
	{ USB_DEVICE(0x19D2, 0x0199) },



    /* add by dimmalex TD Tech Em250/EM350 */
    { USB_DEVICE(HUAWEI_VENDOR_ID, 0x1506) },
    { USB_DEVICE(HUAWEI_VENDOR_ID, 0x1C05) },



#if 1 // add by dimmalex for Quectel
	{ USB_DEVICE(0x05C6, 0x9090) }, /* Quectel UC15 */
	{ USB_DEVICE(0x05C6, 0x9003) }, /* Quectel UC20 */
	{ USB_DEVICE(0x05C6, 0x9215) }, /* Quectel EC20(MDM9215) */
	{ USB_DEVICE(0x2C7C, 0x0125) }, /* Quectel EC20(MDM9x07)/EC25/EG25 */
	{ USB_DEVICE(0x2C7C, 0x0121) }, /* Quectel EC21 */
	{ USB_DEVICE(0x2C7C, 0x0191) }, /* Quectel EG91 */
	{ USB_DEVICE(0x2C7C, 0x0195) }, /* Quectel EG95 */
	{ USB_DEVICE(0x2C7C, 0x0306) }, /* Quectel EG06/EP06/EM06 */
	{ USB_DEVICE(0x2C7C, 0x0512) }, /* Quectel EG12/EP12/EM12/EG16/EG18 */
	{ USB_DEVICE(0x2C7C, 0x0296) }, /* Quectel BG96 */
	{ USB_DEVICE(0x2C7C, 0x0700) }, /* Quectel BG95/BG77/BG600L-M3/BC69 */
	{ USB_DEVICE(0x2C7C, 0x0435) }, /* Quectel AG35 */
	{ USB_DEVICE(0x2C7C, 0x0415) }, /* Quectel AG15 */
	{ USB_DEVICE(0x2C7C, 0x0520) }, /* Quectel AG520 */
	{ USB_DEVICE(0x2C7C, 0x0550) }, /* Quectel AG550 */
	{ USB_DEVICE(0x2C7C, 0x0620) }, /* Quectel EG20 */
	{ USB_DEVICE(0x2C7C, 0x0800) }, /* Quectel RG500/RM500/RG510/RM510 */
	{ USB_DEVICE(0x2C7C, 0x6120) }, /* Quectel UC200 */
	{ USB_DEVICE(0x2C7C, 0x6000) }, /* Quectel EC200/UC200 */
	{ USB_DEVICE(0x2C7C, 0x6026) }, /* Quectel EC200 */
#endif
#if 1 // add by dimmalex for Quectel EC200
	{ USB_DEVICE(0x2C7C, 0x6002) }, /* Quectel EC200/UC200 */
#endif
#if 1 // add by dimmalex for Quectel EC200A
	{ USB_DEVICE(0x2C7C, 0x6005) }, /* Quectel EC200A */
#endif
#if 1 // add by dimmalex for Quectel RM500U/RG200U
    { USB_DEVICE_AND_INTERFACE_INFO(0x2C7C, 0x0900, 0xff, 0x00, 0x00) }, /* Quectel RM500U/RG200U/RX500U */
#endif


    // add by dimmalex for Fibcom begin
	{ USB_DEVICE(0x2CB7, 0x0104), .driver_info = (kernel_ulong_t)&blacklist_456  },  // FM150
	{ USB_DEVICE(0x2CB7, 0x0105), .driver_info = (kernel_ulong_t)&blacklist_456  },
	{ USB_DEVICE(0x2CB7, 0x0106) },
	{ USB_DEVICE(0x2CB7, 0x0107) },
	{ USB_DEVICE(0x2CB7, 0x0108) },
	{ USB_DEVICE(0x2CB7, 0x0109), .driver_info = (kernel_ulong_t)&blacklist_23456  },
	{ USB_DEVICE(0x2CB7, 0x010A), .driver_info = (kernel_ulong_t)&blacklist_23456  },  // FM150
	{ USB_DEVICE(0x2CB7, 0x010B), .driver_info = (kernel_ulong_t)&blacklist_456  },
    { USB_DEVICE(0x2CB7, 0x0110), .driver_info = (kernel_ulong_t)&blacklist_2 },                       // FM150
    { USB_DEVICE(0x2CB7, 0x0111), .driver_info = (kernel_ulong_t)&blacklist_2 },                       // FM150
	{ USB_DEVICE(0x2CB7, 0x0A05), .driver_info = (kernel_ulong_t)&blacklist_016 },   // FM650
	{ USB_DEVICE_INTERFACE_CLASS(0x2CB7, 0x0105, 0xff),	.driver_info = (kernel_ulong_t)&blacklist_6 }, // NL678
	{ USB_DEVICE_INTERFACE_CLASS(0x2CB7, 0x01A0, 0xff) },			              // NL668-AM/NL652-EU (laptop MBIM)
	{ USB_DEVICE(0x1508, 0x1000), .driver_info = (kernel_ulong_t)&blacklist_23456 },
	{ USB_DEVICE(0x1508, 0x1001), .driver_info = (kernel_ulong_t)&blacklist_456 },    // Fibocom NL668 (IOT version)
	//{ USB_DEVICE(0x05C6, 0x9025) },                                                 comment beacuse already add in meiglink slm750
	//{ USB_DEVICE(0x05C6, 0x90DB), .driver_info = (kernel_ulong_t)&blacklist_25 },   comment beacuse already add in simcom sim7600
	//{ USB_DEVICE(0x2C7C, 0x0306), .driver_info = (kernel_ulong_t)&blacklist_456  }, comment beacuse already add in Quectel

	/*Added by Fibocom 2020-03-12 */
	{ USB_DEVICE(0x1782, 0x4028), .driver_info = (kernel_ulong_t)&blacklist_012 },
	{ USB_DEVICE(0x1782, 0x4030), .driver_info = (kernel_ulong_t)&blacklist_012 },
	{ USB_DEVICE(0x1782, 0x4032), .driver_info = (kernel_ulong_t)&blacklist_012 },
	{ USB_DEVICE(0x1782, 0x4033), .driver_info = (kernel_ulong_t)&blacklist_014 },
	{ USB_DEVICE(0x1782, 0x4D00)},

	//for Fibocom FG621-EA
	{ USB_DEVICE(0x2CB7, 0x0A04), .driver_info = (kernel_ulong_t)&blacklist_015 },
	//{ USB_DEVICE(0x2CB7, 0x0A05), .driver_info = (kernel_ulong_t)&blacklist_016 },  comment beacuse already add in FM650
	{ USB_DEVICE(0x2CB7, 0x0A06), .driver_info = (kernel_ulong_t)&blacklist_016 },
	{ USB_DEVICE(0x2CB7, 0x0A07), .driver_info = (kernel_ulong_t)&blacklist_016 },
	//End of changing by Fibocom 2020-03-12


    
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COLT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_QUAD) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_QUAD_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_QUAD) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_QUAD_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COBRA) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COBRA_BUS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_VIPER) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_VIPER_BUS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_GT_MAX_READY) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_KOI_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_SCORPION_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_LITE) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_KOI_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_GTM380_MODEM) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, QUANTA_PRODUCT_Q101) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, QUANTA_PRODUCT_Q111) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, QUANTA_PRODUCT_GLX) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, QUANTA_PRODUCT_GKE) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, QUANTA_PRODUCT_GLE) },
	{ USB_DEVICE(QUANTA_VENDOR_ID, 0xea42),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x1c05, USB_CLASS_COMM, 0x02, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x1c1f, USB_CLASS_COMM, 0x02, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x1c23, USB_CLASS_COMM, 0x02, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E173, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E173S6, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E1750, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x1441, USB_CLASS_COMM, 0x02, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x1442, USB_CLASS_COMM, 0x02, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_K4505, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &huawei_cdc12_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_K3765, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &huawei_cdc12_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0x14ac, 0xff, 0xff, 0xff),	/* Huawei E1820 */
		.driver_info = (kernel_ulong_t) &net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_K4605, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t) &huawei_cdc12_blacklist },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0xff, 0xff) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x01, 0x7C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x02, 0x7C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x03, 0x7C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x04, 0x7C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x05, 0x7C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x02) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x03) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x04) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x06) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x0A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x0B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x0D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x0E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x0F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x10) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x12) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x13) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x14) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x15) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x17) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x18) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x19) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x1A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x1B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x1C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x31) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x32) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x33) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x34) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x35) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x36) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x3A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x3B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x3D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x3E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x3F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x48) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x49) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x4A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x4B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x4C) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x61) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x62) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x63) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x64) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x65) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x66) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x6A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x6B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x6D) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x6E) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x6F) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x72) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x73) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x74) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x75) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x78) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x79) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x7A) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x7B) },
	{ USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, 0xff, 0x06, 0x7C) },


	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_V640) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_V620) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_V740) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_V720) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_U730) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_U740) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_U870) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_XU870) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_X950D) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EV620) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_ES720) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_E725) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_ES620) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EU730) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EU740) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EU870D) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC950D) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC727) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_OVMC760) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC780) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EVDO_FULLSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_FULLSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EVDO_EMBEDDED_FULLSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_EMBEDDED_FULLSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EVDO_HIGHSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED3) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED4) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED5) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED6) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_HIGHSPEED7) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC996D) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MF3470) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC547) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_EVDO_EMBEDDED_HIGHSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_HSPA_EMBEDDED_HIGHSPEED) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_G2) },
	/* Novatel Ovation MC551 a.k.a. Verizon USB551L */
	{ USB_DEVICE_AND_INTERFACE_INFO(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_MC551, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_E362, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_E371, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(NOVATELWIRELESS_VENDOR_ID, NOVATELWIRELESS_PRODUCT_U620L, 0xff, 0x00, 0x00) },

	{ USB_DEVICE(AMOI_VENDOR_ID, AMOI_PRODUCT_H01) },
	{ USB_DEVICE(AMOI_VENDOR_ID, AMOI_PRODUCT_H01A) },
	{ USB_DEVICE(AMOI_VENDOR_ID, AMOI_PRODUCT_H02) },
	{ USB_DEVICE(AMOI_VENDOR_ID, AMOI_PRODUCT_SKYPEPHONE_S2) },

	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5700_MINICARD) },		/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO Mini-Card == Novatel Expedite EV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5500_MINICARD) },		/* Dell Wireless 5500 Mobile Broadband HSDPA Mini-Card == Novatel Expedite EU740 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5505_MINICARD) },		/* Dell Wireless 5505 Mobile Broadband HSDPA Mini-Card == Novatel Expedite EU740 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5700_EXPRESSCARD) },		/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO ExpressCard == Novatel Merlin XV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5510_EXPRESSCARD) },		/* Dell Wireless 5510 Mobile Broadband HSDPA ExpressCard == Novatel Merlin XU870 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5700_MINICARD_SPRINT) },	/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO Mini-Card == Novatel Expedite E720 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5700_MINICARD_TELUS) },	/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO Mini-Card == Novatel Expedite ET620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5720_MINICARD_VZW) }, 	/* Dell Wireless 5720 == Novatel EV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5720_MINICARD_SPRINT) }, 	/* Dell Wireless 5720 == Novatel EV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5720_MINICARD_TELUS) }, 	/* Dell Wireless 5720 == Novatel EV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5520_MINICARD_CINGULAR) },	/* Dell Wireless HSDPA 5520 == Novatel Expedite EU860D */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5520_MINICARD_GENERIC_L) },	/* Dell Wireless HSDPA 5520 */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5520_MINICARD_GENERIC_I) },	/* Dell Wireless 5520 Voda I Mobile Broadband (3G HSDPA) Minicard */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5730_MINICARD_SPRINT) },	/* Dell Wireless 5730 Mobile Broadband EVDO/HSPA Mini-Card */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5730_MINICARD_TELUS) },	/* Dell Wireless 5730 Mobile Broadband EVDO/HSPA Mini-Card */
	{ USB_DEVICE(DELL_VENDOR_ID, DELL_PRODUCT_5730_MINICARD_VZW) }, 	/* Dell Wireless 5730 Mobile Broadband EVDO/HSPA Mini-Card */
	{ USB_DEVICE_AND_INTERFACE_INFO(DELL_VENDOR_ID, DELL_PRODUCT_5800_MINICARD_VZW, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(DELL_VENDOR_ID, DELL_PRODUCT_5800_V2_MINICARD_VZW, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(DELL_VENDOR_ID, DELL_PRODUCT_5804_MINICARD_ATT, 0xff, 0xff, 0xff) },
	{ USB_DEVICE(ANYDATA_VENDOR_ID, ANYDATA_PRODUCT_ADU_E100A) },	/* ADU-E100, ADU-310 */
	{ USB_DEVICE(ANYDATA_VENDOR_ID, ANYDATA_PRODUCT_ADU_500A) },
	{ USB_DEVICE(ANYDATA_VENDOR_ID, ANYDATA_PRODUCT_ADU_620UW) },
	{ USB_DEVICE(AXESSTEL_VENDOR_ID, AXESSTEL_PRODUCT_MV110H) },
	{ USB_DEVICE(YISO_VENDOR_ID, YISO_PRODUCT_U893) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_C100_1, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_C100_2, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1004, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1005, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1006, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1007, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1008, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1009, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100A, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100B, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100C, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100D, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100E, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_100F, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1010, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1011, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_1012, 0xff) },
	{ USB_DEVICE(KYOCERA_VENDOR_ID, KYOCERA_PRODUCT_KPC650) },
	{ USB_DEVICE(KYOCERA_VENDOR_ID, KYOCERA_PRODUCT_KPC680) },
	{ USB_DEVICE(QUALCOMM_VENDOR_ID, 0x6000)}, /* ZTE AC8700 */
	{ USB_DEVICE_AND_INTERFACE_INFO(QUALCOMM_VENDOR_ID, 0x6001, 0xff, 0xff, 0xff), /* 4G LTE usb-modem U901 */
	  .driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE(QUALCOMM_VENDOR_ID, 0x6613)}, /* Onda H600/ZTE MF330 */
	{ USB_DEVICE(QUALCOMM_VENDOR_ID, 0x0023)}, /* ONYX 3G device */
	{ USB_DEVICE(QUALCOMM_VENDOR_ID, 0x9000)}, /* SIMCom SIM5218 */

    // comment by dimmalex for add in the Quectel
	//{ USB_DEVICE(QUALCOMM_VENDOR_ID, 0x9003), /* Quectel UC20 */
	//  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },

    { USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_6001) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CMU_300) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_6003),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_6004) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_6005) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CGU_628A) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHE_628S),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CMU_301),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_628),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_628S) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CDU_680) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CDU_685A) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_720S),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7002),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_629K),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7004),
	  .driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7005) },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CGU_629),
	  .driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_629S),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_CHU_720I),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7212),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7213),
	  .driver_info = (kernel_ulong_t)&net_intf0_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7251),
	  .driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7252),
	  .driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE(CMOTECH_VENDOR_ID, CMOTECH_PRODUCT_7253),
	  .driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_UC864E) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_UC864G) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_CC864_DUAL) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_CC864_SINGLE) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_DE910_DUAL) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_UE910_V2) },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_LE922_USBCFG0),
		.driver_info = (kernel_ulong_t)&telit_le922_blacklist_usbcfg0 },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_LE922_USBCFG3),
		.driver_info = (kernel_ulong_t)&telit_le922_blacklist_usbcfg3 },
	{ USB_DEVICE_INTERFACE_CLASS(TELIT_VENDOR_ID, TELIT_PRODUCT_LE922_USBCFG5, 0xff),
		.driver_info = (kernel_ulong_t)&telit_le922_blacklist_usbcfg0 },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_LE910),
		.driver_info = (kernel_ulong_t)&telit_le910_blacklist },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_LE910_USBCFG4),
		.driver_info = (kernel_ulong_t)&telit_le922_blacklist_usbcfg3 },
	{ USB_DEVICE(TELIT_VENDOR_ID, TELIT_PRODUCT_LE920),
		.driver_info = (kernel_ulong_t)&telit_le920_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_MF622, 0xff, 0xff, 0xff) }, /* ZTE WCDMA products */
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0002, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0003, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0004, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0005, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0006, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0008, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0009, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000a, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000b, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000c, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000d, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000e, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x000f, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0010, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0011, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0012, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0013, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_MF628, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0016, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0017, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0018, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0019, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0020, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0021, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0022, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0023, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0024, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0025, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0028, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0029, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0030, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_MF626, 0xff,
	  0xff, 0xff), .driver_info = (kernel_ulong_t)&zte_mf626_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0032, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0033, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0034, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0037, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&zte_0037_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0038, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0039, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0040, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0042, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0043, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0044, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0048, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0049, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0050, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0051, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0052, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0054, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0055, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0056, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0057, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0058, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0061, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0062, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0063, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0064, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0065, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0066, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0067, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0069, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0076, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0077, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0078, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0079, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0082, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0083, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0086, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0087, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0088, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0089, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0090, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0091, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0092, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0093, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0094, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0095, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0096, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0097, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0104, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0105, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0106, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0108, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0113, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0117, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0118, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0121, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0122, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0123, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0124, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0125, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0126, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0128, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0135, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0136, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0137, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0139, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0142, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0143, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0144, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0145, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0148, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0151, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0153, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0155, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0156, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0157, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0158, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0159, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0161, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0162, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0164, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0165, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0167, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0189, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0191, 0xff, 0xff, 0xff), /* ZTE EuFi890 */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0196, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0197, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0199, 0xff, 0xff, 0xff), /* ZTE MF820S */
	  .driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0200, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0201, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0254, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0257, 0xff, 0xff, 0xff), /* ZTE MF821 */
	  .driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0265, 0xff, 0xff, 0xff), /* ONDA MT8205 */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0284, 0xff, 0xff, 0xff), /* ZTE MF880 */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0317, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0326, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0330, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0395, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0412, 0xff, 0xff, 0xff), /* Telewell TW-LTE 4G */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0414, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0417, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1008, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1010, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1012, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1018, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1021, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1057, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1058, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1059, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1060, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1061, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1062, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1063, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1064, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1065, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1066, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1067, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1068, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1069, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1070, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1071, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1072, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1073, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1074, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1075, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1076, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1077, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1078, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1079, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1080, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1081, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1082, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1083, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1084, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1085, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1086, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1087, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1088, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1089, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1090, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1091, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1092, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1093, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1094, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1095, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1096, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1097, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1098, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1099, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1100, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1101, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1102, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1103, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1104, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1105, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1106, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1107, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1108, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1109, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1110, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1111, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1112, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1113, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1114, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1115, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1116, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1117, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1118, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1119, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1120, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1121, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1122, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1123, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1124, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1125, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1126, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1127, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1128, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1129, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1130, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1131, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1132, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1133, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1134, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1135, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1136, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1137, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1138, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1139, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1140, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1141, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1142, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1143, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1144, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1145, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1146, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1147, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1148, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1149, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1150, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1151, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1152, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1153, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1154, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1155, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1156, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1157, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1158, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1159, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1160, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1161, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1162, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1163, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1164, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1165, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1166, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1167, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1168, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1169, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1170, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1244, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1245, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1246, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1247, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1248, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1249, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1250, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1251, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1252, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1253, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1254, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1255, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&zte_1255_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1256, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1257, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1258, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1259, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1260, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1261, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1262, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1263, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1264, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1265, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1266, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1267, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1268, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1269, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1270, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1271, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1272, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1273, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1274, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1275, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1276, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1277, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1278, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1279, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1280, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1281, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1282, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1283, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1284, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1285, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1286, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1287, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1288, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1289, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1290, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1291, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1292, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1293, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1294, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1295, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1296, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1297, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1298, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1299, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1300, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1301, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1302, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1303, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1333, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1401, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1402, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1424, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1425, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1426, 0xff, 0xff, 0xff),  /* ZTE MF91 */
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1428, 0xff, 0xff, 0xff),  /* Telewell TW-LTE 4G v2 */
		.driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1533, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1534, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1535, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1545, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1546, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1547, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1565, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1566, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1567, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1589, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1590, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1591, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1592, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1594, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1596, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1598, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x1600, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x2002, 0xff,
	  0xff, 0xff), .driver_info = (kernel_ulong_t)&zte_k3765_z_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x2003, 0xff, 0xff, 0xff) },

	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0014, 0xff, 0xff, 0xff) }, /* ZTE CDMA products */
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0027, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0059, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0060, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0070, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0073, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0094, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0130, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf1_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0133, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0141, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0147, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0152, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0168, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0170, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0176, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0x0178, 0xff, 0xff, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xffe9, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff8b, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff8c, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff8d, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff8e, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff8f, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff90, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff91, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff92, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff93, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff94, 0xff, 0xff, 0xff) },

	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_CDMA_TECH, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_AC2726, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_AC8710T, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_MC2718, 0xff, 0xff, 0xff),
	 .driver_info = (kernel_ulong_t)&zte_mc2718_z_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_AD3812, 0xff, 0xff, 0xff),
	 .driver_info = (kernel_ulong_t)&zte_ad3812_z_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZTE_VENDOR_ID, ZTE_PRODUCT_MC2716, 0xff, 0xff, 0xff),
	 .driver_info = (kernel_ulong_t)&zte_mc2716_z_blacklist },
	{ USB_DEVICE(ZTE_VENDOR_ID, ZTE_PRODUCT_ME3620_L),
	 .driver_info = (kernel_ulong_t)&zte_me3620_xl_blacklist },
	{ USB_DEVICE(ZTE_VENDOR_ID, ZTE_PRODUCT_ME3620_MBIM),
	 .driver_info = (kernel_ulong_t)&zte_me3620_mbim_blacklist },
	{ USB_DEVICE(ZTE_VENDOR_ID, ZTE_PRODUCT_ME3620_X),
	 .driver_info = (kernel_ulong_t)&zte_me3620_xl_blacklist },
	{ USB_DEVICE(ZTE_VENDOR_ID, ZTE_PRODUCT_ZM8620_X),
	 .driver_info = (kernel_ulong_t)&zte_zm8620_x_blacklist },
	{ USB_VENDOR_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff, 0x02, 0x01) },
	{ USB_VENDOR_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff, 0x02, 0x05) },
	{ USB_VENDOR_AND_INTERFACE_INFO(ZTE_VENDOR_ID, 0xff, 0x86, 0x10) },

	{ USB_DEVICE(BENQ_VENDOR_ID, BENQ_PRODUCT_H10) },
	{ USB_DEVICE(DLINK_VENDOR_ID, DLINK_PRODUCT_DWM_652) },
	{ USB_DEVICE(ALINK_VENDOR_ID, DLINK_PRODUCT_DWM_652_U5) }, /* Yes, ALINK_VENDOR_ID */
	{ USB_DEVICE(ALINK_VENDOR_ID, DLINK_PRODUCT_DWM_652_U5A) },
	{ USB_DEVICE(QISDA_VENDOR_ID, QISDA_PRODUCT_H21_4512) },
	{ USB_DEVICE(QISDA_VENDOR_ID, QISDA_PRODUCT_H21_4523) },
	{ USB_DEVICE(QISDA_VENDOR_ID, QISDA_PRODUCT_H20_4515) },
	{ USB_DEVICE(QISDA_VENDOR_ID, QISDA_PRODUCT_H20_4518) },
	{ USB_DEVICE(QISDA_VENDOR_ID, QISDA_PRODUCT_H20_4519) },
	{ USB_DEVICE(TOSHIBA_VENDOR_ID, TOSHIBA_PRODUCT_G450) },
	{ USB_DEVICE(TOSHIBA_VENDOR_ID, TOSHIBA_PRODUCT_HSDPA_MINICARD ) }, /* Toshiba 3G HSDPA == Novatel Expedite EU870D MiniCard */
	{ USB_DEVICE(ALINK_VENDOR_ID, 0x9000) },
	{ USB_DEVICE(ALINK_VENDOR_ID, ALINK_PRODUCT_PH300) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ALINK_VENDOR_ID, ALINK_PRODUCT_3GU, 0xff, 0xff, 0xff) },

    // comment by dimmalex for add at sim7600 section
	// { USB_DEVICE(ALINK_VENDOR_ID, SIMCOM_PRODUCT_SIM7100E),
	//  .driver_info = (kernel_ulong_t)&simcom_sim7100e_blacklist },

	{ USB_DEVICE(ALCATEL_VENDOR_ID, ALCATEL_PRODUCT_X060S_X200),
	  .driver_info = (kernel_ulong_t)&alcatel_x200_blacklist
	},
	{ USB_DEVICE(ALCATEL_VENDOR_ID, ALCATEL_PRODUCT_X220_X500D),
	  .driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE(ALCATEL_VENDOR_ID, 0x0052),
	  .driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE(ALCATEL_VENDOR_ID, 0x00b6),
	  .driver_info = (kernel_ulong_t)&net_intf3_blacklist },
	{ USB_DEVICE(ALCATEL_VENDOR_ID, 0x00b7),
	  .driver_info = (kernel_ulong_t)&net_intf5_blacklist },
	{ USB_DEVICE(ALCATEL_VENDOR_ID, ALCATEL_PRODUCT_L100V),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(ALCATEL_VENDOR_ID, ALCATEL_PRODUCT_L800MA),
	  .driver_info = (kernel_ulong_t)&net_intf2_blacklist },
	{ USB_DEVICE(AIRPLUS_VENDOR_ID, AIRPLUS_PRODUCT_MCD650) },
	{ USB_DEVICE(TLAYTECH_VENDOR_ID, TLAYTECH_PRODUCT_TEU800) },
	{ USB_DEVICE(LONGCHEER_VENDOR_ID, FOUR_G_SYSTEMS_PRODUCT_W14),
  	  .driver_info = (kernel_ulong_t)&four_g_w14_blacklist
  	},
	{ USB_DEVICE(LONGCHEER_VENDOR_ID, FOUR_G_SYSTEMS_PRODUCT_W100),
	  .driver_info = (kernel_ulong_t)&four_g_w100_blacklist
	},
	{ USB_DEVICE_INTERFACE_CLASS(LONGCHEER_VENDOR_ID, SPEEDUP_PRODUCT_SU9800, 0xff) },
	{ USB_DEVICE(LONGCHEER_VENDOR_ID, ZOOM_PRODUCT_4597) },
	{ USB_DEVICE(LONGCHEER_VENDOR_ID, IBALL_3_5G_CONNECT) },
	{ USB_DEVICE(HAIER_VENDOR_ID, HAIER_PRODUCT_CE100) },
	{ USB_DEVICE_AND_INTERFACE_INFO(HAIER_VENDOR_ID, HAIER_PRODUCT_CE81B, 0xff, 0xff, 0xff) },
	/* Pirelli  */
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_C100_1, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_C100_2, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1004, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1005, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1006, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1007, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1008, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1009, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100A, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100B, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100C, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100D, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100E, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_100F, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1011, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(PIRELLI_VENDOR_ID, PIRELLI_PRODUCT_1012, 0xff) },
	/* Cinterion */
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_EU3_E) },
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_EU3_P) },
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_PH8),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_INTERFACE_CLASS(CINTERION_VENDOR_ID, CINTERION_PRODUCT_AHXX, 0xff) },
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_PLXX),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_INTERFACE_CLASS(CINTERION_VENDOR_ID, CINTERION_PRODUCT_PH8_2RMNET, 0xff),
		.driver_info = (kernel_ulong_t)&cinterion_rmnet2_blacklist },
	{ USB_DEVICE_INTERFACE_CLASS(CINTERION_VENDOR_ID, CINTERION_PRODUCT_PH8_AUDIO, 0xff),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_INTERFACE_CLASS(CINTERION_VENDOR_ID, CINTERION_PRODUCT_AHXX_2RMNET, 0xff) },
	{ USB_DEVICE_INTERFACE_CLASS(CINTERION_VENDOR_ID, CINTERION_PRODUCT_AHXX_AUDIO, 0xff) },
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_HC28_MDM) },
	{ USB_DEVICE(CINTERION_VENDOR_ID, CINTERION_PRODUCT_HC28_MDMNET) },
	{ USB_DEVICE(SIEMENS_VENDOR_ID, CINTERION_PRODUCT_HC25_MDM) },
	{ USB_DEVICE(SIEMENS_VENDOR_ID, CINTERION_PRODUCT_HC25_MDMNET) },
	{ USB_DEVICE(SIEMENS_VENDOR_ID, CINTERION_PRODUCT_HC28_MDM) }, /* HC28 enumerates with Siemens or Cinterion VID depending on FW revision */
	{ USB_DEVICE(SIEMENS_VENDOR_ID, CINTERION_PRODUCT_HC28_MDMNET) },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD100),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD120),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD140),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD145) },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD155),
		.driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD200),
		.driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD160),
		.driver_info = (kernel_ulong_t)&net_intf6_blacklist },
	{ USB_DEVICE(OLIVETTI_VENDOR_ID, OLIVETTI_PRODUCT_OLICARD500),
		.driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(CELOT_VENDOR_ID, CELOT_PRODUCT_CT680M) }, /* CT-650 CDMA 450 1xEVDO modem */
	{ USB_DEVICE_AND_INTERFACE_INFO(SAMSUNG_VENDOR_ID, SAMSUNG_PRODUCT_GT_B3730, USB_CLASS_CDC_DATA, 0x00, 0x00) }, /* Samsung GT-B3730 LTE USB modem.*/
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM600) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM610) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM500) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM510) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM800) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEM900) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU818) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU816) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU828) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU826) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU518) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU516) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU528) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU526) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM600) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM610) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM500) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM510) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM800) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWM900) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU718) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU716) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU728) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU726) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU518) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU516) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU528) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU526) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM600) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM610) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM500) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM510) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM800) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLM900) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU718) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU716) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU728) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU726) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU518) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU516) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU528) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CLU526) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU881) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CEU882) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU581) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU582) },
	{ USB_DEVICE(YUGA_VENDOR_ID, YUGA_PRODUCT_CWU583) },
	{ USB_DEVICE_AND_INTERFACE_INFO(VIETTEL_VENDOR_ID, VIETTEL_PRODUCT_VT1000, 0xff, 0xff, 0xff) },
	{ USB_DEVICE_AND_INTERFACE_INFO(ZD_VENDOR_ID, ZD_PRODUCT_7000, 0xff, 0xff, 0xff) },
	{ USB_DEVICE(LG_VENDOR_ID, LG_PRODUCT_L02C) }, /* docomo L-02C modem */
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, 0x00a1, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, 0x00a1, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, 0x00a2, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, 0x00a2, 0xff, 0x02, 0x01) },        /* MediaTek MT6276M modem & app port */
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_1COM, 0x0a, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_5COM, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_5COM, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_4COM, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_4COM, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_7208_1COM, 0x02, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_7208_2COM, 0x02, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_FP_1COM, 0x0a, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_FP_2COM, 0x0a, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_FPDC_1COM, 0x0a, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_FPDC_2COM, 0x0a, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_7103_2COM, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_7106_2COM, 0x02, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_4COM2, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(MEDIATEK_VENDOR_ID, MEDIATEK_PRODUCT_DC_4COM2, 0xff, 0x00, 0x00) },
	{ USB_DEVICE(CELLIENT_VENDOR_ID, CELLIENT_PRODUCT_MEN200) },
	{ USB_DEVICE(PETATEL_VENDOR_ID, PETATEL_PRODUCT_NP10T_600A) },
	{ USB_DEVICE(PETATEL_VENDOR_ID, PETATEL_PRODUCT_NP10T_600E) },
	{ USB_DEVICE(TPLINK_VENDOR_ID, TPLINK_PRODUCT_MA180),
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(TPLINK_VENDOR_ID, 0x9000),					/* TP-Link MA260 */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE(CHANGHONG_VENDOR_ID, CHANGHONG_PRODUCT_CH690) },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d01, 0xff, 0x02, 0x01) },	/* D-Link DWM-156 (variant) */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d01, 0xff, 0x00, 0x00) },	/* D-Link DWM-156 (variant) */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d02, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d02, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d03, 0xff, 0x02, 0x01) },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x7d03, 0xff, 0x00, 0x00) },
	{ USB_DEVICE_INTERFACE_CLASS(0x2001, 0x7e19, 0xff),			/* D-Link DWM-221 B1 */
	  .driver_info = (kernel_ulong_t)&net_intf4_blacklist },
	{ USB_DEVICE_AND_INTERFACE_INFO(0x07d1, 0x3e01, 0xff, 0xff, 0xff) }, /* D-Link DWM-152/C1 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x07d1, 0x3e02, 0xff, 0xff, 0xff) }, /* D-Link DWM-156/C1 */
	{ USB_DEVICE_INTERFACE_CLASS(0x2020, 0x4000, 0xff) },                /* OLICARD300 - MT6225 */
	{ USB_DEVICE(INOVIA_VENDOR_ID, INOVIA_SEW858) },
	{ USB_DEVICE(VIATELECOM_VENDOR_ID, VIATELECOM_PRODUCT_CDS7) },
	{ } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, option_ids);

/* The card has three separate interfaces, which the serial driver
 * recognizes separately, thus num_port=1.
 */

static struct usb_serial_driver option_1port_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"option1",
	},
	.description       = "GSM modem (1-port)",
	.id_table          = option_ids,
	.num_ports         = 1,
	.probe             = option_probe,
	.open              = usb_wwan_open,
	.close             = usb_wwan_close,
	.dtr_rts	   = usb_wwan_dtr_rts,
	.write             = usb_wwan_write,
	.write_room        = usb_wwan_write_room,
	.chars_in_buffer   = usb_wwan_chars_in_buffer,
	.set_termios       = usb_wwan_set_termios,
	.tiocmget          = usb_wwan_tiocmget,
	.tiocmset          = usb_wwan_tiocmset,
	.ioctl             = usb_wwan_ioctl,
	.attach            = option_attach,
	.release           = option_release,
	.port_probe        = usb_wwan_port_probe,
	.port_remove	   = usb_wwan_port_remove,
	.read_int_callback = option_instat_callback,
#ifdef CONFIG_PM
	.suspend           = usb_wwan_suspend,
	.resume            = usb_wwan_resume,

#if 1 // add by dimmalex for Quectel
    .reset_resume = usb_wwan_resume,
#endif

#endif
};

static struct usb_serial_driver * const serial_drivers[] = {
	&option_1port_device, NULL
};

struct option_private {
	u8 bInterfaceNumber;
};

module_usb_serial_driver(serial_drivers, option_ids);

static bool is_blacklisted(const u8 ifnum, enum option_blacklist_reason reason,
			   const struct option_blacklist_info *blacklist)
{
	unsigned long num;
	const unsigned long *intf_list;

	if (blacklist) {
		if (reason == OPTION_BLACKLIST_SENDSETUP)
			intf_list = &blacklist->sendsetup;
		else if (reason == OPTION_BLACKLIST_RESERVED_IF)
			intf_list = &blacklist->reserved;
		else {
			BUG_ON(reason);
			return false;
		}

		for_each_set_bit(num, intf_list, MAX_BL_NUM + 1) {
			if (num == ifnum)
				return true;
		}
	}
	return false;
}

static int option_probe(struct usb_serial *serial,
			const struct usb_device_id *id)
{
	struct usb_interface_descriptor *iface_desc =
				&serial->interface->cur_altsetting->desc;
	struct usb_device_descriptor *dev_desc = &serial->dev->descriptor;

#if 0 // add by dimmalex for Quectel RM500Q
	//Quectel UC20's interface 4 can be used as USB Network device
	if (serial->dev->descriptor.idVendor == cpu_to_le16(0x05C6) && serial->dev->descriptor.idProduct == cpu_to_le16(0x9003)
		&& serial->interface->cur_altsetting->desc.bInterfaceNumber >= 4)
		return -ENODEV;

	//Quectel EC20(MDM9215)'s interface 4 can be used as USB Network device
	if (serial->dev->descriptor.idVendor == cpu_to_le16(0x05C6) && serial->dev->descriptor.idProduct == cpu_to_le16(0x9215)
		&& serial->interface->cur_altsetting->desc.bInterfaceNumber >= 4)
		return -ENODEV;

	if (serial->dev->descriptor.idVendor == cpu_to_le16(0x2C7C)) {
		__u16 idProduct = le16_to_cpu(serial->dev->descriptor.idProduct);

		//Quectel module's some interfaces can be used as USB Network device (ecm, rndis, mbim)
		if (serial->interface->cur_altsetting->desc.bInterfaceClass != 0xFF)
			return -ENODEV;

		//Quectel EC25&EC20's interface 4 can be used as USB network device (qmi)
		if ((idProduct != 0x6026 && idProduct != 0x6120 && idProduct != 0x6002 ) && serial->interface->cur_altsetting->desc.bInterfaceNumber >= 4)
			return -ENODEV;
	}
#endif
#if 1 // add by dimmalex for Quectel RG500U
	if (serial->dev->descriptor.idVendor == cpu_to_le16(0x2C7C)) {
		__u16 idProduct = le16_to_cpu(serial->dev->descriptor.idProduct);
		struct usb_interface_descriptor *intf = &serial->interface->cur_altsetting->desc;

		//Quectel module's some interfaces can be used as USB Network device (ecm, rndis, mbim) add for RM500U-CM
		if (intf->bInterfaceClass != 0xFF) {
			//ECM, RNDIS, MBIM, ACM, UAC
			if (idProduct == 0x6004
		                && intf->bInterfaceNumber > 1
				&& intf->bInterfaceClass == 0x0A) {
				//EG060V cdc-acm interface force bind option
			}
			else {
				return -ENODEV;
			}
		}
		else if (intf->bInterfaceSubClass == 0x42) {
			//ADB
			return -ENODEV;
		}
		else if ((idProduct&0xF000) == 0x6000) {
			//ASR
		}
		else if ((idProduct&0xF000) == 0x8000) {
			//HISI
			if (intf->bInterfaceNumber == 0)
				return -ENODEV;
		}
		else if ((idProduct&0xFF00) == 0x0900) {
			//UNISOC
		}
		else if ((idProduct&0xF000) == 0x0000) {
			//MDM interface 4 is QMI
			if (intf->bInterfaceNumber == 4 && intf->bNumEndpoints == 3)
				return -ENODEV;
		}

	}
#endif

	/* Never bind to the CD-Rom emulation interface	*/
	if (iface_desc->bInterfaceClass == 0x08)
		return -ENODEV;

	/*
	 * Don't bind reserved interfaces (like network ones) which often have
	 * the same class/subclass/protocol as the serial interfaces.  Look at
	 * the Windows driver .INF files for reserved interface numbers.
	 */
	if (is_blacklisted(
		iface_desc->bInterfaceNumber,
		OPTION_BLACKLIST_RESERVED_IF,
		(const struct option_blacklist_info *) id->driver_info))
		return -ENODEV;
	/*
	 * Don't bind network interface on Samsung GT-B3730, it is handled by
	 * a separate module.
	 */
	if (dev_desc->idVendor == cpu_to_le16(SAMSUNG_VENDOR_ID) &&
	    dev_desc->idProduct == cpu_to_le16(SAMSUNG_PRODUCT_GT_B3730) &&
	    iface_desc->bInterfaceClass != USB_CLASS_CDC_DATA)
		return -ENODEV;

    /* add by dimmalex for fibocom l710 */
    if ( serial->dev->descriptor.idVendor == 0x2cb7 &&
        serial->dev->descriptor.idProduct == 0x0001 &&
        serial->interface->cur_altsetting->desc.bInterfaceClass == 0x7 )
		return -ENODEV;
    if ( serial->dev->descriptor.idVendor == ZTE_VENDOR_ID &&
        serial->dev->descriptor.idProduct == 0x0579 &&
        serial->interface->cur_altsetting->desc.bInterfaceClass == 0x6 )
		return -ENODEV;

    /* add by dimmalex for INNOFIDEI E7B01 */
    if (id->idVendor == 0x1d53)
    {
        struct usb_device *udev = NULL;
        udev = interface_to_usbdev(serial->interface);
        printk("Innofidei LTE modem\n");
        #ifdef CONFIG_PM
        pm_runtime_set_autosuspend_delay(&udev->dev, 10000);
        usb_enable_autosuspend(udev);
        #else
        usb_disable_autosuspend(udev);
        #endif
    }
    
	/* Store device id so we can use it during attach. */
	usb_set_serial_data(serial, (void *)id);

	return 0;
}

static int option_attach(struct usb_serial *serial)
{
	struct usb_interface_descriptor *iface_desc;
	const struct usb_device_id *id;
	struct usb_wwan_intf_private *data;
	struct option_private *priv;

	data = kzalloc(sizeof(struct usb_wwan_intf_private), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		kfree(data);
		return -ENOMEM;
	}

	/* Retrieve device id stored at probe. */
	id = usb_get_serial_data(serial);
	iface_desc = &serial->interface->cur_altsetting->desc;

	priv->bInterfaceNumber = iface_desc->bInterfaceNumber;
	data->private = priv;

	if (!is_blacklisted(iface_desc->bInterfaceNumber,
			OPTION_BLACKLIST_SENDSETUP,
			(struct option_blacklist_info *)id->driver_info)) {
		data->send_setup = option_send_setup;
	}
	spin_lock_init(&data->susp_lock);

	usb_set_serial_data(serial, data);

	return 0;
}

static void option_release(struct usb_serial *serial)
{
	struct usb_wwan_intf_private *intfdata = usb_get_serial_data(serial);
	struct option_private *priv = intfdata->private;

	kfree(priv);
	kfree(intfdata);
}

static void option_instat_callback(struct urb *urb)
{
	int err;
	int status = urb->status;
	struct usb_serial_port *port = urb->context;
	struct device *dev = &port->dev;
	struct usb_wwan_port_private *portdata =
					usb_get_serial_port_data(port);

	dev_dbg(dev, "%s: urb %p port %p has data %p\n", __func__, urb, port, portdata);

	if (status == 0) {
		struct usb_ctrlrequest *req_pkt =
				(struct usb_ctrlrequest *)urb->transfer_buffer;

		if (!req_pkt) {
			dev_dbg(dev, "%s: NULL req_pkt\n", __func__);
			return;
		}
		if ((req_pkt->bRequestType == 0xA1) &&
				(req_pkt->bRequest == 0x20)) {
			int old_dcd_state;
			unsigned char signals = *((unsigned char *)
					urb->transfer_buffer +
					sizeof(struct usb_ctrlrequest));

			dev_dbg(dev, "%s: signal x%x\n", __func__, signals);

			old_dcd_state = portdata->dcd_state;
			portdata->cts_state = 1;
			portdata->dcd_state = ((signals & 0x01) ? 1 : 0);
			portdata->dsr_state = ((signals & 0x02) ? 1 : 0);
			portdata->ri_state = ((signals & 0x08) ? 1 : 0);

			if (old_dcd_state && !portdata->dcd_state)
				tty_port_tty_hangup(&port->port, true);
		} else {
			dev_dbg(dev, "%s: type %x req %x\n", __func__,
				req_pkt->bRequestType, req_pkt->bRequest);
		}
	} else if (status == -ENOENT || status == -ESHUTDOWN) {
		dev_dbg(dev, "%s: urb stopped: %d\n", __func__, status);
	} else
		dev_err(dev, "%s: error %d\n", __func__, status);

	/* Resubmit urb so we continue receiving IRQ data */
	if (status != -ESHUTDOWN && status != -ENOENT) {
		err = usb_submit_urb(urb, GFP_ATOMIC);
		if (err)
			dev_dbg(dev, "%s: resubmit intr urb failed. (%d)\n",
				__func__, err);
	}
}

/** send RTS/DTR state to the port.
 *
 * This is exactly the same as SET_CONTROL_LINE_STATE from the PSTN
 * CDC.
*/
static int option_send_setup(struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	struct usb_wwan_intf_private *intfdata = usb_get_serial_data(serial);
	struct option_private *priv = intfdata->private;
	struct usb_wwan_port_private *portdata;
	int val = 0;
	int res;

	portdata = usb_get_serial_port_data(port);

	if (portdata->dtr_state)
		val |= 0x01;
	if (portdata->rts_state)
		val |= 0x02;

	res = usb_autopm_get_interface(serial->interface);
	if (res)
		return res;

	res = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
				0x22, 0x21, val, priv->bInterfaceNumber, NULL,
				0, USB_CTRL_SET_TIMEOUT);

	usb_autopm_put_interface(serial->interface);

	return res;
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
