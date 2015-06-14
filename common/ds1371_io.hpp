#pragma once
//=====================================================================//
/*!	@file
	@brief	DS1371 RTC ドライバー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/i2c_io.hpp"
#include <time.h>

namespace device {

	template <class PORT>
	class ds1371_io {

		i2c_io<PORT>	i2c_io_;

		bool get_time_(time_t& tp) const {
			uint8_t reg[4];
			reg[0] = 0x00;	// address
			i2c_io_.send(0x68, reg, 1);

			int ret = i2c_io_.recv(0x68, reg, 4);
			if(ret != 0) {
				return false;
			}

			tp = (static_cast<time_t>(reg[3]) << 24) | (static_cast<time_t>(reg[2]) << 16) |
				 (static_cast<time_t>(reg[1]) << 8)  | static_cast<time_t>(reg[0]);
			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() const {
			i2c_io_.init();

			uint8_t reg[2];
			reg[0] = 0x08;	/// address
			reg[1] = 0x00;
			i2c_io_.send(0x68, reg, 2);
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
			if(!i2c_io_.send(0x68, reg, 5)) {
				return false;
			}
			return true;
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
				if(get_time_(t) != 0) return false;
				if(get_time_(tmp) != 0) return false;
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
