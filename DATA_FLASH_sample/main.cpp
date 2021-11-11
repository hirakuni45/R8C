//=====================================================================//
/*!	@file
	@brief	R8C DATA-FLASH メイン			
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/flash_io.hpp"
#include "common/command.hpp"

namespace {

	typedef device::PORT<device::PORT1, device::bitpos::B0, false> LED;

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B timer_b_;

	typedef utils::fifo<uint8_t, 16> TX_BUFF;  // 送信バッファ
	typedef utils::fifo<uint8_t, 16> RX_BUFF;  // 受信バッファ
	typedef device::uart_io<device::UART0, TX_BUFF, RX_BUFF> UART;
	UART	uart_;

	typedef device::flash_io FLASH;
	FLASH	flash_;

	typedef utils::command<64> COMMAND;
	COMMAND	command_;

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


	void put_hexadecimal_word_(uint16_t val) {
		put_hexadecimal_byte_(val >> 8);
		put_hexadecimal_byte_(val & 255);
	}


	void dump_(uint16_t org, uint16_t end)
	{
		bool adr = true;
		while(org <= end) {
			if(adr) {
				put_hexadecimal_word_(org);
				sci_putch('-');
				adr = false;
			}
			auto val = flash_.read(org);
			sci_putch(' ');
			put_hexadecimal_byte_(val);
			++org;
			if((org & 15) == 0) {
				sci_putch('\n');
				adr = true;
			}
		}
		if((org & 15) != 0) {
			sci_putch('\n');
		}
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
		timer_b_.start(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// LED シグナル用ポートを出力
	LED::DIR = 1;

	sci_puts("Start R8C DATA-FLASH monitor\n");

	command_.set_prompt("# ");

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(command_.service()) {
			if(command_.cmp_word(0, "erase")) {
				bool f = false;
				if(command_.cmp_word(1, "bank0")) {
					f = flash_.erase(FLASH::DATA_AREA::BANK0);
				} else if(command_.cmp_word(1, "bank1")) {
					f = flash_.erase(FLASH::DATA_AREA::BANK1);
				} else {
					sci_puts("Erase bank error...\n");
					f = true;
				}
				if(f) {
					sci_puts("Erase OK!\n");
				} else {
					sci_puts("Erase error...\n");
				}
			} else if(command_.cmp_word(0, "r")) {
				char tmp[8];
				if(command_.get_word(1, sizeof(tmp), tmp)) {
					uint16_t org = get_hexadecimal_(tmp);
					uint8_t val = flash_.read(org);
					put_hexadecimal_word_(org);
					sci_puts("- ");
					put_hexadecimal_byte_(val);
					sci_putch('\n');
				}
			} else if(command_.cmp_word(0, "dump")) {
				char tmp[8];
				if(command_.get_word(1, sizeof(tmp), tmp)) {
					uint16_t org = get_hexadecimal_(tmp);
					uint16_t end = org + 15;
					if(command_.get_words() >= 3) {
						if(command_.get_word(2, sizeof(tmp), tmp)) {
							end = get_hexadecimal_(tmp);
						}
					}
					dump_(org, end);
				}
			} else if(command_.cmp_word(0, "write")) {
				char tmp[8];
				if(command_.get_word(1, sizeof(tmp), tmp)) {
					uint16_t org = get_hexadecimal_(tmp);
					for(uint16_t i = 2; i < command_.get_words(); ++i) {
						if(command_.get_word(i, sizeof(tmp), tmp)) {
							uint16_t val = get_hexadecimal_(tmp);
							if(!flash_.write(org, val)) {
								sci_puts("Write error...\n");
							}
							++org;
						}
					}
				}
			} else if(command_.cmp_word(0, "?") || command_.cmp_word(0, "help")) {
				sci_puts("dump xxxx [end]\n");
				sci_puts("erase bank[01]\n");
				sci_puts("r xxxx\n");
				sci_puts("write xxxx yy ...\n");
				sci_puts("help\n");
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

		if(cnt < 10) LED::P = 1;
		else LED::P = 0;
	}
}
