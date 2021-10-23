#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・コンパレーター I/O 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/vect.h"
#include "M120AN/system.hpp"
#include "M120AN/intr.hpp"
#include "M120AN/comp.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター・フィルター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	enum class comp_filter {
		none,	///< 無し
		f1,
		f8,
		f32
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター・エッジ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	enum class comp_edge {
		a_lt_r,		///< アナログ入力が基準入力より低い時
		a_gt_r,		///< アナログ入力が基準入力より高い時
		ltgt = 3,	///< 低いおよび高い時
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター I/O 制御クラス
		@param[in]	TASK1	コンパレーター１割り込み処理
		@param[in]	TASK3	コンパレーター３割り込み処理
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class TASK1, class TASK3>
	class comp_io {

		static TASK1 task1_;
		static TASK3 task3_;

	public:

		static inline void itask1() {
			task1_();
			WCB1INTR.WCB1F = 0;
		}

		static inline void itask3() {
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
			@param[in]	et	比較モード
			@para,[in]	fl	フィルター
		*/
		//-----------------------------------------------------------------//
		void start1(comp_edge eg = comp_edge::ltgt, comp_filter fl = comp_filter::none, uint8_t ir_lvl = 0) const {
			WCMPR.WCB1M0 = 1;

			ILVL2.B01 = ir_lvl;
			if(ir_lvl) {
				WCB1INTR.WCB1FL = static_cast<uint8_t>(fl);
				WCB1INTR.WCB1S  = static_cast<uint8_t>(eg);
				WCB1INTR.WCB1INTEN = 1;
			} else {
				WCB1INTR.WCB1INTEN = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  チャネル３開始
			@param[in]	et	比較モード
			@para,[in]	fl	フィルター
		*/
		//-----------------------------------------------------------------//
		void start3(comp_edge eg = comp_edge::ltgt, comp_filter fl = comp_filter::none, uint8_t ir_lvl = 0) const {
			WCMPR.WCB3M0 = 1;

			ILVL2.B45 = ir_lvl;
			if(ir_lvl) {
				WCB3INTR.WCB3FL = static_cast<uint8_t>(fl);
				WCB3INTR.WCB3S  = static_cast<uint8_t>(eg);
				WCB3INTR.WCB3INTEN = 1;
			} else {
				WCB3INTR.WCB3INTEN = 0;
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
