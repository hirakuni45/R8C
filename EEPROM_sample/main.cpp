//=====================================================================//
/*!	@file
	@brief	R8C I2C-EEPROM Read/Write メイン @n
			I2C EEPROM Device: 24FC1025
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "common/delay.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "chip/EEPROM.hpp"
#include "common/command.hpp"
#include "common/format.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// I2C ポートの定義クラス
	// P4_B5: SDA
	typedef device::PORT<device::PORT4, device::bitpos::B5> sda_port;
	// P1_B7: SCL
	typedef device::PORT<device::PORT1, device::bitpos::B7> scl_port;

	typedef device::iica_io<sda_port, scl_port> iica;
	iica i2c_;

	typedef chip::EEPROM<iica> eeprom;
	eeprom eeprom_(i2c_);

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

	bool get_dec_(const char* text, uint32_t& val) {
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


	bool get_hexadec_(const char* text, uint32_t& val) {
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


	bool get_value_(uint8_t no, uint32_t& val) {
		char buff[9];
		if(command_.get_word(no, sizeof(buff), buff)) {
			if(get_hexadec_(buff, val)) {
				return true;
			}
		}
		return false;
	}


	bool get_decimal_(uint8_t no, uint32_t& val) {
		char buff[9];
		if(command_.get_word(no, sizeof(buff), buff)) {
			if(get_dec_(buff, val)) {
				return true;
			}
		}
		return false;
	}


	void dump_(uint32_t adr, const uint8_t* src, uint8_t len) {
		utils::format("%05X:") % adr;
		for(uint8_t i = 0; i < len; ++i) {
			utils::format(" %02X") % static_cast<uint32_t>(src[i]);
		}
		sci_putch('\n');
	}


	bool help_(uint8_t cmdn) {
		if(cmdn >= 1 && command_.cmp_word(0, "help")) {
			sci_puts("speed KBPS (KBPS: 10 to 1000 [Kbps])\n");
			sci_puts("type TYPE ID SIZE (TYPE: M256B/M64KB/M128KB, ID: 0 to 7, SIZE: 1 to 256)\n");
			sci_puts("read ADDRESS [END-ADDRESS]\n");
			sci_puts("write ADDRESS DATA ...\n");
			sci_puts("fill ADDRESS LENGTH DATA ...\n");
			return true;
		}
		return false;
	}


	bool speed_(uint8_t cmdn) {
		if(cmdn == 2 && command_.cmp_word(0, "speed")) {
			uint32_t val;
			if(get_decimal_(1, val)) {
				if(val >= 10 && val <= 1000) {
					uint8_t clock = 1000 / val;
					if(clock & 1) ++clock;
					clock >>= 1;
					if(clock == 0) clock = 1;
					i2c_.set_clock(clock);
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


	bool type_(uint8_t cmdn) {
		if(cmdn == 4 && command_.cmp_word(0, "type")) {
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
			if(command_.cmp_word(1, "M256B")) {
				if(id >= 0 && id <= 7) {
					eeprom_.start(static_cast<eeprom::M256B>(id), pgs);
				} else {
					sci_puts("Invalid ID renge.\n");
				}
				return true;
			} else if(command_.cmp_word(1, "M64KB")) {
				if(id >= 0 && id <= 7) {
					eeprom_.start(static_cast<eeprom::M64KB>(id), pgs);
				} else {
					sci_puts("Invalid ID renge.\n");
				}
				return true;
			} else if(command_.cmp_word(1, "M128KB")) {
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


	bool read_(uint8_t cmdn) {
		if(cmdn >= 2 && command_.cmp_word(0, "read")) {
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


	bool write_(uint8_t cmdn) {
		if(cmdn >= 3 && command_.cmp_word(0, "write")) {
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


	bool fill_(uint8_t cmdn) {
		if(cmdn >= 4 && command_.cmp_word(0, "fill")) {
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
		uint8_t intr_level = 2;
		timer_b_.start_timer(60, intr_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// I2C クラスの初期化
	{
		i2c_.start(iica::speed::fast);
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
