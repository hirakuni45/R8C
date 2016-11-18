#pragma once
//=====================================================================//
/*!	@file
	@brief	MMC（SD カード）pFatFS ドライバー @n
			Copyright 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "pfatfs/src/diskio.h"
#include "pfatfs/src/pff.h"
#include "common/delay.hpp"

#include "common/format.hpp"

namespace pfatfs {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MMC テンプレートクラス
		@param[in]	SPI	SPI クラス
		@param[in]	SEL	デバイス選択クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SEL>
	class mmc_io {

		SPI	&spi_;

		enum class command : uint8_t {
			/* Definitions for MMC/SDC command */
			CMD0    = 0x40 + 0,		/* GO_IDLE_STATE */
			CMD1    = 0x40 + 1,		/* SEND_OP_COND (MMC) */
			ACMD41  = 0xC0 + 41,	/* SEND_OP_COND (SDC) */
			CMD8    = 0x40 + 8,		/* SEND_IF_COND */
			CMD16   = 0x40 + 16,	/* SET_BLOCKLEN */
			CMD17   = 0x40 + 17,	/* READ_SINGLE_BLOCK */
			CMD24   = 0x40 + 24,	/* WRITE_BLOCK */
			CMD55   = 0x40 + 55,	/* APP_CMD */
			CMD58   = 0x40 + 58,	/* READ_OCR */
		};
		// Card type flags (CardType)
		static const uint8_t CT_MMC	= 0x01;	/* MMC ver 3 */
		static const uint8_t CT_SD1	= 0x02;	/* SD ver 1 */
		static const uint8_t CT_SD2	= 0x04;	/* SD ver 2 */
		static const uint8_t CT_SDC = (CT_SD1|CT_SD2);	/* SD */
		static const uint8_t CT_BLOCK = 0x08;	/* Block addressing */

		BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */

		void forward_(BYTE d) { }

		//---------------------------------------------------------------//
		//  Deselect the card and release SPI bus
		//---------------------------------------------------------------//
		void release_spi_()
		{
			SEL::P = 1;
			spi_.xchg();
		}


		//---------------------------------------------------------------//
		//  Send a command packet to MMC
		//---------------------------------------------------------------//
		BYTE send_cmd_(command cm, DWORD arg) {
			uint8_t cmd = static_cast<uint8_t>(cm);
			// ACMD<n> is the command sequense of CMD55-CMD<n> */
			if(cmd & 0x80) {
				cmd &= 0x7F;
				BYTE res = send_cmd_(command::CMD55, 0);
				if(res > 1) return res;
			}

			// Select the card
			SEL::P = 1;
			spi_.xchg();

			SEL::P = 0;
			spi_.xchg();

			// Send a command packet
			uint8_t tmp[5];
			tmp[0] = cmd;
			tmp[1] = arg >> 24;  // Argument[31..24]
			tmp[2] = arg >> 16;  // Argument[23..16]
			tmp[3] = arg >> 8;   // Argument[15..8]
			tmp[4] = arg;		 // Argument[7..0]
			spi_.send(tmp, 5);
			BYTE n = 0x01;		// Dummy CRC + Stop
			if(cm == command::CMD0) n = 0x95;  // Valid CRC for CMD0(0)
			if(cm == command::CMD8) n = 0x87;  // Valid CRC for CMD8(0x1AA)
			spi_.xchg(n);

			// Receive a command response
			BYTE res;
			{
				uint8_t n = 10;  // Wait for a valid response in timeout of 10 attempts
				do {
					res = spi_.xchg();
				} while ((res & 0x80) && --n) ;
			}

			return res;  // Return with the response value
		}

		public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		mmc_io(SPI& spi) : spi_(spi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  Initialize Disk Drive
			@return ステータス
		*/
		//-----------------------------------------------------------------//
		DSTATUS disk_initialize()
		{
			spi_.start(10);  // setup slow clock

			SEL::DIR = 1;
			SEL::P = 1;
			// Dummy clocks
			for(uint8_t i = 0; i < 10; ++i) spi_.xchg();

			BYTE ty = 0;
			if(send_cmd_(command::CMD0, 0) == 1) {			/* Enter Idle state */
				if(send_cmd_(command::CMD8, 0x1AA) == 1) {	/* SDv2 */
					BYTE buf[4];
					// Get trailing return value of R7 resp
					spi_.recv(buf, 4);
					if(buf[2] == 0x01 && buf[3] == 0xAA) {	// The card can work at vdd range of 2.7-3.6V
						UINT tmr;
						for(tmr = 1000; tmr; tmr--) {      // Wait for leaving idle state (ACMD41 with HCS bit)
							if (send_cmd_(command::ACMD41, 1UL << 30) == 0) break;
							utils::delay::micro_second(1000);
						}
						if(tmr && send_cmd_(command::CMD58, 0) == 0) {  // Check CCS bit in the OCR
							spi_.recv(buf, 4);
							ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	// SDv2 (HC or SC)
						}
					}
				} else {
					// SDv1 or MMCv3
					command cmd;
					if(send_cmd_(command::ACMD41, 0) <= 1) {
						ty = CT_SD1;
						cmd = command::ACMD41;	// SDv1
					} else {
						ty = CT_MMC;
						cmd = command::CMD1;	// MMCv3
					}
					uint16_t tmr;
					for(tmr = 1000; tmr; tmr--) {  // Wait for leaving idle state
						if(send_cmd_(cmd, 0) == 0) break;
						utils::delay::micro_second(1000);
					}
					if(!tmr || send_cmd_(command::CMD16, 512) != 0) {  // Set R/W block length to 512
						ty = 0;
					}
				}
			}
			CardType = ty;
			release_spi_();

			spi_.start(0);  // boost SPI clock

		 	return ty ? 0 : STA_NOINIT;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  Read partial sector
			@param[out]	buff	Pointer to the read buffer (NULL:Read bytes are forwarded to the stream)
			@param[in]	sector	Sector number (LBA)
			@param[in]	offset	Byte offset to read from (0..511)
			@param[in]	count	Number of bytes to read (ofs + cnt mus be <= 512)
			@return リザルト
		*/
		//-----------------------------------------------------------------//
		DRESULT disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count)
		{
			if(!(CardType & CT_BLOCK)) sector *= 512;  // Convert to byte address if needed

			DRESULT res = RES_ERROR;
			if(send_cmd_(command::CMD17, sector) == 0) {  // READ_SINGLE_BLOCK
				UINT tmr = 1000;
				BYTE d;
				do {  // wait for data packet in timeout of 100ms
					utils::delay::micro_second(100);
					d = spi_.xchg();
				} while (d == 0xFF && --tmr) ;

				if(d == 0xFE) {  // A data packet arrived
					UINT bc = 514 - offset - count;

					// Skip leading bytes
					if(offset) {
						spi_.skip(offset);
					}

					// Receive a part of the sector
					if(buff) {  // Store data to the memory
						spi_.recv(buff, count);
					} else {	/* Forward data to the outgoing stream */
						do {
							auto d = spi_.xchg();
							forward_(d);
						} while(--count) ;
					}
					// Skip trailing bytes and CRC
					spi_.skip(bc);
					res = RES_OK;
				}
			}
			release_spi_();
			return res;
		}


#if _USE_WRITE
		//-----------------------------------------------------------------//
		/*!
			@brief  Write partial sector
			@param[in]	buff	Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) 
			@param[in]	sc		Number of bytes to send, Sector number (LBA) or zero 
			@return リザルト
		*/
		//-----------------------------------------------------------------//
		DRESULT disk_writep(const BYTE* buff, DWORD sc)
		{
			static UINT wc;

			DRESULT res = RES_ERROR;

			UINT bc;
			if(buff) { // Send data bytes
				bc = static_cast<UINT>(sc);
				while (bc && wc) {  // Send data bytes to the card
					spi_.xchg(*buff++);
					wc--;
					bc--;
				}
				res = RES_OK;
			} else {
				if(sc) {	// Initiate sector write transaction
					if(!(CardType & CT_BLOCK)) sc *= 512;	// Convert to byte address if needed
					if(send_cmd_(command::CMD24, sc) == 0) {  // WRITE_SINGLE_BLOCK
						spi_.xchg(0xFF);
						spi_.xchg(0xFE);  // Data block header
						wc = 512;		  // Set byte counter
						res = RES_OK;
					}
				} else {	// Finalize sector write transaction
					bc = wc + 2;
					while(bc--) {
						spi_.xchg(0);	// Fill left bytes and CRC with zeros
					}
					// Receive data resp and wait for end of write process in timeout of 300ms
					if((spi_.xchg() & 0x1F) == 0x05) {
						// Wait for ready (max 1000ms)
						uint16_t tmr;
						for(tmr = 10000; spi_.xchg() != 0xFF && tmr; tmr--) {
							utils::delay::micro_second(100);
						}
						if(tmr) res = RES_OK;
					}
					release_spi_();
				}
			}
			return res;
		}
#endif
	};
}
