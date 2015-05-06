#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・コンパレーターレジスタ定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター制御レジスタ WCMPR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0180> wcmpr_io;
	struct wcmpr_t : public wcmpr_io {
		using wcmpr_io::operator =;
		using wcmpr_io::operator ();
		using wcmpr_io::operator |=;
		using wcmpr_io::operator &=;

		bit_t<wcmpr_io, 0> WCB1M0;
		bit_t<wcmpr_io, 3> WCB1OUT;
		bit_t<wcmpr_io, 4> WCB3M0;
		bit_t<wcmpr_io, 7> WCB3OUT;
	};
	static wcmpr_t WCMPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター B1 割り込み制御レジスタ WCB1INTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0181> wcb1intr_io;
	struct wcb1intr_t : public wcb1intr_io {
		using wcb1intr_io::operator =;
		using wcb1intr_io::operator ();
		using wcb1intr_io::operator |=;
		using wcb1intr_io::operator &=;

		bits_t<wcb1intr_io, 0, 2> WCB1FL;
		bits_t<wcb1intr_io, 4, 2> WCB1S;
		bit_t<wcb1intr_io, 6> WCB1INTEN;
		bit_t<wcb1intr_io, 7> WCB1F;
	};
	static wcb1intr_t WCB1INTR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター B3 割り込み制御レジスタ WCB3INTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0182> wcb3intr_io;
	struct wcb3intr_t : public wcb3intr_io {
		using wcb3intr_io::operator =;
		using wcb3intr_io::operator ();
		using wcb3intr_io::operator |=;
		using wcb3intr_io::operator &=;

		bits_t<wcb3intr_io, 0, 2> WCB3FL;
		bits_t<wcb3intr_io, 4, 2> WCB3S;
		bit_t<wcb3intr_io, 6> WCB3INTEN;
		bit_t<wcb3intr_io, 7> WCB3F;
	};
	static wcb3intr_t WCB3INTR;

}
