#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・フラッシュ・レジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
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
	typedef io8<0x01a9> fst_io;
	struct fst_t : public fst_io {
		using fst_io::operator =;
		using fst_io::operator ();
		using fst_io::operator |=;
		using fst_io::operator &=;

		bit_t<fst_io, 0> RDYSTI;
		bit_t<fst_io, 1> BSYAEI;
		bit_t<fst_io, 2> FST2;
		bit_t<fst_io, 3> FST3;
		bit_t<fst_io, 4> FST4;
		bit_t<fst_io, 5> FST5;
		bit_t<fst_io, 6> FST6;
		bit_t<fst_io, 7> FST7;
	};
	static fst_t FST;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ０ FMR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01aa> fmr0_io;
	struct fmr0_t : public fmr0_io {
		using fmr0_io::operator =;
		using fmr0_io::operator ();
		using fmr0_io::operator |=;
		using fmr0_io::operator &=;

		bit_t<fmr0_io, 1> FMR01;
		bit_t<fmr0_io, 2> FMR02;
		bit_t<fmr0_io, 3> FMSTP;
		bit_t<fmr0_io, 4> CMDRST;
		bit_t<fmr0_io, 5> CMDERIE;
		bit_t<fmr0_io, 6> BSYAEIE;
		bit_t<fmr0_io, 7> RDYSTIE;
	};
	static fmr0_t FMR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ１ FMR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01ab> fmr1_io;
	struct fmr1_t : public fmr1_io {
		using fmr1_io::operator =;
		using fmr1_io::operator ();
		using fmr1_io::operator |=;
		using fmr1_io::operator &=;

		bit_t<fmr1_io, 2> WTFMSTP;
		bit_t<fmr1_io, 3> FMR13;
		bit_t<fmr1_io, 6> FMR16;
		bit_t<fmr1_io, 7> FMR17;
	};
	static fmr1_t FMR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリ制御レジスタ２ FMR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01ac> fmr2_io;
	struct fmr2_t : public fmr2_io {
		using fmr2_io::operator =;
		using fmr2_io::operator ();
		using fmr2_io::operator |=;
		using fmr2_io::operator &=;

		bit_t<fmr2_io, 0> FMR20;
		bit_t<fmr2_io, 1> FMR21;
		bit_t<fmr2_io, 2> FMR22;
		bit_t<fmr2_io, 7> FMR27;
	};
	static fmr2_t FMR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュメモリリフレッシュ制御レジスタ FREFR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01ad> frefr_io;
	struct frefr_t : public frefr_io {
		using frefr_io::operator =;
		using frefr_io::operator ();
		using frefr_io::operator |=;
		using frefr_io::operator &=;

		bit_t<frefr_io, 0> REF0;
		bit_t<frefr_io, 1> REF1;
		bit_t<frefr_io, 2> REF2;
		bit_t<frefr_io, 3> REF3;
		bit_t<frefr_io, 4> REF4;
		bit_t<frefr_io, 5> REF5;
	};
	static frefr_t FREFR;

}
