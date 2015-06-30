#pragma once
//=====================================================================//
/*!	@file
	@brief	EEPROM ドライバー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/i2c_io.hpp"
#include "common/time.h"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  EEPROM テンプレートクラス @n
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
	class eeprom_io {

		static const uint8_t	EEPROM_ADR_ = 0x50;

		i2c_io<PORT>&	i2c_io_;

		bool	exp_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		eeprom_io(i2c_io<PORT>& i2c) : i2c_io_(i2c), exp_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	exp	「true」の場合、２バイトアドレス
		 */
		//-----------------------------------------------------------------//
		void start(bool exp) {
			exp_ = exp;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	EEPROM 読み出し
			@param[in]	adr	読み出しアドレス
			@param[out]	dst	先
			@param[in]	len	長さ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool read(uint32_t adr, uint8_t* dst, uint16_t len) const {
			if(exp_) {
				uint8_t tmp[2];
				tmp[0] = (adr >> 8) & 255;
				tmp[1] =  adr & 255;
				if(!i2c_io_.send(EEPROM_ADR_ | ((adr >> 16) & 7), tmp, 2)) {
					return false;
				}
				if(!i2c_io_.recv(EEPROM_ADR_ | ((adr >> 16) & 7), dst, len)) {
					return false;
				}
			} else {
				uint8_t tmp[1];
				tmp[0] = adr & 255;
				if(!i2c_io_.send(EEPROM_ADR_ | ((adr >> 8) & 7), tmp, 1)) {
					return false;
				}
				if(!i2c_io_.recv(EEPROM_ADR_ | ((adr >> 16) & 7), dst, len)) {
					return false;
				}
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	EEPROM 書き込み
			@param[in]	adr	書き込みアドレス
			@param[out]	src	元
			@param[in]	len	長さ
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		bool write(uint32_t adr, const uint8_t* src, uint16_t len) const {
			if(exp_) {
				if(!i2c_io_.send(EEPROM_ADR_ | ((adr >> 16) & 7), adr >> 8, adr & 255, src, len)) {
					return false;
				}
			} else {
				if(!i2c_io_.send(EEPROM_ADR_ | ((adr >> 8) & 7), adr & 255, src, len)) {
					return false;
				}
			}
			return true;
		}
	};
}
