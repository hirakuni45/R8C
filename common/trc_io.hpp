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
#  error "trb_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  TimerRC I/O 制御クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class trc_io {


	public:
		static INTERRUPT_FUNC void trc_task() {
		}


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
		trc_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭモード開始（最大３チャネルのＰＷＭ出力）
			@param[in]	limit	リミット
			@param[in]	cks		クロック選択（0:f1, 1:f2, 2:f4, 3:f8, 4:f32）
			@param[in]	pfl ポートの初期レベル「fasle」0->1、「true」1->0
			@param[in]	ir_lvl	割り込みレベル（０の場合割り込みを使用しない）
			@return 設定範囲を超えたら「false」
		*/
		//-----------------------------------------------------------------//
		void start_pwm(uint16_t limit, uint8_t cks, bool pfl, uint8_t ir_lvl = 0) const {
			MSTCR.MSTTRC = 0;  // モジュールスタンバイ解除

			TRCMR.CTS = 0;  // カウント停止

			TRCCNT = 0x0000;
			TRCGRA = limit;

			TRCMR = TRCMR.PWM2.b(1) | TRCMR.PWMB.b(1) | TRCMR.PWMC.b(1) | TRCMR.PWMD.b(1);

			// コンペア一致Ａでカウンタクリア
			TRCCR1 = TRCCR1.CCLR.b(1) | TRCCR1.TOA.b(0) | TRCCR1.CKS.b(cks)
				   | TRCCR1.TOB.b(0) | TRCCR1.TOC.b(0) | TRCCR1.TOD.b(0);

			TRCIOR0 = TRCIOR0.IOA.b(0) | TRCIOR0.IOB.b(2);
			TRCIOR1 = TRCIOR1.IOC.b(8 | 2) | TRCIOR1.IOD.b(8 | 2);

			TRCCR2 = TRCCR2.POLB.b(pfl) | TRCCR2.POLC.b(pfl) | TRCCR2.POLD.b(pfl);

			TRCOER = TRCOER.EB.b(0) | TRCOER.EC.b(0) | TRCOER.ED.b(0);

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

			start_pwm(tn, cks, pfl, ir_lvl);

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
			TRCGRB = val;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ値Ｃを設定
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void set_pwm_c(uint16_t val) const {
			TRCGRC = val;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ＰＷＭ値Ｄを設定
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void set_pwm_d(uint16_t val) const {
			TRCGRD = val;
		}

	};
}
