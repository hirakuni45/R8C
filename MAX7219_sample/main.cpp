//=====================================================================//
/*!	@file
	@brief	R8C MAX7219 メイン @n
			P1_0: DIN @n
			P1_1: /CS @n
			P1_2: CLK
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>
#include <cstdlib>
#include "common/vect.h"
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "intr.hpp"
#include "common/intr_utils.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/MAX7219.hpp"

// Dot Matrix LED
#define DOT_MATRIX

namespace {

	device::trb_io<utils::null_task, uint8_t> timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart	uart_;

	typedef device::PORT<device::PORT1, device::bitpos::B0> SPI_SDA;
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_SCL;

	typedef device::spi_io<device::NULL_PORT, SPI_SDA, SPI_SCL, device::soft_spi_mode::CK10> SPI;
	SPI		spi_;

	typedef device::PORT<device::PORT1, device::bitpos::B1> SELECT;
	chip::MAX7219<SPI, SELECT> max7219_(spi_);
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

	// SPI を開始
	{
		spi_.start(10);
	}

	// MAX7219 を開始
	{
		max7219_.start();
	}

	sci_puts("Start R8C MAX7219 sample\n");

	for(uint8_t i = 0; i < 8; ++i) {
#ifdef DOT_MATRIX
		max7219_.set(i, rand() & 0xff);
#else
		max7219_.set_cha(i, '-');
#endif
	}

	uint8_t idx = 0;
	while(1) {
		timer_b_.sync();
		max7219_.service();
		max7219_.set_intensity(0);

		if(sci_length()) {
			if(idx > 7) {
				max7219_.shift_top();
				idx = 7;
			}
#ifdef DOT_MATRIX
			sci_getch();
			auto ch = rand() & 0xff;
			max7219_.set(idx ^ 7, ch);
#else
			auto ch = sci_getch();
			sci_putch(ch);
			max7219_.set_cha(idx ^ 7, ch);
#endif
			++idx;
		}
	}
}
