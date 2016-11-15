#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・A/D変換レジスター定義 @n
			Copyright 2015, 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro16_t<0x0098> AD0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD0L
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro8_t<0x0098> AD0L;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD0H
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro8_t<0x0099> AD0H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro16_t<0x009A> AD1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1L
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro8_t<0x009A> AD1L;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1H
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static ro8_t<0x009B> AD1H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D モードレジスタ ADMOD
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct admod_t : public rw8_t<0x009C> {
		typedef rw8_t<0x009C> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 3> CKS; 
		bits_rw_t<io_, bitpos::B3, 2> MD;
		bits_rw_t<io_, bitpos::B6, 2> ADCAP;
	};
	static admod_t ADMOD;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 入力選択レジスタ ADINSEL
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct adinsel_t : public rw8_t<0x009D> {
		typedef rw8_t<0x009D> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t <io_, bitpos::B0>    CH0; 
		bits_rw_t<io_, bitpos::B6, 2> ADGSEL; 
	};
	static adinsel_t ADINSEL;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御レジスタ０ ADCON0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct adcon0_t : public rw8_t<0x009E> {
		typedef rw8_t<0x009E> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> ADST;
	};
	static adcon0_t ADCON0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 割り込み制御レジスタ ADICSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct adicsr_t : public rw8_t<0x009F> {
		typedef rw8_t<0x009F> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B6> ADIE;
		bit_rw_t<io_, bitpos::B7> ADF;
	};
	static adicsr_t ADICSR;
}
