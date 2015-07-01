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
#include "common/command.hpp"
#include "common/format.hpp"

static void wait_(uint16_t n)
{
	while(n > 0) {
		asm("nop");
		--n;
	}
}

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


static bool get_value_(const char* text, uint32_t& val) {
	val = 0;
	char ch;
	while((ch = *text++) != 0) {
		if(ch >= '0' && ch <= '9') {
			ch -= '0';
		} else if(ch >= 'A' && ch <= 'F') {
			ch -= 'A' + 10;
		} else if(ch >= 'a' && ch <= 'f') {
			ch -= 'a' + 10;
		} else {
			return false;
		}
		val <<= 4;
		val |= ch;
	}
	return true;
}


static void dump_(uint32_t adr, const uint8_t* src, uint8_t len) {
	
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
	wait_(1000);
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
		uart0_.start(19200, ir_level);
	}

	// I2C クラスの初期化
	{
		i2c_io_.init();
	}

	// EEPROM（24FC1025）を開始
	{
		// 2 バイトアドレス、ページサイズ128バイト
		uint8_t device_select = 0;
		eeprom_.start(device_select, true, 128);
	}

	sci_puts("Start R8C EEPROM monitor\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

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
			bool err = true;
			if(cmdn == 1) {
				if(check_key_word_(0, "help")) {
					sci_puts("read xxxx\n");
					sci_puts("write yyyy aa bb cc ...\n");
					err = false;
				}
			} else if(cmdn == 2) {
				if(check_key_word_(0, "read")) {
					char buff[9];
					if(command_.get_word(1, sizeof(buff), buff)) {
						uint32_t adr;
						if(get_value_(buff, adr)) {
							uint8_t tmp[8];
							if(eeprom_.read(adr, tmp, 8)) {
								dump_(adr, tmp, 8);
							} else {
								sci_puts("Stall eeprom...\n");
							}
							err = true;
						}
					}
				}
			} else if(cmdn == 3) {
				if(check_key_word_(0, "write")) {



				}
			}
			if(err) {
				sci_puts("Command error: ");
				sci_puts(command_.get_command());
				sci_putch('\n');
			}
		}
	}
}
