#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・TimerRJ I/O 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/vect.h"
#include "M120AN/system.hpp"
#include "M120AN/intr.hpp"
#include "M120AN/timer_rj.hpp"

/// F_CLK はタイマー周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "trj_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  TimerRJ I/O 制御クラス
		@param[in]	TASK 割り込み内で実行されるクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK>
	class trj_io {
	public:
		static TASK task_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  パルス計測モード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class measurement : uint8_t {
			low_width,	///< Low レベル幅測定
			high_width,	///< High レベル幅測定
			count,		///< パルス数測定
			freq,		///< 周期測定
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  フィルタータイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class filter : uint8_t {
			none,		///< 無し
			f1 = 1,		///< F_CLK / 1  フィルター
			f8 = 2,		///< F_CLK / 8  フィルター
			f32 = 3,	///< F_CLK / 32 フィルター
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  カウンターソース
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class source : uint8_t {
			f1 = 0,		///< F_CLK
			f2 = 3,		///< F_CLK / 2
			f8 = 1,		///< F_CLK / 8
			fHOCO = 2	///< fHOCO（高速オンチップオシレーター）
		};


		static volatile uint8_t trjmr_;
		static volatile uint16_t trj_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  パルス出力用割り込み関数
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static inline void iout() {
			TRJMR = trjmr_;
			TRJ = trj_;
			task_();
			volatile uint8_t tmp = TRJIR();
			TRJIR = 0x00;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  パルス入力用割り込み関数
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static inline void iinp() {
			trj_ = TRJ();
 			task_();
			volatile uint8_t tmp = TRJIR();
			TRJIR = 0x00;
		}


	private:
		bool set_freq_(uint32_t freq, uint16_t& trj, uint8_t& tck) const {
			uint32_t tn = F_CLK / (freq * 2);
			uint8_t cks = 0;
			while(tn > 65536) {
				tn >>= 1;
				++cks;
				if(cks == 2) {
					tn >>= 1;
				}
				if(cks >= 3) return false;
			}
			if(tn) --tn;
			else return false;

			static const uint8_t tbl_[3] = { 0, 3, 1 }; // 1/1, 1/2, 1/8
			tck = tbl_[cks];
			trj = tn;

			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		trj_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  パルス出力の開始（TRJIO/TRJO 端子から、パルスを出力）
			@param[in]	freq	周波数
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		bool pluse_out(uint32_t freq, uint8_t ir_lvl = 0) const {
			MSTCR.MSTTRJ = 0;  // モジュールスタンバイ解除

			TRJCR.TSTART = 0;  // カウンタを停止

			uint16_t trj;
			uint8_t tck;
			if(!set_freq_(freq, trj, tck)) {
				return false;
			}

			TRJMR = trjmr_ = TRJMR.TCK.b(tck) | TRJMR.TCKCUT.b(0) | TRJMR.TMOD.b(1);  // パルス出力モード
			TRJ = trj_ = trj;

			TRJIOC.TEDGSEL = 1;  // L から出力
			TRJIOC.TOPCR = 0;    // トグル出力

			ILVLB.B01 = ir_lvl;
			if(ir_lvl) {
				TRJIR = TRJIR.TRJIE.b(1);
			} else {
				TRJIR = TRJIR.TRJIE.b(0);
			}

			TRJCR.TSTART = 1;  // カウンタを開始

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  TRJ 出力周波数の再設定
			@param[in]	freq	周波数
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		bool set_cycle(uint32_t freq) const {
			uint16_t trj;
			uint8_t tck;
			if(!set_freq_(freq, trj, tck)) {
				return false;
			}

			// パルス出力モード
			// TRJ = 0 だと、割り込みが発生しないので、直接設定する
			if(trj_ != 0 && ILVLB.B01()) {
				trjmr_ = TRJMR.TCK.b(tck) | TRJMR.TCKCUT.b(0) | TRJMR.TMOD.b(1);
				trj_ = trj;
				di();
				volatile uint8_t v = TRJIR();
				TRJIR = TRJIR.TRJIE.b(1);
				ei();
			} else {
				TRJMR = TRJMR.TCK.b(tck) | TRJMR.TCKCUT.b(0) | TRJMR.TMOD.b(1);
				TRJ = trj;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  パルス計測の開始（TRJIO 端子から、パルスを入力）@n
					※VCOUT1 端子から入力する場合は、TRJIOSEL を設定する。
			@param[in]	measur	パルス計測のモード
			@param[in]	s		クロック選択（measur::countの場合は無効）
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
		*/
		//-----------------------------------------------------------------//
		void pluse_inp(measurement measur, source s, uint8_t ir_lvl = 0) const {
			MSTCR.MSTTRJ = 0;  // モジュールスタンバイ解除

			TRJCR = 0x00;  // カウンタ停止

			bool f = TRJIOC.TEDGSEL();
			uint8_t md = 3;
			if(measur == measurement::low_width) {
				f = 0;
			} else if(measur == measurement::high_width) {
				f = 1;
			} else if(measur == measurement::count) {
				md = 2;
			} else if(measur == measurement::freq) {
				md = 4;
			}
			TRJIOC = TRJIOC.TEDGSEL.b(f) | TRJIOC.TIPF.b(0) | TRJIOC.TOPCR.b(0);
			TRJMR = TRJMR.TMOD.b(md) | TRJMR.TCK.b(static_cast<uint8_t>(s))
						   | TRJMR.TEDGPL.b(1) | TRJMR.TCKCUT.b(0);

			ILVLB.B01 = ir_lvl;
			if(ir_lvl) {
				TRJIR = TRJIR.TRJIE.b(1);
			} else {
				TRJIR = 0x00;
			}

			TRJ = trj_ = 0xffff;
			TRJCR = TRJCR.TSTART.b(1);  // カウンタを開始、アンダーフロー・クリア
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測をリスタート
			@param[in]	s		クロック選択（measur::countの場合は無効）
		*/
		//-----------------------------------------------------------------//
		void restart_inp(source s) {
			TRJCR = 0x00;  // カウンタ停止

			TRJMR.TCK = static_cast<uint8_t>(s);

			if(ILVLB.B01()) {
				TRJIR = TRJIR.TRJIE.b(1);
			}
			TRJ = trj_ = 0xffff;
			TRJCR = TRJCR.TSTART.b(1);  // カウンタを再開、アンダーフロー・クリア
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  TRJ カウント値を取得（割り込み内で設定された値）
			@param[out]	count	カウント値
			@return アンダーフローの場合「false」
		*/
		//-----------------------------------------------------------------//
		bool get_count(uint16_t& count) const {
			if(ILVLB.B01()) {
				count = ~trj_;
			} else {
				count = ~TRJ();
			}
			return !TRJCR.TUNDF();
		}

	};

	// スタティック実態定義
	template<class TASK>
	TASK trj_io<TASK>::task_;
	template<class TASK>
	volatile uint8_t trj_io<TASK>::trjmr_;
	template<class TASK>
	volatile uint16_t trj_io<TASK>::trj_;

}
