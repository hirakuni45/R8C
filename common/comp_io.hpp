#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・コンパレーター I/O 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "intr.hpp"
#include "comp.hpp"

/// F_CLK はタイマー周期計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "comp_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター I/O 制御クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK1, class TASK3>
	class comp_io {

		static TASK1 task1_;
		static TASK3 task3_;

	public:

		enum class filter {
			none,
			f1,
			f8,
			f32
		};

		enum class edge {
			a_lt_r,		///< アナログ入力が基準入力より低い時
			a_gt_r,		///< アナログ入力が基準入力より高い時
			ltgt = 3,	///< 低いおよび高い時
		};

		static INTERRUPT_FUNC void itask1() {
			task1_();
			WCB1INTR.WCB1F = 0;
		}

		static INTERRUPT_FUNC void itask3() {
			task3_();
			WCB3INTR.WCB3F = 0;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		comp_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  チャネル１開始
		*/
		//-----------------------------------------------------------------//
		void start1(filter fl = filter::none, edge et = edge::ltgt, uint8_t ir_lvl = 0) const {
			WCMPR.WCB1M0 = 1;

			ILVL2.B01 = ir_lvl;
			if(ir_lvl) {
				WCB1INTR.WCB1FL = static_cast<uint8_t>(fl);
				WCB1INTR.WCB1S  = static_cast<uint8_t>(et);
				WCB1INTR.WCB1INTEN = 1;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  チャネル３開始
		*/
		//-----------------------------------------------------------------//
		void start3(filter fl = filter::none, edge et = edge::ltgt, uint8_t ir_lvl = 0) const {
			WCMPR.WCB3M0 = 1;

			ILVL2.B45 = ir_lvl;
			if(ir_lvl) {
				WCB3INTR.WCB3FL = static_cast<uint8_t>(fl);
				WCB3INTR.WCB3S  = static_cast<uint8_t>(et);
				WCB3INTR.WCB3INTEN = 1;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  チャネル１出力を取得
			@return チャネル１出力
		*/
		//-----------------------------------------------------------------//
		bool get_value1() const {
			return WCMPR.WCB1OUT();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  チャネル３出力を取得
			@return チャネル３出力
		*/
		//-----------------------------------------------------------------//
		bool get_value3() const {
			return WCMPR.WCB3OUT();
		}
	};

}