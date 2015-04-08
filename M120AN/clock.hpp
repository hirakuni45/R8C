#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・クロック・レジスター定義 @n
			Copyright 2014 Kunihito Hiramatsu
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
	typedef io8<0x0020> exckcr_io;
	struct exckcr_t : public exckcr_io {
		using exckcr_io::operator =;
		using exckcr_io::operator ();
		using exckcr_io::operator |=;
		using exckcr_io::operator &=;

		bits_t<exckcr_io, 0, 2> CKPT;
		bit_t<exckcr_io, 6>     XRCUT;
	};
	static exckcr_t EXCKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速／低速オンチップオシレータ制御レジスタ OCOCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0021> ococr_io;
	struct ococr_t : public ococr_io {
		using ococr_io::operator =;
		using ococr_io::operator ();
		using ococr_io::operator |=;
		using ococr_io::operator &=;

		bit_t<ococr_io, 0> HOCOE;
		bit_t<ococr_io, 1> LOCODIS;
	};
	static ococr_t OCOCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	システムクロックｆ制御レジスタ SCKCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0022> sckcr_io;
	struct sckcr_t : public sckcr_io {
		using sckcr_io::operator =;
		using sckcr_io::operator ();
		using sckcr_io::operator |=;
		using sckcr_io::operator &=;

		bits_t<sckcr_io, 0, 3> PHISSEL;
		bit_t<sckcr_io, 5>     WAITM;
		bit_t<sckcr_io, 6>     HSCKSEL;
	};
	static sckcr_t SCKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	システムクロックｆ選択レジスタ PHISEL
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0023> phisel_io;
	struct phisel_t : public phisel_io {
		using phisel_io::operator =;
		using phisel_io::operator ();
		using phisel_io::operator |=;
		using phisel_io::operator &=;
	};
	static phisel_t PHISEL;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	クロック停止制御レジスタ CKSTPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0024> ckstpr_io;
	struct ckstpr_t : public ckstpr_io {
		using ckstpr_io::operator =;
		using ckstpr_io::operator ();
		using ckstpr_io::operator |=;
		using ckstpr_io::operator &=;

		bit_t<ckstpr_io, 0> STPM;
		bit_t<ckstpr_io, 1> WCKSTP;
		bit_t<ckstpr_io, 2> PSCSTP;
		bit_t<ckstpr_io, 7> SCKSEL;
	};
	static ckstpr_t CKSTPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	モード復帰時クロック制御レジスタ CKRSCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0025> ckrscr_io;
	struct ckrscr_t : public ckrscr_io {
		using ckrscr_io::operator =;
		using ckrscr_io::operator ();
		using ckrscr_io::operator |=;
		using ckrscr_io::operator &=;

		bits_t<ckrscr_io, 0, 4> CKST;
		bit_t<ckrscr_io, 5>     PHISRS;
		bit_t<ckrscr_io, 6>     WAITRS;
		bit_t<ckrscr_io, 7>     STOPRS;
	};
	static ckrscr_t CKRSCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	発振停止検出レジスタ BAKCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0026> bakcr_io;
	struct bakcr_t : public bakcr_io {
		using bakcr_io::operator =;
		using bakcr_io::operator ();
		using bakcr_io::operator |=;
		using bakcr_io::operator &=;

		bit_t<bakcr_io, 0> XINBAKE;
		bit_t<bakcr_io, 1> CKSWIE;
		bit_t<bakcr_io, 2> XINHALT;
		bit_t<bakcr_io, 3> CKSWIF;
	};
	static bakcr_t BAKCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ18.432MHz制御レジスタ０ FR18S0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0064> fr18s0_io;
	struct fr18s0_t : public fr18s0_io {
		using fr18s0_io::operator =;
		using fr18s0_io::operator ();
		using fr18s0_io::operator |=;
		using fr18s0_io::operator &=;
	};
	static fr18s0_t FR18S0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ18.432MHz制御レジスタ１ FR18S1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0065> fr18s1_io;
	struct fr18s1_t : public fr18s1_io {
		using fr18s1_io::operator =;
		using fr18s1_io::operator ();
		using fr18s1_io::operator |=;
		using fr18s1_io::operator &=;
	};
	static fr18s1_t FR18S1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ制御レジスタ１ FRV1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0067> frv1_io;
	struct frv1_t : public frv1_io {
		using frv1_io::operator =;
		using frv1_io::operator ();
		using frv1_io::operator |=;
		using frv1_io::operator &=;
	};
	static frv1_t FRV1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	高速オンチップオシレータ制御レジスタ２ FRV2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0068> frv2_io;
	struct frv2_t : public frv2_io {
		using frv2_io::operator =;
		using frv2_io::operator ();
		using frv2_io::operator |=;
		using frv2_io::operator &=;
	};
	static frv2_t FRV2;

}

