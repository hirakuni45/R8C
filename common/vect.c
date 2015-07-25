//=====================================================================//
/*!	@file
	@brief	R8C グループ・ベクター関係 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "vect.h"

extern void reset_start(void);

//-----------------------------------------------------------------//
/*!
	@brief  未定義命令エントリー（割り込み）
*/
//-----------------------------------------------------------------//
INTERRUPT_FUNC void undef_inst_(void)
{
}


//-----------------------------------------------------------------//
/*!
	@brief  BRK 命令エントリー（割り込み）
*/
//-----------------------------------------------------------------//
INTERRUPT_FUNC void brk_inst_(void)
{
}


//-----------------------------------------------------------------//
/*!
	@brief  無効タスクエントリー（割り込み）
*/
//-----------------------------------------------------------------//
INTERRUPT_FUNC void null_task_(void)
{
}


//-----------------------------------------------------------------//
/*!
	@brief  割り込み無効
*/
//-----------------------------------------------------------------//
void di(void)
{
	asm("fclr i");
	asm("nop");
	asm("nop");
}


//-----------------------------------------------------------------//
/*!
	@brief  割り込み有効
*/
//-----------------------------------------------------------------//
void ei(void)
{
	asm("fset i");
	asm("nop");
	asm("nop");
}


// 以下のハードウェアー要因ベクターテーブルは、main.cpp に配置
#if 0
const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
	brk_inst_,  (const void*)0x0000,	// (0)
	null_task_,  (const void*)0x0000,	// (1) flash_ready
	null_task_,  (const void*)0x0000,	// (2)
	null_task_,  (const void*)0x0000,	// (3)

	null_task_,  (const void*)0x0000,	// (4) コンパレーターB1
	null_task_,  (const void*)0x0000,	// (5) コンパレーターB3
	null_task_,  (const void*)0x0000,	// (6)
	null_task_,  (const void*)0x0000,	// (7) タイマＲＣ

	null_task_,  (const void*)0x0000,	// (8)
	null_task_,  (const void*)0x0000,	// (9)
	null_task_,  (const void*)0x0000,	// (10)
	null_task_,  (const void*)0x0000,	// (11)

	null_task_,  (const void*)0x0000,	// (12)
	null_task_,  (const void*)0x0000,	// (13) キー入力
	null_task_,  (const void*)0x0000,	// (14) A/D 変換
	null_task_,  (const void*)0x0000,	// (15)

	null_task_,  (const void*)0x0000,	// (16)
	null_task_,  (const void*)0x0000,	// (17) UART0 送信
	null_task_,  (const void*)0x0000,	// (18) UART0 受信
	null_task_,  (const void*)0x0000,	// (19)

	null_task_,  (const void*)0x0000,	// (20)
	null_task_,  (const void*)0x0000,	// (21) /INT2
	null_task_,  (const void*)0x0000,	// (22) タイマＲＪ２
	null_task_,  (const void*)0x0000,	// (23) 周期タイマ

	null_task_,  (const void*)0x0000,	// (24) タイマＲＢ２
	null_task_,  (const void*)0x0000,	// (25) /INT1
	null_task_,  (const void*)0x0000,	// (26) /INT3
	null_task_,  (const void*)0x0000,	// (27)

	null_task_,  (const void*)0x0000,	// (28)
	null_task_,  (const void*)0x0000,	// (29) /INT0
	null_task_,  (const void*)0x0000,	// (30)
	null_task_,  (const void*)0x0000,	// (31)
};
#endif

/// OFS の定義
#define OFS		0xff
/// OFS2 の定義
#define OFS2	0xff

/// ID の定義
#define ID1		0xff
#define ID2		0xff
#define ID3		0xff
#define ID4		0xff
#define ID5		0xff
#define ID6		0xff
#define ID7		0xff

// R8C M110AN, M120AN の場合、メモリーマップは６４キロバイト以内の為、
// ポインターは２バイト、ベクターテーブルは４バイト単位のアドレスを想定
const void* fixed_vectors_[] __attribute__ ((section (".fvec"))) = {
// 0xFFD8  予約領域 (with OFS2)
	(const void*)0xffff, (const void*)((OFS2 << 8) | 0xff),
// 0xFFDC  未定義命令 (with ID1)
    undef_inst_, (const void*)(ID1 << 8),
// 0xFFE0  オーバーフロー (with ID2)
	null_task_,  (const void*)(ID2 << 8),
// 0xFFE4  BRK 命令
	null_task_,  (const void*)0xff00,
// 0xFFE8  アドレス一致 (with ID3)
	null_task_,  (const void*)(ID3 << 8),
// 0xFFEC  シングルステップ (with ID4)
	null_task_,  (const void*)(ID4 << 8),
// 0xFFF0  ウオッチドッグタイマ、発振停止検出、電圧監視１ (with ID5)  
	null_task_,  (const void*)(ID5 << 8),
// 0xFFF4  予約 (with ID6)
	(const void*)0xffff,  (const void*)((ID6 << 8) | 0xff),
// 0xFFF8  予約 (with ID7)
	(const void*)0xffff,  (const void*)((ID7 << 8) | 0xff),
// 0xFFFC  リセット (with OFS)
	reset_start, (const void*)(OFS << 8),
};

// EOF
