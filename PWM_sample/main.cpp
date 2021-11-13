//=====================================================================//
/*!	@file
	@brief	R8C PWM メイン @n
			PWM 出力 B：P12(TRCIOB) 18 pin @n
			PWM 出力 C: P13(TRCIOC) 17 pin @n
			PWM 出力 D: P10(TRCIOD) 20 pin
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trc_io.hpp"
#include "common/adc_io.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B timer_b_;

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::adc_io<utils::null_task> ADC;
	ADC		adc_;

	typedef device::trc_io<utils::null_task> TIMER_C;
	TIMER_C	timer_c_;
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


// このプロジェクトでは、割り込みを使っていない
//	void TIMER_RC_intr(void) {
//		timer_c_.itask();
//	}

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

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// ADC の設定（CH1のサイクルモード）
	{
		utils::PORT_MAP(utils::port_map::P11::AN1);
		adc_.start(ADC::CH_TYPE::CH1, ADC::CH_GROUP::AN0_AN1, true);
	}

	// ＰＷＭモード設定
	{
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		utils::PORT_MAP(utils::port_map::P13::TRCIOC);
		utils::PORT_MAP(utils::port_map::P10::TRCIOD);
		bool pfl = 0;  // 0->1
		timer_c_.start(10000, pfl);
		uint16_t n = timer_c_.get_pwm_limit();
		timer_c_.set_pwm_b(n >> 2);  // 25%
		timer_c_.set_pwm_c(n - (n >> 2));  // 75%
	}

	sci_puts("Start R8C PWM monitor\n");

	adc_.scan();
	// LED シグナル用ポートを出力
//	PD1.B0 = 1;

//	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();
		if(adc_.get_state()) {
			uint32_t v = adc_.get_value(1);
			v *= timer_c_.get_pwm_limit();
			timer_c_.set_pwm_d(v >> 10);
			adc_.scan();
		}

//		++cnt;
//		if(cnt >= 30) {
//			cnt = 0;
//		}

//		if(cnt < 10) P1.B0 = 1;
//		else P1.B0 = 0;
	}
}
