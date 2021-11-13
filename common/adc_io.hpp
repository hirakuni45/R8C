#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・A/D 制御 
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/vect.h"
#include "M120AN/adc.hpp"
#include "M120AN/intr.hpp"
#include "M120AN/system.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御クラス
		@param[in]	TASK	A/D 変換終了時起動タスク
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK>
	class adc_io {

		static TASK	task_;
		static volatile uint8_t intr_count_;

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  チャネル・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class CH_TYPE : uint8_t {
			CH0,		//< チャネル０のみ
			CH1,		//< チャネル１のみ
			CH0_CH1		//< ０，１チャネル
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  チャネル・グループ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class CH_GROUP : uint8_t {
			AN0_AN1,	//< AN0, AN1
			AN2_AN3,	//< AN2, AN3
			AN4_AN7		//< AN4, AN7
		};


		static inline void itask() {
			++intr_count_;
			task_();
			// IR 関係フラグは必ず mov 命令で・・
			volatile uint8_t r = ADICSR();
			ADICSR = 0x00;
		}

	private:
		uint8_t	level_;
		uint8_t	count_;

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
		adc_io() noexcept : level_(0), count_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  変換開始
			@param[in]	ct		チャネル・タイプ
			@param[in]	grp		チャネル・グループ
			@param[in]	cycle	繰り返し変換の場合「true」
			@param[in]	level	割り込みレベル（０の場合割り込みを使用しない）
		*/
		//-----------------------------------------------------------------//
		void start(CH_TYPE ct, CH_GROUP cg, bool cycle, uint8_t level = 0) noexcept
		{
			level_ = level;

			MSTCR.MSTAD = 0;

			ADCON0.ADST = 0;

			uint8_t md = 0b00;
			if(ct == CH_TYPE::CH0_CH1) md = 0b10;
			if(cycle) md |= 0b01; 
			ADMOD = ADMOD.CKS.b(0b011) | ADMOD.MD.b(md) | ADMOD.ADCAP.b(0b00);

			uint8_t chn = 0;
			if(ct == CH_TYPE::CH1) chn = 1; 
			ADINSEL = ADINSEL.CH0.b(chn) | ADINSEL.ADGSEL.b(static_cast<uint8_t>(cg));

			ILVL7.B01 = level_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換開始
			@param[in]	f	変換停止の場合「false」
		*/
		//-----------------------------------------------------------------//
		void scan(bool f = true) noexcept
		{
			count_ = intr_count_;
			if(f && level_ > 0) {
				ADICSR.ADIE = 1;
			}
			ADCON0.ADST = f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換終了検査
			@return 変換終了なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_state() const noexcept
		{
			if(level_ == 0) {
				bool f = ADICSR.ADF();
				if(f) ADICSR.ADF = 0;
				return f;
			} else {
				return count_ != intr_count_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換終了を同期
		*/
		//-----------------------------------------------------------------//
		void sync() const noexcept {
			while(!get_state()) sleep_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  変換結果の取得
			@param[in]	chanel	チャネル（０、１）
			@return 変換結果
		*/
		//-----------------------------------------------------------------//
		uint16_t get_value(bool chanel) const noexcept {
			if(chanel) {
				return AD1();
			} else {
				return AD0();
			}
		}
	};

	// スタティック実態定義
	template<class TASK>
	TASK adc_io<TASK>::task_;

	template<class TASK>
	volatile uint8_t adc_io<TASK>::intr_count_ = 0;
}
