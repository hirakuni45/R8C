#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・ベクター関係 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <unistd.h>

#define INTERRUPT_FUNC __attribute__ ((interrupt))

#ifdef __cplusplus
extern "C" {
#endif
	void brk_inst_(void);
	void null_task_(void);
#ifdef __cplusplus
};
#endif
