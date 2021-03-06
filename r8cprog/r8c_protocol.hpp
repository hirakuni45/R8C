#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C プログラミング・プロトコル・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "rs232c_io.hpp"
#include <iostream>

namespace r8c {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	R8C プログラミング・プロトコル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class protocol {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	status 構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct status {
			uint8_t	SRD;
			uint8_t SRD1;
			status() : SRD(0), SRD1(0) { }
			int get_id_state() const { return (SRD1 >> 2) & 3; }
			bool get_SR4() const { return (SRD >> 4) & 1; }
			bool get_SR5() const { return (SRD >> 5) & 1; }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ID 構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct id_t {
			uint8_t	buff[8];
			id_t() { buff[7] = 0; }
			void fill() {
				for(int i = 0; i < 7; ++i) buff[i] = 0xff;
			}
			void clear() {
				for(int i = 0; i < 7; ++i) buff[i] = 0x00;
			}
//			const char* str() const { return static_cast<const char*>(&buff[0]); }
		};


	private:
		utils::rs232c_io	rs232c_;
		timeval 			tv_;

		bool			connection_;
		bool			verification_;

		uint32_t	baud_rate_;

		bool command_(uint8_t cmd) {
			bool f = rs232c_.send(static_cast<char>(cmd));
			rs232c_.sync_send();
			return f;
		}


		bool read_(void* dst, uint32_t length) {
			timeval tv;
			tv.tv_sec  = 0;
			tv.tv_usec = 500000;
			uint32_t len = rs232c_.recv(dst, length, tv);
			return len == length;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		protocol() : connection_(false), verification_(false), baud_rate_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	path	シリアルデバイスパス
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool start(const std::string& path) {
			if(!rs232c_.open(path, B9600)) {
				return false;
			}

			if(!rs232c_.enable_RTS(false)) {
				return false;
			}
			if(!rs232c_.enable_DTR(false)) {
				return false;
			}

			tv_.tv_sec  = 1;
			tv_.tv_usec = 0;

			connection_ = false;
			verification_ = false;

			return true;
		}


		void recv_out(int num) {
			for(int i = 0; i < num; ++i) {
				int ch = rs232c_.recv(tv_);
				if(ch > 0) {
					std::cout << static_cast<char>(ch);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コネクションの確立
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool connection() {
			for(int i = 0; i < 16; ++i) {
				if(!command_(0x00)) {
					return false;
				}
				usleep(20000);	// 20[ms]
			}

			if(!command_(0xB0)) {
				return false;
			}

			timeval tv;
			tv.tv_sec  = 1;
			tv.tv_usec = 0;
			int ch = rs232c_.recv(tv);
			if(ch != 0xB0) {
				return false;
			}
			connection_ = true;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	接続速度を変更する
			@param[in]	brate	ボーレート
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool change_speed(speed_t brate) {
			if(!connection_) return false;

			int cmd;
			switch(brate) {
			case B9600:
				cmd = 0xB0;
				baud_rate_ = 9600;
				break;
			case B19200:
				cmd = 0xB1;
				baud_rate_ = 19200;
				break;
			case B38400:
				cmd = 0xB2;
				baud_rate_ = 38400;
				break;
			case B57600:
				cmd = 0xB3;
				baud_rate_ = 57600;
				break;
			case B115200:
				cmd = 0xB4;
				baud_rate_ = 115200;
				break;
			default:
				return false;
			}
			if(!command_(cmd)) {
				return false;
			}
			int ch = rs232c_.recv(tv_);
			if(ch != cmd) {
				return false;
			}

			if(!rs232c_.change_speed(brate)) {
				return false;
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バージョン情報の取得
			@return バージョンを返す（エラーならempty）
		*/
		//-----------------------------------------------------------------//
		std::string get_version() {
			if(!connection_) return std::string();

			if(!command_(0xFB)) {
				return std::string();
			}
			char buff[9];
			if(!read_(buff, 8)) {
				return std::string();
			}
			buff[8] = 0;
			return std::string(buff);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステータス情報の取得
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool get_status(status& st) {
			if(!connection_) return false;

			if(!command_(0x70)) {
				return false;
			}

			char buff[2];
			if(!read_(buff, 2)) {
				return false;
			}

			st.SRD  = buff[0];
			st.SRD1 = buff[1];

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステータス情報のクリア
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool clear_status() {
			if(!connection_) return false;

			if(!command_(0x50)) {
				return false;
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ID 検査
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool id_inspection(const id_t& t) {
			if(!connection_) return false;

			uint8_t buff[12];
			buff[0] = 0xF5;
			buff[1] = 0xDF;
			buff[2] = 0xFF;
			buff[3] = 0x00;
			buff[4] = 0x07;
			for(int i = 0; i < 7; ++i) buff[5 + i] = t.buff[i];
			if(rs232c_.send(buff, 12) != 12) {
				return false;
			}
			rs232c_.sync_send();

			status st;
			if(!get_status(st)) {
				return false;
			}
			if(st.get_id_state() != 3) {
				return false;
			}

			verification_ = true;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リード・ページ
			@param[in]	address	アドレス
			@param[out]	dst	リード・データ
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool read_page(uint32_t address, uint8_t* dst) {
			if(!connection_) return false;
			if(!verification_) return false;

			uint8_t buff[3];
			buff[0] = 0xFF;
			buff[1] = (address >> 8) & 0xff;
			buff[2] = (address >> 16) & 0xff;
			if(rs232c_.send(buff, 3) != 3) {
				return false;
			}
			rs232c_.sync_send();

			// ボーレートから想定される実時間の２倍
			// 1.0f / static_cast<float>(baud_rate) * 10.0f * 256.0f / 1e-6 * 2.0f;
			return read_(dst, 256);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライト・ページ
			@param[in]	address	アドレス
			@param[in]	src	ライト・データ
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool write_page(uint32_t address, const uint8_t* src) {
			if(!connection_) return false;
			if(!verification_) return false;

			char buff[3];
			buff[0] = 0x41;
			buff[1] = (address >> 8) & 0xff;
			buff[2] = (address >> 16) & 0xff;
			if(rs232c_.send(buff, 3) != 3) {
				return false;
			}
			rs232c_.sync_send();

			if(rs232c_.send(src, 256) != 256) {
				return false;
			}
			rs232c_.sync_send();

			status st;
			if(!get_status(st)) {
				return false;
			}
			if(st.get_SR4() != 0) {
				return false;
			}

			return clear_status();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イレース・ページ
			@param[in]	address	アドレス
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool erase_page(uint32_t address) {
			if(!connection_) return false;
			if(!verification_) return false;

			char buff[4];
			buff[0] = 0x20;
			buff[1] = (address >> 8) & 0xff;
			buff[2] = (address >> 16) & 0xff;
			buff[3] = 0xD0;
			if(rs232c_.send(buff, 4) != 4) {
				return false;
			}
			rs232c_.sync_send();

			status st;
			if(!get_status(st)) {
				return false;
			}
			if(st.get_SR5() != 0) {
				return false;
			}

			return clear_status();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	終了
			@return エラー無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool end() {
			connection_ = false;
			verification_ = false;
			return rs232c_.close();
		}

	};

}
