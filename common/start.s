# ===============================================================
/*!	@file
	@brief	R8C スタート・アップ
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2014, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
# ===============================================================
	.text
	.global _reset_start
_reset_start:
	.global	_start
_start:
	/* 割り込みスタック設定 */
	.extern _isp_init
	ldc #_isp_init,isp

	/* ユーザースタック設定 */
	.extern _usp_init
	fset u
	ldc #_usp_init,sp

	/* 可変ベクターテーブルアドレス設定 */
	.extern _variable_vectors_
	ldc #_variable_vectors_,intbl
		
	/* 割り込み許可 */
	ldipl #0
	nop
	nop
	fset i

	.extern __init
	jmp.w __init

	.global _exit
_exit:
	jmp.w	_exit

	.section .data
	.global ___dso_handle
	.weak   ___dso_handle
___dso_handle:
	.long   0
