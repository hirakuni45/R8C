#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C メイン・ヘッダー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "port.hpp"
#include "common/intr_utils.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/adc_io.hpp"
#include "common/flash_io.hpp"
#include "common/eeprom_io.hpp"


typedef device::uart_io<device::UART0, 16, 16> uart0;

typedef device::trb_io<utils::null_task> timer_b;

typedef device::adc_io adc;

typedef device::flash_io flash;

// DS1371 I2C ポートの定義クラス
// P1_B7: SCL
// P4_B5: SDA
struct scl_sda {
	void init() const {  // オープン・ドレイン設定
		device::POD1.B7 = 1;
		device::POD4.B5 = 1;
	}
	void scl_dir(bool b) const { device::PD1.B7 = b; }  // SCL 方向 (0:in, 1:out)
	void scl_out(bool b) const { device::P1.B7 = b; }   // SCL 出力
	bool scl_inp() const { return device::P1.B7(); }    // SCL 入力
	void sda_dir(bool b) const { device::PD4.B5 = b; }  // SDA 方向 (0:in, 1:out)
	void sda_out(bool b) const { device::P4.B5 = b; }   // SDA 出力
	bool sda_inp() const { return device::P4.B5(); }    // SDA 入力
};

typedef device::i2c_io<scl_sda> i2c_io;
typedef device::eeprom_io<scl_sda> eeprom;


