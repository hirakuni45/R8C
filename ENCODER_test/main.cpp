//=====================================================================//
/*!	@file
	@brief	R8C エンコーダー・メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
#include "main.hpp"
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/format.hpp"

static uint8_t enc_lvl_ = 0;
static uint8_t enc_pos_ = 0;
static uint8_t enc_neg_ = 0;
static volatile int8_t enc_cnt_ = 0;

class encoder {
public:
	void operator() () {
		uint8_t lvl = device::P1();  ///< 状態の取得
		enc_pos_ = ~enc_lvl_ &  lvl;  ///< 立ち上がりエッジ検出
		enc_neg_ =  enc_lvl_ & ~lvl;  ///< 立ち下がりエッジ検出
		enc_lvl_ = lvl;  ///< 状態のセーブ

		if(enc_pos_ & device::P1.B0.b()) {
			if(enc_lvl_ & device::P1.B1.b()) {
				--enc_cnt_;
			} else {
				++enc_cnt_;
			}
		}
		if(enc_neg_ & device::P1.B0.b()) {
			if(enc_lvl_ & device::P1.B1.b()) {
				++enc_cnt_;
			} else {
				--enc_cnt_;
			}
		}
		if(enc_pos_ & device::P1.B1.b()) {
			if(enc_lvl_ & device::P1.B0.b()) {
				++enc_cnt_;
			} else {
				--enc_cnt_;
			}
		}
		if(enc_neg_ & device::P1.B1.b()) {
			if(enc_lvl_ & device::P1.B0.b()) {
				--enc_cnt_;
			} else {
				++enc_cnt_;
			}
		}
	}
};

static device::trb_io<encoder> timer_b_;
static uart0 uart0_;

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
		(void*)brk_inst_,   nullptr,	// (0)
		(void*)null_task_,  nullptr,	// (1) flash_ready
		(void*)null_task_,  nullptr,	// (2)
		(void*)null_task_,  nullptr,	// (3)

		(void*)null_task_,  nullptr,	// (4) コンパレーターB1
		(void*)null_task_,  nullptr,	// (5) コンパレーターB3
		(void*)null_task_,  nullptr,	// (6)
		(void*)null_task_,  nullptr,	// (7) タイマＲＣ

		(void*)null_task_,  nullptr,	// (8)
		(void*)null_task_,  nullptr,	// (9)
		(void*)null_task_,  nullptr,	// (10)
		(void*)null_task_,  nullptr,	// (11)

		(void*)null_task_,  nullptr,	// (12)
		(void*)null_task_,  nullptr,	// (13) キー入力
		(void*)null_task_,  nullptr,	// (14) A/D 変換
		(void*)null_task_,  nullptr,	// (15)

		(void*)null_task_,  nullptr,	// (16)
		(void*)uart0_.send_task, nullptr,   // (17) UART0 送信
		(void*)uart0_.recv_task, nullptr,   // (18) UART0 受信
		(void*)null_task_,  nullptr,	// (19)

		(void*)null_task_,  nullptr,	// (20)
		(void*)null_task_,  nullptr,	// (21) /INT2
		(void*)null_task_,  nullptr,	// (22) タイマＲＪ２
		(void*)null_task_,  nullptr,	// (23) 周期タイマ

		(void*)timer_b_.itask,  nullptr,	// (24) タイマＲＢ２
		(void*)null_task_,  nullptr,	// (25) /INT1
		(void*)null_task_,  nullptr,	// (26) /INT3
		(void*)null_task_,  nullptr,	// (27)

		(void*)null_task_,  nullptr,	// (28)
		(void*)null_task_,  nullptr,	// (29) /INT0
		(void*)null_task_,  nullptr,	// (30)
		(void*)null_task_,  nullptr,	// (31)
	};
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

	// エンコーダー入力の設定 P10: (Phi_A), P11: (Phi_B), Vss: (COM)
	{
		utils::PORT_MAP(utils::port_map::P10::PORT);
		utils::PORT_MAP(utils::port_map::P11::PORT);
		device::PD1.B0 = 0;
		device::PD1.B1 = 0;
		device::PUR1.B0 = 1;	///< プルアップ
		device::PUR1.B1 = 1;	///< プルアップ
	}

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(240, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart0_.start(19200, ir_level);
	}

	sci_puts("Start R8C ENCODER monitor\n");

	uint8_t cnt = 0;
	int8_t enc_cnt = 0;
	uint16_t count = 0;
	uint16_t value = 0;
	while(1) {
		timer_b_.sync();

		// エンコーダー値の増減
		if(enc_cnt != enc_cnt_) {
			int8_t d = enc_cnt - enc_cnt_;
			if(d >= 4 || d <= -4) { 
				enc_cnt = enc_cnt_;
				if(d > 0) ++count;
				else --count;
			}
		}

		// 表示ループは１／６０秒で動かす
		if((cnt & 3) == 0) {
			if(count != value) {
				value = count;
				utils::format("%05d\n") % static_cast<uint32_t>(count);
			}
		}

		++cnt;
	}
}
