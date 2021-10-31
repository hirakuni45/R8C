//=====================================================================//
/*!	@file
	@brief	R8C エンコーダー・サンプル @n
			・ポート P1_0: A 相 @n
			・ポート P1_1: B 相 @n
			・各プルアップは 5K ～ 10K オーム（ロータリーエンコーダーの仕様を参照）@n
			・エンコーダーのチャタリングは 2ms～3ms 程度なので、周期は 360Hz としている。 @n
			※プルアップ抵抗の値により変化
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"

#include "chip/ENCODER.hpp"

namespace {

	typedef utils::fifo<uint8_t, 16> TX_BUFF;  // 送信バッファ
	typedef utils::fifo<uint8_t, 16> RX_BUFF;  // 受信バッファ
	typedef device::uart_io<device::UART0, TX_BUFF, RX_BUFF> UART;
	UART	uart_;

	// エンコーダー入力の定義
	typedef device::PORT<device::PORT1, device::bitpos::B0> PHA;
	typedef device::PORT<device::PORT1, device::bitpos::B1> PHB;
	// パラメーターを指定しない場合、DECODE::PHA_POS: A 相の立ち上がりのみでカウントとなる。
	typedef chip::ENCODER<PHA, PHB, uint16_t> ENCODER; 

	typedef device::trb_io<ENCODER, uint8_t> TIMER_B;
	TIMER_B timer_b_;
}

extern "C" {

	void sci_putch(char ch) {
		uart_.putch(ch);
	}


	char sci_getch(void) {
		return uart_.getch();
	}


	uint16_t sci_length() {
		return uart_.length();
	}


	void sci_puts(const char* str) {
		uart_.puts(str);
	}


	void TIMER_RB_intr(void) {
		timer_b_.itask();
	}


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
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
	utils::delay::micro_second(1);	// >=30uS(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// エンコーダークラスの開始
	// 実態は、TIMER_B 内にある。
	{
		TIMER_B::task_.start();
	}

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start(60 * 6, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	sci_puts("Start R8C ENCODER sample\n");

	uint16_t value = TIMER_B::task_.get_count();
	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		++cnt;
		// 表示ループは１／６０秒で動かす
		if(cnt >= 6) {
			auto count = TIMER_B::task_.get_count();
			if(count != value) {
				value = count;
				utils::format("%05d\n") % value;
			}
			cnt = 0;
		}
	}
}
