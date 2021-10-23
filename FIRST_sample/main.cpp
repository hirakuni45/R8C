//=====================================================================//
/*!	@file
	@brief	R8C LED 点滅 @n
			LED は、P1_0(20)、P1_1(19)に接続（吸い込み点灯）@n
			※M120AN(20)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

namespace {

	// LED は '0' で点灯、'1' で消灯するように接続するので、LED クラスの論理を反転して定義する
	typedef device::PORT<device::PORT1, device::bitpos::B0, false> LED0;
	typedef device::PORT<device::PORT1, device::bitpos::B1, false> LED1;

}

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

	// LED 設定
	{
		LED0::DIR = 1;
		LED1::DIR = 1;
	}

	// LED 点滅メイン
	while(1) {
		LED0::P = 0;
		LED1::P = 1;
		// 250ms (0.25s)
		utils::delay::milli_second(250);
		LED0::P = 1;
		LED1::P = 0;
		// 250ms (0.25s)
		utils::delay::milli_second(250);
	}
}
