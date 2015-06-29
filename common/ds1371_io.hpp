#pragma once
//=====================================================================//
/*!	@file
	@brief	DS1371 RTC ドライバー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/i2c_io.hpp"
#include "common/time.h"

namespace device {


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  DS1371 テンプレートクラス @n
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
	class ds1371_io {

		static const uint8_t	DS1371_ADR_ = 0x68;

		i2c_io<PORT>&	i2c_io_;

		bool get_time_(time_t& tp) const {
			uint8_t reg[4];
			reg[0] = 0x00;	// address
			if(!i2c_io_.send(DS1371_ADR_, reg, 1)) {
				return false;
			}
			if(!i2c_io_.recv(DS1371_ADR_, &reg[0], 4)) {
				return false;
			}

			tp = (static_cast<time_t>(reg[3]) << 24) | (static_cast<time_t>(reg[2]) << 16) |
				 (static_cast<time_t>(reg[1]) << 8)  |  static_cast<time_t>(reg[0]);
			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	I2C class
		 */
		//-----------------------------------------------------------------//
		ds1371_io(i2c_io<PORT>& i2c) : i2c_io_(i2c) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() {
			i2c_io_.set_fast();		// 400kbps

			uint8_t reg[3];
			reg[0] = 0x07;	/// address
			reg[1] = 0x00;
			reg[2] = 0x00;
			i2c_io_.send(DS1371_ADR_, reg, 3);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DS1371 時間設定
			@param[in]	t	時間
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_time(time_t t) const {
			i2c_io_.set_fast();		// 400kbps
			uint8_t reg[5];
			reg[0] = 0x00;	/// address
			reg[1] = t;
			reg[2] = t >> 8;
			reg[3] = t >> 16;
			reg[4] = t >> 24;
			return i2c_io_.send(DS1371_ADR_, reg, 5);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DS1371 時間呼び出し
			@param[in]	tp	取得時間
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool get_time(time_t& tp) const {
			i2c_io_.set_fast();		// 400kbps
			time_t t = 0;
			time_t tmp = 0;
			// 二度読んで、同じだったら正しい時間とする
			uint8_t n = 4; // ４回ループして正常に読めなかったら、エラーとする
			do {
				if(!get_time_(t)) return false;
				if(!get_time_(tmp)) return false;
				--n;
				if(n == 0) {
					return false;
				}
			} while(t != tmp) ;

			tp = t; 
			return true;
		}
	};
}
