//=====================================================================//
/*!	@file
	@brief	R8C MCP2515・メイン @n
			MCP2515(SCK)  ---> SPI_SCK (P1_1:19) @n
			MCP2515(MOSI) ---> SPI_MOSI(P1_2:18) @n
			MCP2515(MISO) ---> SPI_MISO(P1_3:17) @n
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/delay.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/adc_io.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/MCP2515.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// ポートの定義と接続
	// P1_0(20):
	typedef device::PORT<device::PORT1, device::bitpos::B0> MCP_CS;
	// P1_1(19):
	typedef device::PORT<device::PORT1, device::bitpos::B1> SPI_SCK;
	// P1_2(18):
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_MOSI;
	// P1_3(17):
	typedef device::PORT<device::PORT1, device::bitpos::B3> SPI_MISO;

	typedef device::spi_io<SPI_MISO, SPI_MOSI, SPI_SCK, device::soft_spi_mode::CK01_> SPI;
	SPI		spi_;

	typedef chip::MCP2515<SPI, MCP_CS> MCP2515;
	MCP2515	mcp2515_(spi_);
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

	uart_.puts("Start R8C MCP2515 sample\n");

	// SPI 開始
	{
		uint32_t speed = 1000000;  // 1M bps
		spi_.start(speed);
	}

	// MFRC522 開始
	mcp2515_.start();

	using namespace utils;

	while(1) {
		timer_b_.sync();

#if 0
		if(uart_.length()) {  // UART のレシーブデータがあるか？
			auto ch = uart_.getch();
			uart_.putch(ch);
		}
#endif
	}
}
