#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・システム・レジスター定義 @n
			Copyright 2014,2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プロセッサモードレジスタ０ PM0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pm0_t : public rw8_t<0x0010> {
		typedef rw8_t<0x0010> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> SRST;
	};
	static pm0_t PM0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  モジュールスタンバイ制御レジスタ MSTCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct mstcr_t : public rw8_t<0x0012> {
		typedef rw8_t<0x0012> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> MSTTRJ;
		bit_rw_t<io_, bitpos::B1> MSTTRB;

		bit_rw_t<io_, bitpos::B4> MSTAD;
		bit_rw_t<io_, bitpos::B5> MSTTRC;
		bit_rw_t<io_, bitpos::B6> MSTUART;
	};
	static mstcr_t MSTCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プロテクトレジスタ PRCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct prcr_t : public rw8_t<0x0013> {
		typedef rw8_t<0x0013> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> PRC0;
		bit_rw_t<io_, bitpos::B1> PRC1;

		bit_rw_t<io_, bitpos::B3> PRC3;
		bit_rw_t<io_, bitpos::B4> PRC4;
	};
	static prcr_t PRCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ハードウェアリセットプロテクトレジスタ HRPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct hrpr_t : public rw8_t<0x0016> {
		typedef rw8_t<0x0016> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> RAMCRE;
	};
	static hrpr_t HRPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  リセット要因判別レジスタ RSTFR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct rstfr_t : public rw8_t<0x005f> {
		typedef rw8_t<0x005f> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> CWR;

		typedef ro8_t<0x005f> ro_;
		bit_ro_t<ro_, bitpos::B1> HWR;
		bit_ro_t<ro_, bitpos::B2> SWR;
		bit_ro_t<ro_, bitpos::B3> WDR;
	};
	static rstfr_t RSTFR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オプション機能選択レジスタ２ OFS2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct ofs2_t : public rw8_t<0xFFDB> {
		typedef rw8_t<0xFFDB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> WDTUFS;
		bits_rw_t<io_, bitpos::B2, 2> WDTRCS;
		bit_rw_t <io_, bitpos::B5>    MSTINI;
	};
	static ofs2_t OFS2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オプション機能選択レジスタ OFS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct ofs_t : public rw8_t<0xFFFF> {
		typedef rw8_t<0xFFFF> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    WDTON;
		bit_rw_t <io_, bitpos::B2>    ROMCR;
		bit_rw_t <io_, bitpos::B3>    ROMCP1;
		bits_rw_t<io_, bitpos::B4, 2> VDSEL;
		bit_rw_t <io_, bitpos::B6>    LVDAS;
		bit_rw_t <io_, bitpos::B7>    CSPRONI;
	};
	static ofs_t OFS;

}
