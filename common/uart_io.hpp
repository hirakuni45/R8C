#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・UART I/O 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/vect.h"
#include "system.hpp"
#include "intr.hpp"
#include "uart.hpp"

/// F_CLK はボーレートパラメーター計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "uart_io.hpp requires F_CLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  UART I/O 制御クラス
		@param[in]	UART	UARTx 定義クラス
		@param[in]	SEND	送信バッファサイズ（最低８バイト）
		@param[in]	RECV	受信バッファサイズ（最低８バイト）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class UART, class SEND, class RECV>
	class uart_io {

		static SEND	send_;
		static RECV	recv_;
		static volatile bool	send_stall_;
		bool	crlf_;

	public:

		//-----------------------------------------------------------------//
		/*!
			@brief  受信割り込みタスク
		*/
		//-----------------------------------------------------------------//
		static void irecv() {
			uint16_t ch = UART::URB();
			///< フレーミングエラー/パリティエラー状態確認
			if(ch & (UART::URB.OER.b() | UART::URB.FER.b() | UART::URB.PER.b() | UART::URB.SUM.b())) {
				// 強制的にエラーフラグを除去する
				UART::UC1.RE = 0;
				UART::UC1.RE = 1;
			} else {
				recv_.put(static_cast<char>(ch));
			}
			volatile uint8_t r = UART::UIR();
			UART::UIR = UART::UIR.URIF.b(false) | UART::UIR.UTIF.b()
				| (r & (UART::UIR.UTIE.b() | UART::UIR.URIE.b()));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信割り込みタスク @n
					※「send_uart_intr」関数から呼ぶ
		*/
		//-----------------------------------------------------------------//
		static inline void isend()
		{
			if(send_.length()) {
				UART::UTBL = send_.get();
			} else {
				send_stall_ = true;
			}
			volatile uint8_t r = UART::UIR();
			UART::UIR = UART::UIR.URIF.b() | UART::UIR.UTIF.b(false)
				| (r & (UART::UIR.UTIE.b() | UART::UIR.URIE.b()));
		}

private:
		// ※同期が必要なら、実装する
		void sleep_() const {
			asm("nop");
		}

		void send_restart_() {
			if(send_stall_ && send_.length() > 0) {
				while(UART::UC1.TI() == 0) sleep_();
				char ch = send_.get();
				send_stall_ = false;
				UART::UTBL = ch;
			}
		}

		void putch_(char ch) {
			if(UART::UIR.UTIE()) {
				/// ７／８ を超えてた場合は、バッファが空になるまで待つ。
				/// ※ヒステリシス動作
				if(send_.length() >= (send_.size() * 7 / 8)) {
					send_restart_();
					while(send_.length() != 0) {
						sleep_();
					}
				}
				send_.put(ch);
				send_restart_();
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

			send_stall_ = true;

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
	template<class UART, class SEND, class RECV>
		SEND uart_io<UART, SEND, RECV>::send_;
	template<class UART, class SEND, class RECV>
		RECV uart_io<UART, SEND, RECV>::recv_;
	template<class UART, class SEND, class RECV>
		volatile bool uart_io<UART, SEND, RECV>::send_stall_ = true;
}
