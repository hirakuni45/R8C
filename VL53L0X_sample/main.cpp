//=====================================================================//
/*! @file
    @brief  VL53L0X 距離センサー・サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstring>
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/uart_io.hpp"
#include "common/fifo.hpp"
#include "common/trb_io.hpp"
#include "common/iica_io.hpp"
#include "chip/VL53L0X.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// I2C ポートの定義クラス
	// P4_B5 (12): SDA
	typedef device::PORT<device::PORT4, device::bitpos::B5> sda_port;
	// P1_B7 (13): SCL
	typedef device::PORT<device::PORT1, device::bitpos::B7> scl_port;

	typedef device::iica_io<sda_port, scl_port> iica;
	iica	i2c_;
	typedef chip::VL53L0X<iica> VLX;
	VLX		vlx_(i2c_);

	utils::command<64> command_;
}

extern "C" {

	void sci_putch(char ch) {
		uart_.putch(ch);
	}


	char sci_getch(void) {
		return uart_.getch();
	}


	uint16_t sci_length() {
		return uart_.length();
	}


	void sci_puts(const char* str) {
		uart_.puts(str);
	}


	void TIMER_RB_intr(void) {
		timer_b_.itask();
		vlx_.add_millis(10);
	}


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}

}


namespace {



}

//  __attribute__ ((section (".exttext")))
int main(int argc, char *argv[])
{
	using namespace device;

// クロック関係レジスタ・プロテクト解除
	PRCR.PRC0 = 1;

// 高速オンチップオシレーターへ切り替え(20MHz)
// ※ F_CLK を設定する事（Makefile内）
	OCOCR.HOCOE = 1;
	utils::delay::micro_second(1);  // >=30us(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(100, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// I2C クラスの初期化
	{
		i2c_.start(iica::speed::fast);
	}

	// VL53L0X を開始
	if(!vlx_.start()) {
		utils::format("VL53L0X start fail\n");
	} else {
		// 20ms
		vlx_.set_measurement_timing_budget(200000);
	}

	sci_puts("Start R8C VL53L0X monitor\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t cnt = 0;
	uint8_t itv = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		++itv;
		if(itv >= 50) {
			auto len = vlx_.read_range_single_millimeters();
			if(vlx_.timeout_occurred()) {
				utils::format("Length: timeout\n");
			} else {
				utils::format("Length: %d [mm]\n") % (len - 50);
			}
			itv = 0;
		}

#if 0
		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn >= 1) {
				if(command_.cmp_word(0, "date")) {
					if(cmdn == 1) {

					} else {

					}
				} else if(command_.cmp_word(0, "help")) {
//					sci_puts("date\n");
//					sci_puts("date yyyy/mm/dd hh:mm[:ss]\n");
				} else {
					char buff[12];
					if(command_.get_word(0, sizeof(buff), buff)) {
						utils::format("Command error: %s\n") % buff;
					}
				}
			}
		}
#endif
	}
}
