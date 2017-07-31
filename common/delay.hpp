#pragma once
//=====================================================================//
/*!	@file
	@brief	delay ユーティリティー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  基準クロック２０ＭＨｚの待ち
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct delay {

		//-----------------------------------------------------------------//
		/*!
			@brief  ナノ秒単位の待ち
			@param[in]	ns	待ち時間（ナノ秒）
		*/
		//-----------------------------------------------------------------//
		static void nano_second(uint16_t ns) {
			ns /= 50;   ///< 20MHz clock base
			while(ns > 0) {
				asm("nop");
				--ns;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  マイクロ秒単位の待ち
			@param[in]	us	待ち時間（マイクロ秒）
		*/
		//-----------------------------------------------------------------//
		static void micro_second(uint16_t us) {
			while(us > 0) {
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				asm("nop");
				--us;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ミリ秒単位の待ち
			@param[in]	ms	待ち時間（ミリ秒）
		*/
		//-----------------------------------------------------------------//
		static void milli_second(uint16_t ms) {
			for(uint16_t i = 0; i < ms; ++i) {
				micro_second(1000);
			}
		}
	};
}
