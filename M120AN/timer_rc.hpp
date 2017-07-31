#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＣレジスター定義
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
		@brief  タイマＲＣカウンタ TRCCNT
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00E8> TRCCNT;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＡ TRCGRA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00EA> TRCGRA;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＢ TRCGRB
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00EC> TRCGRB;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＣ TRCGRC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00EE> TRCGRC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＤ TRCGRD
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw16_t<0x00F0> TRCGRD;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣモードレジスタ TRCMR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcmr_t : public rw8_t<0x00F2> {
		typedef rw8_t<0x00F2> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> PWMB;
		bit_rw_t<io_, bitpos::B1> PWMC;
		bit_rw_t<io_, bitpos::B2> PWMD;
		bit_rw_t<io_, bitpos::B3> PWM2;
		bit_rw_t<io_, bitpos::B4> BUFEA;
		bit_rw_t<io_, bitpos::B5> BUFEB;
		bit_rw_t<io_, bitpos::B7> CTS;
	};
	static trcmr_t TRCMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ制御レジスタ１ TRCCR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trccr1_t : public rw8_t<0x00F3> {
		typedef rw8_t<0x00F3> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    TOA;
		bit_rw_t <io_, bitpos::B1>    TOB;
		bit_rw_t <io_, bitpos::B2>    TOC;
		bit_rw_t <io_, bitpos::B3>    TOD;
		bits_rw_t<io_, bitpos::B4, 3> CKS;
		bit_rw_t <io_, bitpos::B7>    CCLR;
	};
	static trccr1_t TRCCR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ割り込み許可レジスタ TRCIER
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcier_t : public rw8_t<0x00F4> {
		typedef rw8_t<0x00F4> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> IMIEA;
		bit_rw_t<io_, bitpos::B1> IMIEB;
		bit_rw_t<io_, bitpos::B2> IMIEC;
		bit_rw_t<io_, bitpos::B3> IMIED;
		bit_rw_t<io_, bitpos::B7> OVIE;
	};
	static trcier_t TRCIER;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣステータスレジスタ TRCSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcsr_t : public rw8_t<0x00F5> {
		typedef rw8_t<0x00F5> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> IMFA;
		bit_rw_t<io_, bitpos::B1> IMFB;
		bit_rw_t<io_, bitpos::B2> IMFC;
		bit_rw_t<io_, bitpos::B3> IMFD;
		bit_rw_t<io_, bitpos::B7> OVF;
	};
	static trcsr_t TRCSR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ Ｉ／Ｏ制御レジスタ０ TRCIOR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcior0_t : public rw8_t<0x00F6> {
		typedef rw8_t<0x00F6> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 3> IOA;
		bit_rw_t <io_, bitpos::B2>    IOA2;
		bits_rw_t<io_, bitpos::B4, 3> IOB;
		bit_rw_t <io_, bitpos::B6>    IOB2;
	};
	static trcior0_t TRCIOR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ Ｉ／Ｏ制御レジスタ１ TRCIOR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcior1_t : public rw8_t<0x00F7> {
		typedef rw8_t<0x00F7> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 4> IOC;
		bit_rw_t <io_, bitpos::B2>    IOC2;
		bit_rw_t <io_, bitpos::B3>    IOC3;
		bits_rw_t<io_, bitpos::B4, 4> IOD;
		bit_rw_t <io_, bitpos::B6>    IOD2;
		bit_rw_t <io_, bitpos::B7>    IOD3;
	};
	static trcior1_t TRCIOR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ制御レジスタ２ TRCCR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trccr2_t : public rw8_t<0x00F8> {
		typedef rw8_t<0x00F8> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    POLB;
		bit_rw_t <io_, bitpos::B1>    POLC;
		bit_rw_t <io_, bitpos::B2>    POLD;

		bit_rw_t <io_, bitpos::B5>    CSTP;
		bits_rw_t<io_, bitpos::B6, 2> TCEG;
	};
	static trccr2_t TRCCR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣデジタルフィルタ機能制御レジスタ TRCDF
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcdf_t : public rw8_t<0x00F9> {
		typedef rw8_t<0x00F9> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    DFA;
		bit_rw_t <io_, bitpos::B1>    DFB;
		bit_rw_t <io_, bitpos::B2>    DFC;
		bit_rw_t <io_, bitpos::B3>    DFD;
		bit_rw_t <io_, bitpos::B4>    DFTRG;
		bits_rw_t<io_, bitpos::B6, 2> DFCK;
	};
	static trcdf_t TRCDF;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ出力許可レジスタ TRCOER
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcoer_t : public rw8_t<0x00FA> {
		typedef rw8_t<0x00FA> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> EA;
		bit_rw_t<io_, bitpos::B1> EB;
		bit_rw_t<io_, bitpos::B2> EC;
		bit_rw_t<io_, bitpos::B3> ED;
		bit_rw_t<io_, bitpos::B7> PTO;
	};
	static trcoer_t TRCOER;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ A/D 変換トリガ制御レジスタ TRCADCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcadcr_t : public rw8_t<0x00FB> {
		typedef rw8_t<0x00FB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> ADTRGAE;
		bit_rw_t<io_, bitpos::B1> ADTRGBE;
		bit_rw_t<io_, bitpos::B2> ADTRGCE;
		bit_rw_t<io_, bitpos::B3> ADTRGDE;
	};
	static trcadcr_t TRCADCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ波形出力操作レジスタ TRCOPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct trcopr_t : public rw8_t<0x00FC> {
		typedef rw8_t<0x00FC> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> OPSEL;
		bits_rw_t<io_, bitpos::B2, 2> OPOL;
		bit_rw_t <io_, bitpos::B4>    RESTATS;
		bit_rw_t <io_, bitpos::B5>    OPE;
	};
	static trcopr_t TRCOPR;

}
