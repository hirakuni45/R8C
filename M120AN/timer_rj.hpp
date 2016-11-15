#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＪレジスター定義 @n
			Copyright 2015, 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪカウンタレジスタ TRJ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00D8> TRJ;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ制御レジスタ TRJCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trjcr_t : public rw8_t<0x00DA> {
		typedef rw8_t<0x00DA> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> TSTART;
		bit_rw_t<io_, bitpos::B1> TCSTF;
		bit_rw_t<io_, bitpos::B2> TSTOP;
		bit_rw_t<io_, bitpos::B4> TEDGF;
		bit_rw_t<io_, bitpos::B5> TUNDF;
	};
	static trjcr_t TRJCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ I/O 制御レジスタ TRJIOC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trjioc_t : public rw8_t<0x00DB> {
		typedef rw8_t<0x00DB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    TEDGSEL;
		bit_rw_t <io_, bitpos::B1>    TOPCR;
		bits_rw_t<io_, bitpos::B4, 2> TIPF;
		bits_rw_t<io_, bitpos::B6, 2> TIOGT;
	};
	static trjioc_t TRJIOC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪモードレジスタ TRJMR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trjmr_t : public rw8_t<0x00DC> {
		typedef rw8_t<0x00DC> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 3> TMOD;
		bit_rw_t <io_, bitpos::B3>    TEDGPL;
		bits_rw_t<io_, bitpos::B4, 3> TCK;
		bit_rw_t <io_, bitpos::B7>    TCKCUT;
	};
	static trjmr_t TRJMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪイベント選択レジスタ TRJISR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trjisr_t : public rw8_t<0x00DD> {
		typedef rw8_t<0x00DD> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> RCCPSEL;
		bit_rw_t <io_, bitpos::B2>    RCCPSEL2;
	};
	static trjisr_t TRJISR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＪ割り込み制御レジスタ TRJIR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trjir_t : public rw8_t<0x00DE> {
		typedef rw8_t<0x00DE> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> TRJIF;
		bit_rw_t<io_, bitpos::B7> TRJIE;
	};
	static trjir_t TRJIR;

}
