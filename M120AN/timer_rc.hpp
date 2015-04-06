#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマーＲＣレジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣカウンタ TRCCNT
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00E8> TRCCNT;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＡ TRCGRA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00EA> TRCGRA;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＢ TRCGRB
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00EC> TRCGRB;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＣ TRCGRC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00EE> TRCGRC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣジェネラルレジスタＤ TRCGRD
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x00F0> TRCGRD;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣモードレジスタ TRCMR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F2> trcmr_io;
	struct trcmr_t : public trcmr_io {
		using trcmr_io::operator =;
		using trcmr_io::operator ();
		using trcmr_io::operator |=;
		using trcmr_io::operator &=;

		bit_t<trcmr_io, 0> PWMB;
		bit_t<trcmr_io, 1> PWMC;
		bit_t<trcmr_io, 2> PWMD;
		bit_t<trcmr_io, 3> PWM2;
		bit_t<trcmr_io, 4> BUFEA;
		bit_t<trcmr_io, 5> BUFEB;
		bit_t<trcmr_io, 7> CTS;
	};
	static trcmr_t TRCMR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ制御レジスタ１ TRCCR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F3> trccr1_io;
	struct trccr1_t : public trccr1_io {
		using trccr1_io::operator =;
		using trccr1_io::operator ();
		using trccr1_io::operator |=;
		using trccr1_io::operator &=;

		bit_t<trccr1_io, 0> TOA;
		bit_t<trccr1_io, 1> TOB;
		bit_t<trccr1_io, 2> TOC;
		bit_t<trccr1_io, 3> TOD;
		bits_t<trccr1_io, 4, 3> CKS;
		bit_t<trccr1_io, 7> CCLR;
	};
	static trccr1_t TRCCR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ割り込み許可レジスタ TRCIER
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F4> trcier_io;
	struct trcier_t : public trcier_io {
		using trcier_io::operator =;
		using trcier_io::operator ();
		using trcier_io::operator |=;
		using trcier_io::operator &=;

		bit_t<trcier_io, 0> IMIEA;
		bit_t<trcier_io, 1> IMIEB;
		bit_t<trcier_io, 2> IMIEC;
		bit_t<trcier_io, 3> IMIED;
		bit_t<trcier_io, 7> OVIE;
	};
	static trcier_t TRCIER;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣステータスレジスタ TRCSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F5> trcsr_io;
	struct trcsr_t : public trcsr_io {
		using trcsr_io::operator =;
		using trcsr_io::operator ();
		using trcsr_io::operator |=;
		using trcsr_io::operator &=;

		bit_t<trcsr_io, 0> IMFA;
		bit_t<trcsr_io, 1> IMFB;
		bit_t<trcsr_io, 2> IMFC;
		bit_t<trcsr_io, 3> IMFD;
		bit_t<trcsr_io, 7> OVF;
	};
	static trcsr_t TRCSR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ Ｉ／Ｏ制御レジスタ０ TRCIOR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F6> trcior0_io;
	struct trcior0_t : public trcior0_io {
		using trcior0_io::operator =;
		using trcior0_io::operator ();
		using trcior0_io::operator |=;
		using trcior0_io::operator &=;

		bits_t<trcior0_io, 0, 3> IOA;
		bit_t<trcior0_io, 2> IOA2;
		bits_t<trcior0_io, 4, 3> IOB;
		bit_t<trcior0_io, 6> IOB2;
	};
	static trcior0_t TRCIOR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ Ｉ／Ｏ制御レジスタ１ TRCIOR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F7> trcior1_io;
	struct trcior1_t : public trcior1_io {
		using trcior1_io::operator =;
		using trcior1_io::operator ();
		using trcior1_io::operator |=;
		using trcior1_io::operator &=;

		bits_t<trcior1_io, 0, 4> IOC;
		bit_t<trcior1_io, 2> IOC2;
		bit_t<trcior1_io, 3> IOC3;
		bits_t<trcior1_io, 4, 4> IOD;
		bit_t<trcior1_io, 6> IOD2;
		bit_t<trcior1_io, 7> IOD3;
	};
	static trcior1_t TRCIOR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ制御レジスタ２ TRCCR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F8> trccr2_io;
	struct trccr2_t : public trccr2_io {
		using trccr2_io::operator =;
		using trccr2_io::operator ();
		using trccr2_io::operator |=;
		using trccr2_io::operator &=;

		bit_t<trccr2_io, 0> POLB;
		bit_t<trccr2_io, 1> POLC;
		bit_t<trccr2_io, 2> POLD;

		bit_t<trccr2_io, 5> CSTP;
		bits_t<trccr2_io, 6, 2> TCEG;
	};
	static trccr2_t TRCCR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣデジタルフィルタ機能制御レジスタ TRCDF
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00F9> trcdf_io;
	struct trcdf_t : public trcdf_io {
		using trcdf_io::operator =;
		using trcdf_io::operator ();
		using trcdf_io::operator |=;
		using trcdf_io::operator &=;

		bit_t<trcdf_io, 0> DFA;
		bit_t<trcdf_io, 1> DFB;
		bit_t<trcdf_io, 2> DFC;
		bit_t<trcdf_io, 3> DFD;
		bit_t<trcdf_io, 4> DFTRG;
		bits_t<trcdf_io, 6, 2> DFCK;
	};
	static trcdf_t TRCDF;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ出力許可レジスタ TRCOER
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00FA> trcoer_io;
	struct trcoer_t : public trcoer_io {
		using trcoer_io::operator =;
		using trcoer_io::operator ();
		using trcoer_io::operator |=;
		using trcoer_io::operator &=;

		bit_t<trcoer_io, 0> EA;
		bit_t<trcoer_io, 1> EB;
		bit_t<trcoer_io, 2> EC;
		bit_t<trcoer_io, 3> ED;
		bit_t<trcoer_io, 7> PTO;
	};
	static trcoer_t TRCOER;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ A/D 変換トリガ制御レジスタ TRCADCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00FB> trcadcr_io;
	struct trcadcr_t : public trcadcr_io {
		using trcadcr_io::operator =;
		using trcadcr_io::operator ();
		using trcadcr_io::operator |=;
		using trcadcr_io::operator &=;

		bit_t<trcadcr_io, 0> ADTRGAE;
		bit_t<trcadcr_io, 1> ADTRGBE;
		bit_t<trcadcr_io, 2> ADTRGCE;
		bit_t<trcadcr_io, 3> ADTRGDE;
	};
	static trcadcr_t TRCADCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣ波形出力操作レジスタ TRCOPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x00FC> trcopr_io;
	struct trcopr_t : public trcopr_io {
		using trcopr_io::operator =;
		using trcopr_io::operator ();
		using trcopr_io::operator |=;
		using trcopr_io::operator &=;

		bits_t<trcopr_io, 0, 2> OPSEL;
		bits_t<trcopr_io, 2, 2> OPOL;
		bit_t<trcopr_io, 4> RESTATS;
		bit_t<trcopr_io, 5> OPE;
	};
	static trcopr_t TRCOPR;

}
