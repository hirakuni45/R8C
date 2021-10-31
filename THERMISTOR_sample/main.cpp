//=====================================================================//
/*!	@file
	@brief	R8C サーミスタ・メイン (5V 動作) @n
			※ADC_sample を参考 @n
			サーミスター接続：P10_AN0 (20) @n
			サーミスター： NT103_41G, VCC 側 @n
			分圧抵抗：10K, GND 側 @n
			P10 -+-TH---VCC @n
			     |          @n
			     +-10K---GND
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/adc_io.hpp"
#include "common/trb_io.hpp"

// サーミスター、温度変換テンプレートクラス
#include "chip/NTCTH.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B	timer_b_;

	typedef utils::fifo<uint8_t, 16> TX_BUFF;  // 送信バッファ
	typedef utils::fifo<uint8_t, 16> RX_BUFF;  // 受信バッファ
	typedef device::uart_io<device::UART0, TX_BUFF, RX_BUFF> UART;
	UART	uart_;

	typedef device::adc_io<utils::null_task> ADC;
	ADC		adc_;

	// サーミスタ定義：
	// A/D: 10 bits (1023), NT103_41G, 分圧抵抗: 10K (10000) オーム、サーミスタ: ＶＣＣ側
	typedef chip::NTCTH<1023, chip::thermistor::NT103_41G, 10000, true> THMISTER;
	THMISTER thmister_;

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

	// インターバルタイマー開始（タイマーＢ）
	{
		uint8_t ir_level = 2;
		timer_b_.start(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
	}

	uart_.puts("Start R8C THERMISTOR sample\n");

	// ADC の設定
	{
		utils::PORT_MAP(utils::port_map::P10::AN0);
		adc_.start(ADC::CH_TYPE::CH0, ADC::CH_GROUP::AN0_AN1, true);
	}

	uint8_t cnt = 0;
	uint16_t nnn = 0;
	while(1) {
		timer_b_.sync();

		++cnt;
		if(cnt >= 30) {
			cnt = 0;
			adc_.scan();
			adc_.sync();
			{
				auto v = adc_.get_value(0);
				utils::format("(%5d) CH0: %3.2:8y[V], %d\n")
					% nnn
					% static_cast<uint32_t>(((v + 1) * 10) >> 3)
					% v;

				utils::format("温度： %5.2f [度]\n") % thmister_(v);
			}

			++nnn;
		}

		if(uart_.length() !=0) {  // UART のレシーブデータがあるか？
			auto ch = uart_.getch();
			uart_.putch(ch);
		}
	}
}
