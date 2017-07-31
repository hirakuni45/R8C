#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・電圧検出・レジスター定義
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
		@brief  電圧監視回路エッジ選択レジスタ VCAC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct vcac_t : public rw8_t<0x0058> {
		typedef rw8_t<0x0058> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B1> VCAC1;
	};
	static vcac_t VCAC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧検出レジスタ２ VCA2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct vca2_t : public rw8_t<0x005A> {
		typedef rw8_t<0x005A> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> LPE;
		bit_rw_t<io_, bitpos::B5> VC0E;
		bit_rw_t<io_, bitpos::B6> VC1E;
	};
	static vca2_t VCA2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧検出１レベル選択レジスタ VD1LS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct vd1ls_t : public rw8_t<0x005B> {
		typedef rw8_t<0x005B> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B1, 3> VD1S;
	};
	static vd1ls_t VD1LS;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧監視０回路制御レジスタ VW0C
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct vw0c_t : public rw8_t<0x005C> {
		typedef rw8_t<0x005C> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    VW0C0;
		bit_rw_t <io_, bitpos::B1>    VW0C1;
		bits_rw_t<io_, bitpos::B4, 2> VW0F;
	};
	static vw0c_t VW0C;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧監視１回路制御レジスタ VW1C
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct vw1c_t : public rw8_t<0x005D> {
		typedef rw8_t<0x005D> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    VW1C0;
		bit_rw_t <io_, bitpos::B1>    VW1C1;
		bit_rw_t <io_, bitpos::B2>    VW1C2;
		bit_rw_t <io_, bitpos::B3>    VW1C3;
		bits_rw_t<io_, bitpos::B4, 2> VW1F;
		bit_rw_t <io_, bitpos::B7>    VW1C7;
	};
	static vw1c_t VW1C;

}
