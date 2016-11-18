#pragma once
//=====================================================================//
/*!	@file
	@brief	ソフトウェア SPI テンプレートクラス @n
			Copyright 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ソフトウェア SPI テンプレートクラス
		@param[in]	CLK	クロック・クラス
		@param[in]	OUT	出力・クラス
		@param[in]	INP	入力・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class CLK, class OUT, class INP>
	class spi_io {

		uint8_t	delay_;

		inline void clock_() {
			uint8_t n = delay_;
			while(n > 3) { --n; asm("nop"); }
			CLK::P = 1;
			n = delay_;
			while(n > 0) { --n; asm("nop"); }
			CLK::P = 0;
		} 

		void send_(uint8_t d) {
			if(d & 0x80) OUT::P = 1; else OUT::P = 0;	/* bit7 */
			clock_();
			if(d & 0x40) OUT::P = 1; else OUT::P = 0;	/* bit6 */
			clock_();
			if(d & 0x20) OUT::P = 1; else OUT::P = 0;	/* bit5 */
			clock_();
			if(d & 0x10) OUT::P = 1; else OUT::P = 0;	/* bit4 */
			clock_();
			if(d & 0x08) OUT::P = 1; else OUT::P = 0;	/* bit3 */
			clock_();
			if(d & 0x04) OUT::P = 1; else OUT::P = 0;	/* bit2 */
			clock_();
			if(d & 0x02) OUT::P = 1; else OUT::P = 0;	/* bit1 */
			clock_();
			if(d & 0x01) OUT::P = 1; else OUT::P = 0;	/* bit0 */
			clock_();
		}

		uint8_t recv_(bool out = 1) {
			OUT::P = out;
			uint8_t r = 0;
			if(INP::P()) ++r;  // bit7
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit6
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit5
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit4
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit3
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit2
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit1
			clock_();
			r <<= 1; if(INP::P()) ++r;  // bit0
			clock_();
			return r;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		spi_io() : delay_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	delay	クロック遅延（速度）
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(uint8_t delay = 0) {
			delay_ = delay;
			CLK::DIR = 1;
			OUT::DIR = 1;
			INP::DIR = 0;
			return true;
		}


		//----------------------------------------------------------------//
		/*!
			@brief	リード
			@return 読み出しデータ
		*/
		//----------------------------------------------------------------//
		uint8_t xchg() { return recv_(); }


		//----------------------------------------------------------------//
		/*!
			@brief	リード・ライト
			@param[in]	d	書き込みデータ
			@return 読み出しデータ
		*/
		//----------------------------------------------------------------//
		uint8_t xchg(uint8_t d)
		{
			uint8_t r = 0;
			if(INP::P()) ++r;  // bit7
			if (d & 0x80) OUT::P = 1; else OUT::P = 0;	// bit7
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit6
			if (d & 0x40) OUT::P = 1; else OUT::P = 0;	// bit6
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit5
			if (d & 0x20) OUT::P = 1; else OUT::P = 0;	// bit5
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit4
			if (d & 0x10) OUT::P = 1; else OUT::P = 0;	// bit4
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit3
			if (d & 0x08) OUT::P = 1; else OUT::P = 0;	// bit3
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit2
			if (d & 0x04) OUT::P = 1; else OUT::P = 0;	// bit2
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit1
			if (d & 0x02) OUT::P = 1; else OUT::P = 0;	// bit1
			clock_();

			r <<= 1;
			if(INP::P()) ++r;  // bit0
			if (d & 0x01) OUT::P = 1; else OUT::P = 0;	// bit0
			clock_();

			return r;
		}


		//----------------------------------------------------------------//
		/*!
			@brief	スキップ
			@param[in]	count	スキップ数
		*/
		//----------------------------------------------------------------//
		void skip(uint16_t count = 1) {
			OUT::P = 1;
			while(count > 0) {
				clock_();
				clock_();
				clock_();
				clock_();
				clock_();
				clock_();
				clock_();
				clock_();
				--count;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  シリアル送信
			@param[in]	src		送信ソース
			@param[in]	size	送信サイズ
		*/
		//-----------------------------------------------------------------//
		void send(const uint8_t* src, uint16_t size)
		{
			auto end = src + size;
			while(src < end) {
				send_(*src);
				++src;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  シリアル受信
			@param[out]	dst	受信先
			@param[in]	cnt	受信サイズ
		*/
		//-----------------------------------------------------------------//
		void recv(uint8_t* dst, uint16_t size)
		{
			auto end = dst + size;
			while(dst < end) {
				*dst = recv_();
				++dst;
			}
		}

	};
}
