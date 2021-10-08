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
		@param[in]	SPI_IO	SPI I/O クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI_IO>
	class nRF905 {

		SPI_IO&		spi_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	spi	spi_io クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		nRF905(SPI_IO& spi) : spi_(spi)
		{ }


	};
}

