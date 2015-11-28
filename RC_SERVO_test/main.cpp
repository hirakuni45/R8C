//=====================================================================//
/*!	@file
	@brief	R8C RC-Servo メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"

// どちらか片方を有効にする
#define JR_TYPE_SERVO
// #define FUTABA_TYPE_SERVO

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

typedef device::trc_io<timer_intr> trc_type;
static trc_type timer_c_;
static uart0 uart0_;
static adc adc_;

extern "C" {
	void sci_putch(char ch) {
		uart0_.putch(ch);
	}

	char sci_getch(void) {
		return uart0_.getch();
	}

	uint16_t sci_length() {
		return uart0_.length();
	}

	void sci_puts(const char* str) {
		uart0_.puts(str);
	}
}

extern "C" {
	const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
		reinterpret_cast<void*>(brk_inst_),		nullptr,	// (0)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (1) flash_ready
		reinterpret_cast<void*>(null_task_),	nullptr,	// (2)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (3)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (4) コンパレーターB1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (5) コンパレーターB3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (6)
		reinterpret_cast<void*>(timer_c_.itask),nullptr,	// (7) タイマＲＣ

		reinterpret_cast<void*>(null_task_),	nullptr,	// (8)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (9)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (10)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (11)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (12)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (13) キー入力
		reinterpret_cast<void*>(null_task_),	nullptr,	// (14) A/D 変換
		reinterpret_cast<void*>(null_task_),	nullptr,	// (15)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (16)
		reinterpret_cast<void*>(uart0_.isend),	nullptr,	// (17) UART0 送信
		reinterpret_cast<void*>(uart0_.irecv),	nullptr,	// (18) UART0 受信
		reinterpret_cast<void*>(null_task_),	nullptr,	// (19)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (20)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (21) /INT2
		reinterpret_cast<void*>(null_task_),	nullptr,	// (22) タイマＲＪ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (23) 周期タイマ

		reinterpret_cast<void*>(null_task_),    nullptr,	// (24) タイマＲＢ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (25) /INT1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (26) /INT3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (27)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (28)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (29) /INT0
		reinterpret_cast<void*>(null_task_),	nullptr,	// (30)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (31)
	};
}

static uint16_t calc_pwm_(uint16_t adc_value)
{
	auto v = (static_cast<uint32_t>(adc_value) * (rcs_d_ * 2)) >> 10;
	return v - rcs_d_ + rcs_n_;
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
		uart0_.start(19200, ir_level);
	}

	// ADC の設定（CH1のサイクルモード）
	// P10, P11 の A/D 変換
	{
		utils::PORT_MAP(utils::port_map::P10::AN0);
		utils::PORT_MAP(utils::port_map::P11::AN1);
		adc_.setup(adc::cnv_type::CH0_CH1, adc::ch_grp::AN0_AN1, true);
		adc_.start();
	}

	// ＰＷＭモード設定
	{
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		utils::PORT_MAP(utils::port_map::P13::TRCIOC);
		bool pfl = 0;  // 0->1
		uint8_t ir_level = 2;
		timer_c_.start_pwm(50000, trc_type::divide::f8, pfl, ir_level);
		timer_c_.set_pwm_b(rcs_n_);
		timer_c_.set_pwm_c(rcs_n_);
	}

	sci_puts("Start R8C RC-Servo monitor\n");

	while(1) {
		timer_c_.task_.sync();
		if(adc_.get_state()) {
			auto ch_a = calc_pwm_(adc_.get_value(0));
			timer_c_.set_pwm_b(ch_a);
			auto ch_b = calc_pwm_(adc_.get_value(1));
			timer_c_.set_pwm_c(ch_b);			
		}
	}
}
