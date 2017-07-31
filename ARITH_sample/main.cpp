//=====================================================================//
/*!	@file
	@brief	R8C Arith サンプル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/delay.hpp"
#include "common/basic_arith.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	utils::command<64> command_;

	utils::basic_arith<int32_t> arith_;
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
		timer_b_.start_timer(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	sci_puts("Start R8C arith sample\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			if(!arith_.analize(command_.get_command())) {
				auto err = arith_.get_error();
				utils::format("Error: %04X\n") % static_cast<uint16_t>(err());
			} else {
				auto v = arith_.get();
				utils::format("Ans: %d\n") % v;
			}
		}
	}
}
