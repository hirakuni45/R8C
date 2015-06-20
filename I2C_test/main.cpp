//=====================================================================//
/*!	@file
	@brief	R8C I2C メイン @n
			for DS1302 RTC
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "system.hpp"
#include "clock.hpp"
#include "common/port_map.hpp"
#include "common/command.hpp"
#include <cstring>
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
static ds1371 rtc_;

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


static time_t get_time_() {
	time_t t = 0;
	if(!rtc_.get_time(t)) {
		sci_puts("Stall RTC read...\n");
	}
	return t;
}


static const char* wday_[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" 
};


static const char* mon_[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


static void disp_time_(time_t t) {
	struct tm *m = gmtime(&t);

	utils::format("%s %s %d %02d:%02d:%02d  %4d\n")
		% wday_[m->tm_wday]
		% mon_[m->tm_mon]
		% static_cast<uint32_t>(m->tm_mday)
		% static_cast<uint32_t>(m->tm_hour)
		% static_cast<uint32_t>(m->tm_min)
		% static_cast<uint32_t>(m->tm_sec)
		% static_cast<uint32_t>(m->tm_year + 1900);
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


static const char* get_dec_(const char* p, char tmch, int& value) {
	int v = 0;
	char ch;
	while((ch = *p) != 0) {
		++p;
		if(ch == tmch) {
			break;
		} else if(ch >= '0' && ch <= '9') {
			v *= 10;
			v += ch - '0';
		} else {
			return nullptr;
		}
	}
	value = v;
	return p;
}


 __attribute__ ((section (".exttext")))
static void set_time_date_()
{
	time_t t = get_time_();
	if(t == 0) return;

	struct tm *m = gmtime(&t);
	bool err = false;
	if(command_.get_words() == 3) {
		char buff[12];
		if(command_.get_word(1, sizeof(buff), buff)) {
			const char* p = buff;
			int vs[3];
			uint8_t i;
			for(i = 0; i < 3; ++i) {
				p = get_dec_(p, '/', vs[i]);
				if(p == nullptr) {
					break;
				}
			}
			if(p != nullptr && p[0] == 0 && i == 3) {
				if(vs[0] >= 1900 && vs[0] < 2100) m->tm_year = vs[0] - 1900;
				if(vs[1] >= 1 && vs[1] <= 12) m->tm_mon = vs[1] - 1;
				if(vs[2] >= 1 && vs[2] <= 31) m->tm_mday = vs[2];		
			} else {
				err = true;
			}
		}

		if(command_.get_word(2, sizeof(buff), buff)) {
			const char* p = buff;
			int vs[3];
			uint8_t i;
			for(i = 0; i < 3; ++i) {
				p = get_dec_(p, ':', vs[i]);
				if(p == nullptr) {
					break;
				}
			}
			if(p != nullptr && p[0] == 0 && (i == 2 || i == 3)) {
				if(vs[0] >= 0 && vs[0] < 24) m->tm_hour = vs[0];
				if(vs[1] >= 0 && vs[1] < 60) m->tm_min = vs[1];
				if(i == 3 && vs[2] >= 0 && vs[2] < 60) m->tm_sec = vs[2];
				else m->tm_sec = 0;
			} else {
				err = true;
			}
		}
	}

	if(err) {
		sci_puts("Can't analize Time/Date input.\n");
		return;
	}

	time_t tt = mktime(m);
	if(!rtc_.set_time(tt)) {
		sci_puts("Stall RTC write...\n");
	}
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

	// DS1371 RTC を開始
	{
		rtc_.start();
	}

	sci_puts("Start R8C RTC monitor\n");
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
			if(cmdn >= 1) {
				if(check_key_word_(0, "date")) {
					if(cmdn == 1) {
						time_t t = get_time_();
						if(t != 0) {
							disp_time_(t);
						}
					} else {
						set_time_date_();
					}
				} else if(check_key_word_(0, "help")) {
					sci_puts("date\n");
					sci_puts("date yyyy/mm/dd hh:mm[:ss]\n");
				} else {
					char buff[12];
					if(command_.get_word(0, sizeof(buff), buff)) {
						sci_puts("Command error: ");
						sci_puts(buff);
						sci_putch('\n');
					}
				}
			}
		}
	}
}
