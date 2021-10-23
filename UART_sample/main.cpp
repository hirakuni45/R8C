//=====================================================================//
/*!	@file
	@brief	R8C UART サンプル @n
			・８ビット１ストップ・ビット
			P1_0: LED1 @n
			P1_1: LED2 @n
			P1_4: TXD(output) @n
			P1_5: RXD(input)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/uart_io.hpp"
#include "common/fifo.hpp"

#include "common/command.hpp"

#include "common/format.hpp"
#include "common/input.hpp"

namespace {

	typedef device::PORT<device::PORT1, device::bitpos::B0, false> LED0;
	typedef device::PORT<device::PORT1, device::bitpos::B1, false> LED1;

	typedef utils::fifo<uint8_t, 16> TX_BUFF;  // 送信バッファ
	typedef utils::fifo<uint8_t, 16> RX_BUFF;  // 受信バッファ
	typedef device::uart_io<device::UART0, TX_BUFF, RX_BUFF> UART;
	UART uart_;

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
		LED0::DIR = 1;
		LED1::DIR = 1;
		LED0::P = 0;
		LED1::P = 0;
	}

	uart_.puts("Start R8C UART sample\n");

	command_.set_prompt("# ");

	uint8_t cnt = 0;
	while(1) {

		if(command_.service()) {
			auto cmdn = command_.get_words();
			if(cmdn >= 1) {
				char tmp[32];
				if(command_.get_word(0, sizeof(tmp), tmp)) {
					int32_t a = 0;
					auto n = (utils::input("%d", tmp) % a).num();
					if(n == 1) {
						utils::format("Value: %d, 0x%X\n") % a % a;
					} else {
						utils::format("Input only decimal: '%s'\n") % tmp;
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
		utils::delay::milli_second(10);

		// LED の点滅
		++cnt;
		if(cnt < 25) {
			LED0::P = 0;
			LED1::P = 1;
		} else {
			LED0::P = 1;
			LED1::P = 0;
		}
		if(cnt >= 50) cnt = 0;
	}
}
