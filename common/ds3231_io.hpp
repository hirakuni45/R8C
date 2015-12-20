#pragma once
//=====================================================================//
/*!	@file
	@brief	DS3231 RTC ドライバー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/i2c_io.hpp"
#include "common/time.h"

namespace device {


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  DS3231 テンプレートクラス @n
				PORT ポート指定クラス： @n
				class port { @n
				public: @n
					void scl_dir(bool val) const { } @n
					void scl_out(bool val) const { } @n
					bool scl_inp() const { return 0; } @n
					void sda_dir(bool val) const { } @n
					void sda_out(bool val) const { } @n
					bool sda_inp() const { return 0; } @n
				};
		@param[in]	PORT	ポート定義クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class PORT>
	class ds3231_io {

		static const uint8_t	DS3231_ADR_ = 0x68;

		i2c_io<PORT>&	i2c_io_;

		struct t_ {
			uint8_t	reg[7];
			bool operator != (const t_& t) {
				for(uint8_t i = 0; i < 7; ++i) {
					if(reg[i] != t.reg[i]) return true;
				}
				return false;
			}
		};

		bool get_time_(t_& t) const {
			uint8_t reg[1];
			reg[0] = 0x00;	// set address
			if(!i2c_io_.send(DS3231_ADR_, reg, 1)) {
				return false;
			}
			if(!i2c_io_.recv(DS3231_ADR_, &t.reg[0], 7)) {
				return false;
			}
			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	i2c_io クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		ds3231_io(i2c_io<PORT>& i2c) : i2c_io_(i2c) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() {
			i2c_io_.set_fast();		// 400kbps

			uint8_t reg[2];
			reg[0] = 0x0e;	/// address
			reg[1] = 0x00;
			i2c_io_.send(DS3231_ADR_, reg, 2);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DS3231 時間設定
			@param[in]	t	時間
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_time(time_t t) const {
			i2c_io_.set_fast();		// 400kbps
			const tm* tp = gmtime(&t);
			uint8_t reg[8];
			reg[0] = 0x00;	/// address
			reg[1] = ((tp->tm_sec / 10) << 4) | (tp->tm_sec % 10);
			reg[2] = ((tp->tm_min / 10) << 4) | (tp->tm_min % 10);
			reg[3] = ((tp->tm_hour / 10) << 4) | (tp->tm_hour % 10);
			reg[4] = tp->tm_wday + 1;
			reg[5] = ((tp->tm_mday / 10) << 4) | (tp->tm_mday % 10);
			uint16_t y = tp->tm_year - 1900;
			uint8_t century = 0;
			if(y >= 100) { y -= 100; century = 0x80; }
			reg[6] = (((tp->tm_mon + 1) / 10) << 4) | ((tp->tm_mon + 1) % 10) | century;
			reg[7] = ((y / 10) << 4) | (y % 10);
			return i2c_io_.send(DS3231_ADR_, reg, 8);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DS3231 時間呼び出し
			@param[in]	tp	取得時間
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool get_time(time_t& tp) const {
			i2c_io_.set_fast();		// 400kbps

			// 二度読んで、同じだったら正しい時間とする
			t_ t;
			t_ tmp;
			uint8_t n = 4; // ４回ループして正常に読めなかったら、エラーとする
			do {
				if(!get_time_(t)) return false;
				if(!get_time_(tmp)) return false;
				--n;
				if(n == 0) {
					return false;
				}
			} while(t != tmp) ;

			tm ts;
			ts.tm_sec  = ((t.reg[0] >> 4) * 10) + (t.reg[0] & 0xf);
			ts.tm_min  = ((t.reg[1] >> 4) * 10) + (t.reg[1] & 0xf);
			ts.tm_hour = ((t.reg[2] >> 4) * 10) + (t.reg[2] & 0xf);
			ts.tm_mday = ((t.reg[4] >> 4) * 10) + (t.reg[4] & 0xf);
			ts.tm_mon  = ((((t.reg[5] & 0x1) >> 4) * 10) + (t.reg[5] & 0xf)) - 1;
			ts.tm_year = ((t.reg[6] >> 4) * 10) + (t.reg[6] & 0xf);
			ts.tm_year += 1900;
			if(t.reg[5] & 0x80) ts.tm_year += 100;
			tp = mktime(&ts);
			return true;
		}
	};
}