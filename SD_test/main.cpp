//=====================================================================//
/*!	@file
	@brief	R8C SD メイン @n
			for ST7567 SPI (128 x 32) @n
			LCD: Aitendo M-G0812P7567
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

// ポートの配置
// P4_2(1):   LCD_SCK  ,SD_CLK(5)
// P3_7(2):   LCD_/CS
// /RES(3):  (System reset)
// P4_7(4):            ,SD_DO/DAT0(7)
// VSS:(5)   (Power GND)
// P4_6(6):   XIN (高精度なクロック用)
// VCC(7):   (Power +V)
// MODE(8):  (System mode)
// P3_5(9):   I2C_SDA
// P3_4(10):           ,SD_/CS(1)
// P1_0(20):  AN0 (keep)
// P1_1(19):  AN1 (keep)
// P1_2(18):  AN2 (keep)
// P1_3(17):  AN3 (keep)
// P1_4(16):  TXD0 (keep)
// P1_5(15):  RXD0 (keep)
// P1_6(14):  LCD_A0 (share)
// P1_7(13):  TRJIO (keep)
// P4_5(12):  LCD_SDA  ,SD_DI/CMD(2)
// P3_3(11):  I2C_SCL

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


// __attribute__ ((section (".exttext")))
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

	sci_puts("Start R8C SD sample\n");

	FATFS fatfs;
	bool mount = false;
	// pfatfs を開始
	{
		if(pf_mount(&fatfs) != FR_OK) {
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

#if 0
		// 「LICENSE」ファイルを読み込んで表示
		const char* file_name = "LICENSE";
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

					for(UINT i = 0; i < br; ++i) {
						sci_putch(buff[i]);
					}
				} else {
					break;
				}
			}
		}
#endif
	}

	command_.set_prompt("# ");

	// LED シグナル用ポートを出力
	PD1.B0 = 1;

	uint8_t cnt = 0;
	while(1) {
		timer_b_.sync();

		if(cnt >= 20) {
			cnt = 0;
		}
		if(cnt < 10) P1.B0 = 1;
		else P1.B0 = 0;
		++cnt;

		// コマンド入力と、コマンド解析
		if(command_.service()) {
//			uint8_t cmdn = command_.get_words();
		}
	}
}
