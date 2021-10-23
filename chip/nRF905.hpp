#pragma once
//=====================================================================//
/*!	@file
	@brief	NRF905 Single chip 433/868/915MHz Transceiver ドライバー @n
			NORDIC SEMICONDUCTOR @n
			https://infocenter.nordicsemi.com/pdf/nRF905_PS_v1.5.pdf @n
			Copyright 2020 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  nRF905 テンプレートクラス
		@param[in]	SPI		SPI クラス (MISO, MOSI, SCLK)
		@param[in]	SS		SPI/SS クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SS>
	class nRF905 {

		SPI&		spi_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	spi	spi クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		nRF905(SPI& spi) : spi_(spi)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() noexcept
		{
		}



	};
}

