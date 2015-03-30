#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・UART I/O 制御 @n
			Copyright 2014,2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "uart.hpp"
#include "fifo.hpp"

/// F_PCKB はボーレートパラメーター計算で必要で、設定が無いとエラーにします。
#ifndef F_CLK
#  error "uart_io.hpp requires F_CLK to be defined"
#endif

#define INTERRUPT_FUNC __attribute__ ((interrupt))

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
		bool	polling_;

public:
		static INTERRUPT_FUNC void recv_task() {
#if 0
			bool err = false;
			if(SCIx::SSR.ORER()) {	///< 受信オーバランエラー状態確認
				SCIx::SSR = 0x00;	///< 受信オーバランエラークリア
				err = true;
			}
			///< フレーミングエラー/パリティエラー状態確認
			if(SCIx::SSR() & (SCIx::SSR.FER.b() | SCIx::SSR.PER.b())) {
				err = true;
			}
			if(!err) recv_.put(SCIx::RDR());
#endif
		}

		static INTERRUPT_FUNC void send_task() {
			UART::UTBL = send_.get();
			if(send_.length() == 0) {
				UART::UIR.UTIE = 0;
			}
		}

private:
		// ※同期が必要なら、実装する
		void sleep_() {
			asm("nop");
		}


		void putch_(char ch) {
			if(polling_) {
				while(UART::UC1.TI() == 0) sleep_();
				UART::UTBL = ch;
			} else {
				/// ７／８ を超えてた場合は、バッファが空になるまで待つ。
				/// ※ヒステリシス動作
				if(send_.length() >= (send_.size() * 7 / 8)) {
					while(send_.length() != 0) sleep_();
				}
				send_.put(ch);
				UART::UIR.UTIE = 1;
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		uart_io() : crlf_(true), polling_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化 @n
					※ポーリングの場合は設定しなくても良い
			@param[in]	level	割り込みレベル
		*/
		//-----------------------------------------------------------------//
		void initialize(uint8_t level) {
//			intr_level_ = level;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ボーレートを設定して、UART を有効にする
			@param[in]	baud	ボーレート
			@param[in]	polling	ポーリングの場合「true」
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool start(uint32_t baud, bool polling = false) {
			polling_ = polling;

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

			if(polling) {
				UART::UIR = UART::UIR.URIE.b(false) | UART::UIR.UTIE.b(false);
			} else {
				// 送信割り込み許可は、バッファが「空」では無い場合に設定
				UART::UIR = UART::UIR.URIE.b();
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
			@brief	UART 入力文字数を取得
			@return	入力文字数
		 */
		//-----------------------------------------------------------------//
		uint16_t length() {
			if(polling_) {
				if(UART::UC1.RI()) {
					return 1;	///< 受信データあり
				} else {
					return 0;	///< 受信データなし
				}
			} else {
				return recv_.length();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	UART 文字入力
			@return 文字コード
		 */
		//-----------------------------------------------------------------//
		char getch() {
			if(polling_) {
				while(length() == 0) sleep_();
				uint16_t ch = UART::URB();
				// エラー発生時の動作（現状：無視）
//				if(ch & (UART::URB.OER.b() | UART::URB.FER.b() | UART::URB.PER.b() | UART::URB.SUM.b())) {
//				}
				return static_cast<char>(ch);
			} else {
				// バッファが空なら、受信するまで待つ。
				while(recv_.length() == 0) sleep_();
				return recv_.get();
			}
		}

	};

	// 受信、送信バッファのテンプレート内スタティック実態定義
	template<class UART, uint16_t recv_size, uint16_t send_size>
	utils::fifo<recv_size> uart_io<UART, recv_size, send_size>::recv_;
	template<class UART, uint16_t recv_size, uint16_t send_size>
	utils::fifo<send_size> uart_io<UART, recv_size, send_size>::send_;
}
