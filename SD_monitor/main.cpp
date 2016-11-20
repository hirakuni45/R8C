//=====================================================================//
/*!	@file
	@brief	R8C SD モニター
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
#include "system.hpp"
#include "clock.hpp"
#include "common/delay.hpp"
#include "common/port_map.hpp"
#include "common/uart_io.hpp"
#include "common/fifo.hpp"
#include "common/command.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "pfatfs/mmc_io.hpp"


namespace {

	volatile uint8_t input_value_ = 0;

	class timer_task {
	public:
		void operator() () {
			input_value_ = device::P1();
		}
	};

	typedef device::trb_io<timer_task> timer_bt;
	timer_bt timer_b_;

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


#if 0
class file_system {

	FATFS fatfs_;
	bool mount_;
	char path_[128];

public:
	

};



static void mount_service_()
{
	return;

//		if(get_input_positive_() & 0x04) {
//			mount_();
//		}
	// pfatfs を開始
	if(pf_mount(&fatfs_) != FR_OK) {
		sci_puts("SD mount error\n");
		sd_mount_ = false;
	} else {
		sci_puts("SD mount OK!\n");
		sd_mount_ = true;
	}
}
#endif


static bool check_key_word_(uint8_t idx, const char* key)
{
	char buff[12];
	if(command_.get_word(idx, sizeof(buff), buff)) {
		if(strcmp(buff, key) == 0) {
			return true;
		}				
	}
	return false;
}


static bool ls_(uint8_t words)
{
	if(!check_key_word_(0, "ls")) {
		return false;
	}

//	if(sd_mount_) {
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

//	}
	return false;
}


static bool cd_(uint8_t words)
{
	if(!check_key_word_(0, "cd")) {
		return false;
	}



	return false;
}

#if 0
		const char* file_name = "OHAYODEL.WAV";
		if(pf_open(file_name) != FR_OK) {
			sci_puts("Can't open file: '");
			sci_puts(file_name);
			sci_puts("'\n");
		} else {
			for(;;) {
				UINT br;
				char buff[64];
				if(pf_read(buff, sizeof(buff), &br) == FR_OK) {
					if(!br) break;

					for(UINT i = 0; i < br; i+=2) {
						timer_c_.set_pwm_b(buff[i]);
						timer_c_.set_pwm_c(buff[i]);
					}
				} else {
					break;
				}
			}
		}
#endif

//  __attribute__ ((section (".exttext")))
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

	// ポートの設定
	{
		utils::PORT_MAP(utils::port_map::P13::PORT);
		PD1.B3 = 0;
	}

	// タイマーＢ初期化
	{
		uint8_t intr_level = 2;
		timer_b_.start_timer(60, intr_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
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

	sci_puts("Start R8C SD monitor\n");

	// メイン・ループ
	command_.set_prompt("# ");
	while(1) {
		timer_b_.sync();
//		mount_service_();

		// コマンド入力と、コマンド解析
		if(command_.service()) {
			uint8_t ws = command_.get_words();
			if(ws == 0) ;
			else if(ls_(ws)) ;
			else if(cd_(ws)) ;
			else {
				sci_puts("Command error: ");
				sci_puts(command_.get_command());
				sci_putch('\n');
			}
		}
	}
}
