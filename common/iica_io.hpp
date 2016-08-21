#pragma once
//=====================================================================//
/*!	@file
	@brief	IICA(I2C) テンプレートクラス (20MHz system clock) @n
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
	class iica_io {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  I2C の速度タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class speed : uint8_t {
			standard,	///< 100K b.p.s. (Standard mode)
			fast,		///< 400K b.p.s. (Fast mode)
			fast_plus,	///< 1M b.p.s. (Fast plus mode)
		};

	private:
		PORT		port_;
		uint8_t		clock_;
		uint16_t	busy_;

		static const uint8_t slow_clock_ = 10 / 2;
		static const uint8_t fast_clock_ = 4 / 2;

		void start_() const {
			port_.sda_out(0);
			utils::delay::micro_second(clock_);
			port_.scl_out(0);
			utils::delay::micro_second(clock_);
		}


		bool ack_() const {
			port_.sda_out(1);
			utils::delay::micro_second(clock_);
			port_.scl_out(1);
			port_.sda_dir(0);
			utils::delay::micro_second(clock_);
			bool f = port_.sda_inp();
			port_.sda_out(0);
			port_.sda_dir(1);
			port_.scl_out(0);
			return f;
		}


		void out_ack_(bool b) const {
			utils::delay::micro_second(clock_);
			port_.sda_out(b);
			port_.scl_out(1);
			utils::delay::micro_second(clock_);
			port_.scl_out(0);
		}


		bool wait_() const {
			uint16_t cnt = busy_;
			port_.scl_dir(0);
			while(port_.scl_inp() == 0) {
				utils::delay::micro_second(1);
				if(cnt) {
					--cnt;
				} else {
					port_.scl_dir(1);
					return false;  // wait stall
				}
			}
			port_.scl_dir(1);
			return true;
		}


		void stop_() const {
			utils::delay::micro_second(clock_);
			port_.scl_out(1);
			utils::delay::micro_second(clock_);
			port_.sda_out(1);
		}


		bool write_(uint8_t val, bool sync) const {
			for(uint8_t n = 0; n < 8; ++n) {
				port_.sda_out(val & 0x80);
				utils::delay::micro_second(clock_);
				port_.scl_out(1);
				if(n == 0 && sync) {
					if(!wait_()) return false;
				}
				val <<= 1;
				utils::delay::micro_second(clock_);
				port_.scl_out(0);
			}
			return true;
		}


		bool write_(uint8_t data) const {
			if(!write_(data, true)) {
				stop_();
				return false;
			}
			if(ack_()) {
				stop_();
				return false;
			}
			return true;
		}


		bool write_(const uint8_t* src, uint8_t num) const {
			for(uint8_t n = 0; n < num; ++n) {
				if(!write_(*src, true)) {
					stop_();
					return false;
				}
				++src;
				if(ack_()) {
					stop_();
					return false;
				}
			}
			return true;
		}


		bool read_(uint8_t& val, bool sync) const {
			port_.sda_dir(0);
			for(uint8_t n = 0; n < 8; ++n) {
				utils::delay::micro_second(clock_);
				val <<= 1;
				port_.scl_out(1);
				if(n == 0 && sync) {
					if(!wait_()) {
						port_.sda_dir(1);
						return false;
					}
				}
				utils::delay::micro_second(clock_);
				if(port_.sda_inp()) val |= 1;
				port_.scl_out(0);
			}
			port_.sda_dir(1);
			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		iica_io() : clock_(slow_clock_), busy_(200) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	spd	スピード
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(speed spd)
		{
			port_.init();
			port_.scl_dir(1);
			port_.sda_dir(1);
			port_.scl_out(1);
			port_.sda_out(1);
			if(spd == speed::standard) {
				set_standard();
			} else if(spd == speed::fast) {
				set_fast();
			} else {
				return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  クロック設定
			@param[in]	clock	パルス５０％待ち時間（単位マイクロ秒）
		*/
		//-----------------------------------------------------------------//
		void set_clock(uint8_t clock) { clock_ = clock; }


		//-----------------------------------------------------------------//
		/*!
			@brief  標準速度指定（maybe 100KBPS）
		*/
		//-----------------------------------------------------------------//
		void set_standard() { clock_ = slow_clock_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  高速指定（maybe 400KBPS）
		*/
		//-----------------------------------------------------------------//
		void set_fast() { clock_ = fast_clock_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  スレーブデバイスの「待ち」時間の最大値を設定
			@param[in]	busy	待ち時間（単位マイクロ秒）
		*/
		//-----------------------------------------------------------------//
		void set_busy(uint16_t busy) { busy_ = busy; }


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
			write_((address << 1) | 1, false);
			if(ack_()) {
				stop_();
				return false;
			}

			for(uint8_t n = 0; n < num; ++n) {
				if(!read_(*dst, true)) {
					stop_();
					return false;
				}
				bool f = 0;
				if(n == (num - 1)) f = 1;
				out_ack_(f);
				++dst;
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
			write_(address << 1, false);
			if(ack_()) {
				stop_();
				return false;
			}

			if(!write_(src, num)) {
				stop_();
				return false;
			}
			stop_();
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信（ライト）
			@param[in] address スレーブアドレス（７ビット）
			@param[in]	first	ファーストデータ
			@param[in]	src	元
			@param[in]	num	数
			@return 失敗なら「false」が返る
		*/
		//-----------------------------------------------------------------//
		bool send(uint8_t address, uint8_t first, const uint8_t* src, uint8_t num) const {
			start_();
			write_(address << 1, false);
			if(ack_()) {
				stop_();
				return false;
			}

			if(!write_(first)) {
				stop_();
				return false;
			}

			if(!write_(src, num)) {
				stop_();
				return false;
			}
			stop_();
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信（ライト）
			@param[in] address スレーブアドレス（７ビット）
			@param[in]	first	ファースト・データ
			@param[in]	second	セカンド・データ
			@param[in]	src	元
			@param[in]	num	数
			@return 失敗なら「false」が返る
		*/
		//-----------------------------------------------------------------//
		bool send(uint8_t address, uint8_t first, uint8_t second, const uint8_t* src, uint8_t num) const {
			start_();
			write_(address << 1, false);
			if(ack_()) {
				stop_();
				return false;
			}

			if(!write_(first)) {
				stop_();
				return false;
			}
			if(!write_(second)) {
				stop_();
				return false;
			}
			if(!write_(src, num)) {
				stop_();
				return false;
			}
			stop_();
			return true;
		}
	};
}
