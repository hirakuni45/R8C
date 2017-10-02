#pragma once
//=====================================================================//
/*!	@file
	@brief	MCP2515 CAN ドライバー @n
			※MCP2515 の電源は２．７Ｖ～５．５Ｖ（３．３Ｖ、５Ｖが可能）@n
			※ドライバーの電源は通常５Ｖなので、電源が分離されていない場合は５Ｖ駆動
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>
#include <cstring>
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MCP2515 発信周波数型
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	enum class MCP2515_OSC : uint8_t {
		OSC_8MHZ,	///<  8MHz クリスタル
		OSC_16MHZ,	///< 16MHz クリスタル
		OSC_20MHZ,	///< 20MHz クリスタル
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MCP2515 テンプレートクラス
		@param[in]	SPI	SPI クラス
		@param[in]	SEL	選択クラス
		@param[in]	OSC	発信周波数型
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SEL, MCP2515_OSC OSC = MCP2515_OSC::OSC_8MHZ>
	class MCP2515 {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  動作モード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class MODE : uint8_t {
			NORMAL     = 0x00,
			SLEEP      = 0x20,
			LOOPBACK   = 0x40,
			LISTENONLY = 0x60,
			CONFIG     = 0x80,
			POWERUP    = 0xE0,
			MASK       = 0xE0,
		};

#if 0
/*
 *   CANCTRL Register Values
 */
