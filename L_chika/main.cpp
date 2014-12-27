#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"

static void wait_(uint16_t n)
{
	while(n > 0) {
		asm("nop");
		--n;
	}
}


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

// L チカ・メイン
	PD1.B0 = 1;
	while(1) {
		P1.B0 = 0;
		for(uint32_t i = 0; i < 300000; ++i) { asm("nop"); }
		P1.B0 = 1;
		for(uint32_t i = 0; i < 300000; ++i) { asm("nop"); }
	}

}
