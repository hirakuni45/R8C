//=====================================================================//
/*!	@file
	@brief	R8C USB チェッカー・メイン @n
			for ST7567 SPI @n
			LCD: AQM1284A-RN (128x48) Akizuki
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
#include "common/vect.h"
#include "system.hpp"
#include "clock.hpp"
#include "intr.hpp"
#include "port.hpp"
#include "common/intr_utils.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/adc_io.hpp"
#include "common/spi_io.hpp"
#include "chip/ST7565.hpp"
#include "common/monograph.hpp"
#include "common/font6x12.hpp"
#include "common/fixed_string.hpp"

// #include "bitmap/font32.h"

namespace {

// ポートの配置
// P4_2(1):   LCD_SCK(SCL)
// P3_7(2):   LCD_/CS
// /RES(3):  (System reset)
// P4_7(4):   LCD_/RES
// VSS:(5)   (Power GND)
// P4_6(6):
// VCC(7):   (Power +3.3V)
// MODE(8):  (System mode) Flash program for 'L'
// P3_5(9):   SW-A 
// P3_4(10):  SW-B
// P3_3(11):  LCD_A0
// P4_5(12):  LCD_SDA
// P1_7(13):
// P1_6(14):  (System RXD) Flash program for 'RXD'
// P1_5(15):  RXD0
// P1_4(16):  TXD0
// P1_3(17):  AN3
// P1_2(18):  AN2
// P1_1(19):  AN1 voltage sense (19.8V max)
// P1_0(20):  AN0 current sense (1.2 V/A)

	device::trb_io<utils::null_task, uint32_t> timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::adc_io<utils::null_task> adc;
	adc adc_;

	// LCD SCL: P4_2(1)
	typedef device::PORT<device::PORT4, device::bitpos::B2> SPI_SCL;
	// LCD SDA: P4_5(12)
	typedef device::PORT<device::PORT4, device::bitpos::B5> SPI_SDA;

	typedef device::spi_io<SPI_SCL, SPI_SDA, device::NULL_PORT> SPI;
	SPI		spi_;

	// LCD /CS:  P3_7(2)
	typedef device::PORT<device::PORT3, device::bitpos::B7> LCD_SEL;
	// LCD A0:   P3_3(11)
	typedef device::PORT<device::PORT3, device::bitpos::B3> LCD_A0;
	// LCD /RES: P4_7(4)
	typedef device::PORT<device::PORT4, device::bitpos::B7> LCD_RES;

	typedef chip::ST7565<SPI, LCD_SEL, LCD_A0, LCD_RES> LCD;
	LCD 	lcd_(spi_);

	typedef graphics::font6x12 afont;
	graphics::kfont_null kfont_;
	graphics::monograph<128, 24, afont> bitmap_(kfont_);

	typedef utils::fixed_string<32> string32;
	string32	string32_;

	class string_out {
	public:
		string_out() { string32_.clear(); }
		void operator () (char ch) {
			string32_ += ch;
		}
	};

	typedef utils::basic_format<string_out> sformat;
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
}

extern "C" {
	const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
		reinterpret_cast<void*>(brk_inst_),		nullptr,	// (0)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (1) flash_ready
		reinterpret_cast<void*>(null_task_),	nullptr,	// (2)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (3)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (4) コンパレーターB1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (5) コンパレーターB3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (6)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (7) タイマＲＣ

		reinterpret_cast<void*>(null_task_),	nullptr,	// (8)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (9)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (10)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (11)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (12)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (13) キー入力
		reinterpret_cast<void*>(null_task_),	nullptr,	// (14) A/D 変換
		reinterpret_cast<void*>(null_task_),	nullptr,	// (15)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (16)
		reinterpret_cast<void*>(uart_.isend),	nullptr,	// (17) UART0 送信
		reinterpret_cast<void*>(uart_.irecv),	nullptr,	// (18) UART0 受信
		reinterpret_cast<void*>(null_task_),	nullptr,	// (19)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (20)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (21) /INT2
		reinterpret_cast<void*>(null_task_),	nullptr,	// (22) タイマＲＪ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (23) 周期タイマ

		reinterpret_cast<void*>(timer_b_.itask),nullptr,	// (24) タイマＲＢ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (25) /INT1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (26) /INT3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (27)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (28)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (29) /INT0
		reinterpret_cast<void*>(null_task_),	nullptr,	// (30)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (31)
	};
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
	utils::delay::micro_second(1);	// >=30uS(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	PRCR.PRC0 = 0;

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(50, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// ADC の設定
	{
		utils::PORT_MAP(utils::port_map::P10::AN0);
		utils::PORT_MAP(utils::port_map::P11::AN1);
		adc_.start(adc::cnv_type::CH0_CH1, adc::ch_grp::AN0_AN1, true);
	}

	// SPI を開始
	{
		spi_.start(0);
	}

	// LCD を開始
	{
		lcd_.start(0x1C, true, false, 3);  // contrast, reverse=yes, BIAS9=false(BIAS7), Voltage-Ratio: 3
		bitmap_.clear(0);
	}

	utils::format("Start USB Checker\n");

	uint8_t loop = 0;
	while(1) {
		timer_b_.sync();

		adc_.scan(); // A/D scan start
		adc_.sync(); // A/D scan sync

		if(loop >= 25) {
			loop = 0;
			// 400mV/A * 3
			uint32_t i = adc_.get_value(0);
			i <<= 8;
			i /= (124 * 3);
			uint32_t v = adc_.get_value(1);
			v *= 845 * 6;
//			utils::format("Vol: %1.2:8y [V], Cur: %1.2:8y [A]\n") % (v >> 10) % i;

			bitmap_.clear(0);
			sformat("%1.2:8y [V]") % (v >> 10);
			bitmap_.draw_text(0, 0, string32_.c_str());

			sformat("%1.2:8y [A]") % i;
			bitmap_.draw_text(0, 12, string32_.c_str());

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 0);

			bitmap_.clear(0);
			auto s = timer_b_.get_count() / 50;
			auto h = s / 60;
			sformat("%02d:%02d") % (h % 100) % (s % 60);
			bitmap_.draw_text(0, 0, string32_.c_str());

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 3);
		}
		++loop;
	}
}
