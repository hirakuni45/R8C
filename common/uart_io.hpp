#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・UART I/O 制御 @n
			Copyright 2014,2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "intr.hpp"
#include "uart.hpp"
#include "fifo.hpp"


/// F_CLK はボーレートパラメーター計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "uart_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  UART I/O 制御クラス
		@param[in]	UART		UARTx 定義クラス
		@param[in]	recv_size	受信バッファサイズ（最低８バイト）
		@param[in]	send_size	送信バッファサイズ（最低８バイト）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class UART, uint16_t recv_size, uint16_t send_size>
	class uart_io {

		static utils::fifo<recv_size>	recv_;
		static utils::fifo<send_size>	send_;

		bool	crlf_;

	public:
		static INTERRUPT_FUNC void recv_task() {
			uint16_t ch = UART::URB();
			///< フレーミングエラー/パリティエラー状態確認
			if(ch & (UART::URB.OER.b() | UART::URB.FER.b() | UART::URB.PER.b() | UART::URB.SUM.b())) {
//				++recv_err_;
				// 強制的にエラーフラグを除去する
				UART::UC1.RE = 0;
				UART::UC1.RE = 1;
			} else {
				recv_.put(static_cast<char>(ch));
			}
			volatile uint8_t r = UART::UIR();
			UART::UIR = UART::UIR.URIF.b(false) | UART::UIR.UTIF.b()
				| (r & (UART::UIR.UTIE.b() | UART::UIR.URIE.b()));
// (NG)		UART::UIR.URIF = 0;
		}

		static INTERRUPT_FUNC void send_task() {
			if(send_.length()) {
				UART::UTBL = send_.get();
			} else {
			}
			volatile uint8_t r = UART::UIR();
			UART::UIR = UART::UIR.URIF.b() | UART::UIR.UTIF.b(false)
				| (r & (UART::UIR.UTIE.b() | UART::UIR.URIE.b()));
// (NG)		UART::UIR.UTIF = 0;
		}

private:
		// ※同期が必要なら、実装する
		void sleep_() const {
			asm("nop");
		}

		void putch_(char ch) {
			if(UART::UIR.UTIE()) {
				/// ７／８ を超えてた場合は、バッファが空になるまで待つ。
				/// ※ヒステリシス動作
				if(send_.length() >= (send_.size() * 7 / 8)) {
					while(send_.length() != 0) {
						sleep_();
					}
				}
				send_.put(ch);
				if(UART::UC1.TI()) {
					UART::UTBL = send_.get();
				}
			} else {
				while(UART::UC1.TI() == 0) sleep_();
				UART::UTBL = ch;
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		uart_io() : crlf_(true) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  ボーレートを設定して、UART を有効にする
			@param[in]	baud	ボーレート
			@param[in]	ir_level	割り込みレベル、「０」の場合ポーリング
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool start(uint32_t baud, uint8_t ir_level) {
			MSTCR.MSTUART = 0;  // モジュールスタンバイ解除

			UART::UC1 = 0x00;

			uint32_t brr = F_CLK / baud / 16;
			uint8_t cks = 0;
			static uint8_t shift_[] = { 0, 3, 2 };
			while(brr > 256) {
				brr >>= shift_[cks];
				++cks;
				if(cks >= 3) return false;
			}
			UART::UC0 = UART::UC0.CLK.b(cks);
			if(brr) --brr;
			UART::UBRG = static_cast<uint8_t>(brr);

			// 8 ビット、１ストップ、パリティ無し
			UART::UMR = UART::UMR.SMD.b(0b101);

			UART::UC1 = UART::UC1.TE.b() | UART::UC1.RE.b();

			ILVL8.B45 = ir_level;
			ILVL9.B01 = ir_level;
			if(ir_level) {
				UART::UIR = UART::UIR.URIE.b() | UART::UIR.UTIE.b();
			} else {
				UART::UIR = UART::UIR.URIE.b(false) | UART::UIR.UTIE.b(false);
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	CRLF 自動送出
			@param[in]	f	「false」なら無効
		 */
		//-----------------------------------------------------------------//
		void auto_crlf(bool f = true) { crlf_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	UART 文字出力
			@param[in]	ch	文字コード
		 */
		//-----------------------------------------------------------------//
		void putch(char ch) {
			if(crlf_ && ch == '\n') {
				putch_('\r');
			}
			putch_(ch);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	UART 文字列出力
			@param[in]	ptr	文字列
		 */
		//-----------------------------------------------------------------//
		void puts(const char* ptr) {
			char ch;
			while((ch = *ptr++) != 0) {
				putch(ch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	UART 入力文字数を取得
			@return	入力文字数
		 */
		//-----------------------------------------------------------------//
		uint16_t length() {
			if(UART::UIR.URIE()) {
				return recv_.length();
			} else {
				if(UART::UC1.RI()) {
					return 1;	///< 受信データあり
				} else {
					return 0;	///< 受信データなし
				}
			
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	UART 文字入力
			@return 文字コード
		 */
		//-----------------------------------------------------------------//
		char getch() {
			if(UART::UIR.URIE()) {
				// バッファが空なら、受信するまで待つ。
				while(recv_.length() == 0) sleep_();
				return recv_.get();
			} else {
				while(length() == 0) sleep_();
				uint16_t ch = UART::URB();
				// エラー発生時の動作
				if(ch & (UART::URB.OER.b() | UART::URB.FER.b() | UART::URB.PER.b() | UART::URB.SUM.b())) {
					UART::UC1.RE = 0;
					UART::UC1.RE = 1;
					ch = 0;
				}
				return static_cast<char>(ch);
			}
		}

	};

	// 受信、送信バッファのテンプレート内スタティック実態定義
	template<class UART, uint16_t recv_size, uint16_t send_size>
	utils::fifo<recv_size> uart_io<UART, recv_size, send_size>::recv_;
	template<class UART, uint16_t recv_size, uint16_t send_size>
	utils::fifo<send_size> uart_io<UART, recv_size, send_size>::send_;
}
