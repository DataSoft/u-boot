/*
 * Copyright (c) 2013, Toradex AG.  All rights reserved.
 *
 * Derived from downstream U-Boot (drivers/mtd/nand/fsl_nfc.c)
 * and mxsboot
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <nand.h>
#include <malloc.h>

struct mxs_nand_fcb {
	uint32_t		checksum;
	uint32_t		fingerprint;
	uint32_t		version;
	struct {
		uint8_t			data_setup;
		uint8_t			data_hold;
		uint8_t			address_setup;
		uint8_t			dsample_time;
		uint8_t			nand_timing_state;
		uint8_t			rea;
		uint8_t			rloh;
		uint8_t			rhoh;
	}			timing;
	uint32_t		page_data_size;
	uint32_t		total_page_size;
	uint32_t		sectors_per_block;
	uint32_t		number_of_nands;		/* Ignored */
	uint32_t		total_internal_die;		/* Ignored */
	uint32_t		cell_type;			/* Ignored */
	uint32_t		ecc_block_n_ecc_type;
	uint32_t		ecc_block_0_size;
	uint32_t		ecc_block_n_size;
	uint32_t		ecc_block_0_ecc_type;
	uint32_t		metadata_bytes;
	uint32_t		num_ecc_blocks_per_page;
	uint32_t		ecc_block_n_ecc_level_sdk;	/* Ignored */
	uint32_t		ecc_block_0_size_sdk;		/* Ignored */
	uint32_t		ecc_block_n_size_sdk;		/* Ignored */
	uint32_t		ecc_block_0_ecc_level_sdk;	/* Ignored */
	uint32_t		num_ecc_blocks_per_page_sdk;	/* Ignored */
	uint32_t		metadata_bytes_sdk;		/* Ignored */
	uint32_t		erase_threshold;
	uint32_t		boot_patch;
	uint32_t		patch_sectors;
	uint32_t		firmware1_starting_sector;
	uint32_t		firmware2_starting_sector;
	uint32_t		sectors_in_firmware1;
	uint32_t		sectors_in_firmware2;
	uint32_t		dbbt_search_area_start_address;
	uint32_t		badblock_marker_byte;
	uint32_t		badblock_marker_start_bit;
	uint32_t		bb_marker_physical_offset;
	uint32_t		reserved1[9];
	uint32_t		disbbm;
	uint32_t		reserved2[10];
	uint32_t		disbbm_search;
	uint32_t		disbbm_search_limit;
};

struct mxs_nand_dbbt {
	uint32_t		checksum;
	uint32_t		fingerprint;
	uint32_t		version;
	uint32_t		number_bb;
	uint32_t		number_2k_pages_bb;
};

struct mxs_nand_bbt {
	uint32_t		nand;
	uint32_t		number_bb;
	uint32_t		badblock[510];
};

static inline uint8_t parity_13_8(const uint8_t b)
{
	uint32_t parity = 0, tmp;

	tmp = ((b >> 6) ^ (b >> 5) ^ (b >> 3) ^ (b >> 2)) & 1;
	parity |= tmp << 0;

	tmp = ((b >> 7) ^ (b >> 5) ^ (b >> 4) ^ (b >> 2) ^ (b >> 1)) & 1;
	parity |= tmp << 1;

	tmp = ((b >> 7) ^ (b >> 6) ^ (b >> 5) ^ (b >> 1) ^ (b >> 0)) & 1;
	parity |= tmp << 2;

	tmp = ((b >> 7) ^ (b >> 4) ^ (b >> 3) ^ (b >> 0)) & 1;
	parity |= tmp << 3;

	tmp = ((b >> 6) ^ (b >> 4) ^ (b >> 3) ^
		(b >> 2) ^ (b >> 1) ^ (b >> 0)) & 1;
	parity |= tmp << 4;

	return parity;
}

static void create_fcb(struct mtd_info *mtd, uint8_t *buf, int fw1_start_address,
		int fw2_start_address)
{
	int i;
	uint8_t *ecc;
	struct mxs_nand_fcb *fcb = (struct mxs_nand_fcb *)buf;

	fcb->fingerprint =		0x46434220;
	fcb->version =			0x00000001;

	fcb->page_data_size =		mtd->writesize;
	fcb->total_page_size =		mtd->writesize + mtd->oobsize;
	fcb->sectors_per_block =	mtd->erasesize / mtd->writesize;

	/* 45 or 60-bytes BCH ECC, also used by Linux/U-Boot */
#if defined(CONFIG_SYS_NAND_VF610_NFC_45_ECC_BYTES)
	fcb->ecc_block_0_ecc_type = 6;
#elif defined(CONFIG_SYS_NAND_VF610_NFC_60_ECC_BYTES)
	fcb->ecc_block_0_ecc_type = 7;
#endif

	fcb->firmware1_starting_sector = fw1_start_address / mtd->writesize;
	fcb->firmware2_starting_sector = fw2_start_address / mtd->writesize;

	fcb->dbbt_search_area_start_address =	0;

	/* This is typically the first byte of the pages OOB area */
	fcb->bb_marker_physical_offset = mtd->writesize;

	/*
	 * Enable swapping of bad block marker byte (required for boot ROM in
	 * order to detect factory marked bad blocks)
	 */
	fcb->disbbm = 0;

	fcb->disbbm_search = 0;
	fcb->disbbm_search_limit = 8;

	ecc = buf + 512;

	for (i = 0; i < sizeof(struct mxs_nand_fcb); i++)
		ecc[i] = parity_13_8(buf[i]);
}

static int do_write_bcb(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
	int ret;
	struct mtd_info *mtd;
	int dev = nand_curr_device;
	ulong off = 0;
	uint8_t *buf;
	size_t rwsize, maxsize;
	ulong fw1_off, fw2_off = 0;

	if (argc < 2)
		return -1;

	fw1_off = simple_strtoul(argv[1], NULL, 16);
	if (argc > 2)
		fw2_off = simple_strtoul(argv[2], NULL, 16);

	mtd = nand_info[dev];

	/* Allocate one page, should be enought */
	rwsize = mtd->writesize;
	buf = malloc(mtd->writesize);

	/* Set only the first page empty... */
	memset(buf, 0, mtd->writesize);

	create_fcb(mtd, buf, fw1_off, fw2_off);

	puts("Write FCB...\n");
	rwsize = maxsize = mtd->writesize;
	ret = nand_write(mtd, off, &rwsize, (unsigned char *)buf);

	printf("FCB %d bytes written to 0x0: %s\n", rwsize, 
			ret ? "ERROR" : "OK");
	return 0;
}

U_BOOT_CMD(
	writebcb, 3, 0, do_write_bcb,
	"Write Boot Control Block (FCB and DBBT)",
	"fw1-off [fw2-off]"
);
