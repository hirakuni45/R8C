#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＢレジスター定義 @n
			・タイマモード @n
			・波形発生モード（任意もパルス幅を連続して出力）@n
			・ワンショットモード（ワンショットパルスを出力）@n
			・ウェエイトワンショットモード（遅延ワンショットパルス出力）@n
			Copyright 2015 Kunihito Hiramatsu
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
	typedef io8<0x00E0> trbcr_io;
	struct trbcr_t : public trbcr_io {
		using trbcr_io::operator =;
		using trbcr_io::operator ();
		using trbcr_io::operator |=;
		using trbcr_io::operator &=;

		bit_t<trbcr_io, 0> TSTART;
		bit_t<trbcr_io, 1> TCSTF;
		bit_t<trbcr_io, 2> TSTOP;
	};
	static trbcr_t TRBCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢワンショット制御レジスタ TRBOCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00E1> trbocr_io;
	struct trbocr_t : public trbocr_io {
		using trbocr_io::operator =;
		using trbocr_io::operator ();
		using trbocr_io::operator |=;
		using trbocr_io::operator &=;

		bit_t<trbocr_io, 0> TOSST;
		bit_t<trbocr_io, 1> TOSSP;
		bit_t<trbocr_io, 2> TOSSTF;
	};
	static trbocr_t TRBOCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢ I/O 制御レジスタ TRBIOC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00E2> trbioc_io;
	struct trbioc_t : public trbioc_io {
		using trbioc_io::operator =;
		using trbioc_io::operator ();
		using trbioc_io::operator |=;
		using trbioc_io::operator &=;

		bit_t<trbioc_io, 0> TOPL;
		bit_t<trbioc_io, 1> TOCNT;
		bit_t<trbioc_io, 2> INOSTG;
		bit_t<trbioc_io, 3> INOSEG;
	};
	static trbioc_t TRBIOC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢモードレジスタ TRBMR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00E3> trbmr_io;
	struct trbmr_t : public trbmr_io {
		using trbmr_io::operator =;
		using trbmr_io::operator ();
		using trbmr_io::operator |=;
		using trbmr_io::operator &=;

		bits_t<trbmr_io, 0, 2> TMOD;
		bit_t<trbmr_io, 2> TCNT16;
		bit_t<trbmr_io, 3> TWRC;
		bits_t<trbmr_io, 4, 3> TCK;
		bit_t<trbmr_io, 7> TCKCUT;
	};
	static trbmr_t TRBMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢプリスケーラレジスタ TRBPRE
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x00E4> TRBPRE;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢプライマリレジスタ TRBPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x00E5> TRBPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢセカンダリレジスタ TRBSC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x00E6> TRBSC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＢ割り込み制御レジスタ TRBIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00E7> trbir_io;
	struct trbir_t : public trbir_io {
		using trbir_io::operator =;
		using trbir_io::operator ();
		using trbir_io::operator |=;
		using trbir_io::operator &=;

		bit_t<trbir_io, 6> TRBIF;
		bit_t<trbir_io, 7> TRBIE;
	};
	static trbir_t TRBIR;

}
