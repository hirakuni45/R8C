#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・ウォッチドッグ・レジスター定義
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2014, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマ機能レジスタ RISR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct risr_t : public rw8_t<0x0030> {
		typedef rw8_t<0x0030> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> UFIF;
		bit_rw_t<io_, bitpos::B7> RIS;
	};
	static risr_t RISR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマリセットレジスタ WDTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static wo8_t<0x0031> WDTR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマスタートレジスタ WDTS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static wo8_t<0x0032> WDTS;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマ制御レジスタ WDTC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct wdtc_t : public rw8_t<0x0033> {
		typedef rw8_t<0x0033> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> WDTC6;
		bit_rw_t<io_, bitpos::B7> WDTC7;
	};
	static wdtc_t WDTC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  カウントソース保護モードレジスタ CSPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct cspr_t : public rw8_t<0x0034> {
		typedef rw8_t<0x0034> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B7> CSPRO;
	};
	static cspr_t CSPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  周期タイマ割り込み制御レジスタ WDTIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct wdtir_t : public rw8_t<0x0035> {
		typedef rw8_t<0x0035> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> WDTIF;
		bit_rw_t<io_, bitpos::B7> WDTIE;
	};
	static wdtir_t WDTIR;

}
