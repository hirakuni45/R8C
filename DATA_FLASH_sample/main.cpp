//=====================================================================//
/*!	@file
	@brief	R8C DATA-FLASH メイン			
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/port_map.hpp"
#include "common/intr_utils.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/flash_io.hpp"
#include "common/command.hpp"
#include "common/delay.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::flash_io flash;
	flash flash_;

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


namespace {

	uint16_t get_hexadecimal_(const char* str)
	{
		uint16_t v = 0;
		char ch;
		while((ch = *str++) != 0) {
			v <<= 4;
			if(ch >= '0' && ch <= '9') v |= ch - '0';
			else if(ch >= 'A' && ch <= 'F') v |= ch - 'A' + 10;
			else if(ch >= 'a' && ch <= 'f') v |= ch - 'a' + 10;
			else return 0;
		}
		return v;
	}


	void put_hexadecimal_(uint8_t val) {
		val &= 0xf;
		if(val > 9) val += 'A' - 10;
		else val += '0';
		sci_putch(val);	
	}


	void put_hexadecimal_byte_(uint8_t val) {
		put_hexadecimal_(val >> 4);
		put_hexadecimal_(val);
	}
}

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

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	sci_puts("Start R8C FLASH monitor\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(command_.service()) {
			if(command_.cmp_word(0, "erase")) {
				bool f = false;
				if(command_.cmp_word(1, "bank0")) {
					f = flash_.erase(flash_io::data_area::bank0);
				} else if(command_.cmp_word(1, "bank1")) {
					f = flash_.erase(flash_io::data_area::bank1);
				} else {
					sci_puts("Erase bank error...\n");
					f = true;
				}
				if(!f) {
					sci_puts("Erase error...\n");
				}
			} else if(command_.cmp_word(0, "r")) {
				char buff[5];
				if(command_.get_word(1, sizeof(buff), buff)) {
					uint16_t ofs = get_hexadecimal_(buff);
					uint8_t v = flash_.read(ofs);
					put_hexadecimal_byte_(v);
					sci_putch('\n');
				}
			} else if(command_.cmp_word(0, "write")) {
				char buff[5];
				if(command_.get_word(1, sizeof(buff), buff)) {
					uint16_t ofs = get_hexadecimal_(buff);
					if(command_.get_word(2, sizeof(buff), buff)) {
						uint16_t val = get_hexadecimal_(buff);
						if(!flash_.write(ofs, val)) {
							sci_puts("Write error...\n");
						}
					}
				}
			} else if(command_.cmp_word(0, "?")) {
				sci_puts("erase bank[01]\n");
				sci_puts("r xxxx\n");
				sci_puts("write xxxx yy\n");
			} else {
				const char* p = command_.get_command();
				if(p[0]) {
					sci_puts("command error: ");
					sci_puts(p);
					sci_puts("\n");
				}
			}
		}

		++cnt;
		if(cnt >= 30) {
			cnt = 0;
		}

		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
	}
}
