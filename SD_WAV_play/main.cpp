//=====================================================================//
/*!	@file
	@brief	R8C SD WAVE Player
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
#include "system.hpp"
#include "clock.hpp"
#include "common/intr_utils.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/uart_io.hpp"
#include "common/fifo.hpp"
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/trc_io.hpp"
#include "common/spi_io.hpp"
#include "pfatfs/mmc_io.hpp"

#include "wav_in.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef device::trc_io<utils::null_task> timer_c;
	timer_c timer_c_;

	struct wave_t {
		uint8_t	left;
		uint8_t	right;
	};

	wave_t wave_buff_[256];
	volatile uint8_t wave_put_ = 0;
	volatile uint8_t wave_get_ = 0;

	void clear_wave_() {
		for(uint16_t i = 0; i < 256; ++i) {
			wave_buff_[i].left  = 128;
			wave_buff_[i].right = 128;
		}
	}

	class wave_out {
		public:
		void operator() () {
			const volatile wave_t& t = wave_buff_[wave_get_];
			timer_c_.set_pwm_b(t.left);
			timer_c_.set_pwm_c(t.right);
			++wave_get_;
		}
	};

	typedef device::trb_io<wave_out, uint8_t> timer_audio;

	audio::wav_in wav_in_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	utils::command<64> command_;

	// P4_2(1):   SD_CLK(5)
	typedef device::PORT<device::PORT4, device::bitpos::B2> SPI_SCL;
	// P4_5(12):  SD_DI/CMD(2) (port output)
	typedef device::PORT<device::PORT4, device::bitpos::B5> SPI_SDO;
	// P4_7(4):   SD_DO/DAT0(7) (port input)
	typedef device::PORT<device::PORT4, device::bitpos::B7> SPI_SDI;
	typedef device::spi_io<SPI_SCL, SPI_SDO, SPI_SDI> SPI;
	SPI		spi_;

	// P3_4(10): SD_/CS(1)
	typedef device::PORT<device::PORT3, device::bitpos::B4> SD_SEL;

	pfatfs::mmc_io<SPI, SD_SEL> mmc_io_(spi_);
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

	DSTATUS disk_initialize() {
		return mmc_io_.disk_initialize();
	}

	DRESULT disk_readp(BYTE* buff, DWORD sector, UINT offset, UINT count) {
		return mmc_io_.disk_readp(buff, sector, offset, count);
	}

	DRESULT disk_writep(const BYTE* buff, DWORD sc) {
		return mmc_io_.disk_writep(buff, sc);
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
		reinterpret_cast<void*>(uart_.isend),	nullptr,	// (17) UART0 送信
		reinterpret_cast<void*>(uart_.irecv),	nullptr,	// (18) UART0 受信
		reinterpret_cast<void*>(null_task_),	nullptr,	// (19)

		reinterpret_cast<void*>(null_task_),	nullptr,	// (20)
		reinterpret_cast<void*>(null_task_),	nullptr,	// (21) /INT2
		reinterpret_cast<void*>(null_task_),	nullptr,	// (22) タイマＲＪ２
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

static FATFS fatfs_;

static void play_wav_()
{
//	uint32_t pos = 0;
	uint16_t n = 0;
//	while(pos < wav_in_.get_size()) {
	while(1) {
		uint8_t d;
		do {
			timer_b_.sync();
			d = wave_get_ - wave_put_;
		} while(d < 128) ;
		UINT br;
		if(pf_read(&wave_buff_[wave_put_], 128 * 2, &br) == FR_OK) {
			if(br == 0) break;
			wave_put_ += 128;
//			pos += 256;
//			++n;
//			if(n >= (11025 / 128)) {
//				n = 0;
//				sci_putch('.');
//			}
		} else {
			break;
		}
	}
	clear_wave_();
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

	clear_wave_();

	// ＰＷＭモード設定
	{
		// PWM cycle F_CLK(20MHz / 2 / 256 ---> 39.0625KHz
		utils::PORT_MAP(utils::port_map::P12::TRCIOB);
		utils::PORT_MAP(utils::port_map::P13::TRCIOC);
		bool pfl = 0;  // 0->1
		uint8_t ir_level = 2;
		timer_c_.start_pwm(255, timer_c::divide::f4, pfl, ir_level);
	}

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start_timer(11025, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t intr_level = 1;
		uart_.start(19200, intr_level);
	}

	// SPI、SD_SEL ポートの設定
	{
		utils::PORT_MAP(utils::port_map::P42::PORT);
		utils::PORT_MAP(utils::port_map::P45::PORT);
		utils::PORT_MAP(utils::port_map::P47::PORT);
		utils::PORT_MAP(utils::port_map::P34::PORT);
	}

	// SPI のスタート
	{
		spi_.start(10);
	}

	sci_puts("Start R8C SD WAVE Player\n");

	bool mount = false;
	// pfatfs を開始
	{
		if(pf_mount(&fatfs_) != FR_OK) {
			sci_puts("SD mount error\n");
		} else {
			sci_puts("SD mount OK!\n");
			mount = true;
		}
	}

	if(mount) {
		DIR dir;
		if(pf_opendir(&dir, "") != FR_OK) {
			sci_puts("Can't open dir\n");
		} else {

			for(;;) {
				FILINFO fno;
				// Read a directory item
				if(pf_readdir(&dir, &fno) != FR_OK) {
					sci_puts("Can't read dir\n");
					break;
				}
				if(!fno.fname[0]) break;

				if(fno.fattrib & AM_DIR) {
					utils::format("          /%s\n") % fno.fname;
				} else {
					utils::format("%8d  %s\n") % static_cast<uint32_t>(fno.fsize) % fno.fname;
				}
			}
		}

		const char* file_name = "OHAYODEL.WAV";
		if(pf_open(file_name) != FR_OK) {
			sci_puts("Can't open file: '");
			sci_puts(file_name);
			sci_puts("'\n");
		} else {
			if(!wav_in_.load_header()) {
				sci_puts("WAV file header error: '");
				sci_puts(file_name);
				sci_puts("'\n");		
			} else if(wav_in_.get_chanel() != 2) {
				utils::format("WAV chanel error(2): %d '%s'\n")
					% static_cast<uint32_t>(wav_in_.get_chanel()) % file_name;
			} else if(wav_in_.get_rate() != 11025) {
				utils::format("WAV sample rate error(11025): %d '%s'\n")
					% static_cast<uint32_t>(wav_in_.get_rate()) % file_name;
			} else if(wav_in_.get_bits() != 8) {
				utils::format("WAV sample bits error(8): %d '%s'\n")
					% static_cast<uint32_t>(wav_in_.get_bits()) % file_name;
			} else {
				sci_puts("Play WAVE: '");
				sci_puts(file_name);
				sci_puts("'\n");
				play_wav_();
			}
		}
	}

	sci_puts("End play.\n");
	while(1) ;
}
