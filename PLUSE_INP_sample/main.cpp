//=====================================================================//
/*!	@file
	@brief	R8C タイマーＪ、パルス計測サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trj_io.hpp"
#include "common/format.hpp"

namespace {

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::trj_io<utils::null_task> TIMER_J;
	TIMER_J	timer_j_;

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B timer_b_;

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


	void TIMER_RJ_intr(void) {
		timer_j_.iinp();
	}

}


int main(int argc, char *argv[])
{
	using namespace device;

// クロック関係レジスタ・プロテクト解除
	PRCR.PRC0 = 1;

// 高速オンチップオシレーターへ切り替え(20MHz)
// ※ F_CLK を設定する事（Makefile内）
	OCOCR.HOCOE = 1;
	utils::delay::micro_second(1);	// >=30uS(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	const uint8_t interval = 100;
	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(interval, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// TRJ のパルス周期測定
	auto srcclk = TIMER_J::source::f1;
	uint32_t master = F_CLK;
	{
		utils::PORT_MAP(utils::port_map::P17::TRJIO);
		device::PINSR.TRJIOSEL = 0;	// TRJIO を選択
		// uint8_t ir_level = 2;
		timer_j_.pluse_inp(TIMER_J::measurement::freq, srcclk);
	}

	sci_puts("Start R8C PLUSE input sample\n");

	uint8_t n = 0;
	while(1) {
		timer_b_.sync();
		++n;
		if(n >= interval) {
			uint16_t cnt;
			bool f = timer_j_.get_count(cnt);
			if(!f) {
				utils::format("Range error.\n");
			} else if(cnt == 0) {
				utils::format("Terminate error.\n");
			} else {
				uint32_t frq = master / static_cast<uint32_t>(cnt + 1);
				utils::format("Freq: %d Hz (%d, %d)\n") % frq %
					static_cast<uint32_t>(cnt) % static_cast<uint32_t>(master);
			}
			timer_j_.restart_inp(srcclk);
			n = 0;
		}
	}
}
