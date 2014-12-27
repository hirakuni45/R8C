#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・シリアルインターフェース定義 @n
			Copyright 2014 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UART0 ビットレートレジスタ U0BRG
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0081> u0brg_io;
	struct u0brg_t : public u0brg_io {
		using u0brg_io::operator =;
		using u0brg_io::operator ();
		using u0brg_io::operator |=;
		using u0brg_io::operator &=;
	};
	static u0brg_t U0BRG;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UART0 送受信制御レジスタ０ U0C0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0084> u0c0_io;
	struct u0c0_t : public u0c0_io {
		using u0c0_io::operator =;
		using u0c0_io::operator ();
		using u0c0_io::operator |=;
		using u0c0_io::operator &=;

		bits_t<u0c0_io, 0, 2> CLK;
		bit_t <u0c0_io, 3>    TXEPT;
		bit_t <u0c0_io, 4>    DFE;
		bit_t <u0c0_io, 5>    NCH;
		bit_t <u0c0_io, 6>    CKPOL;
		bit_t <u0c0_io, 7>    UFORM;
	};
	static u0c0_t U0C0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UART0 送受信制御レジスタ１ U0C1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0085> u0c1_io;
	struct u0c1_t : public u0c1_io {
		using u0c1_io::operator =;
		using u0c1_io::operator ();
		using u0c1_io::operator |=;
		using u0c1_io::operator &=;

		bit_t <u0c1_io, 0> TE;
		bit_t <u0c1_io, 1> TI;
		bit_t <u0c1_io, 2> RE;
		bit_t <u0c1_io, 3> RI;
		bit_t <u0c1_io, 4> U0IRS;
		bit_t <u0c1_io, 5> U0RPM;
	};
	static u0c1_t U0C1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UART0 受信バッファレジスタ U0RB
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io16_ro<0x0086> u0rb_io;
	struct u0rb_t : public u0rb_io {
		using u0rb_io::operator ();

		bits_t<u0rb_io, 0, 8> U0RBL;
		bits_t<u0rb_io, 8, 8> U0RBH;

		bit_t <u0rb_io, 12> OER;
		bit_t <u0rb_io, 13> FER;
		bit_t <u0rb_io, 14> PER;
		bit_t <u0rb_io, 15> SUM;
	};
	static u0rb_t U0RB;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	UART0 割り込みフラグと許可レジスタ１ U0IR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0088> u0ir_io;
	struct u0ir_t : public u0ir_io {
		using u0ir_io::operator =;
		using u0ir_io::operator ();
		using u0ir_io::operator |=;
		using u0ir_io::operator &=;

		bit_t <u0ir_io, 2> RIE;
		bit_t <u0ir_io, 3> TIE;
		bit_t <u0ir_io, 6> RIF;
		bit_t <u0ir_io, 7> TIF;
	};
	static u0ir_t U0IR;

}
