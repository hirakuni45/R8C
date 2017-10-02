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
#define MODE_ONESHOT    0x08
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
		static const uint8_t MCP_RESET   = 0xC0;

		static const uint8_t MCP_RX0IF   = 0x01;
		static const uint8_t MCP_RX1IF   = 0x02;
		static const uint8_t MCP_TX0IF   = 0x04;
		static const uint8_t MCP_TX1IF   = 0x08;
		static const uint8_t MCP_TX2IF   = 0x10;
		static const uint8_t MCP_ERRIF   = 0x20;
		static const uint8_t MCP_WAKIF   = 0x40;
		static const uint8_t MCP_MERRF   = 0x80;


#define MCP_BxBFS_MASK    0x30
#define MCP_BxBFE_MASK    0x0C
#define MCP_BxBFM_MASK    0x03

#define MCP_BxRTS_MASK    0x38
#define MCP_BxRTSM_MASK   0x07

#define MCP_RXB_RX_ANY      0x60
#define MCP_RXB_RX_EXT      0x40
#define MCP_RXB_RX_STD      0x20
#define MCP_RXB_RX_STDEXT   0x00
#define MCP_RXB_RX_MASK     0x60
#define MCP_RXB_BUKT_MASK   (1<<2)


/*
** Bits in the TXBnCTRL registers.
*/
#define MCP_TXB_TXBUFE_M    0x80
#define MCP_TXB_ABTF_M      0x40
#define MCP_TXB_MLOA_M      0x20
#define MCP_TXB_TXERR_M     0x10
#define MCP_TXB_TXREQ_M     0x08
#define MCP_TXB_TXIE_M      0x04
#define MCP_TXB_TXP10_M     0x03


/*
 *   Begin mt
 */
#if 0
#define TIMEOUTVALUE    50
#define MCP_SIDH        0
#define MCP_SIDL        1
#define MCP_EID8        2
#define MCP_EID0        3
#endif


#define MCP_DLC_MASK        0x0F                                        /* 4 LSBits                     */


#if 0
/*
 *   Define SPI Instruction Set
 */
#define MCP_LOAD_TX0        0x40
#define MCP_LOAD_TX1        0x42
#define MCP_LOAD_TX2        0x44

#define MCP_RTS_TX0         0x81
#define MCP_RTS_TX1         0x82
#define MCP_RTS_TX2         0x84
#define MCP_RTS_ALL         0x87

#define MCP_READ_RX0        0x90
#define MCP_READ_RX1        0x94

#define MCP_READ_STATUS     0xA0

#define MCP_RX_STATUS       0xB0
#endif

		SPI&		spi_;

		MODE		mode_;

		uint32_t	id_;
		uint8_t		rtr_;
		uint8_t		ext_;
		uint8_t		msg_[8];
		uint8_t		len_;

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


		void write_mf_(REGA adrs, bool ext, uint32_t id) noexcept
		{
			uint16_t canid = id & 0x0FFFF;
			uint8_t tmp[4];

				static const uint8_t MCP_SIDH = 0;
				static const uint8_t MCP_SIDL = 1;
				static const uint8_t MCP_EID8 = 2;
				static const uint8_t MCP_EID0 = 3;

				static const uint8_t MCP_TXB_EXIDE_M = 0x08;  // In TXBnSIDL

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
			write_(static_cast<REGA>(mcp_addr + 5), msg_, len_);

		    if(rtr_ == 1) {                                                   /* if RTR set bit in byte       */
				static const uint8_t MCP_RTR_MASK = 0x40;  // (1<<6) Bit 6
				len_ |= MCP_RTR_MASK;  
			}

			// write the RTR and DLC
			write_(static_cast<REGA>(mcp_addr+4), len_);
            // write CAN id
			write_id_(static_cast<REGA>(mcp_addr), ext_, id_);
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
/*          The followingn two functions of the MCP2515 do not work, there is a bug in the silicon.
            case (MCP_STD): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_STD | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_STD);
            break;

            case (MCP_EXT): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_EXT | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_EXT);
            break;
*/
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
			@brief	メッセージ設定
			@param[in]	id	ID
			@param[in]	rtr	RTR 値
			@param[in]	ext	拡張フラグ
			@param[in]	src	ソース
			@param[in]	len	送信バイト数
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_msg(uint32_t id, uint8_t rtr, uint8_t ext, void* src, uint8_t len)
		{
			id_  = id;
			rtr_ = rtr;
			ext_ = ext;
			len_ = len;
			uint8_t l = len;
			if(l > sizeof(msg_)) l = sizeof(msg_);
			std::memcpy(msg_, src, l);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メッセージ送信
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool send_msg()
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
			@brief	送信
			@param[in]	id	ID
			@param[in]	ext	拡張フラグ
			@param[in]	src	ソース
			@param[in]	len	送信バイト数
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool send(uint32_t id, uint8_t ext, const void* src, uint8_t len)
		{
			set_msg(id, 0, ext, src, len);
			return send_msg();
    	}



	};
}
