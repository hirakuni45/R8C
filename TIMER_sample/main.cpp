//=====================================================================//
/*!	@file
	@brief	R8C タイマー・メイン @n
			P1_0: LED @n
			P1_1: LED
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/trb_io.hpp"

namespace {

	typedef device::PORT<device::PORT1, device::bitpos::B0, false> LED0;
	typedef device::PORT<device::PORT1, device::bitpos::B1, false> LED1;

	volatile uint16_t trb_count_;

	/// タイマー割り込みで実行する動作の定義
	class trb_intr_task {
	public:
		void operator() () {
			++trb_count_;
		}
	};

	typedef device::trb_io<trb_intr_task, uint8_t> TIMER_B;
	TIMER_B	timer_b_;

}


extern "C" {

	void TIMER_RB_intr()
	{
		timer_b_.itask();
	}

}

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

	// タイマーの設定
	{
		uint8_t intr_level = 1;
		uint16_t freq = 60; // 60Hz
		timer_b_.start(freq, intr_level);
	}

	// LED 設定
	{
		LED0::DIR = 1;
		LED1::DIR = 1;
		LED0::P = 0;
		LED1::P = 0;
	}

	// タイマー・メイン
	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();
		if(cnt < 20) {
			LED0::P = 0;
			LED1::P = 1;
		} else {
			LED0::P = 1;
			LED1::P = 0;
		}
		++cnt;
		if(cnt >= 60) cnt = 0;
	}
}
