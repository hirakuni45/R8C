#pragma once
//=====================================================================//
/*!	@file
	@brief	I2C テンプレートクラス (20MHz system clock) @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/delay.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  I2C テンプレートクラス @n
				SCL、SDA ポート指定クラス： @n
				class port { @n
				public: @n
					void init() const { } @n
					void out(bool val) const { } @n
					bool inp() const { return 0; } @n
				};
		@param[in]	SCL	ポート定義クラス
		@param[in]	SDA	ポート定義クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SCL, class SDA>
	class i2c_io {
		SCL	scl_;
		SDA	sda_;
		uint16_t	clock_;

		void start_() const {
			scl_.out(1);
			utils::delay::micro_second(clock_ >> 1);
			sda_.out(0);
		}

		void stop_() const {
			scl_.out(1);
			utils::delay::micro_second(clock_ >> 1);
			sda_.out(1);
		}

		bool ack_() const {
			scl_.out(0);
			utils::delay::micro_second(clock_ >> 1);
			scl_.out(1);
			utils::delay::micro_second(clock_ >> 1);
			bool f = sda_.inp();
			return f;
		}

		void write_(uint8_t val) const {
			for(uint8_t n = 0; n < 8; ++n) {
				scl_.out(0);
				if(val & 0x80) sda_.out(1); else sda_.out(0);
				utils::delay::micro_second(clock_ >> 1);
				scl_.out(1);
				val <<= 1;
				utils::delay::micro_second(clock_ >> 1);
			}
		}

		uint8_t read_() const {
			uint8_t val;
			for(uint8_t n = 0; n < 8; ++n) {
				scl_.out(0);
				val <<= 1;
				utils::delay::micro_second(clock_ >> 1);
				scl_.out(1);
				utils::delay::micro_second(clock_ >> 1);
				if(sda_.inp()) val |= 1;
			}
			return val;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		i2c_io() : clock_(10) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void init() {
			scl_.init();
			sda_.init();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  低速指定（100KBPS）
		*/
		//-----------------------------------------------------------------//
		void set_slow() { clock_ = 10; }


		//-----------------------------------------------------------------//
		/*!
			@brief  高速指定（400KBPS）
		*/
		//-----------------------------------------------------------------//
		void set_fast() { clock_ = 4; }


		//-----------------------------------------------------------------//
		/*!
			@brief  受信（リード）
			@param[in] address スレーブアドレス（７ビット）
			@param[out]	dst	先
			@param[in]	num	数
			@return 失敗なら「false」が返る
		*/
		//-----------------------------------------------------------------//
		bool recv(uint8_t address, uint8_t* dst, uint8_t num) const {
			start_();
			write_((address << 1) | 1);
			if(ack_()) {
				stop_();
				return false;
			}
			for(uint8_t n = 0; n < num; ++n) {
				*dst++ = read_();
				if(ack_()) {
					stop_();
					return false;
				}
			}
			stop_();
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信（ライト）
			@param[in] address スレーブアドレス（７ビット）
			@param[in]	src	元
			@param[in]	num	数
			@return 失敗なら「false」が返る
		*/
		//-----------------------------------------------------------------//
		bool send(uint8_t address, const uint8_t* src, uint8_t num) const {
			start_();
			write_(address << 1);
			if(ack_()) {
				stop_();
				return false;
			}
			for(uint8_t n = 0; n < num; ++n) {
				write_(*src);
				if(ack_()) {
					stop_();
					return false;
				}
			}
			stop_();
			return true;
		}
	};
}
