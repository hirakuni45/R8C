//=====================================================================//
/*! @file
    @brief  R8C 起動前初期化
    @author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdlib.h>
#include "common/init.h"

int main(int argc, char**argv);

extern short _datainternal;
extern short _datastart;
extern short _dataend;

extern short _bssstart;
extern short _bssend;

extern short _preinit_array_start;
extern short _preinit_array_end;
extern short _init_array_start;
extern short _init_array_end;
extern short _fini_array_start;
extern short _fini_array_end;

//-----------------------------------------------------------------//
/*!
	@brief  メイン関数起動前初期化
*/
//-----------------------------------------------------------------//
void _init(void)
{
	{  // R/W-data セクションのコピー
		short *src = &_datainternal;
		short *dst = &_datastart;
		while(dst < &_dataend) {
			*dst++ = *src++;
		}
	}

	{  // bss セクションのクリア
		short *dst = &_bssstart;
		while(dst < &_bssend) {
			*dst++ = 0;
		}
	}

	{  // C++ 静的コンストラクターの実行
		short *p = &_fini_array_start;
		while(p < &_fini_array_end) {
			void (*prog)(void) = (void *)*p++;
			(*prog)();
		}
	}

	{  // C++ 事前静的コンストラクターの実行
		short *p = &_preinit_array_start;
		while(p < &_preinit_array_end) {
			void (*prog)(void) = (void *)*p++;
			(*prog)();
		}
	}

	{  // C++ 静的コンストラクターの実行
		short *p = &_init_array_start;
		while(p < &_init_array_end) {
			void (*prog)(void) = (void *)*p++;
			(*prog)();
		}
	}

	// main の起動
	static int argc = 0;
	static char **argv = 0;
	int ret = main(argc, argv);
	exit(0);
}

// EOF
