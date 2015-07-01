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
		@brief  A/D レジスタ AD0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x0098> AD0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD0L
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x0098> AD0L;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD0H
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x0099> AD0H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io16<0x009A> AD1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1L
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x009A> AD1L;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D レジスタ AD1H
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x009B> AD1H;


	/// @brief レジスタ定義
	typedef io8<0x009C> admod_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D モードレジスタ ADMOD
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
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


	/// @brief レジスタ定義
	typedef io8<0x009D> adinsel_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 入力選択レジスタ ADINSEL
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct adinsel_t : public adinsel_io {
		using adinsel_io::operator =;
		using adinsel_io::operator ();
		using adinsel_io::operator |=;
		using adinsel_io::operator &=;

		bit_t<adinsel_io, 0> CH0; 
		bits_t<adinsel_io, 6, 2> ADGSEL; 
	};
	static adinsel_t ADINSEL;


	/// @brief レジスタ定義
	typedef io8<0x009E> adcon0_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 制御レジスタ０ ADCON0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct adcon0_t : public adcon0_io {
		using adcon0_io::operator =;
		using adcon0_io::operator ();
		using adcon0_io::operator |=;
		using adcon0_io::operator &=;

		bit_t<adcon0_io, 0> ADST;
	};
	static adcon0_t ADCON0;


	/// @brief レジスタ定義
	typedef io8<0x009F> adicsr_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  A/D 割り込み制御レジスタ ADICSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
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
