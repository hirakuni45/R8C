//=====================================================================//
/*!	@file
	@brief	R8C LCD メイン @n
			for ST7567 SPI (128 x 32) @n
			LCD: Aitendo M-G0812P7567
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
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
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/ST7565.hpp"
#include "common/monograph.hpp"

namespace {

	device::trb_io<utils::null_task> timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	utils::command<64> command_;

	// LCD SCL: P4_2(1)
	typedef device::PORT<device::PORT4, device::bitpos::B2> SPI_SCL;
	// LCD SDA: P4_5(12)
	typedef device::PORT<device::PORT4, device::bitpos::B5> SPI_SDA;

	typedef device::spi_io<SPI_SCL, SPI_SDA, device::NULL_PORT> SPI;
	SPI		spi_;

	// LCD /CS: P3_7(2)
	typedef device::PORT<device::PORT3, device::bitpos::B7> LCD_SEL;
	// LCD A0:  P1_6(14)
	typedef device::PORT<device::PORT1, device::bitpos::B6> LCD_A0;

	typedef chip::ST7565<SPI, LCD_SEL, LCD_A0> LCD;
	LCD 	lcd_(spi_);

	graphics::kfont_null kfont_;
	graphics::monograph<128, 32> bitmap_(kfont_);
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

static uint8_t v_ = 91;
static uint8_t m_ = 123;

#if 0
static void randmize_(uint8_t v, uint8_t m)
{
	v_ = v;
	m_ = m;
}
#endif

static uint8_t rand_()
{
	v_ += v_ << 2;
	++v_;
	uint8_t n = 0;
	if(m_ & 0x02) n = 1;
	if(m_ & 0x40) n ^= 1;
	m_ += m_;
	if(n == 0) ++m_;
	return v_ ^ m_;
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

	// SPI 開始
	{
		spi_.start(10);
	}

	// LCD を開始
	{
		lcd_.start(0x00);
		spi_.start(0);  // Boost SPI clock
		bitmap_.clear(0);
	}

	sci_puts("Start R8C LCD monitor\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
//	PD1.B0 = 1;

	uint8_t cnt = 0;
	uint16_t x = rand_() & 127;
	uint16_t y = rand_() & 31;
	uint16_t xx;
	uint16_t yy;
	uint8_t loop = 20;
	while(1) {
		timer_b_.sync();
		lcd_.copy(bitmap_.fb(), bitmap_.page_num());

		if(loop >= 20) {
			loop = 0;
			bitmap_.clear(0);
			bitmap_.frame(0, 0, 128, 32, 1);
		}
		xx = rand_() & 127;
		yy = rand_() & 31;
		bitmap_.line(x, y, xx, yy, 1);
		x = xx;
		y = yy;
		++loop;

//		bitmap_.line(0, 0, 127, 31, 1);
//		bitmap_.line(0, 31, 127, 0, 1);

		if(cnt >= 20) {
			cnt = 0;
		}

		++cnt;

		// コマンド入力と、コマンド解析
		if(command_.service()) {
		}
	}
}
