#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C メイン・ヘッダー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/uart_io.hpp"
#include "common/fifo.hpp"

typedef utils::fifo<16> buffer;
typedef device::uart_io<device::UART0, buffer, buffer> uart0;
