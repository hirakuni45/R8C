//=====================================================================//
/*!	@file
	@brief	R8C RF433 メイン @n
			433MHz TX/RX Module @n
			RF_TX (out)  ---> (P1_0:20) @n
			RF_RX (inp)  ---> (P1_1:19)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "port.hpp"
#include "common/delay.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "chip/TX_MOD.hpp"
#include "chip/RX_MOD.hpp"

namespace {

	// ポートの定義と接続
	// P1_0(20):
	typedef device::PORT<device::PORT1, device::bitpos::B0> RF_TX;
	// P1_1(19):
	typedef device::PORT<device::PORT1, device::bitpos::B1> RF_RX;

	typedef chip::TX_MOD<RF_TX> TX;
	typedef chip::RX_MOD<RF_RX> RX;

	class rf_task {

		TX		tx_;
		RX		rx_;

		bool	flag_;

		volatile uint8_t	recv_;

	public:
		rf_task() : flag_(0), recv_(0) {
			RF_TX::DIR = 1;  // out
			RF_RX::DIR = 0;  // inp
		}

		void task() {
			RF_TX::P = flag_;
			flag_ = !flag_;

			recv_ <<= 1;
			if(RF_RX::P()) ++recv_;
		}

		uint8_t get() const { return recv_; }

		void operator() () {
			task();
		}
	};

	typedef device::trb_io<rf_task, uint8_t> TRB;
	TRB  trb_;

	typedef utils::fifo<uint8_t, 16> BUFFER;
	typedef device::uart_io<device::UART0, BUFFER, BUFFER> UART;
	UART uart_;

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
		trb_.itask();
	}


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}

}


// __attribute__ ((section (".exttext")))
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
	// ※無線データ変調で利用するので、優先順位は最大にする。
	{
		uint8_t ir_level = 2;
		trb_.start_timer(2000, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
	}

	uart_.puts("Start R8C RF433 TX/RX sample\n");

	using namespace utils;

	uint16_t cnt = 0;

	while(1) {
		trb_.sync();

		++cnt;
		if(cnt >= 500) {
			uint8_t d = TRB::task_.get();
			utils::format("%02X\n") % static_cast<uint32_t>(d);
			cnt = 0;
		}
	}
}
