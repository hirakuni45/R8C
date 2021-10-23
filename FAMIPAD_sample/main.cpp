//=====================================================================//
/*!	@file
	@brief	R8C ファミコン互換パッド入力、サンプル @n
			Maybe (4021B) 8-Stage Static Shift Register @n
			B0: DIR-Right @n
			B1: DIR-Left @n
			B2: DIR-Down @n
			B3: DIR-Up @n
			B4: START @n
			B5: SELECT @n
			B6: B @n
			B7: A
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"

namespace {
	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	// RED: +V, YELOW: GND, BLUE: P/S, BROWN: CLK, WHITE: OUT
	typedef device::PORT<device::PORT1, device::bitpos::B0> PAD_PS;
	typedef device::PORT<device::PORT1, device::bitpos::B1> PAD_CLK;
	typedef device::PORT<device::PORT1, device::bitpos::B2> PAD_OUT;
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

	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		utils::PORT_MAP(utils::port_map::P12::PORT);
		PAD_PS::DIR = 1;
		PAD_CLK::DIR = 1;
		PAD_OUT::DIR = 0;
	}

	sci_puts("Start R8C FAMI-PAD sample\n");

	uint8_t cnt = 0;
	uint8_t data = 0;
	while(1) {
		timer_b_.sync();

		PAD_PS::P = 0; // seirial
		uint8_t d = 0;
		for(uint8_t i = 0; i < 8; ++i) {
			d <<= 1;
			if(!PAD_OUT::P()) ++d;
			PAD_CLK::P = 1;
			utils::delay::micro_second(1);
			PAD_CLK::P = 0;
			utils::delay::micro_second(1);
		}
		PAD_PS::P = 1; // parallel

		if(data != d) {
			data = d;
			utils::format("%08b\n") % static_cast<uint16_t>(data);
		}

		++cnt;
		if(cnt >= 60) cnt = 0;
	}
}
