//=====================================================================//
/*!	@file
	@brief	R8C USB チェッカー・メイン @n
			for ST7567 SPI @n
			LCD: AQM1284A-RN (128x48) Akizuki
	@author	平松邦仁 (hira@rvf-rc45.net)
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

// #include "bitmap/font32.h"

#include "checker.hpp"

// ポートの配置
// P4_2(1):   LCD_SCK(SCL)
// P3_7(2):   LCD_/CS
// /RES(3):  (System reset)
// P4_7(4):   LCD_/RES
// VSS:(5)   (Power GND)
// P4_6(6):
// VCC(7):   (Power +3.3V)
// MODE(8):  (System mode) Flash program for 'L'
// P3_5(9):   SW-A 
// P3_4(10):  SW-B
// P3_3(11):  LCD_A0
// P4_5(12):  LCD_SDA
// P1_7(13):
// P1_6(14):  (System RXD) Flash program for 'RXD'
// P1_5(15):  RXD0
// P1_4(16):  TXD0
// P1_3(17):  AN3
// P1_2(18):  AN2
// P1_1(19):  AN1 voltage sense (19.8V max)
// P1_0(20):  AN0 current sense (1.2 V/A)

namespace {
	app::checker checker_;
}

extern "C" {
	void sci_putch(char ch) {
#ifdef UART
		checker_.uart_.putch(ch);
#endif
	}

	char sci_getch(void) {
#ifdef UART
		return checker_.uart_.getch();
#else
		return 0;
#endif
	}

#if 0
	uint16_t sci_length() {
		return checker_.uart_.length();
	}

	void sci_puts(const char* str) {
		checker_.uart_.puts(str);
	}
#endif
}

extern "C" {
	const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
		reinterpret_cast<void*>(brk_inst_),		nullptr,	// (0)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (1) flash_ready
		reinterpret_cast<void*>(null_task_),	nullptr,	// (2)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (3)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (4) コンパレーターB1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (5) コンパレーターB3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (6)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (7) タイマＲＣ

		reinterpret_cast<void*>(null_task_),	nullptr,	// (8)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (9)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (10)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (11)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (12)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (13) キー入力
		reinterpret_cast<void*>(null_task_),	nullptr,	// (14) A/D 変換
		reinterpret_cast<void*>(null_task_),	nullptr,	// (15)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (16)
#ifdef UART
		reinterpret_cast<void*>(checker_.uart_.isend),	nullptr,	// (17) UART0 送信
		reinterpret_cast<void*>(checker_.uart_.irecv),	nullptr,	// (18) UART0 受信
#else
		reinterpret_cast<void*>(null_task_),	nullptr,	// (17)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (18)
#endif
		reinterpret_cast<void*>(null_task_),	nullptr,	// (19)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (20)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (21) /INT2
		reinterpret_cast<void*>(null_task_),	nullptr,	// (22) タイマＲＪ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (23) 周期タイマ

		reinterpret_cast<void*>(checker_.timer_b_.itask),nullptr,	// (24) タイマＲＢ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (25) /INT1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (26) /INT3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (27)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (28)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (29) /INT0
		reinterpret_cast<void*>(null_task_),	nullptr,	// (30)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (31)
	};
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

	checker_.init();

	while(1) {
		checker_.service();
	}
}
