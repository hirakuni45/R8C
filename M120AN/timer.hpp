#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・タイマー・レジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  タイマＲＣカウンタ TRCCNT
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io16<0x00E8> trccnt_io;
	struct trccnt_t : public trccnt_io {
		using trccnt_io::operator =;
		using trccnt_io::operator ();
		using trccnt_io::operator |=;
		using trccnt_io::operator &=;

	};
	static trccnt_t TRCCNT;




}
