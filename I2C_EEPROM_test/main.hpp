#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C メイン・ヘッダー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "port.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/adc_io.hpp"
#include "common/flash_io.hpp"
#include "common/eeprom_io.hpp"
#include "port_def.hpp"

typedef device::uart_io<device::UART0, 16, 16> uart0;

typedef device::trb_io<utils::null_task> timer_b;

typedef device::adc_io adc;

typedef device::flash_io flash;

typedef device::i2c_io<scl_sda> i2c_io;

typedef device::eeprom_io<scl_sda> eeprom;

