#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・電圧検出・レジスター定義 @n
			Copyright 2014 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧監視回路エッジ選択レジスタ VCAC
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0058> vcac_io;
	struct vcac_t : public vcac_io {
		using vcac_io::operator =;
		using vcac_io::operator ();
		using vcac_io::operator |=;
		using vcac_io::operator &=;

		bit_t<vcac_io, 1> VCAC1;
	};
	static vcac_t VCAC;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧検出レジスタ２ VCA2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x005a> vca2_io;
	struct vca2_t : public vca2_io {
		using vca2_io::operator =;
		using vca2_io::operator ();
		using vca2_io::operator |=;
		using vca2_io::operator &=;

		bit_t<vca2_io, 0> LPE;
		bit_t<vca2_io, 5> VC0E;
		bit_t<vca2_io, 6> VC1E;
	};
	static vca2_t VCA2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧検出１レベル選択レジスタ VD1LS
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x005b> vd1ls_io;
	struct vd1ls_t : public vd1ls_io {
		using vd1ls_io::operator =;
		using vd1ls_io::operator ();
		using vd1ls_io::operator |=;
		using vd1ls_io::operator &=;

		bits_t<vd1ls_io, 1, 3> VD1S;
	};
	static vd1ls_t VD1LS;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧監視０回路制御レジスタ VW0C
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x005c, 0xc0> vw0c_io;
	struct vw0c_t : public vw0c_io {
		using vw0c_io::operator =;
		using vw0c_io::operator ();
		using vw0c_io::operator |=;
		using vw0c_io::operator &=;

		bit_t<vw0c_io, 0> VW0C0;
		bit_t<vw0c_io, 1> VW0C1;
		bits_t<vw0c_io, 4, 2> VW0F;
	};
	static vw0c_t VW0C;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  電圧監視１回路制御レジスタ VW1C
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x005d> vw1c_io;
	struct vw1c_t : public vw1c_io {
		using vw1c_io::operator =;
		using vw1c_io::operator ();
		using vw1c_io::operator |=;
		using vw1c_io::operator &=;

		bit_t<vw1c_io, 0> VW1C0;
		bit_t<vw1c_io, 1> VW1C1;
		bit_t<vw1c_io, 2> VW1C2;
		bit_t<vw1c_io, 3> VW1C3;
		bits_t<vw1c_io, 4, 2> VW1F;
		bit_t<vw1c_io, 7> VW1C7;
	};
	static vw1c_t VW1C;

}
