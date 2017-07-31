#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・TimerRC I/O 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "intr.hpp"
#include "timer_rc.hpp"

/// F_CLK はタイマー周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "trc_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  TimerRC I/O 制御クラス
		@param[in]	TASK 割り込み内で実行されるクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK>
	class trc_io {
	public:
		static TASK task_;

		static volatile uint16_t	pwm_b_;
		static volatile uint16_t	pwm_c_;
		static volatile uint16_t	pwm_d_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  PWM-A 割り込み
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static inline void itask() {
			task_();
			volatile uint8_t f = TRCSR();
			TRCGRB = pwm_b_;
			TRCGRC = pwm_c_;
			TRCGRD = pwm_d_;
			TRCSR = 0x00;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  カウンターディバイド
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class divide : uint8_t {
			f1,		///< F_CLK / 1
			f2,		///< F_CLK / 2
			f4,		///< F_CLK / 4
			f8,		///< F_CLK / 8
			f32		///< F_CLK / 32
		};

	private:

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		trc_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭモード開始（最大３チャネルのＰＷＭ出力）
			@param[in]	limit	リミット
			@param[in]	cks		クロック選択
			@param[in]	pfl ポートの初期レベル「fasle」0->1、「true」1->0
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		void start_pwm(uint16_t limit, divide cks, bool pfl, uint8_t ir_lvl = 0) const {
			MSTCR.MSTTRC = 0;  // モジュールスタンバイ解除

			TRCMR.CTS = 0;  // カウント停止

			TRCCNT = 0x0000;
			TRCGRA = limit;
			TRCGRB = pwm_b_ = 128;
			TRCGRC = pwm_c_ = 128;
			TRCGRD = pwm_d_ = 128;

			TRCMR = TRCMR.PWM2.b(1) | TRCMR.PWMB.b(1) | TRCMR.PWMC.b(1) | TRCMR.PWMD.b(1);

			// コンペア一致Ａでカウンタクリア
			TRCCR1 = TRCCR1.CCLR.b(1) | TRCCR1.TOA.b(0) | TRCCR1.CKS.b(static_cast<uint8_t>(cks))
				   | TRCCR1.TOB.b(0) | TRCCR1.TOC.b(0) | TRCCR1.TOD.b(0);

			TRCIOR0 = TRCIOR0.IOA.b(0) | TRCIOR0.IOB.b(2);
			TRCIOR1 = TRCIOR1.IOC.b(8 | 2) | TRCIOR1.IOD.b(8 | 2);

			TRCCR2 = TRCCR2.POLB.b(pfl) | TRCCR2.POLC.b(pfl) | TRCCR2.POLD.b(pfl);

			TRCOER = TRCOER.EB.b(0) | TRCOER.EC.b(0) | TRCOER.ED.b(0);

			ILVL3.B45 = ir_lvl;
			if(ir_lvl) {
				TRCIER = TRCIER.IMIEA.b(1);  // カウンターＡのマッチをトリガーにして割り込み
//				TRCIER = TRCIER.IMIEB.b(1);
//				TRCIER = TRCIER.IMIEC.b(1);
			} else {
				TRCIER = 0x00;
			}

			TRCMR.CTS = 1;  // カウント開始
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭモード開始（最大３チャネルのＰＷＭ出力）
			@param[in]	hz	周期（周波数）
			@param[in]	pfl ポートの初期レベル「fasle」0->1、「true」1->0
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		bool start_pwm(uint16_t hz, bool pfl, uint8_t ir_lvl = 0) const {
			// 周波数から最適な、カウント値を計算
			uint32_t tn = F_CLK / hz;
			uint8_t cks = 0;
			while(tn > 65536) {
				tn >>= 1;
				++cks;
				if(cks == 4) {
					tn >>= 1;
				}
				if(cks >= 5) return false;
			}
			if(tn) --tn;
			if(tn == 0) return false;

			start_pwm(tn, static_cast<divide>(cks), pfl, ir_lvl);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭリミット値を取得
			@return リミット値
		*/
		//-----------------------------------------------------------------//
		uint16_t get_pwm_limit() const {
			return TRCGRA();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ値Ｂを設定
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void set_pwm_b(uint16_t val) const {
			if(ILVL3.B45()) {
				pwm_b_ = val;
			} else {
				TRCGRB = val;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ値Ｃを設定
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void set_pwm_c(uint16_t val) const {
			if(ILVL3.B45()) {
				pwm_c_ = val;
			} else {
				TRCGRC = val;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ値Ｄを設定
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void set_pwm_d(uint16_t val) const {
			if(ILVL3.B45()) {
				pwm_d_ = val;
			} else {
				TRCGRD = val;
			}
		}

	};

	// スタティック実態定義
	template<class TASK>
	TASK trc_io<TASK>::task_;
	template<class TASK>
	volatile uint16_t trc_io<TASK>::pwm_b_;
	template<class TASK>
	volatile uint16_t trc_io<TASK>::pwm_c_;
	template<class TASK>
	volatile uint16_t trc_io<TASK>::pwm_d_;

}
