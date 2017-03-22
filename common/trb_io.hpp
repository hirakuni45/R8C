#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・タイマー RB I/O 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "intr.hpp"
#include "timer_rb.hpp"

/// F_CLK はタイマー周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "trb_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマー RB I/O 制御クラス
		@param[in]	TASK 割り込み内で実行されるクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK, typename CNT>
	class trb_io {
	public:
		typedef TASK task_type;

		static TASK task_;

		static volatile CNT	count_;

		static INTERRUPT_FUNC void itask() {
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
		__attribute__ ((section (".text"))) 
		trb_io() : limit_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  タイマー開始
			@param[in]	hz	周期（周波数）
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		bool start_timer(uint16_t hz, uint8_t ir_lvl = 0) {
			MSTCR.MSTTRB = 0;  // モジュールスタンバイ解除

			TRBCR.TSTART = 0;

			uint32_t tn = F_CLK / hz;
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
				TRBIR = TRBIR.TRBIE.b(false);
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
		CNT get_count() const { return count_; }


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
