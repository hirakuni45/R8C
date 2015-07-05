//=====================================================================//
/*!	@file
	@brief	R8C SD メイン @n
			for ST7567 SPI (128 x 32) @n
			LCD: Aitendo M-G0812P7567
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
#include "pfatfs/src/pff.h"

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
static spi_base spi_base_;
static spi_ctrl spi_ctrl_;
// static lcd lcd_;
// static mono_graph bitmap_;

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

// static uint8_t v_ = 91;
// static uint8_t m_ = 123;

#if 0
static void randmize_(uint8_t v, uint8_t m)
{
	v_ = v;
	m_ = m;
}
#endif

#if 0
static uint8_t rand_()
{
	v_ += v_ << 2;
	++v_;
	uint8_t n = 0;
	if(m_ & 0x02) n = 1;
	if(m_ & 0x40) n ^= 1;
	m_ += m_;
	if(n == 0) ++m_;
	return v_ ^ m_;
}
#endif

//  __attribute__ ((section (".exttext")))
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

	// spi_base, spi_ctrl ポートの初期化
	{
		spi_ctrl_.init();
		spi_base_.init();
	}

	sci_puts("Start R8C SD monitor\n");

	FATFS fatfs;
	bool mount = false;
	// pfatfs を開始
	{
		if(pf_mount(&fatfs) != FR_OK) {
			sci_puts("SD mount error\n");
		} else {
			sci_puts("SD mount OK!\n");
			mount = true;
		}
	}

	if(mount) {
		DIR dir;
		if(pf_opendir(&dir, "") != FR_OK) {
			sci_puts("Can't open dir\n");
		} else {

			for(;;) {
				FILINFO fno;
				// Read a directory item
				if(pf_readdir(&dir, &fno) != FR_OK) {
					sci_puts("Can't read dir\n");
					break;
				}
				if(!fno.fname[0]) break;

				if(fno.fattrib & AM_DIR) {
					utils::format("        /%s\n") % fno.fname;
				} else {
					utils::format("%8d  %s\n") % static_cast<uint32_t>(fno.fsize) % fno.fname;
				}
			}
		}
	}




#if 0
	// LCD を開始
	{
		lcd_.start();
		bitmap_.init();
		bitmap_.clear(0);
	}
#endif

	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t cnt = 0;
//	uint16_t x = rand_() & 127;
//	uint16_t y = rand_() & 31;
//	uint16_t xx;
//	uint16_t yy;
//	uint8_t loop = 20;
	while(1) {
		timer_b_.sync();
//		lcd_.copy(bitmap_.fb());
//		if(loop >= 20) {
//			loop = 0;
//			bitmap_.clear(0);
//			bitmap_.frame(0, 0, 128, 32, 1);
//		}
//		xx = rand_() & 127;
//		yy = rand_() & 31;
//		bitmap_.line(x, y, xx, yy, 1);
//		x = xx;
//		y = yy;
//		++loop;

//		bitmap_.line(0, 0, 127, 31, 1);
//		bitmap_.line(0, 31, 127, 0, 1);

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		// コマンド入力と、コマンド解析
		if(command_.service()) {
#if 0
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
#endif
		}
	}
}
