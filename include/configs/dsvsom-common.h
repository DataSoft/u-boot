/*
 * Copyright 2016 DataSoft, Corp.
 *
 * Configuration settings for the dsvsom
 *
 * Based on colibri_vf.h:
 * Copyright 2014 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __DSVSOM_COMMON_H
#define __DSVSOM_COMMON_H

#define CONFIG_SYS_CACHELINE_SIZE	32

#include <asm/arch/imx-regs.h>

#define CONFIG_VF610
#define CONFIG_SYS_THUMB_BUILD
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET
#define CONFIG_SYS_FSL_CLK

#define CONFIG_ARCH_MISC_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 2 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F

/* Allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_VERSION_VARIABLE
#define CONFIG_BAUDRATE			115200

#undef CONFIG_DM_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */

#define CONFIG_CMD_EEPROM
#define CONFIG_ID_EEPROM
#define CONFIG_SYS_I2C_EEPROM_NXID
#ifndef CONFIG_SYS_I2C_EEPROM_ADDR
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#endif
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2
#ifndef CONFIG_SYS_EEPROM_BUS_NUM
#define CONFIG_SYS_EEPROM_BUS_NUM	1
#endif

/* NAND support */
#define CONFIG_CMD_NAND
#define CONFIG_CMD_WRITEBCB
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		NFC_BASE_ADDR

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=vf610_nfc"
/* WARNING: This needs to match the DFU_ALT_NAND_INFO macro below*/
#define MTDPARTS_DEFAULT	"mtdparts=vf610_nfc:"	\
				"256k(vf-bcb)ro,"	\
				"512k(u-boot)ro,"	\
				"128k(u-boot-env),"	\
                                "128k(fdt),"            \
                                "8m(kernel-image),"     \
                                "8m(user-data),"        \
                                "-(rootfs)"             \

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_UBI
#define CONFIG_MTD_UBI_FASTMAP
#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */

#define CONFIG_BOARD_LATE_INIT

#define CONFIG_LOADADDR			0x80008000
#define CONFIG_FDTADDR			0x84000000

/* We boot from the gfxRAM area of the OCRAM. */
#define CONFIG_SYS_TEXT_BASE		0x3f408000
#define CONFIG_BOARD_SIZE_LIMIT		524288

#define UBI_BOOTCMD	\
	"ubiargs=rw ubi.mtd=rootfs root=ubi0:ubi-rootfs rootfstype=ubifs " \
	"ubi.fm_autoconvert=1\0" \
	"ubiboot=run setup; " \
	"setenv bootargs ${defargs} ${ubiargs} ${mtdparts} "   \
	"${setupargs}; echo Booting from NAND...; " \
	"nand read ${kernel_addr_r} ${kernel_partition}; " \
	"nand read ${fdt_addr_r} ${fdt_partition}; " \
	"bootz ${kernel_addr_r} - ${fdt_addr_r}\0" \

#define DFU_BOOT_DELAY "1" /* How long to wait for dfu activity before booting from nand*/
#define CONFIG_BOOTCOMMAND "dfu 0 nand 0 " DFU_BOOT_DELAY "; run ubiboot"

/* WARNING: This needs to match the MTDPARTS_DEFAULT macro above*/
#define DFU_ALT_NAND_INFO       "vf-bcb part 0,1;"              \
                                "u-boot part 0,2;"              \
                                "fdt part 0,4;"                 \
                                "kernel-image part 0,5;"        \
                                "user-data part 0,6;"           \
                                "rootfs part 0,7"               \

#define CONFIG_EXTRA_ENV_SETTINGS \
	"kernel_addr_r=0x82000000\0" \
	"fdt_addr_r=0x84000000\0" \
	"kernel_partition=kernel-image\0" \
	"fdt_partition=fdt\0" \
	"console=ttyLP2\0" \
	"setup=setenv setupargs " \
	"console=${console},${baudrate}n8 ${memargs}; " \
	"setenv defargs " \
	"g_cdc.host_addr=${eth2addr} g_cdc.dev_addr=${eth3addr} ds_serial=${serial}\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"dfu_alt_info=" DFU_ALT_NAND_INFO "\0" \
	UBI_BOOTCMD

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#undef CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		\
			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	0x80010000
#define CONFIG_SYS_MEMTEST_END		0x87C00000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000
#define CONFIG_CMDLINE_EDITING

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		(128 * 1024)	/* regular stack */

/* Physical memory map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			(0x80000000)
#define PHYS_SDRAM_SIZE			(256 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */
#define CONFIG_SYS_NO_FLASH

#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(64 * 2048)
#define CONFIG_ENV_RANGE		(4 * 64 * 2048)
#define CONFIG_ENV_OFFSET		(12 * 64 * 2048)
#endif

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_CACHELINE_SIZE 32

/* USB Host Support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_VF
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET

/* USB DFU */
#define CONFIG_USB_FUNCTION_DFU
#define CONFIG_DFU_NAND
#define CONFIG_SYS_DFU_DATA_BUF_SIZE (1024 * 1024)

/* USB Storage */
#define CONFIG_USB_STORAGE
#define CONFIG_USB_FUNCTION_MASS_STORAGE

#endif
