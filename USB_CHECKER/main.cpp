//=====================================================================//
/*!	@file
	@brief	R8C USB チェッカー・メイン @n
			for ST7567 SPI @n
			LCD: AQM1284A-RN (128x48) Akizuki
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

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


	void TIMER_RB_intr(void) {
		checker_.timer_b_.itask();
	}


#ifdef UART
	void UART0_TX_intr(void) {
		checker_.uart_.isend();
	}


	void UART0_RX_intr(void) {
		checker_.uart_.irecv();
	}
#endif
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
