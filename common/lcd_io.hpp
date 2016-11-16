#pragma once
//=====================================================================//
/*!	@file
	@brief	ST7567 LCD ドライバー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/spi_io.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  LCD テンプレートクラス
		@param[in]	SPI		SPI ポート定義クラス
		@param[in]	SEL		LCD 選択
		@param[in]	CMD		コマンド選択（CMD:L、DATA:H）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SEL, class CMD>
	class lcd_io {

		SPI&	spi_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		lcd_io(SPI& spi) : spi_(spi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start() const {
			// 100ms setup...
			for(uint8_t i = 0; i < 10; ++i) {
				utils::delay::micro_second(10000);
			}

			CMD::P = 0;
			SEL::P = 0;	// device enable

			spi_.xchg(0xae);	// display off
			spi_.xchg(0x40);	// display start line of 0
			spi_.xchg(0xa1);	// ADC set to reverse
			spi_.xchg(0xc0);	// common output mode: set scan direction normal operation
			spi_.xchg(0xa6);	// display normal (none reverse)
			spi_.xchg(0xa2);	// select bias b0:0=1/9, b0:1=1/7
			spi_.xchg(0x2f);	// all power control circuits on

			spi_.xchg(0xf8);	// set booster ratio to
			spi_.xchg(0x00);	// 4x

			spi_.xchg(0x27);	// set V0 voltage resistor ratio to large

			spi_.xchg(0x81);	// set contrast
			spi_.xchg(0x0);	// contrast value, EA default: 0x016

			spi_.xchg(0xac);	// indicator
			spi_.xchg(0x00);	// disable

			spi_.xchg(0xa4);	// all pixel on disable
			spi_.xchg(0xaf);	// display on

			for(uint8_t i = 0; i < 5; ++i) {
				utils::delay::micro_second(10000);
			}

			SEL::P = 1;	// device disable
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  コピー
			@param[in]	p	フレームバッファソース
		*/
		//-----------------------------------------------------------------//
		void copy(const uint8_t* p) const {
			SEL::P = 0;
			for(uint8_t page = 0; page < 4; ++page) {
				CMD::P = 0;
				uint8_t tmp[3];
				tmp[0] = 0xb0 + page;
				tmp[1] = 0x10;
				tmp[2] = 0x04;
				spi_.send(tmp, 3);
				CMD::P = 1;
				spi_.send(p, 128);
				p += 128;
			}
			SEL::P = 1;
		}

	};
}
