//=====================================================================//
/*!	@file
	@brief	R8C タイマーJ、パルス出力 メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include <cstring>
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/command.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"

static timer_b timer_b_;
static uart0 uart0_;
static utils::command<64> command_;

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

static timer_j timer_j_;

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


static bool get_decimal_(uint8_t no, uint32_t& val) {
	char buff[9];
	if(command_.get_word(no, sizeof(buff), buff)) {
		if(get_dec_(buff, val)) {
			return true;
		}
	}
	return false;
}


static bool help_(uint8_t cmdn) {
	if(cmdn >= 1 && check_key_word_(0, "help")) {
		sci_puts("freq FREQUENCY[Hz]\n");
		return true;
	}
	return false;
}


static bool freq_(uint8_t cmdn) {
	if(cmdn >= 2) {
		uint32_t val;
		if(get_decimal_(1, val)) {
			if(!timer_j_.set_cycle(val)) {
				sci_puts("TRJ out of range.\n");
			}
			return true;
		}
	}
	return false;
}

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
		uart0_.start(19200, ir_level);
	}

	// TRJ のパルス出力設定
	{
		utils::PORT_MAP(utils::port_map::P17::TRJIO);
		if(!timer_j_.pluse_out(1000)) {
			sci_puts("TRJ out of range.\n");
		}
	}

	sci_puts("Start R8C PLUSE output monitor\n");

	command_.set_prompt("# ");

	while(1) {
		timer_b_.sync();

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn == 0) ;
			else if(help_(cmdn)) ;
			else if(freq_(cmdn)) ;
			else {
				sci_puts("Command error: ");
				sci_puts(command_.get_command());
				sci_putch('\n');
			}
		}
	}
}
