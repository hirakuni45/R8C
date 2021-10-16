#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ定義
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#ifdef VSC_INTELL_SENSE
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;
#endif

#include <cstdint>

#include "common/vect.h"
#include "M120AN/system.hpp"
#include "M120AN/clock.hpp"
#include "M120AN/port.hpp"
#include "M120AN/intr.hpp"
#include "M120AN/adc.hpp"
#include "M120AN/flash.hpp"
#include "M120AN/timer_rb.hpp"
#include "M120AN/timer_rc.hpp"
#include "M120AN/timer_rj.hpp"
#include "M120AN/uart.hpp"
#include "M120AN/vdetect.hpp"
#include "M120AN/watchdog.hpp"

#include "common/delay.hpp"
#include "common/port_map.hpp"
