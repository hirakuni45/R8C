//=====================================================================//
/*!	@file
	@brief	R8C グループ・ベクター関係
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <stdlib.h>
#include "vect.h"

extern void reset_start(void);

void undef_inst(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  未定義命令エントリー（割り込み）
*/
//-----------------------------------------------------------------//
void undef_inst(void) { }


void brk_inst(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  BRK 命令エントリー（割り込み）
*/
//-----------------------------------------------------------------//
void brk_inst(void) { }


void null_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  無効タスクエントリー（割り込み）
*/
//-----------------------------------------------------------------//
void null_intr(void) { }


void FLASH_READY_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  フラッシュ・レディ割り込み
*/
//-----------------------------------------------------------------//
void FLASH_READY_intr(void) { }


void TIMER_RB_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  タイマーＲＢ割り込み
*/
//-----------------------------------------------------------------//
void TIMER_RB_intr(void) { }


void TIMER_RC_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  タイマーＲＣ割り込み
*/
//-----------------------------------------------------------------//
void TIMER_RC_intr(void) { }


void TIMER_RJ_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  タイマーＲＪ割り込み
*/
//-----------------------------------------------------------------//
void TIMER_RJ_intr(void) { }


void ADC_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  ADC 変換終了割り込み
*/
//-----------------------------------------------------------------//
void ADC_intr(void) { }


void UART0_TX_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  UART 送信割り込み
*/
//-----------------------------------------------------------------//
void UART0_TX_intr(void) { }


void INT0_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  INT0 割り込み
*/
//-----------------------------------------------------------------//
void INT0_intr(void) { }


void INT1_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  INT1 割り込み
*/
//-----------------------------------------------------------------//
void INT1_intr(void) { }


void INT2_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  INT2 割り込み
*/
//-----------------------------------------------------------------//
void INT2_intr(void) { }


void INT3_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  INT3 割り込み
*/
//-----------------------------------------------------------------//
void INT3_intr(void) { }


void UART0_RX_intr(void) __attribute__((weak));
//-----------------------------------------------------------------//
/*!
	@brief  UART 受信割り込み
*/
//-----------------------------------------------------------------//
void UART0_RX_intr(void) { }


//-----------------------------------------------------------------//
/*!
	@brief  割り込みテーブル
*/
//-----------------------------------------------------------------//
const void* variable_vectors_[] __attribute__ ((section (".vvec"))) = {
	brk_inst,        NULL,	// (0)
	FLASH_READY_intr,NULL,	// (1) flash_ready
	null_intr,       NULL,	// (2)
	null_intr,       NULL,	// (3)

	null_intr,       NULL,	// (4) コンパレーターB1
	null_intr,       NULL,	// (5) コンパレーターB3
	null_intr,       NULL,	// (6)
	TIMER_RC_intr,   NULL,	// (7) タイマＲＣ

	null_intr,       NULL,	// (8)
	null_intr,       NULL,	// (9)
	null_intr,       NULL,	// (10)
	null_intr,       NULL,	// (11)

	null_intr,       NULL,	// (12)
	null_intr,       NULL,	// (13) キー入力
	ADC_intr,        NULL,	// (14) A/D 変換
	null_intr,       NULL,	// (15)

	null_intr,       NULL,	// (16)
	UART0_TX_intr,   NULL,	// (17) UART0 送信
	UART0_RX_intr,   NULL,	// (18) UART0 受信
	null_intr,       NULL,	// (19)

	null_intr,       NULL,	// (20)
	INT2_intr,       NULL,	// (21) /INT2
	TIMER_RJ_intr,   NULL,	// (22) タイマＲＪ２
	null_intr,       NULL,	// (23) 周期タイマ（ウオッチドッグ）

	TIMER_RB_intr,   NULL,	// (24) タイマＲＢ２
	INT1_intr,       NULL,	// (25) /INT1
	INT3_intr,       NULL,	// (26) /INT3
	null_intr,       NULL,	// (27)

	null_intr,       NULL,	// (28)
	INT0_intr,       NULL,	// (29) /INT0
	null_intr,       NULL,	// (30)
	null_intr,       NULL,	// (31)
};

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
    undef_inst, (const void*)(ID1 << 8),
// 0xFFE0  オーバーフロー (with ID2)
	null_intr,  (const void*)(ID2 << 8),
// 0xFFE4  BRK 命令
	null_intr,  (const void*)0xff00,
// 0xFFE8  アドレス一致 (with ID3)
	null_intr,  (const void*)(ID3 << 8),
// 0xFFEC  シングルステップ (with ID4)
	null_intr,  (const void*)(ID4 << 8),
// 0xFFF0  ウオッチドッグタイマ、発振停止検出、電圧監視１ (with ID5)  
	null_intr,  (const void*)(ID5 << 8),
// 0xFFF4  予約 (with ID6)
	(const void*)0xffff,  (const void*)((ID6 << 8) | 0xff),
// 0xFFF8  予約 (with ID7)
	(const void*)0xffff,  (const void*)((ID7 << 8) | 0xff),
// 0xFFFC  リセット (with OFS)
	reset_start, (const void*)(OFS << 8),
};


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

