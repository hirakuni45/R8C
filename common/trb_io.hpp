#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・タイマー RB2 I/O 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/vect.h"
#include "M120AN/system.hpp"
#include "M120AN/intr.hpp"
#include "M120AN/timer_rb.hpp"

/// F_CLK はタイマー周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "trb_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマー RB2 ベースクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class trb_base {
	public:
		enum class OUTPUT : uint8_t {
			NONE,	///< 出力無効
			LOW,	///< 初期値：H、時間が来たら：L
			HIGH,	///< 初期値：L、時間が来たら：H
			TOGGLE,	///< 反転（周期の１／２）
		};
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマー RB2 I/O 制御クラス
		@param[in]	TASK	割り込み内で実行されるクラス
		@param[in]	CNT		内部カウンタの型
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK, typename CNT>
	class trb_io : public trb_base {
	public:
		typedef TASK task_type;

		static TASK task_;

		static volatile CNT	count_;

		static inline void itask() {
			++count_;
			task_();
			// IR 関係フラグは必ず mov 命令で・・
			volatile uint8_t r = TRBIR();
			TRBIR = TRBIR.TRBIF.b(false) | (r & TRBIR.TRBIE.b());
		}

		uint16_t	limit_;

	private:

		// ※同期が必要なら、実装する
		void sleep_() const {
			asm("nop");
		}

		CNT get_timer_() const {
			return TRBPRE() | (TRBPR() << 8);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
///		__attribute__ ((section (".text"))) 
		trb_io() : limit_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  タイマー開始
			@param[in]	freq	周期（周波数）
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@param[in]	out		出力指定
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		bool start(uint32_t freq, uint8_t ir_lvl = 0, OUTPUT out = OUTPUT::NONE)
		{
			MSTCR.MSTTRB = 0;  // モジュールスタンバイ解除

			TRBCR.TSTART = 0;

			uint32_t tn = F_CLK / freq;
			uint8_t div = 0;
			while(tn > 65536) {
				tn >>= 1;
				++div;
				if(div == 4) {
					tn >>= 1;
				}
				if(div >= 7) return false;
			}
			if(tn) --tn;
			if(tn == 0) return false;

			static const uint8_t tbl[8] = {
				0b000, 0b011, 0b100, 0b001, 0b101, 0b110, 0b111
			};

			// タイマーモード、１６ビットタイマー
			TRBMR = TRBMR.TMOD.b(0) | TRBMR.TCNT16.b() | TRBMR.TCK.b(tbl[div]);

			limit_  = tn;
			TRBPRE = tn & 0xff;
			TRBPR  = tn >> 8;

			ILVLC.B01 = ir_lvl;
			if(ir_lvl) {
				TRBIR = TRBIR.TRBIE.b();				
			} else {
				TRBIR = TRBIR.TRBIE.b(0);
			}

			switch(out) {
			case OUTPUT::LOW:
				break;
			case OUTPUT::HIGH:
				break;
			case OUTPUT::TOGGLE:
				TRBIOC = TRBIOC.TOPL.b(0) | TRBIOC.TOCNT.b(0);
				break;
			default:
				break;
			}

			TRBCR.TSTART = 1;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  タイマー同期
		*/
		//-----------------------------------------------------------------//
		void sync() const {
			if(TRBIR.TRBIE()) {
				volatile CNT n = count_;
				while(n == count_) sleep_();
			} else {
				while(TRBIR.TRBIF() == 0) sleep_();
				TRBIR.TRBIF = 0;
				++count_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  カウント値の取得
			@return カウント値
		*/
		//-----------------------------------------------------------------//
		auto get_count() const { return count_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  リミット値の取得（ダウンカウントレジスタの設定値）
			@return リミット値
		*/
		//-----------------------------------------------------------------//
		uint16_t get_limit() const { return limit_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  タイマー値の取得（クロック毎にダウンカウントされる値）
			@return タイマー値
		*/
		//-----------------------------------------------------------------//
		uint16_t get_timer() const {
			uint16_t n = get_timer_();
			// 桁上がりを考慮して、連続して同じ値が読まれるまでループする。
			while(n != get_timer_()) {
				n = get_timer_();
			}
			return n;
		}
	};

	// スタティック実態定義
	template<class TASK, typename CNT>
	TASK trb_io<TASK, CNT>::task_;
	template<class TASK, typename CNT>
	volatile CNT trb_io<TASK, CNT>::count_;
}
