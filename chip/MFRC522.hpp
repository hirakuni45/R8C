#pragma once
//=====================================================================//
/*!	@file
	@brief	MFRC522 クラス @n
			NXP Semiconductors @n
			Interface: SPI, Vcc: 3.3V
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MFRC522 テンプレートクラス
		@param[in]	SPI	spi クラス
		@param[in]	CS	チップ・セレクト
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class CS>
	class MFRC522 {

		SPI&	spi_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクタ
		 */
		//-----------------------------------------------------------------//
		MFRC522(SPI& spi) noexcept : spi_(spi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() noexcept
		{
			CS::DIR = 1;
			CS::P = 1;


		}



	};
}
