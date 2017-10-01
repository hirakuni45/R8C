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
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MCP2515 テンプレートクラス
		@param[in]	SPI	SPI クラス
		@param[in]	SEL	選択クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SEL>
	class MCP2515 {
	public:
#if 0
/*
 *   CANCTRL Register Values
 */
#define MCP_SLEEP      0x20
#define MCP_LOOPBACK   0x40
#define MCP_LISTENONLY 0x60
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
			@brief  MCP2515 モード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class MODE : uint8_t {
			NORMAL   = 0x00,
			CONFIG   = 0x80,
			POWERUP  = 0xE0,
			MASK     = 0xE0
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

		static const uint8_t SPI_READ    = 0x02;
		static const uint8_t SPI_WRITE   = 0x03;
		static const uint8_t SPI_BITMOD  = 0x05;
		static const uint8_t SPI_RESET   = 0xC0;

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

		uint8_t read_(REGA adrs) noexcept
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(SPI_READ);
			spi_.xchg(static_cast<uint8_t>(adrs));
			uint8_t ret = spi_.xchg();
			SEL::P = 1;
			return ret;
		}


		void write_(REGA adrs, uint8_t data)
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(SPI_WRITE);
			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.xchg(data);
			SEL::P = 1;
		}


		void modify_(REGA adrs, uint8_t mask, uint8_t data)
		{
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			SEL::P = 0;
			spi_.xchg(SPI_BITMOD);
   			spi_.xchg(static_cast<uint8_t>(adrs));
			spi_.xchg(mask);
			spi_.xchg(data);
			SEL::P = 1;
		}


		bool set_ctrl_mode_(MODE mode)
		{
			modify_(REGA::CANCTRL, static_cast<uint8_t>(MODE::MASK), static_cast<uint8_t>(mode));
			auto ret = read_(REGA::CANCTRL);
			ret &= static_cast<uint8_t>(MODE::MASK);
			return ret == static_cast<uint8_t>(mode);
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
			@brief	開始
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool start() noexcept
		{
			SEL::DIR = 1;  // output
			SEL::P = 0;    // device disable
			// SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			spi_.xchg(SPI_RESET);
			SEL::P = 1;
			utils::delay::micro_second(10);
			return true;
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


#if 0
INT8U MCP_CAN::mcp2515_init(const INT8U canIDMode, const INT8U canSpeed, const INT8U canClock)
{

  INT8U res;

    mcp2515_reset();
    
    mcpMode = MCP_LOOPBACK;

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0)
    {
#if DEBUG_MODE
      Serial.print("Entering Configuration Mode Failure...\r\n"); 
#endif
      return res;
    }
#if DEBUG_MODE
    Serial.print("Entering Configuration Mode Successful!\r\n");
#endif

    // Set Baudrate
    if(mcp2515_configRate(canSpeed, canClock))
    {
#if DEBUG_MODE
      Serial.print("Setting Baudrate Failure...\r\n");
#endif
      return res;
    }
#if DEBUG_MODE
    Serial.print("Setting Baudrate Successful!\r\n");
#endif

    if ( res == MCP2515_OK ) {

                                                                        /* init canbuffers              */
        mcp2515_initCANBuffers();

                                                                        /* interrupt mode               */
        mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);

	//Sets BF pins as GPO
	mcp2515_setRegister(MCP_BFPCTRL,MCP_BxBFS_MASK | MCP_BxBFE_MASK);
	//Sets RTS pins as GPI
	mcp2515_setRegister(MCP_TXRTSCTRL,0x00);

        switch(canIDMode)
        {
            case (MCP_ANY):
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_ANY);
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
            case (MCP_STDEXT): 
            mcp2515_modifyRegister(MCP_RXB0CTRL,
            MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
            MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
            mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
            MCP_RXB_RX_STDEXT);
            break;
    
            default:
#if DEBUG_MODE        
            Serial.print("`Setting ID Mode Failure...\r\n");
#endif           
            return MCP2515_FAIL;
            break;
}    


        res = mcp2515_setCANCTRL_Mode(mcpMode);                                                                
        if(res)
        {
#if DEBUG_MODE        
          Serial.print("Returning to Previous Mode Failure...\r\n");
#endif           
          return res;
        }

    }
    return res;

}

		//-----------------------------------------------------------------//
		/*!
			@brief	モード設定
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool begin(uint8_t idmodeset, uint8_t speedset, uint8_t clockset)
		{
			return init_(idmodeset, speedset, clockset);
		}
#endif

	};
}
