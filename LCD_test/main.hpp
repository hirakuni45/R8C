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
#include "common/ds1371_io.hpp"
#include "common/lcd_io.hpp"
#include "common/monograph.hpp"

typedef device::uart_io<device::UART0, 16, 16> uart0;

typedef device::trb_io<utils::null_task> timer_b;

typedef device::adc_io adc;

typedef device::flash_io flash;

typedef graphics::monograph mono_graph;

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

typedef device::ds1371_io<scl_sda> ds1371;

// LCD SCL: P4_2(1)
// LCD SDA: P4_5(12)
// LCD A0:  P3_3(11)
// LCD /CS: P3_7(2)
struct spi_base {
	void init() const {
		device::PD4.B2 = 1;
		device::PD4.B5 = 1;
	}
	void scl_out(bool b) const { device::P4.B2 = b; }
	void sda_out(bool b) const { device::P4.B5 = b; }
};

struct spi_ctrl {
	void init() const {
		device::PD3.B3 = 1;
		device::PD3.B7 = 1;
	}
	void a0_out(bool b) const { device::P3.B3 = b; }
	void cs_out(bool b) const { device::P3.B7 = b; }
};

typedef device::lcd_io<spi_base, spi_ctrl> lcd;
