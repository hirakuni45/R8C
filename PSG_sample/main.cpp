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

	static constexpr uint16_t TRC_LIM = 256;  // サンプリング分解能（２５６固定）
	// サンプリング周期を変更すると、アタックやリリースを変更する必要がある。
// #define LOW_PROFILE
#ifdef LOW_PROFILE
	static constexpr auto TRC_DIV = device::trc_base::DIVIDE::F8;  // TRC の分周器パラメーター（１／８）
	static constexpr uint16_t SAMPLE = F_CLK / 8 / TRC_LIM;  // サンプリング周期（F_CLK は CPU の動作周波数）
	static constexpr uint16_t BSIZE = 256;  // SAMPLE / TICK * 2 以上で、２のｎ乗倍になる値
#else
	// サンプリングを倍にした、より高音質な設定
	static constexpr auto TRC_DIV = device::trc_base::DIVIDE::F4;  // TRC の分周器パラメーター（１／４）
	static constexpr uint16_t SAMPLE = F_CLK / 4 / TRC_LIM;  // サンプリング周期（F_CLK は CPU の動作周波数）
	static constexpr uint16_t BSIZE = 512;  // SAMPLE / TICK * 2 以上で、２のｎ乗倍になる値
#endif
	static constexpr uint16_t TICK = 100;	// サンプルの楽曲では、１００を前提にしている。
	static constexpr uint16_t CNUM = 3;		// 同時発音数（大きくすると処理負荷が増えるので注意）
	typedef utils::psg_base PSG;
	typedef utils::psg_mng<SAMPLE, TICK, BSIZE, CNUM> PSG_MNG;
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

	// ドラゴンクエスト１・ラダトーム城（Dragon Quest 1 Chateau Ladutorm）
	constexpr PSG::SCORE score0_[] = {
		PSG::CTRL::VOLUME, 128,
		PSG::CTRL::SQ50,
		PSG::CTRL::TEMPO, 80,
		PSG::CTRL::ATTACK, 175,
		// 1
		PSG::KEY::Q,   8,
		PSG::KEY::E_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		// 2
		PSG::KEY::A_4, 8*3,
		PSG::KEY::Q,   8*5,
		// 3
		PSG::KEY::Q,   8,
		PSG::KEY::F_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::F_5, 8,
		// 4
		PSG::KEY::B_4, 8*3,
		PSG::KEY::Q,   8*5,
		// 5
		PSG::KEY::Q,   8,
		PSG::KEY::G_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::G_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::G_5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::G_5, 8,
		// 6
		PSG::KEY::F_5, 16,
		PSG::KEY::G_5, 16,
		PSG::KEY::A_5, 16,
		PSG::KEY::G_5, 8,
		PSG::KEY::F_5, 8,
		// 7
		PSG::KEY::E_5, 16,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::D_5, 16,
		PSG::KEY::Eb5, 16,
		// 8
		PSG::KEY::E_5, 8*8,
		// 9
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::E_5, 4,
		PSG::KEY::Eb5, 4,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 4,
		PSG::KEY::B_4, 4,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 10
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::E_5, 4,
		PSG::KEY::Eb5, 4,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 4,
		PSG::KEY::B_4, 4,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 11
		PSG::KEY::B_4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 12
		PSG::KEY::E_5, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::E_5, 8,
		// 13
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::F_5, 4,
		PSG::KEY::E_5, 4,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 4,
		PSG::KEY::Cs5, 4,
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		// 14
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::F_5, 4,
		PSG::KEY::E_5, 4,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 4,
		PSG::KEY::Cs5, 4,
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		// 15
		PSG::KEY::B_4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::F_5, 8,
		// 16
		PSG::KEY::E_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::B_3, 8,
		// 17
		PSG::KEY::Q,   8,
		PSG::KEY::E_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		// 18
		PSG::KEY::A_4, 8*3,
		PSG::KEY::Q,   8*5,
		// 19
		PSG::KEY::Q,   8,
		PSG::KEY::F_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::F_5, 8,
		// 20
		PSG::KEY::B_4, 8*3,
		PSG::KEY::Q,   8*5,
		// 21
		PSG::KEY::Q,   8,
		PSG::KEY::G_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::G_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::G_5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::G_5, 8,
		// 22
		PSG::KEY::F_5, 16,
		PSG::KEY::G_5, 16,
		PSG::KEY::A_5, 16,
		PSG::KEY::G_5, 8,
		PSG::KEY::F_5, 8,
		// 23
		PSG::KEY::E_5, 16,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::D_5, 16,
		PSG::KEY::Eb5, 16,
		// 24
		PSG::KEY::E_5, 8*8,
		// 25
		PSG::KEY::Q   ,8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::E_5, 4,
		PSG::KEY::Eb5, 4,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 4,
		PSG::KEY::B_4, 4,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 26
		PSG::KEY::Q   ,8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::E_5, 4,
		PSG::KEY::Eb5, 4,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 4,
		PSG::KEY::B_4, 4,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 27
		PSG::KEY::B_4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::A_4, 8,
		// 28
		PSG::KEY::E_5, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		// 29
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::F_5, 4,
		PSG::KEY::E_5, 4,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 4,
		PSG::KEY::Cs5, 4,
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		// 30
		PSG::KEY::Q,   8,
		PSG::KEY::A_5, 4,
		PSG::KEY::Gs5, 4,
		PSG::KEY::A_5, 8,
		PSG::KEY::F_5, 4,
		PSG::KEY::E_5, 4,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 4,
		PSG::KEY::Cs5, 4,
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		// 31
		PSG::KEY::B_4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::F_5, 8,
		// 32
		PSG::KEY::E_5, 8,
		PSG::KEY::A_4, 8,  // F_4, 8
		PSG::KEY::Gs4, 8,  // E_4, 8
		PSG::KEY::F_4, 8,  // D_4, 8
		PSG::KEY::E_4, 8,  // C_4, 8
		PSG::KEY::D_4, 8,  // B_3, 8
		PSG::KEY::C_4, 8,  // A_3, 8
		PSG::KEY::B_3, 8,  // G_3, 8
		// 33
		PSG::KEY::Q,   8,
		PSG::KEY::E_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		// 34
		PSG::KEY::A_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::Cs5, 8,
		// 35
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::E_5, 8,  // Cs5, 8 
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::A_5, 8,  // C_5, 8
		PSG::KEY::F_5, 8,
		// 36
		PSG::KEY::Gs5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::A_5, 8,
		PSG::KEY::B_5, 8,
		PSG::KEY::Gs5, 8,
		// 37
		PSG::KEY::Bb5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Cs5, 8,
		// 38
		PSG::KEY::Gs5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::D_5, 8,
		// 39
		PSG::KEY::Fs5, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::Bb4, 8,
		// 40
		PSG::KEY::E_5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		// 41
		PSG::KEY::B_4, 16,
		PSG::KEY::Bb4, 16,
		PSG::KEY::B_4, 16,
		PSG::KEY::Fs5, 16,  // PSG::KEY::Fs4, 16,
		// 42
		PSG::KEY::Eb4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Fs4, 8,
		// 43
		PSG::KEY::Q,   8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::Fs4, 8,
		// 44
		PSG::KEY::E_4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Gs4, 8,
		// 45
		PSG::KEY::Cs5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_4, 8,
		// 46
		PSG::KEY::Eb5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::A_4, 8,
		// 47
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		// 48
		PSG::KEY::E_5, 8,
		PSG::KEY::E_6, 8,  // PSG::KEY::Gs5, 8,
		PSG::KEY::D_6, 8,  // PSG::KEY::F_5, 8,
		PSG::KEY::C_6, 8,  // PSG::KEY::E_5, 8,
		PSG::KEY::B_5, 8,  // PSG::KEY::D_5, 8,
		PSG::KEY::A_5, 8,  // PSG::KEY::C_5, 8,
		PSG::KEY::Gs5, 8,  // PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,  // PSG::KEY::Gs4, 8,
		// 49
		PSG::KEY::Cs5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::G_4, 8,
		PSG::KEY::Bb4, 8,
		// 50
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_4, 8,
		// 51
		PSG::KEY::Eb5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::A_4, 8,
		// 52
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::B_4, 8,
		// 53
		PSG::KEY::F_5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::F_5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::Cs5, 8,
		// 54
		PSG::KEY::Fs5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Cs5, 8,
		// 55
		PSG::KEY::G_5, 8,  // KEY::Bb4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::G_5, 8,  // KEY::Bb4, 8,
		PSG::KEY::Cs5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Cs5, 8,
		// 56
		PSG::KEY::Gs5, 8,  // KEY::C_5, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::Gs3, 8,
		// 57
		PSG::KEY::Q,   8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::Gs5, 8,
		// 58
		PSG::KEY::Cs5, 8,
		PSG::KEY::Bb5, 8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::Bb5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Bb5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::Bb5, 8,
		// 59
		PSG::KEY::Eb5, 8,
		PSG::KEY::B_5, 8,
		PSG::KEY::A_5, 8,
		PSG::KEY::B_5, 8,
		PSG::KEY::Gb5, 8,
		PSG::KEY::B_5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::B_5, 8,
		// 60
		PSG::KEY::E_5, 8,
		PSG::KEY::Eb5, 8,
		PSG::KEY::E_5, 8,
		PSG::KEY::Fs5, 8,
		PSG::KEY::Gs5, 8,
		PSG::KEY::A_5, 8,
		PSG::KEY::B_5, 8,
		PSG::KEY::Gs5, 8,






		PSG::CTRL::END
	};

	constexpr PSG::SCORE score1_[] = {
		PSG::CTRL::VOLUME, 128,
		PSG::CTRL::SQ50,
		PSG::CTRL::TEMPO, 80,
		PSG::CTRL::ATTACK, 175,
		// 1
		PSG::KEY::A_2, 8,
		PSG::KEY::Q,   8*7,
		// 2
		PSG::KEY::Q,   8,
		PSG::KEY::A_2, 8,
		PSG::KEY::C_3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::F_3, 8,
		PSG::KEY::E_3, 8,
		// 3
		PSG::KEY::D_3, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::D_4, 8,
		// 4
		PSG::KEY::Gs3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::Gs3, 8,
		// 5
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		// 6
		PSG::KEY::D_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		// 7
		PSG::KEY::C_4, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::Fs4, 8,
		// 8
		PSG::KEY::Gs4, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::B_3, 8,
		// 9
		PSG::KEY::A_3, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::Eb4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::E_4, 8,
		PSG::KEY::C_4, 8,
		// 10
		PSG::KEY::G_3, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::E_4, 8, // PSG::KEY::Fs3, 8,
		PSG::KEY::Eb4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::E_4, 8,
		PSG::KEY::C_4, 8,
		// 11
		PSG::KEY::F_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::D_3, 16,
		PSG::KEY::Eb3, 16,
		// 12
		PSG::KEY::E_3, 16*2,
		PSG::KEY::A_3, 16*2,
		// 13
		PSG::KEY::D_3, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 4,
		PSG::KEY::Cs4, 4,
		PSG::KEY::Ds4, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 8,
		// 14
		PSG::KEY::C_4, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 4,
		PSG::KEY::Cs4, 4,
		PSG::KEY::D_4, 8,  // B_3, 8
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 8,
		// 15
		PSG::KEY::F_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::D_3, 32,
		// 16
		PSG::KEY::E_3, 16,
		PSG::KEY::Q,   8*6,
		// 17
		PSG::KEY::A_2, 16,
		PSG::KEY::Q,   8*6,
		// 18
		PSG::KEY::Q,   8,
		PSG::KEY::A_2, 8,
		PSG::KEY::C_3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::F_3, 8,
		PSG::KEY::E_3, 8,
		// 19
		PSG::KEY::D_3, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::D_4, 8,
		// 20
		PSG::KEY::Gs3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::Gs3, 8,
		// 21
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		// 22
		PSG::KEY::D_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		// 23
		PSG::KEY::C_4, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::G_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::Fs4, 8,
		// 24
		PSG::KEY::Gs4, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::E_4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::C_4, 8,
		PSG::KEY::B_3, 8,
		// 25
		PSG::KEY::A_3, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::Eb4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::E_4, 8,
		PSG::KEY::C_3, 8,
		// 26
		PSG::KEY::A_3, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,
		PSG::KEY::C_4, 4,
		PSG::KEY::B_3, 4,
		PSG::KEY::C_4, 8,  // Fs3, 8
		PSG::KEY::Eb4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::E_4, 8,
		PSG::KEY::C_3, 8,
		// 27
		PSG::KEY::F_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::D_3, 16,
		PSG::KEY::Eb3, 16,
		// 28
		PSG::KEY::E_3, 32,
		PSG::KEY::A_3, 32,
		// 29
		PSG::KEY::D_3, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 4,
		PSG::KEY::Cs4, 4,
		PSG::KEY::D_4, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 8,
		// 30
		PSG::KEY::C_4, 8,
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 4,
		PSG::KEY::Cs4, 4,
		PSG::KEY::D_4, 8,  // B_3, 8
		PSG::KEY::F_4, 4,
		PSG::KEY::E_4, 4,
		PSG::KEY::F_4, 8,
		PSG::KEY::D_4, 8,
		// 31
		PSG::KEY::F_3, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::D_3, 32,
		// 32
		PSG::KEY::E_3, 16,
		PSG::KEY::Q,   16*3,
		// 33
		PSG::KEY::A_3, 16,
		PSG::KEY::Q,   16*3,
		// 34
		PSG::KEY::Q,   8*8,
		// 35
		PSG::KEY::Q,   8*8,
		// 36
		PSG::KEY::B_4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::Gs4, 8,
		PSG::KEY::A_4, 8,
		PSG::KEY::B_4, 8,
		PSG::KEY::C_5, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::B_4, 8,
		// 37
		PSG::KEY::Cs5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		// 38
		PSG::KEY::F_4, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::F_4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::D_5, 8,
		PSG::KEY::Bb4, 8,
		// 39
		PSG::KEY::Eb4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Eb4, 8,
		PSG::KEY::Bb4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb4, 8,
		// 40
		PSG::KEY::Cs4, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb3, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::Bb3, 8,
		PSG::KEY::Fs4, 8,
		PSG::KEY::Bb3, 8,
		// 41
		PSG::KEY::B_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::Eb3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::Cs3, 8,
		PSG::KEY::Fs3, 8,
		// 42
		PSG::KEY::B_2, 8,
		PSG::KEY::Bb2, 8,
		PSG::KEY::B_2, 8,
		PSG::KEY::C_3, 8,
		PSG::KEY::Eb3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::E_3, 8,
		// 43
		PSG::KEY::E_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::Gs3, 8,  // PSG::KEY::E_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::Fs3, 8,  // PSG::KEY::B_2, 8,
		PSG::KEY::B_3, 8,
		// 44
		PSG::KEY::Gs3, 8,  // PSG::KEY::E_3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::A_3, 8,
		PSG::KEY::B_3, 8,
		PSG::KEY::Cs4, 8,
		PSG::KEY::D_4, 8,
		PSG::KEY::B_3, 8,
		// 45
		PSG::KEY::A_3, 16,
		PSG::KEY::A_2, 16,
		PSG::KEY::A_3, 16,
		PSG::KEY::A_2, 16,
		// 46
		PSG::KEY::Fs3, 16,
		PSG::KEY::Fs2, 16,
		PSG::KEY::B_2, 16,
		PSG::KEY::B_2, 16,
		// 47
		PSG::KEY::E_3, 16,
		PSG::KEY::E_2, 16,
		PSG::KEY::E_3, 16,
		PSG::KEY::E_2, 16,
		// 48
		PSG::KEY::E_3, 16,
		PSG::KEY::Q,   16*3,
		// 49
		PSG::KEY::E_4, 16,
		PSG::KEY::E_4, 16,
		PSG::KEY::E_4, 16,
		PSG::KEY::E_4, 16,
		// 50
		PSG::KEY::D_4, 16,
		PSG::KEY::D_4, 16,
		PSG::KEY::D_4, 16,
		PSG::KEY::D_4, 16,
		// 51
		PSG::KEY::C_4, 16,
		PSG::KEY::C_4, 16,
		PSG::KEY::C_4, 16,
		PSG::KEY::C_4, 16,
		// 52
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		// 53
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		PSG::KEY::B_3, 16,
		// 54
		PSG::KEY::A_3, 16,
		PSG::KEY::A_3, 16,
		PSG::KEY::A_3, 16,
		PSG::KEY::A_3, 16,
		// 55
		PSG::KEY::Eb3, 16,
		PSG::KEY::Eb3, 16,  //  PSG::KEY::Eb2, 16, 
		PSG::KEY::Eb3, 16,
		PSG::KEY::Eb3, 16,  //  PSG::KEY::Eb2, 16, 
		// 56
		PSG::KEY::Gs2, 8,
		PSG::KEY::Gs3, 8,
		PSG::KEY::Fs3, 8,
		PSG::KEY::E_3, 8,
		PSG::KEY::Eb3, 8,
		PSG::KEY::Cs3, 8,
		PSG::KEY::C_3, 8,
		PSG::KEY::Gs2, 8,
		// 57
		PSG::KEY::Cs3, 16,
		PSG::KEY::Q,   16*3,
		// 58
		PSG::KEY::Q,   16*4,
		// 59
		PSG::KEY::Q,   16*4,
		// 60
		PSG::KEY::Q,   16*4,



		PSG::CTRL::END
	};

	constexpr PSG::SCORE score_test_[] = {
		PSG::CTRL::VOLUME, 128,
		PSG::CTRL::TRI,
		PSG::CTRL::TEMPO,1,
		PSG::CTRL::FOR,100,
		PSG::KEY::A_4, 128,
		PSG::CTRL::BEFORE,
		PSG::CTRL::REPEAT
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
		timer_b_.start(TICK, ir_level);
	}

	// UART の設定 (P1_4: TXD0[in], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t ir_level = 1;
		uart_.start(57600, ir_level);
	}

	// タイマーＣ初期化ＰＳＧ用
	{
		// SAMPLE 周期
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		uint8_t ir_lvl = 2;
		timer_c_.start_psg(TRC_LIM, TRC_DIV, ir_lvl);
	}

	sci_puts("Start R8C PSG sample\n");

	psg_mng_.set_score(0, score0_);
	psg_mng_.set_score(1, score1_);
//	psg_mng_.set_score(0, score_test_);

	auto pos = pwm_pos_;
	uint8_t delay = 100;
	uint16_t bar = 0;
	bool pause = false;
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

		if(sci_length() > 0) {
			auto ch = sci_getch();
			if(ch == ' ') {
				pause = !pause;
				psg_mng_.pause(pause);
			}
		}

		{  // 楽譜のデバッグ用（小節番号を表示）
			auto tmp = (psg_mng_.get_total_count(0) / 64);
			if(bar != tmp) {
				bar = tmp;
//				utils::format("%d\n") % bar;
			}
		}
	}
}
