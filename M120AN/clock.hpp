#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・クロック・レジスター定義 @n
			Copyright 2014,2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	外部クロック制御レジスタ EXCKCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct exckcr_t : public rw8_t<0x0020> {
		typedef rw8_t<0x0020> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> CKPT;
		bit_rw_t <io_, bitpos::B6>    XRCUT;
	};
	static exckcr_t EXCKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速／低速オンチップオシレータ制御レジスタ OCOCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct ococr_t : public rw8_t<0x0021> {
		typedef rw8_t<0x0021> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> HOCOE;
		bit_rw_t<io_, bitpos::B1> LOCODIS;
	};
	static ococr_t OCOCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	システムクロックｆ制御レジスタ SCKCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct sckcr_t : public rw8_t<0x0022> {
		typedef rw8_t<0x0022> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 3> PHISSEL;
		bit_rw_t <io_, bitpos::B5>    WAITM;
		bit_rw_t <io_, bitpos::B6>    HSCKSEL;
	};
	static sckcr_t SCKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	システムクロックｆ選択レジスタ PHISEL
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct phisel_t : public rw8_t<0x0023> {
		typedef rw8_t<0x0023> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> PHISEL0;
		bit_rw_t<io_, bitpos::B1> PHISEL1;
		bit_rw_t<io_, bitpos::B2> PHISEL2;
		bit_rw_t<io_, bitpos::B3> PHISEL3;
		bit_rw_t<io_, bitpos::B4> PHISEL4;
		bit_rw_t<io_, bitpos::B5> PHISEL5;
		bit_rw_t<io_, bitpos::B6> PHISEL6;
		bit_rw_t<io_, bitpos::B7> PHISEL7;
	};
	static phisel_t PHISEL;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	クロック停止制御レジスタ CKSTPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct ckstpr_t : public rw8_t<0x0024> {
		typedef rw8_t<0x0024> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> STPM;
		bit_rw_t<io_, bitpos::B1> WCKSTP;
		bit_rw_t<io_, bitpos::B2> PSCSTP;
		bit_rw_t<io_, bitpos::B7> SCKSEL;
	};
	static ckstpr_t CKSTPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	モード復帰時クロック制御レジスタ CKRSCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct ckrscr_t : public rw8_t<0x0025> {
		typedef rw8_t<0x0025> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 4> CKST;
		bit_rw_t <io_, bitpos::B5>    PHISRS;
		bit_rw_t <io_, bitpos::B6>    WAITRS;
		bit_rw_t <io_, bitpos::B7>    STOPRS;
	};
	static ckrscr_t CKRSCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	発振停止検出レジスタ BAKCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct bakcr_t : public rw8_t<0x0026> {
		typedef rw8_t<0x0026> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> XINBAKE;
		bit_rw_t<io_, bitpos::B1> CKSWIE;
		bit_rw_t<io_, bitpos::B2> XINHALT;
		bit_rw_t<io_, bitpos::B3> CKSWIF;
	};
	static bakcr_t BAKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ18.432MHz制御レジスタ 0 FR18S0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x0064> FR18S0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ18.432MHz制御レジスタ 1 FR18S1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x0065> FR18S1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ制御レジスタ１ FRV1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x0067> FRV1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ制御レジスタ２ FRV2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x0068> FRV2;

}

