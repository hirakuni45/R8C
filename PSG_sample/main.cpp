//=====================================================================//
/*!	@file
	@brief	R8C PSG サンプル・メイン @n
			PWM 出力 B：P12(TRCIOB) 18 pin
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/trb_io.hpp"
#include "common/trc_io.hpp"
#include "common/psg_mng.hpp"
#include "common/format.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> TIMER_B;
	TIMER_B timer_b_;

	typedef utils::fifo<uint8_t, 16> TX_BUFF;  // 送信バッファ
	typedef utils::fifo<uint8_t, 16> RX_BUFF;  // 受信バッファ
	typedef device::uart_io<device::UART0, TX_BUFF, RX_BUFF> UART;
	UART	uart_;

	static const uint16_t BSIZE = 512;
	static const uint16_t CNUM = 4;
	typedef utils::psg_base PSG;
	typedef utils::psg_mng<BSIZE, CNUM> PSG_MNG;
	PSG_MNG	psg_mng_;

	volatile uint16_t pwm_pos_;

	class pwm_task {
	public:
		void operator () ()
		{
			device::TRCGRB = psg_mng_.get_wav((pwm_pos_ + (BSIZE / 2)) & (BSIZE - 1));
			++pwm_pos_;
			pwm_pos_ &= BSIZE - 1;
		}
	};

	typedef device::trc_io<pwm_task> TIMER_C;
	TIMER_C	timer_c_;

	// テスト楽譜
	const PSG::SCORE score0_[] = {
		PSG::CTRL::VOLUME, 15,
		PSG::CTRL::SQ50,
		PSG::CTRL::TEMPO, 8,
		PSG::CTRL::FOR, 3,
		PSG::KEY::C_3, 16,
		PSG::KEY::D_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::F_3, 16,
		PSG::KEY::G_3, 16,
		PSG::KEY::A_4, 16,
		PSG::KEY::B_4, 16,
		PSG::KEY::C_4, 16,
		PSG::CTRL::BEFORE,
		PSG::CTRL::TRI,
		PSG::CTRL::TEMPO, 10,
		PSG::KEY::C_4, 16,
		PSG::KEY::B_4, 16,
		PSG::KEY::A_4, 16,
		PSG::KEY::G_3, 16,
		PSG::KEY::F_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::D_3, 16,
		PSG::KEY::C_3, 16,
		PSG::CTRL::END
	};

	const PSG::SCORE score1_[] = {
		PSG::CTRL::VOLUME, 15,
		PSG::CTRL::SQ50,
		PSG::CTRL::TEMPO, 8,
		PSG::CTRL::FOR, 3,
		PSG::KEY::Cs_3, 16,
		PSG::KEY::Ds_3, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Fs_3, 16,
		PSG::KEY::Gs_3, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Cs_4, 16,
		PSG::CTRL::BEFORE,
		PSG::CTRL::TRI,
		PSG::CTRL::TEMPO, 10,
		PSG::KEY::Cs_4, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Gs_3, 16,
		PSG::KEY::Fs_3, 16,
		PSG::KEY::Q, 16,
		PSG::KEY::Ds_3, 16,
		PSG::KEY::Cs_3, 16,
		PSG::CTRL::END
	};
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
	utils::delay::micro_second(1);  // >=30us(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start(100, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// タイマーＣ初期化（ＰＷＭ）
	{
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		bool pfl = 0;  // 0->1
		uint8_t ir_lvl = 2;
		timer_c_.start(256, TIMER_C::DIVIDE::F4, pfl, ir_lvl);
	}

	sci_puts("Start R8C PSG sample\n");

//	psg_mng_.set_volume(0, 15);
//	psg_mng_.set_volume(1, 0);
//	psg_mng_.set_volume(2, 0);
//	psg_mng_.set_volume(3, 0);

//	psg_mng_.set_wtype(0, PSG_MNG::WTYPE::SQ50);
// 	psg_mng_.set_wtype(0, PSG_MNG::WTYPE::TRI);
//	psg_mng_.set_freq(0, 880);
//	psg_mng_.set_key(0, PSG_MNG::KEY::A_3);

	psg_mng_.set_score(0, score0_);
	psg_mng_.set_score(1, score1_);

	auto pos = pwm_pos_;
	uint8_t delay = 100;
	while(1) {
		timer_b_.sync();

		auto org = pwm_pos_;
		auto n = org - pos;
		psg_mng_.render(n & (BSIZE - 1));
		pos = org;

		if(delay > 0) {
			delay--;
		} else {
			psg_mng_.service();
		}
//		cnt++;
//		if(cnt >= 50) {
//			cnt = 0;
//			utils::format("Num: %d\n") % n;
//		}
	}
}
