//=====================================================================//
/*!	@file
	@brief	R8C LED Matrix Tetris 16x16 @n
			MAX7219 (8x8 LED Matrix) X 4 @n
			MAX7219 LED Matrix モジュールの配置に係る仕様に注意！ @n
			デージーチェーン構造: ->[0]->[1]->[2]->[3] @n
			[1]:B7-B0  [3]:B0-B7 @n
			+8       , +31       @n
			+9       , +20       @n
			+10      , +29       @n
			+11      , +28       @n
			+12      , +27       @n
			+13      , +26       @n
			+14      , +25       @n
			+15      , +24       @n
			[0]:B7-B0, [2]:B0-B7 @n
			+0       , +23       @n
			+1       , +22       @n
			+2       , +21       @n
			+3       , +20       @n
			+4       , +19       @n
			+5       , +18       @n
			+6       , +17       @n
			+7       , +16       @n
			P1_0: DIN @n
			P1_1: /CS @n
			P1_2: CLK
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/intr_utils.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/MAX7219.hpp"
#include "common/monograph.hpp"
#include "common/font6x12.hpp"

#include "tetris.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B	timer_b_;

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::PORT<device::PORT1, device::bitpos::B0> SPI_OUT;
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_CLK;

	typedef device::spi_io<device::NULL_PORT, SPI_OUT, SPI_CLK, device::soft_spi_mode::CK10> SPI;
	SPI		spi_;

	typedef device::PORT<device::PORT1, device::bitpos::B1> SELECT;
	// デージーチェイン：４個
	typedef chip::MAX7219<SPI, SELECT, 4> MAX7219;
	MAX7219	max7219_(spi_);

	// MAX7219 モジュールを組み合わせて 16x16 マトリックスを組む場合にメモリ位置などを補正する。
	class PLOT {
	public:
		typedef int8_t value_type;

		static const int8_t WIDTH  = 16;
		static const int8_t HEIGHT = 16;

		void clear(uint8_t v = 0)
		{
			for(uint8_t i = 0; i < 32; ++i) {
				max7219_[i] = v;
			}
		} 

		void operator() (int8_t x, int8_t y, bool val)
		{
			if(x < 0 || x >= WIDTH) return;
			if(y < 0 || y >= HEIGHT) return;

			uint8_t md = (x >> 3) | ((y & 0b1000) >> 2);
			uint8_t xm = x & 7;
			uint8_t ym = y & 7;
			switch(md) {
			case 0b10:  // 2 Module-0
				if(val) { max7219_[8 * 0 + ym] |=  (1 << (xm ^ 7)); }
				else    { max7219_[8 * 0 + ym] &= ~(1 << (xm ^ 7)); }
				break;
			case 0b00:  // 0 Module-1
				if(val) { max7219_[8 * 1 + ym] |=  (1 << (xm ^ 7)); }
				else    { max7219_[8 * 1 + ym] &= ~(1 << (xm ^ 7)); }
				break;
			case 0b11:  // 3 Module-2
				if(val) { max7219_[8 * 2 + (ym ^ 7)] |=  (1 << xm); }
				else    { max7219_[8 * 2 + (ym ^ 7)] &= ~(1 << xm); }
				break;
			case 0b01:  // 1 Module-3
				if(val) { max7219_[8 * 3 + (ym ^ 7)] |=  (1 << xm); }
				else    { max7219_[8 * 3 + (ym ^ 7)] &= ~(1 << xm); }
				break;
			default:
				break;
			}
		}
	};

	typedef graphics::font6x12 AFONT;
	typedef graphics::kfont_null KFONT;
	KFONT	kfont_;
	typedef graphics::monograph<PLOT, AFONT, KFONT> MONOG;
	MONOG	monog_(kfont_);

	typedef app::tetris<MONOG> TETRIS;
	TETRIS	tetris_(monog_);
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
		timer_b_.start(60, ir_level);
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
		uint32_t clk = 10'000'000;  // SPI クロック周波数 (10MHz)
		spi_.start(clk);
	}

	// MAX7219 を開始（４個デージーチェイン）
	{
		max7219_.start();
		max7219_.set_intensity(0);
	}

#if 0
	monog_.plot(0, 0, 1);
	monog_.plot(15, 0, 1);
	monog_.plot(0, 15, 1);
	monog_.plot(15, 15, 1);

	monog_.line(0, 0, 15, 15, 1);
#endif

	tetris_.init();

	while(1) {
		timer_b_.sync();

		monog_.clear(0);
		tetris_.service();

		max7219_.service();
	}
}
