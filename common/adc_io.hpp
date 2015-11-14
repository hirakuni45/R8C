#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・A/D 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "adc.hpp"
#include "intr.hpp"
#include "system.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class adc_io {

	public:
		enum class cnv_type : uint8_t {
			CH0,		//< チャネル０のみ
			CH1,		//< チャネル１のみ
			CH0_CH1		//< ０，１チャネル
		};

		enum class ch_grp : uint8_t {
			AN0_AN1,	//< AN0, AN1
			AN2_AN3,	//< AN2, AN3
			AN4_AN7		//< AN4, AN7
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
			@param[in]	ct		変換タイプ
			@param[in]	grp		グループ
			@param[in]	cycle	繰り返し変換の場合「true」
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
		*/
		//-----------------------------------------------------------------//
		void setup(cnv_type ct, ch_grp cg, bool cycle, uint8_t ir_lvl = 0) {
			ADCON0.ADST = 0;

			MSTCR.MSTAD = 0;

			uint8_t md = 0;
			if(ct == cnv_type::CH0_CH1) md = 2;
			if(cycle) ++md; 
			ADMOD = ADMOD.CKS.b(3) | ADMOD.MD.b(md);

			uint8_t chn = 0;
			if(ct == cnv_type::CH1) chn = 1; 
			ADINSEL = ADINSEL.CH0.b(chn) | ADINSEL.ADGSEL.b(static_cast<uint8_t>(cg));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換開始
			@param[in]	f	変換停止の場合「false」
		*/
		//-----------------------------------------------------------------//
		void start(bool f = 1) {
			ADCON0.ADST = f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換終了検査
			@return 変換終了なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_state() const {
			bool f = ADICSR.ADF();
			if(f) ADICSR.ADF = 0;
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換結果の取得
			@param[in]	chanel	チャネル（０、１）
			@return 変換結果
		*/
		//-----------------------------------------------------------------//
		uint16_t get_value(uint8_t chanel) const {
			if(chanel) {
				return AD1();
			} else {
				return AD0();
			}
		}
	};

}
