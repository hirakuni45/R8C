//=====================================================================//
/*!	@file
	@brief	R8C BMP180/BMP280 サンプル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "system.hpp"
#include "clock.hpp"
#include "common/intr_utils.hpp"
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/command.hpp"
#include "common/format.hpp"
#include "chip/BMP180.hpp"
#include "chip/BMP280.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// I2C ポートの定義クラス
	// P4_B5: SDA
	typedef device::PORT<device::PORT4, device::bitpos::B5> sda_port;
	// P1_B7: SCL
	typedef device::PORT<device::PORT1, device::bitpos::B7> scl_port;

	typedef device::iica_io<sda_port, scl_port> iica;
	iica i2c_;
//	chip::BMP180<iica> bmpx_(i2c_);
	chip::BMP280<iica> bmpx_(i2c_);

	utils::command<64> command_;
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


//__attribute__ ((section (".exttext")))
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
		timer_b_.start_timer(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// I2C クラスの初期化
	{
		i2c_.start(iica::speed::fast);
	}

	// BMP180/BMP280 を開始
	{
		if(!bmpx_.start()) {
			utils::format("Stall BMP180/BMP280 start (%d)\n") % static_cast<uint32_t>(i2c_.get_last_error());
		}
	}

	sci_puts("Start R8C BMP180/BMP280 sample\n");
	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t n = 0;
	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		++n;
		if(n >= 60) {
			n = 0;

			auto t = bmpx_.get_temperature();
			utils::format("Temperature: %d.%02d C\n") % (t / 100) % (t % 100);

			auto p = bmpx_.get_pressure();
			utils::format("Pressure: %d.%02d hPa\n") % (p / 100) % (p % 100);

//			auto a = bmpx_.get_altitude();
//			utils::format("Altitude: %7.2f m\n") % a;
		}

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t cmdn = command_.get_words();
			if(cmdn >= 1) {
//				if(command_.cmp_word(0, "date")) {
//				} else if(command_.cmp_word(0, "help")) {
//					sci_puts("date\n");
//					sci_puts("date yyyy/mm/dd hh:mm[:ss]\n");
//				} else {
//					char buff[12];
//					if(command_.get_word(0, sizeof(buff), buff)) {
//						sci_puts("Command error: ");
//						sci_puts(buff);
//						sci_putch('\n');
//					}
//				}
			}
		}
	}
}
