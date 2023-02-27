#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C programmer クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "r8c_protocol.hpp"
#include "string_utils.hpp"
#include <set>
#include <boost/format.hpp>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@brief	r8c_prog クラス
 */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
class r8c_prog {
	bool	verbose_;
	bool	progress_;

	r8c::protocol	proto_;
	std::string		ver_;
	r8c::protocol::id_t	id_;
	std::set<uint32_t>	set_;

public:
	r8c_prog(bool verbose, bool progress) : verbose_(verbose), progress_(progress) {
		id_.fill();
	}

	bool get_progress() const { return progress_; }

	const r8c::protocol::id_t& get_id() const { return id_; }

	bool set_id(const std::string& text) {
		utils::strings ss = utils::split_text(text, ":, \t");
		bool err = false;
		if(ss.size() != 7) {
			err = true;
		} else {
			for(int i = 0; i < 7; ++i) {
				int val;
				if(utils::string_to_int(ss[i], val)) {
					if(val >= 0 && val <= 255) {
						id_.buff[i] = val;
					} else {
						err = true;
						break;
					}
				} else {
					err = true;
					break;
				}
			}
		}
		if(err) {
			return false;
		}
		return true;
	}


	bool start(const std::string& path, const std::string& brate) {
		using namespace r8c;

		// 開始
		if(!proto_.start(path)) {
			std::cerr << std::endl;
			std::cerr << "Can't open path: '" << path << "'" << std::endl;
			return false;
		}

		// コネクション
		if(!proto_.connection()) {
			proto_.end();
			std::cerr << std::endl;
			std::cerr << "Connection device error..." << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "Connection OK." << std::endl;
		}

		// ボーレート変更
		int val;
		if(!utils::string_to_int(brate, val)) {
			std::cerr << std::endl;
			std::cerr << "Baud rate conversion error: '" << brate << std::endl;
			return false;
		}
		speed_t speed;
		switch(val) {
		case 9600:   speed = B9600;   break;
		case 19200:  speed = B19200;  break;
		case 38400:  speed = B38400;  break;
		case 57600:  speed = B57600;  break;
		case 115200: speed = B115200; break;
		default:
			proto_.end();
			std::cerr << "Baud rate error: " << brate << std::endl;
			return false;
		}

		if(!proto_.change_speed(speed)) {
			proto_.end();
			std::cerr << std::endl;
			std::cerr << "Change speed error: " << brate << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "Change speed OK: " << brate << " [bps]" << std::endl;
		}

		// バージョンの取得
		ver_ = proto_.get_version();
		if(ver_.empty()) {
			proto_.end();
			std::cerr << std::endl;
			std::cerr << "Get version error..." << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "Version: '" << ver_ << "'" << std::endl;
		}

		// ID チェック認証
		if(!proto_.id_inspection(id_)) {
			std::cerr << std::endl;
			std::cerr << "ID error: ";
			for(int i = 0; i < 7; ++i) {
				std::cerr << std::hex << std::setw(2) << std::uppercase << std::setfill('0')
						  << "0x" << static_cast<int>(id_.buff[i]) << ' ';
			}
			proto_.end();
			std::cerr << std::dec << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "ID OK: ";
			for(int i = 0; i < 7; ++i) {
				std::cout << std::hex << std::setw(2) << std::uppercase << std::setfill('0')
						  << "0x" << static_cast<int>(id_.buff[i]) << ' ';
			}
			std::cout << std::endl;
		}

		set_.clear();

		return true;
	}


	bool read(uint32_t top, uint8_t* data) {
		if(!proto_.read_page(top, data)) {
			std::cerr << std::endl;
			std::cerr << "Read error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
			return false;
		}
		return true;
	}


	bool erase_page(uint32_t top) {
		uint32_t area = 1024;
		if(top >= 0x8000) area = 4096;

		uint32_t adr = top & ~(area - 1);
		if(set_.find(adr) != set_.end()) {
			return true;
		}
		set_.insert(adr);

		// イレース
		if(!proto_.erase_page(top)) {
			std::cerr << std::endl;
			std::cerr << "Erase error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
			return false;
		}
		return true;
	}


	bool write(uint32_t top, const uint8_t* data) {
		using namespace r8c;
		// ページ書き込み
		if(!proto_.write_page(top, data)) {
			std::cerr << std::endl;
   			std::cerr << "Write error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
			return false;
		}
		return true;
   	}


	bool verify_page(uint32_t top, const uint8_t* data) {
		// ページ読み込み
		uint8_t tmp[256];
   		if(!proto_.read_page(top, tmp)) {
			std::cerr << std::endl;
   			std::cerr << "Read error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
   			return false;
   		}

		uint32_t erc = 0;
		for(int i = 0; i < 256; ++i) {
			if(data[i] != tmp[i]) {
				if(erc == 0) {
					std::cerr << std::endl;
				}
   				std::cerr << boost::format("Verify error at 0x%06X: 0x%02X -> 0x%02X")
					% (top + i) % static_cast<uint32_t>(data[i]) % static_cast<uint32_t>(tmp[i]) << std::endl;
				++erc;
			}
		}
		return erc == 0;
	}

	void end() {
		proto_.end();
	}
};
