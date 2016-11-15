#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＢレジスター定義 @n
			・タイマモード @n
			・波形発生モード（任意もパルス幅を連続して出力）@n
			・ワンショットモード（ワンショットパルスを出力）@n
			・ウェエイトワンショットモード（遅延ワンショットパルス出力）@n
			Copyright 2015, 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢ制御レジスタ TRBCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trbcr_t : public rw8_t<0x00E0> {
		typedef rw8_t<0x00E0> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> TSTART;
		bit_rw_t<io_, bitpos::B1> TCSTF;
		bit_rw_t<io_, bitpos::B2> TSTOP;
	};
	static trbcr_t TRBCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢワンショット制御レジスタ TRBOCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trbocr_t : public rw8_t<0x00E1> {
		typedef rw8_t<0x00E1> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> TOSST;
		bit_rw_t<io_, bitpos::B1> TOSSP;
		bit_rw_t<io_, bitpos::B2> TOSSTF;
	};
	static trbocr_t TRBOCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢ I/O 制御レジスタ TRBIOC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trbioc_t : public rw8_t<0x00E2> {
		typedef rw8_t<0x00E2> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> TOPL;
		bit_rw_t<io_, bitpos::B1> TOCNT;
		bit_rw_t<io_, bitpos::B2> INOSTG;
		bit_rw_t<io_, bitpos::B3> INOSEG;
	};
	static trbioc_t TRBIOC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢモードレジスタ TRBMR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trbmr_t : public rw8_t<0x00E3> {
		typedef rw8_t<0x00E3> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> TMOD;
		bit_rw_t <io_, bitpos::B2>    TCNT16;
		bit_rw_t <io_, bitpos::B3>    TWRC;
		bits_rw_t<io_, bitpos::B4, 3> TCK;
		bit_rw_t <io_, bitpos::B7>    TCKCUT;
	};
	static trbmr_t TRBMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢプリスケーラレジスタ TRBPRE
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x00E4> TRBPRE;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢプライマリレジスタ TRBPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x00E5> TRBPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢセカンダリレジスタ TRBSC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x00E6> TRBSC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢ割り込み制御レジスタ TRBIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trbir_t : public rw8_t<0x00E7> {
		typedef rw8_t<0x00E7> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> TRBIF;
		bit_rw_t<io_, bitpos::B7> TRBIE;
	};
	static trbir_t TRBIR;

}
