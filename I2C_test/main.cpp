//=====================================================================//
/*!	@file
	@brief	R8C I2C メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/port_map.hpp"
#include "common/command.hpp"
#include <cstring>
#include <cstdlib>
#include "common/ds1371_io.hpp"
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

static utils::command<64> command_;

// DS1371 I2C ポートの定義クラス
// P1_B7: SCL
// P4_B5: SDA
struct scl_sda {
	void init() const {
		device::POD1.B7 = 1;
		device::POD4.B5 = 1;
	}
	void scl_dir(bool b) const { device::PD1.B7 = b; }
	void scl_out(bool b) const { device::P1.B7 = b; }
	bool scl_inp() const { return device::P1.B7(); }
	void sda_dir(bool b) const { device::PD4.B5 = b; }
	void sda_out(bool b) const { device::P4.B5 = b; }
	bool sda_inp() const { return device::P4.B5(); }
};

static device::ds1371_io<scl_sda> rtc_;

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


static void disp_time_(time_t t) {
	struct tm *m = gmtime(&t);

	static const char* wday_[] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" 
	};
	static const char* mon_[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

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


#if 0
static void cmd_date_()
{
		time_t tt = 0;
		rtc_.get_time(tt);
		struct tm *tp = localtime(&tt);

		if(ss.size() == 1) {
			printf("%d/%d/%d ", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
			static const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			printf("%s ", days[tp->tm_wday % 7]);
			printf("%02d:%02d.%02d\n", tp->tm_hour, tp->tm_min, tp->tm_sec);
		} else {
			struct tm tmp = *tp;
			std::string time;
			if(ss.size() >= 3) {
				int year, mon, day;
				if(sscanf(ss[1].c_str(), "%d/%d/%d", &year, &mon, &day) == 3) {
					if(year >= 1900 && year < 2100) tmp.tm_year = year - 1900;
					if(mon >= 1 && mon <= 12) tmp.tm_mon = mon - 1;
					if(day >= 1 && day <= 31) tmp.tm_mday = day;
				}
				time = ss[2];
			} else if(ss.size() >= 2) {
				time = ss[1];
			}
			if(!time.empty()) {
				int hour, min, sec;
				if(sscanf(time.c_str(), "%d:%d.%d", &hour, &min, &sec) == 3) {
					if(hour >= 0 && hour < 24) tmp.tm_hour = hour;
					if(min >= 0 && min < 60) tmp.tm_min = min;
					if(sec >= 0 && sec < 60) tmp.tm_sec = sec;
				} else if(sscanf(time.c_str(), "%d:%d", &hour, &min) == 2) {
					if(hour >= 0 && hour < 24) tmp.tm_hour = hour;
					if(min >= 0 && min < 60) tmp.tm_min = min;
				}
			}
			time_t tt = mktime(&tmp);
			rtc_.set_time(tt);
		}
		return true;
}

static uint16_t get_hexadecimal_(const char* str)
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


static void put_hexadecimal_(uint8_t val) {
	val &= 0xf;
	if(val > 9) val += 'A' - 10;
	else val += '0';
	sci_putch(val);	
}


static void put_hexadecimal_byte_(uint8_t val) {
	put_hexadecimal_(val >> 4);
	put_hexadecimal_(val);
}
#endif


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


	{  // DS1371 RTC の許可
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

		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn >= 1) {
				if(check_key_word_(0, "date")) {			   
					time_t t;
					if(rtc_.get_time(t)) {
						disp_time_(t);
					} else {
						sci_puts("Stall RTC error\n");
					}
				} else {
					sci_puts("Command error: ");
					char buff[12];
					if(command_.get_word(0, sizeof(buff), buff)) {
						sci_puts(buff);
						sci_putch('\n');
					}
				}
			}
		}
	}
}
