//=====================================================================//
/*!	@file
	@brief	R8C RC-Servo メイン
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/adc_io.hpp"
#include "common/trc_io.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"

// どちらか片方を有効にする
#define JR_TYPE_SERVO
// #define FUTABA_TYPE_SERVO

namespace {

#ifdef JR_TYPE_SERVO
	static const uint16_t rcs_n_ =  3750;  // ニュートラル(JR)：  1500uS --> 3750
	static const uint16_t rcs_d_ =  1500;  // 可動範囲(JR)：+-600uS --> +-1500
#endif

#ifdef FUTABA_TYPE_SERVO
	static const uint16_t rcs_n_ =  3800;  // ニュートラル(FUTABA)：  1520uS --> 3800
	static const uint16_t rcs_d_ =  1500;  // 可動範囲(JR)：+-600uS --> +-1500
#endif

	class timer_intr {
		static volatile uint8_t trc_sync_;

	public:
		void operator() () {
			++trc_sync_;
		}

		void sync() const {
			volatile uint8_t v = trc_sync_;
			while(v == trc_sync_) {
			}
		}
	};
	volatile uint8_t timer_intr::trc_sync_ = 0;

	typedef device::trc_io<timer_intr> TRC;
	TRC		timer_c_;

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART	uart_;

	typedef device::adc_io<utils::null_task> ADC;
	ADC		adc_;

	uint16_t calc_pwm_(uint16_t adc_value)
	{
		auto v = (static_cast<uint32_t>(adc_value) * (rcs_d_ * 2)) >> 10;
		return v - rcs_d_ + rcs_n_;
	}
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


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}


	void TIMER_RC_intr(void) {
		timer_c_.itask();
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

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// ADC の設定（CH1のサイクルモード）
	// P10, P11 の A/D 変換
	{
		utils::PORT_MAP(utils::port_map::P10::AN0);
		utils::PORT_MAP(utils::port_map::P11::AN1);
		adc_.start(ADC::CH_TYPE::CH0_CH1, ADC::CH_GROUP::AN0_AN1, true);
	}

	// ＰＷＭモード設定
	{
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		utils::PORT_MAP(utils::port_map::P13::TRCIOC);
		bool pfl = 0;  // 0->1
		uint8_t ir_level = 2;
		timer_c_.start(50000, TRC::DIVIDE::F8, pfl, ir_level);
		timer_c_.set_pwm_b(rcs_n_);
		timer_c_.set_pwm_c(rcs_n_);
	}

	sci_puts("Start R8C RC-Servo monitor\n");

	while(1) {
		timer_c_.task_.sync();
		adc_.scan();
		adc_.sync();
		auto ch_a = calc_pwm_(adc_.get_value(0));
		timer_c_.set_pwm_b(ch_a);
		auto ch_b = calc_pwm_(adc_.get_value(1));
		timer_c_.set_pwm_c(ch_b);
	}
}
