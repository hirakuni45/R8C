#include <stdlib.h>

#define INTERRUPT_FUNC __attribute__ ((interrupt))


extern void reset_start(void);


// 未定義命令
INTERRUPT_FUNC void undef_inst_(void)
{
}


// null interrupt TASK
INTERRUPT_FUNC void null_task_(void)
{
}


// R8C M110AN, M120AN の場合、ポインターは２バイト、ベクターテーブルは４バイト単位のアドレスを想定
const void* fixed_vectors_[] __attribute__ ((section (".vec"))) = {
// 0xFFD8  予約領域 (with OSF2)
	(const void*)0xffff, (const void*)0xffff,
// 0xFFDC  未定義命令 (with ID1)
    undef_inst_, (const void*)0xff00,
// 0xFFE0  オーバーフロー (with ID2)
	null_task_,  (const void*)0xff00,
// 0xFFE4  BRK 命令
	null_task_,  (const void*)0xff00,
// 0xFFE8  アドレス一致 (with ID3)
	null_task_,  (const void*)0xff00,
// 0xFFEC  シングルステップ (with ID4)
	null_task_,  (const void*)0xff00,
// 0xFFF0  ウオッチドッグタイマ、発振停止検出、電圧監視１ (with ID5)  
	null_task_,  (const void*)0xff00,
// 0xFFF4  予約 (with ID6)
	(const void*)0xffff,  (const void*)0xffff,
// 0xFFF8  予約 (with ID7)
	(const void*)0xffff,  (const void*)0xffff,
// 0xFFFC  リセット (with OFS)
	reset_start, (const void*)0xff00,
};

// EOF
