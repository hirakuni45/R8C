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

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  UART I/O 制御クラス
		@param[in]	UART		UARTx 定義クラス
		@param[in]	recv_size	受信バッファサイズ
		@param[in]	send_size	送信バッファサイズ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class UART, uint16_t recv_size, uint16_t send_size>
	class uart_io {

		static utils::fifo<recv_size>	recv_;
		static utils::fifo<send_size>	send_;

		bool	polling_;

		// ※必要なら、実装する
		void sleep_() { }

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		uart_io() : polling_(false) { }


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
				UART::UIR = UART::UIR.URIE.b() | UART::UIR.UTIE.b();
			}

			return true;
		}

	};
}
