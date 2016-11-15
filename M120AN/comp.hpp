#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・コンパレーターレジスタ定義 @n
			Copyright 2015, 2016 Kunihito Hiramatsu
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
	struct wcmpr_t : public rw8_t<0x0180> {
		typedef rw8_t<0x0180> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> WCB1M0;
		bit_rw_t<io_, bitpos::B3> WCB1OUT;
		bit_rw_t<io_, bitpos::B4> WCB3M0;
		bit_rw_t<io_, bitpos::B7> WCB3OUT;
	};
	static wcmpr_t WCMPR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター B1 割り込み制御レジスタ WCB1INTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct wcb1intr_t : public rw8_t<0x0181> {
		typedef rw8_t<0x0181> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> WCB1FL;
		bits_rw_t<io_, bitpos::B4, 2> WCB1S;
		bit_rw_t <io_, bitpos::B6>    WCB1INTEN;
		bit_rw_t <io_, bitpos::B7>    WCB1F;
	};
	static wcb1intr_t WCB1INTR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  コンパレーター B3 割り込み制御レジスタ WCB3INTR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct wcb3intr_t : public rw8_t<0x0182> {
		typedef rw8_t<0x0182> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> WCB3FL;
		bits_rw_t<io_, bitpos::B4, 2> WCB3S;
		bit_rw_t <io_, bitpos::B6>    WCB3INTEN;
		bit_rw_t <io_, bitpos::B7>    WCB3F;
	};
	static wcb3intr_t WCB3INTR;

}
