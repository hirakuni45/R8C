//=====================================================================//
/*!	@file
	@brief	R8C タイマーＪ、パルス計測サンプル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/intr_utils.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trj_io.hpp"
#include "common/format.hpp"

namespace {

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	typedef device::trj_io<utils::null_task> timer_j;
	timer_j timer_j_;

	typedef device::trb_io<utils::null_task> timer_b;
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

	const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
		reinterpret_cast<void*>(brk_inst_),		nullptr,	// (0)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (1) flash_ready
		reinterpret_cast<void*>(null_task_),	nullptr,	// (2)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (3)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (4) コンパレーターB1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (5) コンパレーターB3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (6)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (7) タイマＲＣ

		reinterpret_cast<void*>(null_task_),	nullptr,	// (8)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (9)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (10)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (11)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (12)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (13) キー入力
		reinterpret_cast<void*>(null_task_),	nullptr,	// (14) A/D 変換
		reinterpret_cast<void*>(null_task_),	nullptr,	// (15)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (16)
		reinterpret_cast<void*>(uart_.isend),	nullptr,	// (17) UART0 送信
		reinterpret_cast<void*>(uart_.irecv),	nullptr,	// (18) UART0 受信
		reinterpret_cast<void*>(null_task_),	nullptr,	// (19)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (20)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (21) /INT2
		reinterpret_cast<void*>(timer_j_.iinp), nullptr,	// (22) タイマＲＪ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (23) 周期タイマ

		reinterpret_cast<void*>(timer_b_.itask),nullptr,	// (24) タイマＲＢ２
		reinterpret_cast<void*>(null_task_),	nullptr,	// (25) /INT1
		reinterpret_cast<void*>(null_task_),	nullptr,	// (26) /INT3
		reinterpret_cast<void*>(null_task_),	nullptr,	// (27)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (28)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (29) /INT0
		reinterpret_cast<void*>(null_task_),	nullptr,	// (30)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (31)
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

	const uint8_t interval = 100;
	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(interval, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// TRJ のパルス周期測定
	timer_j::source srcclk = timer_j::source::f1;
	uint32_t master = F_CLK;
	{
		utils::PORT_MAP(utils::port_map::P17::TRJIO);
		device::PINSR.TRJIOSEL = 0;	// TRJIO を選択
		// uint8_t ir_level = 2;
		timer_j_.pluse_inp(timer_j::measurement::freq, srcclk);
	}

	sci_puts("Start R8C PLUSE input sample\n");

	uint8_t n = 0;
	while(1) {
		timer_b_.sync();
		++n;
		if(n >= interval) {
			uint16_t cnt;
			bool f = timer_j_.get_count(cnt);
			if(!f) {
				utils::format("Range error.\n");
			} else if(cnt == 0) {
				utils::format("Terminate error.\n");
			} else {
				uint32_t frq = master / static_cast<uint32_t>(cnt + 1);
				utils::format("Freq: %d Hz (%d, %d)\n") % frq %
					static_cast<uint32_t>(cnt) % static_cast<uint32_t>(master);
			}
			timer_j_.restart_inp(srcclk);
			n = 0;
		}
	}
}
