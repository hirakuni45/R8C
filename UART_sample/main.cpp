//=====================================================================//
/*!	@file
	@brief	R8C UART サンプル @n
			・８ビット１ストップ・ビット
			P1_0: LED1 @n
			P1_1: LED2 @n
			P1_4: TXD(output) @n
			P1_5: RXD(input)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "intr.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/uart_io.hpp"
#include "common/fifo.hpp"

#include "common/command.hpp"

#include "common/format.hpp"
#include "common/input.hpp"

namespace {
	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

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


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}

};


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

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);

		// ※「0」を設定するとポーリングとなる。
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
	}

	// LED ポート設定
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		PD1.B0 = 1;
		PD1.B1 = 1;
	}

	uart_.puts("Start R8C UART sample\n");

	command_.set_prompt("# ");

	uint8_t cnt = 0;
	while(1) {

		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn >= 1) {
				char tmp[32];
				if(command_.get_word(0, sizeof(tmp), tmp)) {
					int a = 0;
					int n = (utils::input("%d", tmp) % a).num();
					if(n == 1) {
						utils::format("Value: %d\n") % a;
					} else {
						utils::format("Input decimal ?\n");
					}
				}
			}
		}

#if 0
		if(uart_.length()) {  // UART のレシーブデータがあるか？
			char ch = uart_.getch();
			uart_.putch(ch);
		}
#endif

#if 0
		// 文字の出力
		for(char ch = 0x20; ch < 0x7f; ++ch) {
			uart_.putch(ch);
		} 
		uart_.putch('\n');
#endif

		// 10ms ソフトタイマー
		for(uint16_t i = 0; i < 10; ++i) {
			utils::delay::micro_second(1000);
		}

		// LED の点滅
		++cnt;
		if(cnt < 50) {
			P1.B0 = 0;
			P1.B1 = 1;
		} else {
			P1.B0 = 1;
			P1.B1 = 0;
		}
		if(cnt >= 100) cnt = 0;
	}
}
