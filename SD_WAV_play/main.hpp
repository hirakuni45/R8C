#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C メイン・ヘッダー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "M120AN/port.hpp"
#include "common/intr_utils.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trc_io.hpp"
#include "common/adc_io.hpp"
#include "common/flash_io.hpp"
#include "common/ds1371_io.hpp"
#include "common/lcd_io.hpp"
#include "common/monograph.hpp"
#include "port_def.hpp"

typedef device::uart_io<device::UART0, 16, 16> uart0;

typedef device::trb_io<utils::null_task> timer_b;

typedef device::trc_io timer_c;

typedef device::adc_io adc;

typedef device::flash_io flash;

typedef graphics::monograph mono_graph;

typedef device::ds1371_io<scl_sda> ds1371;

typedef device::lcd_io<spi_base, spi_ctrl> lcd;
