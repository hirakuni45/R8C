#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・フラッシュ・レジスター定義 @n
			Copyright 2015, 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリステータスレジスタ FST
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct fst_t : public rw8_t<0x01A9> {
		typedef rw8_t<0x01A9> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> RDYSTI;
		bit_rw_t<io_, bitpos::B1> BSYAEI;
		bit_rw_t<io_, bitpos::B2> FST2;
		bit_rw_t<io_, bitpos::B3> FST3;
		bit_rw_t<io_, bitpos::B4> FST4;
		bit_rw_t<io_, bitpos::B5> FST5;
		bit_rw_t<io_, bitpos::B6> FST6;
		bit_rw_t<io_, bitpos::B7> FST7;
	};
	static fst_t FST;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ０ FMR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct fmr0_t : public rw8_t<0x01AA> {
		typedef rw8_t<0x01AA> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B1> FMR01;
		bit_rw_t<io_, bitpos::B2> FMR02;
		bit_rw_t<io_, bitpos::B3> FMSTP;
		bit_rw_t<io_, bitpos::B4> CMDRST;
		bit_rw_t<io_, bitpos::B5> CMDERIE;
		bit_rw_t<io_, bitpos::B6> BSYAEIE;
		bit_rw_t<io_, bitpos::B7> RDYSTIE;
	};
	static fmr0_t FMR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ１ FMR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct fmr1_t : public rw8_t<0x01AB> {
		typedef rw8_t<0x01AB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> WTFMSTP;
		bit_rw_t<io_, bitpos::B3> FMR13;
		bit_rw_t<io_, bitpos::B6> FMR16;
		bit_rw_t<io_, bitpos::B7> FMR17;
	};
	static fmr1_t FMR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ２ FMR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct fmr2_t : public rw8_t<0x01AC> {
		typedef rw8_t<0x01AC> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> FMR20;
		bit_rw_t<io_, bitpos::B1> FMR21;
		bit_rw_t<io_, bitpos::B2> FMR22;
		bit_rw_t<io_, bitpos::B7> FMR27;
	};
	static fmr2_t FMR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリリフレッシュ制御レジスタ FREFR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct frefr_t : public rw8_t<0x01AD> {
		typedef rw8_t<0x01AD> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> REF0;
		bit_rw_t<io_, bitpos::B1> REF1;
		bit_rw_t<io_, bitpos::B2> REF2;
		bit_rw_t<io_, bitpos::B3> REF3;
		bit_rw_t<io_, bitpos::B4> REF4;
		bit_rw_t<io_, bitpos::B5> REF5;
	};
	static frefr_t FREFR;

}
