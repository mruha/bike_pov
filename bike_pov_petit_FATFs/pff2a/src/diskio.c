/*-------------------------------------------------------------------------*/
/* PFF - Low level disk control module for AVR            (C)ChaN, 2010    */
/*-------------------------------------------------------------------------*/

#include "pff.h"
#include "diskio.h"

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include <avr/io.h>			/* Device include file */
#include "../../hdr/bike_pov_type.h"
#include "../../hdr/bike_pov_SPI_dd.h"

#define	MMC_SEL		(!(PORTB &  ( 1 << 1 )))		/* CS status (true:CS == L) */

//void init_spi (void);		/* Initialize SPI port (usi.S) */
//void SPI_delay_100us (void);		/* Delay 100 microseconds (usi.S) */

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_BLOCK			0x08	/* Block addressing */


static
BYTE CardType;


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* 1st byte (Start + Index) */
	DWORD arg		/* Argument (32 bits) */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	SPI_receive( SPI_DEVICE_SD );

	/* Send a command packet */
	SPI_send(cmd, SPI_DEVICE_SD );						/* Start + Command index */
	SPI_send((BYTE)(arg >> 24), SPI_DEVICE_SD);		/* Argument[31..24] */
	SPI_send((BYTE)(arg >> 16), SPI_DEVICE_SD);		/* Argument[23..16] */
	SPI_send((BYTE)(arg >> 8), SPI_DEVICE_SD);			/* Argument[15..8] */
	SPI_send((BYTE)arg, SPI_DEVICE_SD);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	SPI_send(n, SPI_DEVICE_SD);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = SPI_receive( SPI_DEVICE_SD );
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
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if _USE_WRITE
	if (CardType && MMC_SEL) disk_writep(0, 0);	/* Finalize write process if it is in progress */
#endif
//	init_spi();		/* Initialize ports to control MMC */
	for (n = 10; n; n--) SPI_receive( SPI_DEVICE_SD );	/* 80 dummy clocks with CS=H */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) 				/* Enter Idle state */
	{			
		if (send_cmd(CMD8, 0x1AA) == 1) 		/* SDv2 */
		{	
			for (n = 0; n < 4; n++) ocr[n] = SPI_receive( SPI_DEVICE_SD );		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) SPI_delay_100us();	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) /* Check CCS bit in the OCR */
				{		
					for (n = 0; n < 4; n++) ocr[n] = SPI_receive( SPI_DEVICE_SD );
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--) SPI_delay_100us();	/* Wait for leaving idle state */
			if (!tmr || send_cmd(CMD16, 512) != 0)			/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;

	SPI_receive( SPI_DEVICE_SD );

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD lba,		/* Sector number (LBA) */
	WORD ofs,		/* Byte offset to read from (0..511) */
	WORD cnt		/* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE rc;
	WORD bc;


	if (!(CardType & CT_BLOCK)) lba *= 512;		/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, lba) == 0) {		/* READ_SINGLE_BLOCK */

		bc = 40000;
		do {							/* Wait for data packet */
			rc = SPI_receive( SPI_DEVICE_SD );
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {				/* A data packet arrived */
			bc = 514 - ofs - cnt;

			/* Skip leading bytes */
			if (ofs) {
				do SPI_receive( SPI_DEVICE_SD ); while (--ofs);
			}

			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do {
					*buff++ = SPI_receive( SPI_DEVICE_SD );
				} while (--cnt);
			}

			/* Skip trailing bytes and CRC */
			do SPI_receive( SPI_DEVICE_SD ); while (--bc);

			res = RES_OK;
		}
	}

	SPI_receive( SPI_DEVICE_SD );

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_writep (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sa			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	WORD bc;
	static WORD wc;

	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = (WORD)sa;
		while (bc && wc) {		/* Send data bytes to the card */
			SPI_send(*buff++,SPI_DEVICE_SD);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sa) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sa *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sa) == 0) {			/* WRITE_SINGLE_BLOCK */
				SPI_send(0xFF, SPI_DEVICE_SD); SPI_send(0xFE, SPI_DEVICE_SD);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) SPI_send(0,SPI_DEVICE_SD);	/* Fill left bytes and CRC with zeros */
			if ((SPI_receive( SPI_DEVICE_SD ) & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; SPI_receive( SPI_DEVICE_SD ) != 0xFF && bc; bc--) SPI_delay_100us();	/* Wait ready */
				if (bc) res = RES_OK;
			}
			SPI_receive( SPI_DEVICE_SD );
		}
	}

	return res;
}
#endif
