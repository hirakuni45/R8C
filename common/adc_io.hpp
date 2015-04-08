#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・A/D 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "adc.hpp"
#include "intr.hpp"
#include "system.hpp"

/// F_CLK は周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "adc_io.hpp requires F_CLK to be defined"
#endif

#define INTERRUPT_FUNC __attribute__ ((interrupt))

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class adc_io {

	public:
		enum class conversion_type {
			single,
			multi
		};

	private:

		// ※同期が必要なら、実装する
		void sleep_() const {
			asm("nop");
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		adc_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  変換開始
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
		*/
		//-----------------------------------------------------------------//
		void start(uint8_t ir_lvl = 0) {

			MSTCR.MSTAD = 0;




		}

	};

}
