#pragma once
//=====================================================================//
/*!	@file
	@brief	SJIS, UTF16 変換
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UTF-16 から SJIS コードを求めるマップの生成
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void init_utf16_to_sjis();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	SJIS から UTF-16 コードを求める
		@param[in]	sjis	SJIS コード
		@return UTF16 コード
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	uint16_t sjis_to_utf16(uint16_t sjis);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UTF-16 から SJIS コードを求める
		@param[in]	utf16	UTF-16 コード
		@return SJIS コード
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	uint16_t utf16_to_sjis(uint16_t utf16);

};
