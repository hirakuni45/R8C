//=====================================================================//
/*!	@file
	@brief	R8C/L3G4200D サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/iica_io.hpp"
#include "chip/L3G4200D.hpp"
#include "common/command.hpp"
#include "common/format.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// I2C ポートの定義クラス
	// P4_B5(12): SDA
	typedef device::PORT<device::PORT4, device::bitpos::B5> sda_port;
	// P1_B7(13): SCL
	typedef device::PORT<device::PORT1, device::bitpos::B7> scl_port;

	typedef device::iica_io<sda_port, scl_port> iica;
	iica i2c_;
	chip::L3G4200D<iica> l3g_(i2c_);

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
	}


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}

}


// __attribute__ ((section (".exttext")))
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

	// タイマーＢ初期化 (100Hz)
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

	sci_puts("Start R8C/L3G4200D sample\n");
	command_.set_prompt("# ");

	// L3G4200D を開始
	{
		if(!l3g_.start()) {
			utils::format("Stall L3G4200D start\n");
		}

		l3g_.calibrate();
	}

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t itv = 0;
	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

///		if(itv >= 30) {
			auto t = l3g_.get_temperature();
			auto n = l3g_.get_normalize();
			n.x *= 0.01f;  // 100Hz
			n.y *= 0.01f;
			n.z *= 0.01f;
			utils::format("%5.4f, %5.4f, %5.4f\n") % n.x % n.y % n.z;
///			itv = 0;
///		} else {
///			++itv;
///		}

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn >= 1) {
			}
		}
	}
}
