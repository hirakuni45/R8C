#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C グループ・フラッシュ制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/vect.h"
#include "flash.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  フラッシュ制御クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class flash_io {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  データ・バンク定義
		*/
		//-----------------------------------------------------------------//
		enum class data_bank {
			bank0,	///< 0x3000 to 0x33FF (1024)
			bank1,	///< 0x3400 to 0x37FF (1024)
		};

	private:
		void sleep_() const { asm("nop"); }

		void sync_() const {
			while(FST.FST7() == 0) {
				sleep_();
			}
		}

		void enable_(uint16_t ofs) const {
			FMR0.FMR01 = 0;
			FMR0.FMR01 = 1;  // CPU 書き換え有効
			FMR0.FMR02 = 0;
			FMR0.FMR02 = 1;  // EW1 モード
			if(ofs < 0x0400) {
				FMR1.FMR16 = 0;
				FMR1.FMR16 = 1;
			} else {
				FMR1.FMR17 = 1;
				FMR1.FMR17 = 0;
			}
		}

		void disable_(uint16_t ofs) const {
			if(ofs < 0x0400) {
				FMR1.FMR16 = 0;
			} else {
				FMR1.FMR17 = 0;
			}
			FMR0.FMR01 = 0;  // CPU 書き換え無効
		}

		bool write_(uint16_t ofs, uint8_t data) const {
			wr8_(0x3000 + ofs, 0x40);
			wr8_(0x3000 + ofs, data);
			sync_();

			bool ret = FST.FST4();
			if(ret) {
				wr8_(0x3000, 0x50);  // ステータス消去 
			}

			return !ret;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		flash_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  消去
			@param[in]	bank	バンク
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool erase(data_bank bank) const {
			uint16_t ofs;
			if(bank == data_bank::bank0) {
				ofs = 0;
			} else if(bank == data_bank::bank1) {
				ofs = 0x0400;
			} else {
				return false;
			}

			di();
			enable_(ofs);

			wr8_(0x3000,       0x20);  // ブロック消去
			wr8_(0x3000 + ofs, 0xd0);
			sync_();

			bool ret = FST.FST5();
			if(ret) {
				wr8_(0x3000, 0x50);  // ステータス消去 
			}

			wr8_(0x3000, 0xff);  // リードアレイコマンド（以降、通常読み出し）

			disable_(ofs);
			ei();

			return !ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  読み出し
			@param[in]	ofs	開始オフセット
			@param[in]	len	バイト数
			@param[out]	dst	先
		*/
		//-----------------------------------------------------------------//
		void read(uint16_t ofs, uint16_t len, uint8_t* dst) const {
			if(ofs >= 0x0800 || (ofs + len) > 0x0800) {
				return;
			}

			// ※リードアレイコマンド発行済みが前提
			for(uint16_t i = 0; i < len; ++i) {
				*dst = rd8_(0x3000 + ofs + i);
				++dst;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  読み出し
			@param[in]	ofs	開始オフセット
			@return データ
		*/
		//-----------------------------------------------------------------//
		uint8_t read(uint16_t ofs) const {
			if(ofs >= 0x0800) {
				return 0;
			}

			return rd8_(0x3000 + ofs);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  書き込み
			@param[in]	ofs	開始オフセット
			@param[in]	data	書き込みデータ
			@return 
		*/
		//-----------------------------------------------------------------//
		bool write(uint16_t ofs, uint8_t data) const {
			if(ofs >= 0x0800) {
				return false;
			}

			di();
			enable_(ofs);

			bool ret = write_(ofs, data);

			wr8_(0x3000, 0xff);  // リードアレイコマンド（以降、通常読み出し）

			disable_(ofs);
			ei();

			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  書き込み
			@param[in]	src ソース
			@param[in]	ofs	開始オフセット
			@param[in]	len	バイト数
		*/
		//-----------------------------------------------------------------//
		bool write(const uint8_t* src, uint16_t ofs, uint16_t len) const {
			if(ofs >= 0x0800 || (ofs + len) > 0x0800) {
				return false;
			}

			di();
			enable_(ofs);

			bool ret;
			for(uint16_t i = 0; i < len; ++i) {
				ret = write_(ofs + i, *src);
				if(!ret) break;
				++src;
			}

			wr8_(0x3000, 0xff);  // リードアレイコマンド（以降、通常読み出し）

			disable_(ofs);
			ei();

			return ret;
		}

	};
}

