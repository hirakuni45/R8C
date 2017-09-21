//=====================================================================//
/*!	@file
	@brief	R8C エンコーダー・サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/format.hpp"
#include "common/intr_utils.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"

namespace {

	uint8_t enc_lvl_ = 0;
	volatile int8_t enc_cnt_ = 0;

	class encoder {
	public:
		void operator() () {
			uint8_t lvl = device::P1();  ///< 状態の取得
			uint8_t enc_pos = ~enc_lvl_ &  lvl;  ///< 立ち上がりエッジ検出
			uint8_t enc_neg =  enc_lvl_ & ~lvl;  ///< 立ち下がりエッジ検出
			enc_lvl_ = lvl;  ///< 状態のセーブ

			if(enc_pos & device::P1.B0.b()) {
				if(lvl & device::P1.B1.b()) {
					--enc_cnt_;
				} else {
					++enc_cnt_;
				}
			}
			if(enc_neg & device::P1.B0.b()) {
				if(lvl & device::P1.B1.b()) {
					++enc_cnt_;
				} else {
					--enc_cnt_;
				}
			}
			if(enc_pos & device::P1.B1.b()) {
				if(lvl & device::P1.B0.b()) {
					++enc_cnt_;
				} else {
					--enc_cnt_;
				}
			}
			if(enc_neg & device::P1.B1.b()) {
				if(lvl & device::P1.B0.b()) {
					--enc_cnt_;
				} else {
					++enc_cnt_;
				}
			}
		}
	};

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;
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
		uart_.start(57600, ir_level);
	}

	sci_puts("Start R8C ENCODER sample\n");

	uint8_t cnt = 0;
	uint16_t count = 0;
	uint16_t value = 0;
	while(1) {
		timer_b_.sync();

		// エンコーダー値の増減
		if(enc_cnt_ >= 4) {
			enc_cnt_ = 0;
			--count;
		} else if(enc_cnt_ <= -4) { 
			enc_cnt_ = 0;
			++count;
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
