#pragma once
//=====================================================================//
/*!	@file
	@brief	MAX7219 ドライバー
			Copyright 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/iica_io.hpp"
#include "common/time.h"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MAX7219 テンプレートクラス
		@param[in]	SPI		SPI クラス
		@param[in]	SELECT	デバイス選択
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class SELECT>
	class MAX7219 {

		SPI&	spi_;

		uint8_t	limit_;
		uint8_t	data_[8];

		enum class command : uint8_t {
			NO_OP        = 0x00,
			DIGIT_0      = 0x01,
			DIGIT_1      = 0x02,
			DIGIT_2      = 0x03,
			DIGIT_3      = 0x04,
			DIGIT_4      = 0x05,
			DIGIT_5      = 0x06,
			DIGIT_6      = 0x07,
			DIGIT_7      = 0x08,
			DECODE_MODE  = 0x09,
			INTENSITY    = 0x0A,
			SCAN_LIMIT   = 0x0B,
			SHUTDOWN     = 0x0C,
			DISPLAY_TEST = 0x0F,
		};

		// MAX7212 D15 first
		void out_(command cmd, uint8_t dat) {
			SELECT::P = 0;
			uint8_t tmp[2];
			tmp[0] = static_cast<uint8_t>(cmd);
			tmp[1] = dat;
			spi_.send(tmp, 2);
			SELECT::P = 1;  // load
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	spi	SPI クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		MAX7219(SPI& spi) : spi_(spi), limit_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	limit	スキャン・リミット
			@return エラーなら「false」を返す
		 */
		//-----------------------------------------------------------------//
		bool start(uint8_t limit = 8) {
			if(limit_ > 8 || limit == 0) {
				return false;
			}
			limit_ = limit;

			SELECT::DIR = 1;  // output;
			SELECT::PU  = 0;  // pull-up disable
			SELECT::P = 1;    // /CS = H

			for(uint8_t i = 0; i < sizeof(data_); ++i) {
				data_[i] = 0;
			}

			out_(command::SHUTDOWN, 0x01);  // ノーマル・モード
			out_(command::DECODE_MODE, 0x00);  // デコード・モード
			out_(command::SCAN_LIMIT, limit - 1);  // 表示桁設定
			set_intensity(0);  // 輝度（最低）

			service();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief 輝度の設定
			@param[in]	inten	輝度値（最小：０、最大：１５）
			@return エラー（初期化不良）なら「false」
		 */
		//-----------------------------------------------------------------//
		bool set_intensity(uint8_t inten) {
			if(limit_ == 0) return false;
			out_(command::INTENSITY, inten);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief データ転送
			@return エラー（初期化不良）なら「false」
		 */
		//-----------------------------------------------------------------//
		bool service() {
			if(limit_ == 0) return false;

			for(uint8_t i = 0; i < limit_; ++i) {
				out_(static_cast<command>(static_cast<uint8_t>(command::DIGIT_0) + i), data_[i]);
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief 値の設定
			@param[in]	idx	インデックス（０～７）
			@param[in]	dat	データ
		 */
		//-----------------------------------------------------------------//
		void set(uint8_t idx, uint8_t dat) {
			if(idx < limit_) {
				data_[idx] = dat;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief キャラクターの設定
			@param[in]	idx	インデックス（０～７）
			@param[in]	cha	キャラクターコード
			@param[in]	dp	小数点
		 */
		//-----------------------------------------------------------------//
		void set_cha(uint8_t idx, char cha, bool dp = false) {
			uint8_t d = 0;
			switch(cha) {
			case ' ':
				break;
			case '0':
				d = 0b1111110;
				break;
			case '1':
				d = 0b0110000;
				break;
			case '2':
				d = 0b1101101;
				break;
			case '3':
				d = 0b1111001;
				break;
			case '4':
				d = 0b0110011;
				break;
			case '5':
				d = 0b1011011;
				break;
			case '6':
				d = 0b1011111;
				break;
			case '7':
				d = 0b1110000;
				break;
			case '8':
				d = 0b1111111;
				break;
			case '9':
				d = 0b1111011;
				break;
			case '-':
				d = 0b0000001;
				break;
			case 'A':
			case 'a':
				d = 0b1110111;
				break;
			case 'B':
			case 'b':
				d = 0b0011111;
				break;
			default:
				break;
			}
			if(dp) d |= 0x80;
			set(idx, d);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief [] オペレーターによるアクセス
			@param[in]	idx	インデックス（０～７）
		 */
		//-----------------------------------------------------------------//
		uint8_t& operator[] (uint8_t idx) { return &data_[idx]; }
	};
}
