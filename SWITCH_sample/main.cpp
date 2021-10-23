//=====================================================================//
/*!	@file
	@brief	R8C スイッチ入力、サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/intr_utils.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"

namespace {

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B	timer_b_;

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

namespace {

	uint8_t inp_lvl_ = 0;
	uint8_t inp_pos_ = 0;
	uint8_t inp_neg_ = 0;

	void switch_service_()
	{
		uint8_t lvl = ~device::P1();  ///< 状態の取得
		inp_pos_ = ~inp_lvl_ &  lvl;  ///< 立ち上がりエッジ検出
		inp_neg_ =  inp_lvl_ & ~lvl;  ///< 立ち下がりエッジ検出
		inp_lvl_ = lvl;  ///< 状態のセーブ
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

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// スイッチ入力の設定 P10、P11、COM:Vss
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		device::PD1.B0 = 0;
		device::PD1.B1 = 0;
		device::PUR1.B0 = 1;	///< プルアップ
		device::PUR1.B1 = 1;	///< プルアップ
	}

	sci_puts("Start R8C SWITCH sample\n");

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		switch_service_();

		if(inp_pos_ & device::P1.B0.b()) {
			sci_puts("SW0 - positive\n");
		}
		if(inp_pos_ & device::P1.B1.b()) {
			sci_puts("SW1 - positive\n");
		}

		if(inp_neg_ & device::P1.B0.b()) {
			sci_puts("SW0 - negative\n");
		}
		if(inp_neg_ & device::P1.B1.b()) {
			sci_puts("SW1 - negative\n");
		}

		if(inp_lvl_ & device::P1.B0.b()) {
			if((cnt % 10) == 0) {
				sci_puts("SW0 - ON\n");
			}
		}
		if(inp_lvl_ & device::P1.B1.b()) {
			if((cnt % 10) == 0) {
				sci_puts("SW1 - ON\n");
			}
		}

		++cnt;
		if(cnt >= 60) cnt = 0;
	}
}
