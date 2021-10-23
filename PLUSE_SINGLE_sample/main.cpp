//=====================================================================//
/*!	@file
	@brief	R8C タイマーJ、パルス出力サンプル @n
			P17(13) からパルス出力。@n
			コンソールから、任意の周波数を設定（有限な設定範囲）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trj_io.hpp"
#include "common/command.hpp"

namespace {

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B	timer_b_;

	// P1_B0 (20)
	typedef device::PORT<device::PORT1, device::bitpos::B0> OUT;

	// P1_B1 (19)
	typedef device::PORT<device::PORT1, device::bitpos::B1> SIG;
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

#if 0
	void TIMER_RJ_intr(void) {
		timer_j_.iout();
	}
#endif
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

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(100, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	sci_puts("Start R8C PLUSE single output sample\n");

	OUT::DIR = 1;
	OUT::P = 0;

	SIG::DIR = 1;
	OUT::P = 0;

	uint8_t cnt = 0;

	while(1) {
		timer_b_.sync();
		++cnt;
		if(cnt >= 5) {
			cnt = 0;
		}
		if(cnt < 2) {
			OUT::P = 1;
			utils::delay::micro_second(30);
			SIG::P = 1;
			utils::delay::micro_second(30);
			SIG::P = 0;
			utils::delay::micro_second(30);
			SIG::P = 1;

		} else {
			OUT::P = 0;
			utils::delay::micro_second(60);
			SIG::P = 0;
			utils::delay::micro_second(60);
			SIG::P = 1;
			utils::delay::micro_second(60);
			SIG::P = 0;
		}
	}
}
