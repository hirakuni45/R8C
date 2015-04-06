#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＪレジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪカウンタレジスタ TRJ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00D8> TRJ;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ制御レジスタ TRJCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00DA> trjcr_io;
	struct trjcr_t : public trjcr_io {
		using trjcr_io::operator =;
		using trjcr_io::operator ();
		using trjcr_io::operator |=;
		using trjcr_io::operator &=;

		bit_t<trjcr_io, 0> TSTART;
		bit_t<trjcr_io, 1> TCSTF;
		bit_t<trjcr_io, 2> TSTOP;
		bit_t<trjcr_io, 5> TEDGF;
		bit_t<trjcr_io, 6> TUNDF;
	};
	static trjcr_t TRJCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ I/O 制御レジスタ TRJIOC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00DB> trjioc_io;
	struct trjioc_t : public trjioc_io {
		using trjioc_io::operator =;
		using trjioc_io::operator ();
		using trjioc_io::operator |=;
		using trjioc_io::operator &=;

		bit_t<trjioc_io, 0> TEDGSEL;
		bit_t<trjioc_io, 1> TOPCR;
		bits_t<trjioc_io, 4, 2> TIPF;
		bits_t<trjioc_io, 6, 2> TIOGT;
	};
	static trjioc_t TRJIOC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪモードレジスタ TRJIOC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00DC> trjmr_io;
	struct trjmr_t : public trjmr_io {
		using trjmr_io::operator =;
		using trjmr_io::operator ();
		using trjmr_io::operator |=;
		using trjmr_io::operator &=;

		bits_t<trjmr_io, 0, 3> TMOD;
		bit_t<trjmr_io, 3> TEDGPL;
		bits_t<trjmr_io, 4, 3> TCK;
		bit_t<trjmr_io, 7> TCKCUT;
	};
	static trjmr_t TRJMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪイベント選択レジスタ TRJISR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00DD> trjisr_io;
	struct trjisr_t : public trjisr_io {
		using trjisr_io::operator =;
		using trjisr_io::operator ();
		using trjisr_io::operator |=;
		using trjisr_io::operator &=;

		bits_t<trjisr_io, 0, 2> RCCPSEL;
		bit_t<trjisr_io, 2> RCCPSEL2;
	};
	static trjisr_t TRJISR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ割り込み制御レジスタ TRJIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00DE> trjir_io;
	struct trjir_t : public trjir_io {
		using trjir_io::operator =;
		using trjir_io::operator ();
		using trjir_io::operator |=;
		using trjir_io::operator &=;

		bit_t<trjir_io, 6> TRJIF;
		bit_t<trjir_io, 7> TRJIE;
	};
	static trjir_t TRJIR;





}
