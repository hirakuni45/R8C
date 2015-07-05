/*------------------------------------------------------------------------/
/  Bitbanging MMCv3/SDv1/SDv2 (in SPI mode) control module for PFF
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/--------------------------------------------------------------------------/
 Features:

 * Very Easy to Port
   It uses only 4-6 bit of GPIO port. No interrupt, no SPI port is used.

 * Platform Independent
   You need to modify only a few macros to control GPIO ports.

/-------------------------------------------------------------------------*/
#include "pffconf.h"
#include "diskio.h"
#include "port_def.hpp"
#include "common/delay.hpp"

static spi_base spi_base_;
static spi_ctrl spi_ctrl_;

static void dly_us_(unsigned int n)
{
	utils::delay::micro_second(n);
}

static void forward_(BYTE d)
{
}


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
static const BYTE CMD0		= (0x40+0);		/* GO_IDLE_STATE */
static const BYTE CMD1		= (0x40+1);		/* SEND_OP_COND (MMC) */
static const BYTE ACMD41	= (0xC0+41);	/* SEND_OP_COND (SDC) */
static const BYTE CMD8		= (0x40+8);		/* SEND_IF_COND */
static const BYTE CMD16		= (0x40+16);	/* SET_BLOCKLEN */
static const BYTE CMD17		= (0x40+17);	/* READ_SINGLE_BLOCK */
static const BYTE CMD24		= (0x40+24);	/* WRITE_BLOCK */
static const BYTE CMD55		= (0x40+55);	/* APP_CMD */
static const BYTE CMD58		= (0x40+58);	/* READ_OCR */

/* Card type flags (CardType) */
static const BYTE CT_MMC	= 0x01;	/* MMC ver 3 */
static const BYTE CT_SD1	= 0x02;	/* SD ver 1 */
static const BYTE CT_SD2	= 0x04;	/* SD ver 2 */
static const BYTE CT_SDC    = (CT_SD1|CT_SD2);	/* SD */
static const BYTE CT_BLOCK	= 0x08;	/* Block addressing */


static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */



/*-----------------------------------------------------------------------*/
/* Transmit a byte to the MMC (bitbanging)                               */
/*-----------------------------------------------------------------------*/
static void xmit_mmc(BYTE d)
{
	spi_base_.sda_out(d & 0x80);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x40);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x20);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x10);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x08);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x04);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x02);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
	spi_base_.sda_out(d & 0x01);
	spi_base_.scl_out(1);
	spi_base_.scl_out(0);
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from the MMC (bitbanging)                              */
/*-----------------------------------------------------------------------*/
static BYTE rcvr_mmc()
{
	BYTE r = 0;
	spi_base_.sda_out(1);

	if(spi_base_.sda_inp()) ++r; // B7
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B6
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B5
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B4
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B3
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B2
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B1
	spi_base_.scl_out(1);
	r <<= 1;
	spi_base_.scl_out(0);
	if(spi_base_.sda_inp()) ++r; // B0
	spi_base_.scl_out(1);

	spi_base_.scl_out(0);

	return r;
}



/*-----------------------------------------------------------------------*/
/* Skip bytes on the MMC (bitbanging)                                    */
/*-----------------------------------------------------------------------*/

static
void skip_mmc (
	UINT n		/* Number of bytes to skip */
)
{
	spi_base_.sda_out(1);

	do {
		spi_base_.scl_out(1); // B7
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B6
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B5
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B4
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B3
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B2
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B1
		spi_base_.scl_out(0);
		spi_base_.scl_out(1); // B0
		spi_base_.scl_out(0);
	} while (--n);
}


/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void release_spi (void)
{
	spi_ctrl_.sd_sel(1);
	rcvr_mmc();
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	spi_ctrl_.sd_sel(1);
	rcvr_mmc();

	spi_ctrl_.sd_sel(0);
	rcvr_mmc();

	/* Send a command packet */
	xmit_mmc(cmd);					/* Start + Command index */
	xmit_mmc((BYTE)(arg >> 24));	/* Argument[31..24] */
	xmit_mmc((BYTE)(arg >> 16));	/* Argument[23..16] */
	xmit_mmc((BYTE)(arg >> 8));		/* Argument[15..8] */
	xmit_mmc((BYTE)arg);			/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) */
	xmit_mmc(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcvr_mmc();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, buf[4];
	UINT tmr;

// SPI ポートは、共有している為、メイン側で初期化
//	init_port();

	spi_ctrl_.sd_sel(1);
	skip_mmc(10);			/* Dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) buf[n] = rcvr_mmc();	/* Get trailing return value of R7 resp */
			if (buf[2] == 0x01 && buf[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {				/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd(ACMD41, 1UL << 30) == 0) break;
					dly_us_(1000);
				}
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) buf[n] = rcvr_mmc();
					ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
				if (send_cmd(cmd, 0) == 0) break;
				dly_us_(1000);
			}
			if (!tmr || send_cmd(CMD16, 512) != 0)			/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Byte offset to read from (0..511) */
	UINT count		/* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE d;
	UINT bc, tmr;


	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {		/* READ_SINGLE_BLOCK */

		tmr = 1000;
		do {							/* Wait for data packet in timeout of 100ms */
			dly_us_(100);
			d = rcvr_mmc();
		} while (d == 0xFF && --tmr);

		if (d == 0xFE) {				/* A data packet arrived */
			bc = 514 - offset - count;

			/* Skip leading bytes */
			if (offset) skip_mmc(offset);

			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do
					*buff++ = rcvr_mmc();
				while (--count);
			} else {	/* Forward data to the outgoing stream */
				do {
					d = rcvr_mmc();
					forward_(d);
				} while (--count);
			}

			/* Skip trailing bytes and CRC */
			skip_mmc(bc);

			res = RES_OK;
		}
	}

	release_spi();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE

DRESULT disk_writep (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sc			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	UINT bc, tmr;
	static UINT wc;


	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = (UINT)sc;
		while (bc && wc) {		/* Send data bytes to the card */
			xmit_mmc(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write transaction */
			if (!(CardType & CT_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				xmit_mmc(0xFF); xmit_mmc(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write transaction */
			bc = wc + 2;
			while (bc--) xmit_mmc(0);	/* Fill left bytes and CRC with zeros */
			if ((rcvr_mmc() & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 300ms */
				for (tmr = 10000; rcvr_mmc() != 0xFF && tmr; tmr--)	/* Wait for ready (max 1000ms) */
					dly_us_(100);
				if (tmr) res = RES_OK;
			}
			release_spi();
		}
	}

	return res;
}
#endif
