//=====================================================================//
/*!	@file
	@brief	R8C PLUSE OUT/LCD メイン @n
			for ST7567 SPI (128 x 32) @n
			LCD: Aitendo M-G0812P7567 @n
			ENCODER: A: P10, B: P11 Com: Vss
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
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
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/trj_io.hpp"
#include "common/spi_io.hpp"
#include "chip/ST7565.hpp"
#include "common/monograph.hpp"

#include "bitmap/font32.h"

namespace {

// ポートの配置
// P4_2(1):   LCD_SCK  ,SD_CLK(5)
// P3_7(2):   LCD_/CS
// /RES(3):  (System reset)
// P4_7(4):            ,SD_DO/DAT0(7)
// VSS:(5)   (Power GND)
// P4_6(6):   XIN (高精度なクロック用)
// VCC(7):   (Power +V)
// MODE(8):  (System mode)
// P3_5(9):   I2C_SDA
// P3_4(10):           ,SD_/CS(1)
// P1_0(20):  AN0 (keep)
// P1_1(19):  AN1 (keep)
// P1_2(18):  AN2 (keep)
// P1_3(17):  AN3 (keep)
// P1_4(16):  TXD0 (keep)
// P1_5(15):  RXD0 (keep)
// P1_6(14):  LCD_A0 (share)
// P1_7(13):  TRJIO (keep)
// P4_5(12):  LCD_SDA  ,SD_DI/CMD(2)
// P3_3(11):  I2C_SCL

	const uint8_t* nmbs_[] = {
		nmb_0, nmb_1, nmb_2, nmb_3, nmb_4,
		nmb_5, nmb_6, nmb_7, nmb_8, nmb_9,
		txt_hz, txt_k
	};

	uint8_t enc_lvl_ = 0;
	volatile int8_t enc_cnt_ = 0;

	class encoder {
	public:
		void operator() () {
			uint8_t lvl = device::P1();  ///< 状態の取得
			uint8_t pos = ~enc_lvl_ &  lvl;  ///< 立ち上がりエッジ検出
			uint8_t neg =  enc_lvl_ & ~lvl;  ///< 立ち下がりエッジ検出
			enc_lvl_ = lvl;  ///< 状態のセーブ

			if(pos & device::P1.B0.b()) {
				if(lvl & device::P1.B1.b()) {
					--enc_cnt_;
				} else {
					++enc_cnt_;
				}
			}
			if(neg & device::P1.B0.b()) {
				if(lvl & device::P1.B1.b()) {
					++enc_cnt_;
				} else {
					--enc_cnt_;
				}
			}
			if(pos & device::P1.B1.b()) {
				if(lvl & device::P1.B0.b()) {
					++enc_cnt_;
				} else {
					--enc_cnt_;
				}
			}
			if(neg & device::P1.B1.b()) {
				if(lvl & device::P1.B0.b()) {
					--enc_cnt_;
				} else {
					++enc_cnt_;
				}
			}
		}
	};

	device::trb_io<encoder, uint8_t> timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	utils::command<64> command_;

	// LCD SCL: P4_2(1)
	typedef device::PORT<device::PORT4, device::bitpos::B2> SPI_SCL;
	// LCD SDA: P4_5(12)
	typedef device::PORT<device::PORT4, device::bitpos::B5> SPI_SDA;

	// MISO, MOSI, SCK
	typedef device::spi_io<device::NULL_PORT, SPI_SDA, SPI_SCL, device::soft_spi_mode::CK10> SPI;
	SPI		spi_;

	// LCD /CS: P3_7(2)
	typedef device::PORT<device::PORT3, device::bitpos::B7> LCD_SEL;
	// LCD A0:  P1_6(14)
	typedef device::PORT<device::PORT1, device::bitpos::B6> LCD_A0;
	// LCD RES:
	typedef device::NULL_PORT LCD_RES;

	typedef chip::ST7565<SPI, LCD_SEL, LCD_A0, LCD_RES> LCD;
	LCD 	lcd_(spi_);

	graphics::kfont_null kfont_;
	graphics::monograph<128, 32> bitmap_(kfont_);

	typedef device::trj_io<utils::null_task> timer_j;
	timer_j timer_j_;
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


	void TIMER_RJ_intr(void) {
		timer_j_.iout();
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
	utils::delay::micro_second(1);	// >=30uS(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	PRCR.PRC0 = 0;

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(240, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// エンコーダー入力の設定 P10: (Phi_A), P11: (Phi_B), Vss: (COM)
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		device::PD1.B0 = 0;
		device::PD1.B1 = 0;
		device::PUR1.B0 = 1;	///< プルアップ
		device::PUR1.B1 = 1;	///< プルアップ
	}

	// SPI を開始
	{
		spi_.start(10000000);
	}

	// LCD を開始
	{
		lcd_.start(0x00);
		bitmap_.clear(0);
	}

	uint32_t count = 20;

	// TRJ のパルス出力設定
	{
		utils::PORT_MAP(utils::port_map::P17::TRJIO);
		uint8_t ir_level = 1;
		if(!timer_j_.pluse_out(count, ir_level)) {
			sci_puts("TRJ out of range.\n");
		}
	}

	sci_puts("Start R8C PLUSE OUT/LCD\n");
	command_.set_prompt("# ");

	uint8_t cnt = 0;
	uint32_t value = 0;
	while(1) {
		timer_b_.sync();

		// エンコーダー値の増減
		int32_t d = 0;
		if(enc_cnt_ >= 4) {
			enc_cnt_ = 0;
			d = 1;
		} else if(enc_cnt_ <= -4) { 
			enc_cnt_ = 0;
			d = -1;
		}
		if(d) {
			if(count < 100) {
				d *= 1;
			} else if(count < 1000) { // 1KHz
				d *= 10; // 10Hz step
			} else if(count < 10000) { // 10KHz
				d *= 100; // 100Hz step
			} else if(count < 100000) { // 100KHz
				d *= 1000; // 1KHz step
			} else if(count < 1000000) { // 1MHz
				d *= 10000; // 10KHz step
			} else {
				d *= 100000; // 100KHz step
			}
			count += static_cast<uint32_t>(d);
			if(count < 20) count = 20;
			else if(count > 10000000) count = 10000000;
		}

		if(value != count) {
			value = count;

			timer_j_.set_cycle(count);

			if(count > 99999) {
				utils::format("%dKHz\n") % (count / 1000);
			} else {
				utils::format("%dHz\n") % count;
			}
		}

		// 1/15 sec
		if((cnt & 15) == 0) {
			bitmap_.clear(0);
			uint32_t n = count;
			bool khz = false;
			if(n > 99999) {
				n /= 1000;
				khz = true;
			}
			bitmap_.draw_mobj(20 * 4, 0, nmbs_[n % 10]);
			n /= 10;
			bitmap_.draw_mobj(20 * 3, 0, nmbs_[n % 10]);
			n /= 10;
			bitmap_.draw_mobj(20 * 2, 0, nmbs_[n % 10]);
			n /= 10;
			bitmap_.draw_mobj(20 * 1, 0, nmbs_[n % 10]);
			n /= 10;
			bitmap_.draw_mobj(20 * 0, 0, nmbs_[n % 10]);
			if(khz) {
				bitmap_.draw_mobj(20 * 5, 0, nmbs_[11]);
				bitmap_.draw_mobj(20 * 5 + 11, 0, nmbs_[10]);
			} else {
				bitmap_.draw_mobj(20 * 5, 0, nmbs_[10]);
			}
			lcd_.copy(bitmap_.fb(), 4);
		}

		++cnt;
	}
}
