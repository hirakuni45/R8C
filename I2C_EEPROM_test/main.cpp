//=====================================================================//
/*!	@file
	@brief	R8C I2C メイン @n
			for EEPROM 24FC1025
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include <cstring>
#include "system.hpp"
#include "clock.hpp"
#include "common/delay.hpp"
#include "common/command.hpp"
#include "common/format.hpp"

static timer_b timer_b_;
static uart0 uart0_;
static utils::command<64> command_;
static i2c_io i2c_io_;
static eeprom eeprom_(i2c_io_);

extern "C" {
	void sci_putch(char ch) {
		uart0_.putch(ch);
	}

	char sci_getch(void) {
		return uart0_.getch();
	}

	uint16_t sci_length() {
		return uart0_.length();
	}

	void sci_puts(const char* str) {
		uart0_.puts(str);
	}
}

extern "C" {
	const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
		(void*)brk_inst_,   nullptr,	// (0)
		(void*)null_task_,  nullptr,	// (1) flash_ready
		(void*)null_task_,  nullptr,	// (2)
		(void*)null_task_,  nullptr,	// (3)

		(void*)null_task_,  nullptr,	// (4) コンパレーターB1
		(void*)null_task_,  nullptr,	// (5) コンパレーターB3
		(void*)null_task_,  nullptr,	// (6)
		(void*)null_task_,  nullptr,	// (7) タイマＲＣ

		(void*)null_task_,  nullptr,	// (8)
		(void*)null_task_,  nullptr,	// (9)
		(void*)null_task_,  nullptr,	// (10)
		(void*)null_task_,  nullptr,	// (11)

		(void*)null_task_,  nullptr,	// (12)
		(void*)null_task_,  nullptr,	// (13) キー入力
		(void*)null_task_,  nullptr,	// (14) A/D 変換
		(void*)null_task_,  nullptr,	// (15)

		(void*)null_task_,  nullptr,	// (16)
		(void*)uart0_.send_task, nullptr,   // (17) UART0 送信
		(void*)uart0_.recv_task, nullptr,   // (18) UART0 受信
		(void*)null_task_,  nullptr,	// (19)

		(void*)null_task_,  nullptr,	// (20)
		(void*)null_task_,  nullptr,	// (21) /INT2
		(void*)null_task_,  nullptr,	// (22) タイマＲＪ２
		(void*)null_task_,  nullptr,	// (23) 周期タイマ

		(void*)timer_b_.itask,  nullptr,	// (24) タイマＲＢ２
		(void*)null_task_,  nullptr,	// (25) /INT1
		(void*)null_task_,  nullptr,	// (26) /INT3
		(void*)null_task_,  nullptr,	// (27)

		(void*)null_task_,  nullptr,	// (28)
		(void*)null_task_,  nullptr,	// (29) /INT0
		(void*)null_task_,  nullptr,	// (30)
		(void*)null_task_,  nullptr,	// (31)
	};
}


static bool check_key_word_(uint8_t idx, const char* key)
{
	char buff[12];
	if(command_.get_word(idx, sizeof(buff), buff)) {
		if(strcmp(buff, key) == 0) {
			return true;
		}				
	}
	return false;
}


static bool get_dec_(const char* text, uint32_t& val) {
	val = 0;
	char ch;
	while((ch = *text++) != 0) {
		if(ch >= '0' && ch <= '9') {
			ch -= '0';
		} else {
			return false;
		}
		val *= 10;
		val += ch;
	}
	return true;
}


static bool get_hexadec_(const char* text, uint32_t& val) {
	val = 0;
	char ch;
	while((ch = *text++) != 0) {
		if(ch >= '0' && ch <= '9') {
			ch -= '0';
		} else if(ch >= 'A' && ch <= 'F') {
			ch -= 'A' - 10;
		} else if(ch >= 'a' && ch <= 'f') {
			ch -= 'a' - 10;
		} else {
			return false;
		}
		val <<= 4;
		val |= ch;
	}
	return true;
}


static bool get_value_(uint8_t no, uint32_t& val) {
	char buff[9];
	if(command_.get_word(no, sizeof(buff), buff)) {
		if(get_hexadec_(buff, val)) {
			return true;
		}
	}
	return false;
}


static bool get_decimal_(uint8_t no, uint32_t& val) {
	char buff[9];
	if(command_.get_word(no, sizeof(buff), buff)) {
		if(get_dec_(buff, val)) {
			return true;
		}
	}
	return false;
}


static void dump_(uint32_t adr, const uint8_t* src, uint8_t len) {
	utils::format("%05X:") % adr;
	for(uint8_t i = 0; i < len; ++i) {
		utils::format(" %02X") % static_cast<uint32_t>(src[i]);
	}
	sci_putch('\n');
}


static bool help_(uint8_t cmdn) {
	if(cmdn >= 1 && check_key_word_(0, "help")) {
		sci_puts("speed KBPS (KBPS: 10 to 1000 [Kbps])\n");
		sci_puts("type TYPE ID SIZE (TYPE: M256B/M64KB/M128KB, ID: 0 to 7, SIZE: 1 to 256)\n");
		sci_puts("read ADRESS [LENGTH]\n");
		sci_puts("write ADRESS DATA ...\n");
		sci_puts("fill ADDRESS LENGTH DATA ...\n");
		return true;
	}
	return false;
}


static bool speed_(uint8_t cmdn) {
	if(cmdn == 2 && check_key_word_(0, "speed")) {
		uint32_t val;
		if(get_decimal_(1, val)) {
			if(val >= 10 && val <= 1000) {
				uint8_t clock = 1000 / val;
				if(clock & 1) ++clock;
				clock >>= 1;
				if(clock == 0) clock = 1;
				i2c_io_.set_clock(clock);
			} else {
				sci_puts("Invalid SPEED renge.\n");
			}
		} else {
			sci_puts("Invalid SPEED.\n");
		}
		return true;
	}
	return false;
}


static bool type_(uint8_t cmdn) {
	if(cmdn == 4 && check_key_word_(0, "type")) {
		uint32_t id;
		if(!get_decimal_(2, id)) {
			sci_puts("Invalid ID.\n");
			return true;
		}
		uint32_t pgs;
		if(!get_decimal_(3, pgs)) {
			sci_puts("Invalid Page-Size.\n");
			return true;
		}
		if(pgs == 0 || pgs > 256) {
			sci_puts("Invalid Page-Size renge.\n");
			return true;
		}
		if(check_key_word_(1, "M256B")) {
			if(id >= 0 && id <= 7) {
				eeprom_.start(static_cast<eeprom::M256B>(id), pgs);
			} else {
				sci_puts("Invalid ID renge.\n");
			}
			return true;
		} else if(check_key_word_(1, "M64KB")) {
			if(id >= 0 && id <= 7) {
				eeprom_.start(static_cast<eeprom::M64KB>(id), pgs);
			} else {
				sci_puts("Invalid ID renge.\n");
			}
			return true;
		} else if(check_key_word_(1, "M128KB")) {
			if(id >= 0 && id <= 3) {
				eeprom_.start(static_cast<eeprom::M128KB>(id), pgs);
			} else {
				sci_puts("Invalid ID renge.\n");
			}
			return true;
		} else {
			sci_puts("Invalid TYPE.\n");
			return true;
		}
	}
	return false;
}


static bool read_(uint8_t cmdn) {
	if(cmdn >= 2 && check_key_word_(0, "read")) {
		uint32_t adr;
		if(get_value_(1, adr)) {
			uint32_t end = adr + 16;
			if(cmdn >= 3) {
				if(!get_value_(2, end)) {
					sci_puts("Invalid ADDRESS.\n");
					return true;
				}
				++end;
			}
			while(adr < end) {
				uint8_t tmp[16];
				uint16_t len = 16;
				if(len > (end - adr)) {
					len = end - adr;
				}
				if(eeprom_.read(adr, tmp, len)) {
					dump_(adr, tmp, len);
				} else {
					sci_puts("Stall EEPROM read...\n");
				}
				adr += len;
			}
		} else {
			sci_puts("Invalid ADDRESS.\n");
		}
		return true;
	}
	return false;
}


static bool write_(uint8_t cmdn) {
	if(cmdn >= 3 && check_key_word_(0, "write")) {
		if(cmdn > 10) {
			sci_puts("Too many data.\n");
			return true;
		}

		uint32_t adr;
		if(get_value_(1, adr)) {
			cmdn -= 2;
			uint8_t tmp[8];
			for(uint8_t i = 0; i < cmdn; ++i) {
				uint32_t data;
				if(get_value_(2 + i, data)) {
					tmp[i] = data;
				} else {
					sci_puts("Invalid DATA.\n");
					return true;
				}
			}
			if(!eeprom_.write(adr, tmp, cmdn)) {
				sci_puts("Stall EEPROM write...\n");
			}
		} else {
			sci_puts("Invalid ADDRESS.\n");
		}
		return true;
	}
	return false;
}


static bool fill_(uint8_t cmdn) {
	if(cmdn >= 4 && check_key_word_(0, "fill")) {
		if(cmdn > 11) {
			sci_puts("Too many data.\n");
			return true;
		}

		uint32_t adr;
		if(get_value_(1, adr)) {
			uint32_t len = 0;
			if(get_value_(2, len)) {
				cmdn -= 3;
				uint8_t tmp[8];
				for(uint8_t i = 0; i < cmdn; ++i) {
					uint32_t data;
					if(get_value_(3 + i, data)) {
						tmp[i] = data;
					} else {
						sci_puts("Invalid DATA.\n");
						return true;
					}
				}
				while(len > 0) {
					if(cmdn > len) cmdn = len;
					if(!eeprom_.write(adr, tmp, cmdn)) {
						sci_puts("Stall EEPROM write...\n");
						break;
					} else {
						sci_putch('.');
					}
					len -= cmdn;
					if(len > 0) {
						if(!eeprom_.sync_write(adr)) {
							sci_puts("Stall EEPROM write: 'write sync time out'\n");
						}
					}
					adr += cmdn;
				}
				sci_putch('\n');
				return true;
			} else {
				sci_puts("Invalid LENGTH.\n");
			}
		} else {
			sci_puts("Invalid ADDRESS.\n");
		}
	}
	return false;
}


 __attribute__ ((section (".exttext")))
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
		uint8_t intr_level = 2;
		timer_b_.start_timer(60, intr_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart0_.start(19200, ir_level);
	}

	// I2C クラスの初期化
	{
		i2c_io_.init();
	}

	// EEPROMを開始
	{
		// 24FC1025: 128KB、ID0, ページサイズ128バイト
		eeprom_.start(eeprom::M128KB::ID0, 128);

		// 24FC512:  64KB、 ID0, ページサイズ128バイト
//		eeprom_.start(eeprom::M64KB::ID0, 128);
	}

	// LED シグナル用ポートを出力
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		PD1.B0 = 1;
	}

	sci_puts("Start R8C EEPROM monitor\n");
	command_.set_prompt("# ");

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn == 0) ;
			else if(help_(cmdn)) ;
			else if(speed_(cmdn)) ;
			else if(type_(cmdn)) ;
			else if(read_(cmdn)) ;
			else if(write_(cmdn)) ;
			else if(fill_(cmdn)) ;
			else {
				sci_puts("Command error: ");
				sci_puts(command_.get_command());
				sci_putch('\n');
			}
		}
	}
}
