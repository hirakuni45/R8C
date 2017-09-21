//=====================================================================//
/*!	@file
	@brief	R8C LED 点滅 @n
			LED は、P1_0、P1_1に接続（吸い込み点灯）@n
			※M120AN(20)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/vect.h"
#include "common/delay.hpp"
#include "common/port_map.hpp"

int main(int argc, char *argv[])
{
	using namespace device;

// クロック関係レジスタ・プロテクト解除
	PRCR.PRC0 = 1;

// 高速オンチップオシレーターへ切り替え
	OCOCR.HOCOE = 1;
	utils::delay::micro_second(1);  // >=30us(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// ポート設定
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		PD1.B0 = 1;
		PD1.B1 = 1;
	}

	// LED 点滅メイン
	while(1) {
		P1.B0 = 0;
		P1.B1 = 1;
		// 250ms (0.25s)
		for(uint16_t i = 0; i < 250; ++i) {
			utils::delay::micro_second(1000); // 1ms
		}
		P1.B0 = 1;
		P1.B1 = 0;
		// 250ms (0.25s)
		for(uint16_t i = 0; i < 250; ++i) {
			utils::delay::micro_second(1000); // 1ms
		}		
	}
}
