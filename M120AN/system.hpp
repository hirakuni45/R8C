#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・システム・レジスター定義 @n
			Copyright 2014 Kunihito Hiramatsu
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
	typedef io8<0x0010> pm0_io;
	struct pm0_t : public pm0_io {
		using pm0_io::operator =;
		using pm0_io::operator ();
		using pm0_io::operator |=;
		using pm0_io::operator &=;

		bit_t<pm0_io, 3> SRST;
	};
	static pm0_t PM0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  モジュールスタンバイ制御レジスタ MSTCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0012> mstcr_io;
	struct mstcr_t : public mstcr_io {
		using mstcr_io::operator =;
		using mstcr_io::operator ();
		using mstcr_io::operator |=;
		using mstcr_io::operator &=;

		bit_t<mstcr_io, 0> MSTTRJ;
		bit_t<mstcr_io, 1> MSTTRB;

		bit_t<mstcr_io, 4> MSTAD;
		bit_t<mstcr_io, 5> MSTTRC;
		bit_t<mstcr_io, 6> MSTUART;
	};
	static mstcr_t MSTCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プロテクトレジスタ PRCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0013> prcr_io;
	struct prcr_t : public prcr_io {
		using prcr_io::operator =;
		using prcr_io::operator ();
		using prcr_io::operator |=;
		using prcr_io::operator &=;

		bit_t<prcr_io, 0> PRC0;
		bit_t<prcr_io, 1> PRC1;

		bit_t<prcr_io, 3> PRC3;
		bit_t<prcr_io, 4> PRC4;
	};
	static prcr_t PRCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ハードウェアリセットプロテクトレジスタ HRPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0016> hrpr_io;
	struct hrpr_t : public hrpr_io {
		using hrpr_io::operator =;
		using hrpr_io::operator ();
		using hrpr_io::operator |=;
		using hrpr_io::operator &=;

		bit_t<hrpr_io, 0> RAMCRE;
	};
	static hrpr_t HRPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  リセット要因判別レジスタ RSTFR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x005f> rstfr_io;
	typedef io8_ro<0x005f> rstfr_io_ro;
	struct rstfr_t : public rstfr_io, rstfr_io_ro {
		using rstfr_io::operator =;
		using rstfr_io::operator ();
		using rstfr_io::operator |=;
		using rstfr_io::operator &=;

		using rstfr_io_ro::operator ();

		bit_t<rstfr_io, 0> CWR;

		bit_t<rstfr_io_ro, 1> HWR;
		bit_t<rstfr_io_ro, 2> SWR;
		bit_t<rstfr_io_ro, 3> WDR;
	};
	static rstfr_t RSTFR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オプション機能選択レジスタ２ OFS2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0xffdb> ofs2_io;
	struct ofs2_t : public ofs2_io {
		using ofs2_io::operator =;
		using ofs2_io::operator ();
		using ofs2_io::operator |=;
		using ofs2_io::operator &=;

		bits_t<ofs2_io, 0, 2> WDTUFS;
		bits_t<ofs2_io, 2, 2> WDTRCS;
		bit_t<ofs2_io, 5> MSTINI;
	};
	static ofs2_t OFS2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オプション機能選択レジスタ OFS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0xffff> ofs_io;
	struct ofs_t : public ofs_io {
		using ofs_io::operator =;
		using ofs_io::operator ();
		using ofs_io::operator |=;
		using ofs_io::operator &=;

		bit_t<ofs_io, 0> WDTON;
		bit_t<ofs_io, 2> ROMCR;
		bit_t<ofs_io, 3> ROMCP1;
		bits_t<ofs_io, 4, 2> VDSEL;
		bit_t<ofs_io, 6> LVDAS;
		bit_t<ofs_io, 7> CSPRONI;
	};
	static ofs_t OFS;

}
