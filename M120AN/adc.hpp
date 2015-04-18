#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・A/D変換レジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ ADx
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x0098> AD0;
	static io8<0x0098> AD0L;
	static io8<0x0099> AD0H;
	static io16<0x009A> AD1;
	static io8<0x009A> AD1L;
	static io8<0x009B> AD1H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D モードレジスタ ADMOD
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x009C> admod_io;
	struct admod_t : public admod_io {
		using admod_io::operator =;
		using admod_io::operator ();
		using admod_io::operator |=;
		using admod_io::operator &=;

		bits_t<admod_io, 0, 3> CKS; 
		bits_t<admod_io, 3, 2> MD;
		bits_t<admod_io, 6, 2> ADCAP;
	};
	static admod_t ADMOD;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 入力選択レジスタ ADINSEL
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x009D> adinsel_io;
	struct adinsel_t : public adinsel_io {
		using adinsel_io::operator =;
		using adinsel_io::operator ();
		using adinsel_io::operator |=;
		using adinsel_io::operator &=;

		bit_t<adinsel_io, 0> CH0; 
		bits_t<adinsel_io, 6, 2> ADGSEL; 
	};
	static adinsel_t ADINSEL;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御レジスタ０ ADCON0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x009E> adcon0_io;
	struct adcon0_t : public adcon0_io {
		using adcon0_io::operator =;
		using adcon0_io::operator ();
		using adcon0_io::operator |=;
		using adcon0_io::operator &=;

		bit_t<adcon0_io, 0> ADST;
	};
	static adcon0_t ADCON0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 割り込み制御レジスタ ADICSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x009F> adicsr_io;
	struct adicsr_t : public adicsr_io {
		using adicsr_io::operator =;
		using adicsr_io::operator ();
		using adicsr_io::operator |=;
		using adicsr_io::operator &=;

		bit_t<adicsr_io, 6> ADIE;
		bit_t<adicsr_io, 7> ADF;
	};
	static adicsr_t ADICSR;

}
