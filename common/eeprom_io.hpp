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

		uint8_t	ds_;
		bool	exp_;
		bool	ad_mix_;
		uint8_t	pagen_;

		uint8_t i2c_adr_(uint32_t adr) const {
			uint8_t a = EEPROM_ADR_ | ds_;
			if(ad_mix_) {
				if(adr >> 16) a |= 4;
			}
			return a;
		}

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief 256 バイトまでの EEPROM の ID (0 to 7)
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class M256B {
			ID0,
			ID1,
			ID2,
			ID3,
			ID4,
			ID5,
			ID6,
			ID7,
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief 64K バイトまでの EEPROM の ID (0 to 7)
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class M64KB {
			ID0,
			ID1,
			ID2,
			ID3,
			ID4,
			ID5,
			ID6,
			ID7,
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief 128K バイトまでの EEPROM の ID (0 to 3)
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class M128KB {
			ID0,
			ID1,
			ID2,
			ID3,
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	i2c_io クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		eeprom_io(i2c_io<PORT>& i2c) : i2c_io_(i2c), ds_(0),
									   exp_(false), ad_mix_(false), pagen_(1) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	256 バイトまでの EEPROM を開始
			@param[in]	type	デバイスのタイプとID
			@param[in]	pagen	ページサイズ（書き込み一時バッファのサイズ）	
		 */
		//-----------------------------------------------------------------//
		void start(M256B type_id, uint8_t pagen) {
			ds_ = static_cast<uint8_t>(type_id);
			exp_ = false;
			ad_mix_ = false;
			pagen_ = pagen;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	64K バイトまでの EEPROM を開始
			@param[in]	type	デバイスのタイプとID
			@param[in]	pagen	ページサイズ（書き込み一時バッファのサイズ）	
		 */
		//-----------------------------------------------------------------//
		void start(M64KB type_id, uint8_t pagen) {
			ds_ = static_cast<uint8_t>(type_id);
			exp_ = true;
			ad_mix_ = false;
			pagen_ = pagen;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	128K バイトまでの EEPROM を開始
			@param[in]	type	デバイスのタイプとID
			@param[in]	pagen	ページサイズ（書き込み一時バッファのサイズ）	
		 */
		//-----------------------------------------------------------------//
		void start(M128KB type_id, uint8_t pagen) {
			ds_ = static_cast<uint8_t>(type_id);
			exp_ = true;
			ad_mix_ = true;
			pagen_ = pagen;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	書き込み状態の検査
			@param[in]	adr	検査アドレス
			@return 「false」なら、書き込み中
		 */
		//-----------------------------------------------------------------//
		bool get_write_state(uint32_t adr) const {
			uint8_t tmp[1];
			return read(adr, tmp, 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	書き込み同期
			@param[in]	adr	検査アドレス
			@param[in]	delay 待ち時間（10us単位）
			@return デバイスエラーなら「false」
		 */
		//-----------------------------------------------------------------//
		bool sync_write(uint32_t adr, uint16_t delay = 600) const {
			bool ok = false;
			for(uint16_t i = 0; i < delay; ++i) {
				utils::delay::micro_second(10);
				uint8_t tmp[1];
				if(read(adr, tmp, 1)) {
					ok = true;
					break;
				}
			}
			return ok;
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
				if(!i2c_io_.send(i2c_adr_(adr), tmp, 2)) {
					return false;
				}
				if(!i2c_io_.recv(i2c_adr_(adr), dst, len)) {
					return false;
				}
			} else {
				uint8_t tmp[1];
				tmp[0] = adr & 255;
				if(!i2c_io_.send(i2c_adr_(adr), tmp, 1)) {
					return false;
				}
				if(!i2c_io_.recv(i2c_adr_(adr), dst, len)) {
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
			const uint8_t* end = src + len;
			while(src < end) {
				uint16_t l = pagen_ - (adr & (pagen_ - 1));
				if(len < l) l = len;
				if(exp_) {
					if(!i2c_io_.send(i2c_adr_(adr), adr >> 8, adr & 255, src, l)) {
						return false;
					}
				} else {
					if(!i2c_io_.send(i2c_adr_(adr), adr & 255, src, l)) {
						return false;
					}
				}
				src += l;
				if(src < end) {  // 書き込み終了を待つポーリング
					if(!sync_write(adr)) {
						return false;
					}
				}
				adr += l;
			}
			return true;
		}
	};
}
