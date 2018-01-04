#pragma once
//=====================================================================//
/*!	@file
	@brief	AD9833 class @n
			ANALOG DEVICES @n
			Interface: SPI, Vcc: 3.3V to 5V
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  AD9833 テンプレートクラス
		@param[in]	SPI	spi クラス
		@param[in]	SEL	デバイス・セレクト(FSYNC)
		@param[in]	REFCLK	リファレンス・クロック（通常25MHz）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SEL, uint32_t REFCLK = 25000000>
	class AD9833 {

		SPI&	spi_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクタ
		 */
		//-----------------------------------------------------------------//
		AD9833(SPI& spi) noexcept : spi_(spi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start()
		{
		}



	};
}
