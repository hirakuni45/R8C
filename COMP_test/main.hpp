#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C メイン・ヘッダー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/intr_utils.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/adc_io.hpp"
#include "common/comp_io.hpp"

typedef device::uart_io<device::UART0, 16, 16> uart0;

typedef device::trb_io<utils::null_task> timer_b;

typedef device::adc_io adc;

typedef device::comp_io<utils::null_task, utils::null_task> comp;
