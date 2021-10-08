#pragma once
//=====================================================================//
/*!	@file
	@brief	ADNS2051 class @n
			Agilent ADNS-2051 @n
			Optical Mouse Sensor @n
			Interface: Maybe SPI, Vcc: 5V @n
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ADNS2051 テンプレートクラス
		@param[in]	SDIO	ポート・クラス
		@param[in]	SCLK	ポート・クラス
		@param[in]	PD		ポート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SDIO, class SCLK, class PD>
	class ADNS2051 {

#if 0
		void write_byte_(uint8_t d) {
			for(uint8_t i = 0; i < 8; ++i) {
				D7::P = d & 1;
				d >>= 1;
				W_CLK::P = 1;
				utils::delay::micro_second(1);
				W_CLK::P = 0;
				utils::delay::micro_second(1);
			}
		}
#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクタ
		 */
		//-----------------------------------------------------------------//
		ADNS2051() noexcept
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start()
		{
			SDIO::DIR = 0;
			SCLK::DIR = 1;
			PD::DIR = 1;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	変化量を取得
		 */
		//-----------------------------------------------------------------//
		void get() noexcept
		{

		}
	};
}