#define ABORT_TX        0x10
#define CLKOUT_ENABLE   0x04
#define CLKOUT_DISABLE  0x00
#define CLKOUT_PS1      0x00
#define CLKOUT_PS2      0x01
#define CLKOUT_PS4      0x02
#define CLKOUT_PS8      0x03
#endif


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  ＩＤモード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class ID_MODE : uint8_t {
			ANY,	///<  
			STD,	///<
			EXT,	///<
			STDEXT,	///<
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  速度
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class SPEED : uint8_t {
			BPS_5K,		///< 5K
			BPS_10K,	///< 10K
			BPS_20K,	///< 20K
			BPS_31K25,	///< 31.25K
			BPS_33K33,	///< 33.33K
			BPS_40K,	///< 40K
			BPS_50K,	///< 50K
			BPS_80K,	///< 80K
			BPS_100K,	///< 100K
			BPS_125K,	///< 125K
			BPS_200K,	///< 200K
			BPS_250K,	///< 250K
			BPS_500K,	///< 500K
			BPS_1000K,	///< 1000K (1M)
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  エラー定義
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static const uint8_t MCP_EFLG_RX1OVR    = 1 << 7;
		static const uint8_t MCP_EFLG_RX0OVR    = 1 << 6;
		static const uint8_t MCP_EFLG_TXBO      = 1 << 5;
		static const uint8_t MCP_EFLG_TXEP      = 1 << 4;
		static const uint8_t MCP_EFLG_RXEP      = 1 << 3;
		static const uint8_t MCP_EFLG_TXWAR     = 1 << 2;
		static const uint8_t MCP_EFLG_RXWAR     = 1 << 1;
		static const uint8_t MCP_EFLG_EWARN     = 1 << 0;
		static const uint8_t MCP_EFLG_ERRORMASK = 0xF8;

	private:
		enum class REGA : uint8_t {
			RXF0SIDH  = 0x00,
			RXF0SIDL  = 0x01,
			RXF0EID8  = 0x02,
			RXF0EID0  = 0x03,
			RXF1SIDH  = 0x04,
			RXF1SIDL  = 0x05,
			RXF1EID8  = 0x06,
			RXF1EID0  = 0x07,
			RXF2SIDH  = 0x08,
			RXF2SIDL  = 0x09,
			RXF2EID8  = 0x0A,
			RXF2EID0  = 0x0B,
			BFPCTRL   = 0x0C,
			TXRTSCTRL = 0x0D,
			CANSTAT   = 0x0E,
			CANCTRL   = 0x0F,
			RXF3SIDH  = 0x10,
			RXF3SIDL  = 0x11,
			RXF3EID8  = 0x12,
			RXF3EID0  = 0x13,
			RXF4SIDH  = 0x14,
			RXF4SIDL  = 0x15,
			RXF4EID8  = 0x16,
			RXF4EID0  = 0x17,
			RXF5SIDH  = 0x18,
			RXF5SIDL  = 0x19,
			RXF5EID8  = 0x1A,
			RXF5EID0  = 0x1B,
			TEC       = 0x1C,
			REC       = 0x1D,
			RXM0SIDH  = 0x20,
			RXM0SIDL  = 0x21,
			RXM0EID8  = 0x22,
			RXM0EID0  = 0x23,
			RXM1SIDH  = 0x24,
			RXM1SIDL  = 0x25,
			RXM1EID8  = 0x26,
			RXM1EID0  = 0x27,
			CNF3      = 0x28,
			CNF2      = 0x29,
			CNF1      = 0x2A,
			CANINTE   = 0x2B,
			CANINTF   = 0x2C,
			EFLG      = 0x2D,
			TXB0CTRL  = 0x30,
			TXB1CTRL  = 0x40,
			TXB2CTRL  = 0x50,
			RXB0CTRL  = 0x60,
			RXB0SIDH  = 0x61,
			RXB1CTRL  = 0x70,
			RXB1SIDH  = 0x71,
		};

		static const uint8_t MCP_READ    = 0x02;
		static const uint8_t MCP_WRITE   = 0x03;
		static const uint8_t MCP_BITMOD  = 0x05;
		static const uint8_t MCP_STATUS  = 0xA0;
		static const uint8_t MCP_RESET   = 0xC0;

		static const uint8_t MCP_RX0IF   = 0x01;
		static const uint8_t MCP_RX1IF   = 0x02;
		static const uint8_t MCP_TX0IF   = 0x04;
		static const uint8_t MCP_TX1IF   = 0x08;
		static const uint8_t MCP_TX2IF   = 0x10;
		static const uint8_t MCP_ERRIF   = 0x20;
		static const uint8_t MCP_WAKIF   = 0x40;
		static const uint8_t MCP_MERRF   = 0x80;

		static const uint8_t MCP_BxBFS_MASK   = 0x30;
		static const uint8_t MCP_BxBFE_MASK   = 0x0C;
		static const uint8_t MCP_BxBFM_MASK   = 0x03;

		static const uint8_t MCP_BxRTS_MASK   = 0x38;
		static const uint8_t MCP_BxRTSM_MASK  = 0x07;

		static const uint8_t MCP_RXB_RX_ANY    = 0x60;
		static const uint8_t MCP_RXB_RX_EXT    = 0x40;
		static const uint8_t MCP_RXB_RX_STD    = 0x20;
		static const uint8_t MCP_RXB_RX_STDEXT = 0x00;
		static const uint8_t MCP_RXB_RX_MASK   = 0x60;
		static const uint8_t MCP_RXB_BUKT_MASK = 1 << 2;

		static const uint8_t MCP_TXB_TXBUFE_M = 0x80;
		static const uint8_t MCP_TXB_ABTF_M   = 0x40;
		static const uint8_t MCP_TXB_MLOA_M   = 0x20;
		static const uint8_t MCP_TXB_TXERR_M  = 0x10;
		static const uint8_t MCP_TXB_TXREQ_M  = 0x08;
		static const uint8_t MCP_TXB_TXIE_M   = 0x04;
		static const uint8_t MCP_TXB_TXP10_M  = 0x03;

		static const uint8_t MCP_STAT_RXIF_MASK = 0x03;
		static const uint8_t MCP_STAT_RX0IF = 1 << 0;
		static const uint8_t MCP_STAT_RX1IF = 1 << 1;

		static const uint8_t MCP_DLC_MASK = 0x0F;  // 4 LSBits

		static const uint8_t MCP_SIDH = 0;
		static const uint8_t MCP_SIDL = 1;
		static const uint8_t MCP_EID8 = 2;
		static const uint8_t MCP_EID0 = 3;

		static const uint8_t MCP_TXB_EXIDE_M = 0x08;  // In TXBnSIDL


		SPI&		spi_;

		MODE		mode_;

		uint32_t	send_id_;
		uint8_t		send_msg_[8];
		uint8_t		send_rtr_;
		uint8_t		send_ext_;
		uint8_t		send_len_;

		uint32_t	recv_id_;
		uint8_t		recv_msg_[8];
		uint8_t		recv_rtr_;
		uint8_t		recv_ext_;
		uint8_t		recv_len_;


		uint8_t read_(REGA adrs) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_READ);
			spi_.xchg(static_cast<uint8_t>(adrs));
			uint8_t ret = spi_.xchg();
			SEL::P = 1;
			return ret;
		}


		void read_(REGA adrs, void* dst, uint8_t len) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_READ);
			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.recv(dst, len);
			SEL::P = 1;
		}


		void write_(REGA adrs, uint8_t data) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_WRITE);
			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.xchg(data);
			SEL::P = 1;
		}


		void write_(REGA adrs, const void* src, uint8_t len) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_WRITE);
			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.send(src, len);
			SEL::P = 1;
		}


		void modify_(REGA adrs, uint8_t mask, uint8_t data) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_BITMOD);
   			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.xchg(mask);
			spi_.xchg(data);
			SEL::P = 1;
		}


		uint8_t status_() noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_STATUS);
    		auto sts = spi_.xchg();
			SEL::P = 1;
			return sts;
		}


		void write_mf_(REGA adrs, bool ext, uint32_t id) noexcept
		{
			uint16_t canid = id & 0x0FFFF;
			uint8_t tmp[4];

			if(ext) {
				tmp[MCP_EID0] = canid & 0xFF;
				tmp[MCP_EID8] = canid >> 8;
				canid = id >> 16;
				tmp[MCP_SIDL] = canid & 0x03;
				tmp[MCP_SIDL] += (canid & 0x1C) << 3;
				tmp[MCP_SIDL] |= MCP_TXB_EXIDE_M;
				tmp[MCP_SIDH] = canid >> 5;
			} else {
				tmp[MCP_EID0] = canid & 0xFF;
				tmp[MCP_EID8] = canid >> 8;
				canid = id >> 16;
				tmp[MCP_SIDL] = (canid & 0x07) << 5;
				tmp[MCP_SIDH] = canid >> 3;
			}
			write_(adrs, tmp, 4);
		}


		void write_id_(REGA mcp_addr, bool ext, uint32_t id)
		{
			uint8_t tmp[4];
			uint16_t canid = id & 0x0FFFF;

			if(ext) {
				tmp[MCP_EID0] = canid & 0xFF;
				tmp[MCP_EID8] = canid >> 8;
				canid = id >> 16;
				tmp[MCP_SIDL] = canid & 0x03;
				tmp[MCP_SIDL] += (canid & 0x1C) << 3;
				tmp[MCP_SIDL] |= MCP_TXB_EXIDE_M;
				tmp[MCP_SIDH] = canid >> 5;
			} else {
				tmp[MCP_SIDH] = canid >> 3;
				tmp[MCP_SIDL] = (canid & 0x07 ) << 5;
				tmp[MCP_EID0] = 0;
				tmp[MCP_EID8] = 0;
			}
    		write_(mcp_addr, tmp, 4);
		}


		void read_id_(REGA mcp_addr, uint8_t& ext, uint32_t& id)
		{
			uint8_t tmp[4];
			ext = 0;
			id = 0;
			read_(mcp_addr, tmp, 4);

			id = (static_cast<uint32_t>(tmp[MCP_SIDH]) << 3) | (static_cast<uint32_t>(tmp[MCP_SIDL]) >> 5);

			if((tmp[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M) {  // extended id
				id = (id << 2) | static_cast<uint32_t>(tmp[MCP_SIDL] & 0x03);
				id = (id << 8) | static_cast<uint32_t>(tmp[MCP_EID8]);
				id = (id << 8) + tmp[MCP_EID0];
				ext = 1;
			}
		}


		void reset_() noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(MCP_RESET);
			SEL::P = 1;
			utils::delay::micro_second(10);
		}


		bool set_ctrl_mode_(MODE mode) noexcept
		{
			modify_(REGA::CANCTRL, static_cast<uint8_t>(MODE::MASK), static_cast<uint8_t>(mode));
			auto ret = read_(REGA::CANCTRL);
			ret &= static_cast<uint8_t>(MODE::MASK);
			return ret == static_cast<uint8_t>(mode);
		}


		void init_buffers_(uint32_t mask = 0, uint32_t filt = 0) noexcept
		{
			bool ext = true;
			write_mf_(REGA::RXM0SIDH, ext, mask);  // Set both masks to 0
			write_mf_(REGA::RXM1SIDH, ext, mask);  // Mask register ignores ext bit

			bool std = false;
			// Set all filters to 0
			write_mf_(REGA::RXF0SIDH, ext, filt);  // RXB0: extended
			write_mf_(REGA::RXF1SIDH, std, filt);  // RXB1: standard
			write_mf_(REGA::RXF2SIDH, ext, filt);  // RXB2: extended
			write_mf_(REGA::RXF3SIDH, std, filt);  // RXB3: standard
			write_mf_(REGA::RXF4SIDH, ext, filt);
			write_mf_(REGA::RXF5SIDH, std, filt);

			// Clear, deactivate the three
			// transmit buffers
			// TXBnCTRL -> TXBnD7
			REGA a1 = REGA::TXB0CTRL;
			REGA a2 = REGA::TXB1CTRL;
			REGA a3 = REGA::TXB2CTRL;
			for(uint8_t i = 0; i < 14; ++i) {
				write_(a1, 0);
				write_(a2, 0);
				write_(a3, 0);
				a1 = static_cast<REGA>(static_cast<uint8_t>(a1) + 1);
				a2 = static_cast<REGA>(static_cast<uint8_t>(a2) + 1);
				a3 = static_cast<REGA>(static_cast<uint8_t>(a3) + 1);
			}
			write_(REGA::RXB0CTRL, 0);
			write_(REGA::RXB1CTRL, 0);
		}


		bool config_rate_(SPEED speed) noexcept
		{
			static const uint8_t cfg123[] = {
				/*    5K    at  8MHz */	0x1F, 0xBF, 0x87,
				/*    5K    at 16MHz */	0x3F, 0xFF, 0x87,
				/*    5K    at 20MHz */	0x00, 0x00, 0x00,  // fail
				/*   10K    at  8MHz */	0x0F, 0xBF, 0x87,
				/*   10K    at 16MHz */	0x1F, 0xFF, 0x87,
				/*   10K    at 20MHz */	0x00, 0x00, 0x00,  // fail
				/*   20K    at  8MHz */	0x07, 0xBF, 0x87,
				/*   20K    at 16MHz */	0x0F, 0xFF, 0x87,
				/*   20K    at 20MHz */	0x00, 0x00, 0x00,  // fail
				/*   31.25K at  8MHz */	0x07, 0xA4, 0x84,
				/*   31.25K at 16MHz */	0x00, 0x00, 0x00,  // fail
				/*   31.25K at 20MHz */	0x00, 0x00, 0x00,  // fail
				/*   33.33K at  8MHz */	0x47, 0xE2, 0x85,
				/*   33.33K at 16MHz */	0x4E, 0xF1, 0x85,
				/*   33.33K at 20MHz */	0x00, 0x00, 0x00,  // fail
				/*   40K    at  8MHz */	0x03, 0xBF, 0x87,
				/*   40K    at 16MHz */	0x07, 0xFF, 0x87,
				/*   40K    at 20MHz */	0x09, 0xFF, 0x87,
				/*   50K    at  8MHz */	0x03, 0xB4, 0x86,
				/*   50K    at 16MHz */	0x07, 0xFA, 0x87,
				/*   50K    at 20MHz */	0x09, 0xFA, 0x87,
				/*   80K    at  8MHz */	0x01, 0xBF, 0x87,
				/*   80K    at 16MHz */	0x03, 0xFF, 0x87,
				/*   80K    at 20MHz */	0x04, 0xFF, 0x87,
				/*  100K    at  8MHz */	0x01, 0xB4, 0x86,
				/*  100K    at 16MHz */	0x03, 0xFA, 0x87,
				/*  100K    at 20MHz */	0x04, 0xFA, 0x87,
				/*  125K    at  8MHz */	0x01, 0xB1, 0x85,
				/*  125K    at 16MHz */	0x03, 0xF0, 0x86,
				/*  125K    at 20MHz */	0x03, 0xFA, 0x87,
				/*  200K    at  8MHz */	0x00, 0xB4, 0x86,
				/*  200K    at 16MHz */	0x01, 0xFA, 0x87,
				/*  200K    at 20MHz */	0x01, 0xFF, 0x87,
				/*  250K    at  8MHz */	0x00, 0xB1, 0x85,
				/*  250K    at 16MHz */	0x41, 0xF1, 0x85,
				/*  250K    at 20MHz */	0x41, 0xFB, 0x86,
				/*  500K    at  8MHz */	0x00, 0x90, 0x82,
				/*  500K    at 16MHz */	0x00, 0xF0, 0x86,
				/*  500K    at 20MHz */	0x00, 0xFA, 0x87,
				/* 1000K    at  8MHz */	0x00, 0x80, 0x80,
				/* 1000K    at 16MHz */	0x00, 0xD0, 0x82,
				/* 1000K    at 20MHz */	0x00, 0xD9, 0x82,
			};

			uint8_t idx = static_cast<uint8_t>(speed) * 3 * static_cast<uint8_t>(OSC);
			auto cfg1 = cfg123[idx + 0];
			auto cfg2 = cfg123[idx + 1];
			auto cfg3 = cfg123[idx + 2];

			if(cfg1 == 0 && cfg2 == 0 && cfg3 == 0) {
				return false;
			}

			write_(REGA::CNF1, cfg1);
			write_(REGA::CNF2, cfg2);
			write_(REGA::CNF3, cfg3);

			return true;
		}


		bool get_next_free_tx_(uint8_t& ans)
		{
			static const REGA ctrlregs[] = {
				REGA::TXB0CTRL, REGA::TXB1CTRL, REGA::TXB2CTRL
			};

			ans = 0x00;
			// check all 3 TX-Buffers
			for(uint8_t i = 0; i < sizeof(ctrlregs); ++i) {
				auto ctrlval = read_(ctrlregs[i]);
				if((ctrlval & MCP_TXB_TXREQ_M) == 0) {
					// return SIDH-address of Buffer
					ans = static_cast<uint8_t>(ctrlregs[i]) + 1;
					return true;
				}
			}
			return false;
		}


		void write_can_msg_(uint8_t adrs)
		{
			uint8_t mcp_addr;
			mcp_addr = adrs;
			// write data bytes
			write_(static_cast<REGA>(mcp_addr + 5), send_msg_, send_len_);

		    if(send_rtr_ == 1) {  // if RTR set bit in byte
				static const uint8_t MCP_RTR_MASK = 0x40;  // (1<<6) Bit 6
				send_len_ |= MCP_RTR_MASK;
			}

			// write the RTR and DLC
			write_(static_cast<REGA>(mcp_addr+4), send_len_);
            // write CAN id
			write_id_(static_cast<REGA>(mcp_addr), send_ext_, send_id_);
		}


		void read_can_msg_(uint8_t adrs)
		{
			auto mcp_addr = adrs;
			read_id_(static_cast<REGA>(mcp_addr), recv_ext_, recv_id_);

			auto ctrl = read_(static_cast<REGA>(mcp_addr - 1));
			recv_len_ = read_(static_cast<REGA>(mcp_addr + 4));

			if(ctrl & 0x08) {
				recv_rtr_ = 1;
			} else {
				recv_rtr_ = 0;
			}
			recv_len_ &= MCP_DLC_MASK;
			read_(static_cast<REGA>(mcp_addr + 5), recv_msg_, recv_len_);
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクタ
		 */
		//-----------------------------------------------------------------//
		MCP2515(SPI& spi) noexcept : spi_(spi), mode_(MODE::NORMAL) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		 */
		//-----------------------------------------------------------------//
		bool init(ID_MODE id_mode, SPEED speed)
		{
			reset_();

			if(!set_ctrl_mode_(MODE::CONFIG)) {
				return false;
			}

			// Set Baudrate
			if(!config_rate_(speed)) {
				return false;
			}

			init_buffers_();

			// interrupt mode
		
	        write_(REGA::CANINTE, MCP_RX0IF | MCP_RX1IF);

			// Sets BF pins as GPO
			write_(REGA::BFPCTRL, MCP_BxBFS_MASK | MCP_BxBFE_MASK);
			// Sets RTS pins as GPI
			write_(REGA::TXRTSCTRL, 0x00);

			switch(id_mode) {
			case ID_MODE::ANY:
				modify_(REGA::RXB0CTRL,
					MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
					MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
				modify_(REGA::RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_ANY);
				break;
#if 0
			// The followingn two functions of the MCP2515 do not work, there is a bug in the silicon.
            case ID_MODE::STD: 
				modify_(REGA::RXB0CTRL,
					MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
					MCP_RXB_RX_STD | MCP_RXB_BUKT_MASK );
				modify_(REGA::RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_STD);
            break;

            case ID_MODE::EXT: 
				modify_(REGA::RXB0CTRL,
					MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
					MCP_RXB_RX_EXT | MCP_RXB_BUKT_MASK );
				modify_(REGA::RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_EXT);
            break;
#endif

			case ID_MODE::STDEXT: 
				modify_(REGA::RXB0CTRL,
					MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
					MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
				modify_(REGA::RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_STDEXT);
				break;
    
            default:
				return false;
			}

        	return set_ctrl_mode_(MODE::LOOPBACK);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	id_mode	ID モード
			@param[in]	speed	CAN 速度
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool start(ID_MODE id_mode, SPEED speed) noexcept
		{
			SEL::DIR = 1;  // output
			SEL::P = 1;    // device disable

			return init(id_mode, speed);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	モード設定
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_mode(MODE mode) noexcept
		{
			mode_ = mode;
			return set_ctrl_mode_(mode);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マスクの初期化
			@param[in]	num		種別
			@param[in]	ext		拡張
			@param[in]	data	データ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool init_mask(uint8_t num, uint8_t ext, uint32_t data) noexcept
		{
			if(!set_ctrl_mode_(MODE::CONFIG)) {
				return false;
			}
    
			bool ret = true;
			if(num == 0) {
				write_mf_(REGA::RXM0SIDH, ext, data);
			} else if(num == 1) {
				write_mf_(REGA::RXM1SIDH, ext, data);
			} else {
				ret = false;
			}
    
			if(!set_ctrl_mode(mode_)) {
				return false;
			}

			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フィルターの初期化
			@param[in]	num		種別
			@param[in]	ext		拡張
			@param[in]	data	データ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool init_filt(uint8_t num, uint8_t ext, uint32_t data) noexcept
		{
			if(!set_ctrl_mode_(MODE::CONFIG)) {
				return false;
			}
    
//			uint8_t ext = 0;
//			if((data & 0x80000000) == 0x80000000) ext = 1;

			bool ret = true;
			switch( num ) {

			case 0:
				write_mf_(REGA::RXF0SIDH, ext, data);
				break;

			case 1:
				write_mf_(REGA::RXF1SIDH, ext, data);
				break;

			case 2:
				write_mf_(REGA::RXF2SIDH, ext, data);
				break;

			case 3:
				write_mf_(REGA::RXF3SIDH, ext, data);
				break;

			case 4:
				write_mf_(REGA::RXF4SIDH, ext, data);
				break;

			case 5:
				write_mf_(REGA::RXF5SIDH, ext, data);
				break;

			default:
				ret = false;
			}

			if(!set_ctrl_mode(mode_)) {
				return false;
			}
    
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メッセージ設定
			@param[in]	id	ID
			@param[in]	rtr	RTR 値
			@param[in]	ext	拡張フラグ
			@param[in]	src	ソース
			@param[in]	len	送信バイト数（最大８バイト）
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_msg(uint32_t id, uint8_t rtr, uint8_t ext, const void* src, uint8_t len) noexcept
		{
			if(len >= sizeof(send_msg_)) {
				return false;
			}
			send_id_  = id;
			send_rtr_ = rtr;
			send_ext_ = ext;
			send_len_ = len;
			uint8_t l = len;
			if(l > sizeof(send_msg_)) l = sizeof(send_msg_);
			std::memcpy(send_msg_, src, l);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メッセージ送信
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool send_msg() noexcept
		{
			static const uint16_t timeoutloop = 50;
			uint16_t loop = 0;
			uint8_t txbuf_n;
			bool ret;
			do {
				ret = get_next_free_tx_(txbuf_n);
				++loop;
			} while(!ret && (loop < timeoutloop)) ;

			if(loop >= timeoutloop) {  // get tx buff time out
        		return false;
			}

			write_can_msg_(txbuf_n);
			modify_(static_cast<REGA>(txbuf_n - 1), MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M);

			loop = 0;
			uint8_t res;
			do {
				++loop;
				// read send buff ctrl reg
				res = read_(static_cast<REGA>(txbuf_n - 1));
				res &= 0x08;
			} while (res && (loop < timeoutloop)) ;   

			// send msg timeout
			if(loop == timeoutloop) {
				return false;
    		}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メッセージ受信
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool recv_msg() noexcept
		{
			auto stat = status_();

			if(stat & MCP_STAT_RX0IF) {  // Msg in Buffer 0
				read_can_msg_(static_cast<uint8_t>(REGA::RXB0SIDH));
				modify_(REGA::CANINTF, MCP_RX0IF, 0);
			} else if(stat & MCP_STAT_RX1IF) {  // Msg in Buffer 1
				read_can_msg_(static_cast<uint8_t>(REGA::RXB1SIDH));
				modify_(REGA::CANINTF, MCP_RX1IF, 0);
			} else { 
				return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	送信
			@param[in]	id	ID
			@param[in]	ext	拡張フラグ
			@param[in]	src	ソース
			@param[in]	len	送信バイト数
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool send(uint32_t id, uint8_t ext, const void* src, uint8_t len) noexcept
		{
			set_msg(id, 0, ext, src, len);
			return send_msg();
    	}


		//-----------------------------------------------------------------//
		/*!
			@brief	受信
			@param[out]	id	ID
			@param[out]	ext	拡張フラグ
			@param[out]	dst	ソース
			@param[out]	len	受信バイト数
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool recv(uint32_t& id, uint8_t& ext, void* dst, uint8_t& len) noexcept
		{
			if(recv_msg()) {
				return false;
			}

			id  = recv_id_;
			len = recv_len_;
			ext = recv_ext_;
			std::memcpy(dst, recv_msg_, recv_len_);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レシーブ・チェック
			@return レシーブなら「true」
		 */
		//-----------------------------------------------------------------//
		bool check_recv() noexcept
		{
			auto res = status_();  // RXnIF in Bit 1 and 0
			return (res & MCP_STAT_RXIF_MASK) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーの取得（警告を含む）
			@return エラー
		 */
		//-----------------------------------------------------------------//
		uint8_t get_error() noexcept
		{
			return read_(REGA::EFLG);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーの取得
			@return エラー
		 */
		//-----------------------------------------------------------------//
		uint8_t get_only_error() noexcept
		{
			return get_error() & MCP_EFLG_ERRORMASK;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	受信エラーカウントの取得
			@return カウント
		 */
		//-----------------------------------------------------------------//
		uint8_t get_error_count_recv() noexcept
		{
			return read_(REGA::REC);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	送信エラーカウントの取得
			@return カウント
		 */
		//-----------------------------------------------------------------//
		uint8_t get_error_count_send() noexcept
		{
			return read_(REGA::TEC);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ONE SHOT の設定
			@param[in]	ena	不許可なら「false」
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool enable_one_shot_send(bool ena = true) noexcept
		{
			static const uint8_t MODE_ONESHOT = 0x08;
			modify_(REGA::CANCTRL, MODE_ONESHOT, ena ? MODE_ONESHOT : 0);
			bool f = (read_(REGA::CANCTRL) & MODE_ONESHOT) != MODE_ONESHOT;
			return f == ena;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	GPO 出力
			@param[in]	data	データ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_gpo(uint8_t data) noexcept
		{
			modify_(REGA::BFPCTRL, MCP_BxBFS_MASK, (data << 4));
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	GPI 入力
			@param[out]	data	データ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		uint8_t get_gpi(uint8_t& data) noexcept
		{
			auto res = read_(REGA::TXRTSCTRL) & MCP_BxRTS_MASK;
			data = res >> 3;
			return true;
		}
	};
}
