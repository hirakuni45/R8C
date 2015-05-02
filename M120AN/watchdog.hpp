#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・ウォッチドッグ・レジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマ機能レジスタ RISR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0030> risr_io;
	struct risr_t : public risr_io {
		using risr_io::operator =;
		using risr_io::operator ();
		using risr_io::operator |=;
		using risr_io::operator &=;

		bit_t<risr_io, 6> UFIF;
		bit_t<risr_io, 7> RIS;
	};
	static risr_t RISR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマリセットレジスタ WDTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8_wo<0x0031> WDTR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマスタートレジスタ WDTS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8_wo<0x0032> WDTS;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ウォッチドッグタイマ制御レジスタ WDTC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0033> wdtc_io;
	struct wdtc_t : public wdtc_io {
		using wdtc_io::operator =;
		using wdtc_io::operator ();
		using wdtc_io::operator |=;
		using wdtc_io::operator &=;

		bit_t<wdtc_io, 6> WDTC6;
		bit_t<wdtc_io, 7> WDTC7;
	};
	static wdtc_t WDTC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  カウントソース保護モードレジスタ CSPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0034> cspr_io;
	struct cspr_t : public cspr_io {
		using cspr_io::operator =;
		using cspr_io::operator ();
		using cspr_io::operator |=;
		using cspr_io::operator &=;

		bit_t<cspr_io, 7> CSPRO;
	};
	static cspr_t CSPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  周期タイマ割り込み制御レジスタ WDTIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0035> wdtir_io;
	struct wdtir_t : public wdtir_io {
		using wdtir_io::operator =;
		using wdtir_io::operator ();
		using wdtir_io::operator |=;
		using wdtir_io::operator &=;

		bit_t<wdtir_io, 6> WDTIF;
		bit_t<wdtir_io, 7> WDTIE;
	};
	static wdtir_t WDTIR;

}
