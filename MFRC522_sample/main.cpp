//=====================================================================//
/*!	@file
	@brief	R8C MFRC522・メイン @n
																@n
			＊＊＊＊＊　電源は必ず３．３Ｖで使う事！ ＊＊＊＊＊ @n
																@n
			MFRC522(SDA)  ---> MFRC_CS (P0_0) @n
			MFRC522(SCK)  ---> SPI_SCK (P0_1) @n
			MFRC522(MOSI) ---> SPI_MOSI(P0_2) @n
			MFRC522(MISO) ---> SPI_MISO(P0_3) @n
			MFRC522(IRQ)       N.C @n
			MFRC522(GND)  ---> GND @n
			MFRC522(RES)  ---> MFRC_RES(P4_2) @n
			MFRC522(3.3V) ---> 3.3V
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
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
#include "chip/MFRC522.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::adc_io<utils::null_task> adc;
	adc adc_;

	// ポートの定義と接続
	// P1_0(20):
	typedef device::PORT<device::PORT1, device::bitpos::B0> MFRC_CS;
	// P1_1(19):
	typedef device::PORT<device::PORT1, device::bitpos::B1> SPI_SCK;
	// P1_2(18):
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_MOSI;
	// P1_3(17):
	typedef device::PORT<device::PORT1, device::bitpos::B3> SPI_MISO;
	// P4_2(1):
	typedef device::PORT<device::PORT4, device::bitpos::B2> MFRC_RES;


	typedef device::spi_io<SPI_SCK, SPI_MOSI, SPI_MISO> SPI;
	SPI		spi_;

	typedef chip::MFRC522<SPI, MFRC_CS, MFRC_RES> MFRC522;
	MFRC522	mfrc522_(spi_);
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

	uart_.puts("Start R8C MFRC522 sample\n");

	// SPI 開始
	{
		uint8_t speed = 10;
		spi_.start(speed);
	}

	// MFRC522 開始
	mfrc522_.start();

	// バージョンの表示
	char tmp[64];
	mfrc522_.list_version(tmp, sizeof(tmp));
	utils::format("%s") % tmp;

#if 0
	// セルフ・テスト
	if(mfrc522_.self_test()) {
		utils::format("MFRC522 Self test: OK\n");
	} else {
		utils::format("MFRC522 Self test: NG\n");
	}
#endif

	using namespace utils;

	while(1) {
		timer_b_.sync();

		// Look for new cards
		if(mfrc522_.detect_card()) {
			utils::format("Card Detect !\n");
#if 0
	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
#endif
		}

		if(uart_.length()) {  // UART のレシーブデータがあるか？
			auto ch = uart_.getch();
			uart_.putch(ch);
		}
	}
}
