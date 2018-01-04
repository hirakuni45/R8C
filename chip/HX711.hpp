#pragma once
//=====================================================================//
/*!	@file
	@brief	HX711 ドライバー @n
			※ロードセル用２４ビットＡ／Ｄコンバーター
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

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  HX711 テンプレートクラス
		@param[in]	SCK		クロック・ポート
		@param[in]	DAT		データ・ポート
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SCK, class DAT>
	class HX711 {


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		HX711() noexcept { }




	};
}
