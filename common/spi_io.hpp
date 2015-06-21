#pragma once
//=====================================================================//
/*!	@file
	@brief	SPI テンプレートクラス (20MHz system clock) @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/delay.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  SPI テンプレートクラス @n
				PORT ポート指定クラス： @n
				class port { @n
				public: @n
					void scl_out(bool val) const { } @n
					void sda_out(bool val) const { } @n
					bool sda_inp() const { return b; } @n
				};
		@param[in]	PORT	ポート定義クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class PORT>
	class spi_io {
		PORT		port_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		spi_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void init() const {
			port_.init();
			port_.scl_out(1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ライト
			@param[in]	d	データ
		*/
		//-----------------------------------------------------------------//
		void write(uint8_t d) const {
			for(uint8_t i = 0; i < 8; ++i) {
				port_.scl_out(0);
				port_.sda_out(d & 0x80);
				asm("nop");
//				utils::delay::micro_second(1);
				port_.scl_out(1);
				d <<= 1;
				asm("nop");
//				utils::delay::micro_second(1);
			}
		}

	};
}
