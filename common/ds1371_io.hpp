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

	template <class PORT>
	class ds1371_io {

		static const uint8_t	DS1371_ADR_ = 0x68;

		i2c_io<PORT>	i2c_io_;

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
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() {
			i2c_io_.init();
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