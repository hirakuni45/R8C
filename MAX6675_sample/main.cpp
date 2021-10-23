//=====================================================================//
/*!	@file
	@brief	R8C MAX6675・メイン
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/adc_io.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/MAX6675.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::adc_io<utils::null_task> adc;
	adc adc_;

	// P1_0(20):
	typedef device::PORT<device::PORT1, device::bitpos::B0> SPI_SCK;
	// P1_1(19):
	typedef device::PORT<device::PORT1, device::bitpos::B1> MAX_CS;
	// P1_2(18):
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_SDI;

	// MISO, MOSI, SCK
	typedef device::spi_io<SPI_SDI, device::NULL_PORT, SPI_SCK, device::soft_spi_mode::CK10> SPI;
	SPI		spi_;

	typedef chip::MAX6675<SPI, MAX_CS> MAX6675;
	MAX6675	max6675_(spi_);
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
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
	}

	uart_.puts("Start R8C MAX6675 sample\n");

	// SPI 開始
	spi_.start(10);

	// MAX6675 開始
	max6675_.start();

	using namespace utils;

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		++cnt;
		if(cnt >= 30) {
			cnt = 0;

			auto v = max6675_.get_temp();
			utils::format("%6.3f\n") % v;
		}

		if(uart_.length()) {  // UART のレシーブデータがあるか？
			auto ch = uart_.getch();
			uart_.putch(ch);
		}
	}
}
