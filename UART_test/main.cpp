#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "uart_io.hpp"

static void wait_(uint16_t n)
{
	while(n > 0) {
		asm("nop");
		--n;
	}
}

static device::uart_io<device::UART0, 16, 16> uart_io_;

int main(int argc, char *ragv[])
{
	using namespace device;

// クロック関係レジスタ・プロテクト解除
	PRCR.PRC0 = 1;

// 高速オンチップオシレーターへ切り替え
	OCOCR.HOCOE = 1;
	wait_(1000);
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_2 となっているので注意！
	MSTCR.MSTUART = 0;  // モジュールスタンバイ制御
	PMH1E.P14SEL2 = 0;
	PMH1.P14SEL = 1;
	PMH1E.P15SEL2 = 0;
	PMH1.P15SEL = 1;
	uart_io_.start(19200, true);

	for(char ch = 0x20; ch < 0x7f; ++ch) {
		uart_io_.putch(ch);
	} 
	uart_io_.putch('\n');

	// L チカ・メイン
	PD1.B0 = 1;
	uint8_t v = 0;
	while(1) {
		P1.B0 = v;
		for(uint32_t i = 0; i < 50000; ++i) {
			if(uart_io_.length()) {
				char ch = uart_io_.getch();
				uart_io_.putch(ch);
			}
		}
		v ^= 1;
	}
}
