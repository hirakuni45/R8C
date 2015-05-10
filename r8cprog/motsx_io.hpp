#pragma once
//=====================================================================//
/*!	@file
	@brief	モトローラーＳフォーマット入出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <string>
#include "file_io.hpp"
#include <iomanip>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Motolora Sx I/O クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class motsx_io {

		uint32_t	amin_;
		uint32_t	amax_;

		std::vector<uint8_t>	memory_;

		bool write_byte_(uint32_t address, uint8_t val) {
			if(address < memory_.size()) {
				memory_[address] = val;
				return true;
			} else {
				return false;
			}
		}

		bool load_(utils::file_io& fio) {
			amin_ = 0xffffffff;
			amax_ = 0x00000000;

			uint32_t value = 0;
			uint32_t type = 0;
			uint32_t length = 0;
			uint32_t address = 0;
			uint32_t sum = 0;
			int vcnt = 0;

			bool toend = false;
			int mode = 0;

			while(1) {
				char ch;
				if(!fio.get_char(ch)) {
					break;
				}

			   	if(ch == ' ') {
			   	} else if(ch == 0x0d || ch == 0x0a) {
				   	if(toend) break;
			   	} else if(mode == 0 && ch == 'S') {
			   		mode = 1;
			   		value = vcnt = 0;
			   	} else if(ch >= '0' && ch <= '9') {
			   		value <<= 4;
			   		value |= ch - '0';
			   		++vcnt;
			   	} else if(ch >= 'A' && ch <= 'F') {
			   		value <<= 4;
			   		value |= ch - 'A' + 10;
			   		++vcnt;
			   	} else {
					std::cerr << "S format illegual character: '";
			   		if(ch >= 0x20 && ch <= 0x7f) {
						std::cerr << ch;
			   		} else {
						std::cerr << std::hex << std::setw(2)
								  << static_cast<int>(ch) << std::dec;
			   		}
					std::cerr << "'" << std::endl;
			   		return false;
			   	}

			   	if(mode == 1) {		// タイプ取得
			   		if(vcnt == 1) {
			   			type = value;
			   			mode = 2;
			   			value = vcnt = 0;
			   		}
			   	} else if(mode == 2) {	// レングス取得
			   		if(vcnt == 2) {
			   			length = value;
			   			sum = value;
			   			mode = 3;
			   			value = vcnt = 0;
			   		}
			   	} else if(mode == 3) {	// アドレス取得
			   		int alen = 0;
			   		if(type == 0) {
			   			alen = 4;
			   		} else if(type == 1) {
			   			alen = 4;
			   		} else if(type == 2) {
			   			alen = 6;
			   		} else if(type == 3) {
			   			alen = 8;
			   		} else if(type == 5) {
			   			alen = 4;
			   		} else if(type == 7) {
			   			alen = 8;
			   		} else if(type == 8) {
			   			alen = 6;
			   		} else if(type == 9) {
			   			alen = 4;
			   		} else {
			   			return false;
			   		}
			   		if(vcnt == alen) {
			   			address = value;
			   			if(type >= 1 && type <= 3) {
			   				if(amin_ > address) amin_ = address;
			   			}
			   			alen >>= 1;
			   			length -= alen;
			   			length -= 1;	// SUM の分サイズを引く
			   			// ('.');
			   			while(alen > 0) {
			   				sum += value;
			   				value >>= 8;
			   				--alen;
			   			}
				   		if(type >= 1 && type <= 3) {
				   			mode = 4;
				   		} else if(type >= 7 && type <= 9) {
				   			mode = 5;
				   		} else {
				   			mode = 4;
				   		}
				   		value = vcnt = 0;
				   	}
			   	} else if(mode == 4) {	// データ・レコード
			   		if(vcnt >= 2) {
			   			if(type >= 1 && type <= 3) {
			   				write_byte_(address, value);
			   				if(amax_ < address) amax_ = address;
			   				++address;
			   			}
			   			sum += value;
			   			value = vcnt = 0;
			   			--length;
			   			if(length == 0) {
			   				mode = 5;
			   			}
			   		}
			   	} else if(mode == 5) {	// SUM
			   		if(vcnt >= 2) {
			   			value &= 0xff;
			   			sum ^= 0xff;
			   			sum &= 0xff;
			   			if(sum != value) {	// SUM エラー
							std::cerr << "S format SUM error: ";
							std::cerr << std::hex << std::setw(2)
									  << static_cast<int>(value)
									  << " -> "
									  << static_cast<int>(sum)
									  << std::dec << std::endl;
			   				return false;
			   			} else {
			   				if(type >= 7 && type <= 9) {
			   					toend = true;
			   				}
			   				mode = 0;
			   				value = vcnt = 0;
			   			}
			   		}
			   	}
		   	}
		   	return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		motsx_io() : amin_(0xffffffff), amax_(0x00000000) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	path	ファイルパス
			@return エラー無しなら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path) {
			utils::file_io fio;
			if(!fio.open(path, "rb")) {
				return false;
			}

			for(int i = 0; i < 65536; ++i) {
				memory_.push_back(0xff);
			}

			if(!load_(fio)) {
				return false;
			}

			fio.close();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブ
			@param[in]	path	ファイルパス
			@return エラー無しなら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path) {
			return false;
		}


		uint32_t get_min() const { return amin_; }

		uint32_t get_max() const { return amax_; }

		const std::vector<uint8_t>& get_memory() const { return memory_; }

	};
}

