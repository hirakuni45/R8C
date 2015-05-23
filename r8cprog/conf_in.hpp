#pragma once
//=====================================================================//
/*!	@file
	@brief	conf ファイルのパース
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "string_utils.hpp"
#include "file_io.hpp"
#include <boost/foreach.hpp>
#include <utility>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	conf ファイルのパースクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class conf_in {
	public:
		struct default_t {
			std::string	programmer_;
			std::string	device_;
			std::string port_;
			std::string speed_;
			std::string id_;

			bool analize(const std::string& s) {
				bool ok = true;
				utils::strings ss = utils::split_text(s, "=");
				if(ss.size() == 2) {
					if(ss[0] == "programmer") programmer_ = ss[1];
					else if(ss[0] == "device") device_ = ss[1];
					else if(ss[0] == "port") port_ = ss[1];
					else if(ss[0] == "speed") speed_ = ss[1];
					else if(ss[0] == "id") id_ = ss[1];
					else ok = false;
				} else {
					ok = false;
				}
				return ok;
			}
		};


		static std::string strip_space_(const std::string& in) {
			int first = 0;
			while(in[first] == ' ' || in[first] == '\t') {
				++first;
			}
			int last = in.size();
			do {
				--last;
			} while(in[last] == ' ' || in[last] == '\t') ;
			std::string out = in.substr(first, last - first + 1);
			return std::move(out);
		}

		struct programmer_t {
			std::string comment_;

			bool analize(const std::string& s) {
				bool ok = true;
				utils::strings ss = utils::split_text(s, "=");
				if(ss.size() == 2) {
					std::string cmd;
					utils::strip_char(ss[0], std::string(" \t"), cmd);
					if(cmd == "comment") comment_ = strip_space_(ss[1]);
					else ok = false;
				} else {
					ok = false;
				}
				return ok;
			}
		};


		struct device_t {
			std::string	group_;
			std::string	rom_;
			std::string ram_;
			std::string	comment_;
			std::string	rom_area_;
			std::string ram_area_;

			bool analize(const std::string& s) {
				bool ok = true;
				utils::strings ss = utils::split_text(s, "=");
				if(ss.size() == 2) {
					std::string cmd;
					utils::strip_char(ss[0], std::string(" \t"), cmd);
					if(cmd == "group") group_ = strip_space_(ss[1]);
					else if(cmd == "rom") rom_ = strip_space_(ss[1]);
					else if(cmd == "ram") ram_ = strip_space_(ss[1]);
					else if(cmd == "comment") comment_ = strip_space_(ss[1]);
					else if(cmd == "rom-area") rom_area_ = strip_space_(ss[1]);
					else if(cmd == "data-area") ram_area_ = strip_space_(ss[1]);
					else ok = false;
				} else {
					ok = false;
				}
				return ok;
			}
		};

	private:
		default_t	default_;
		programmer_t	programmer_;
		device_t	device_;

		int			ana_mode_;
		std::string	name_;
		std::string body_;

		bool check_symbol_(char ch) {
			if(ch >= '0' && ch <= '9') return true;
			if(ch >= 'A' && ch <= 'Z') return true;
			if(ch >= 'a' && ch <= 'z') return true;
			if(ch == '_') return true;
			return false;
		}

		bool symbol_analize_(const std::string& org) {
			BOOST_FOREACH(char ch, org) {
				if(ch == '\n' || ch == '\r') continue;
				if(ana_mode_ == 0) {
					if(ch == ' ' || ch == '\t') continue;
					if(ch == '{') {
						++ana_mode_;
					} else if(check_symbol_(ch)) {
						name_ += ch;
					} else {
						return false;
					}
				} else if(ana_mode_ == 1) {
					if(ch == '}') {
						++ana_mode_;
					} else {
						body_ += ch;
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
		conf_in() :
			ana_mode_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	conf ファイルの読み込みとパース
			@param[in]	file	ファイル名
			@return 読み込み成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& file) {

			utils::file_io fio;
			if(!fio.open(file, "rb")) {
				return false;
			}

			std::string line;
			int mode = -1;
			uint32_t lno = 0;
			uint32_t err = 0;
			while(fio.get_line(line)) {
				++lno;
				std::string org = line;
				line.clear();

				if(org.empty()) continue;

				std::string cmd;
				utils::strip_char(org, std::string(" \t"), cmd);
				if(cmd.empty()) ;
				else if(cmd[0] == '#') {
					continue;
				} else if(cmd == "[DEFAULT]") {
					mode = 0;
					continue;
				} else if(cmd == "[PROGRAMMER]") {
					mode = 1;
					name_.clear();
					body_.clear();
					ana_mode_ = 0;
					continue;
				} else if(cmd == "[DEVICE]") {
					mode = 2;
					name_.clear();
					body_.clear();
					ana_mode_ = 0;
					continue;
				} else if(cmd[0] == '[') {
					++err;
					break;
				}

				if(mode == 0) {
					if(!default_.analize(cmd)) {
						++err;
						std::cerr << "(" << lno << ") ";
						std::cerr << "Default section error: " << org << "'" << std::endl; 
						break;
					}
				} else if(mode == 1) {
					if(!symbol_analize_(org)) {
						std::cerr << "(" << lno << ") ";
						std::cerr << "Programmer section error: " << org << "'" << std::endl; 
						break;
					}
					if(ana_mode_ == 2) {
						if(default_.programmer_ == name_) {
							if(!programmer_.analize(body_)) {
								std::cerr << "(" << lno << ") ";
								std::cerr << "Programmer body error: '" << org << "'";
								std::cerr << std::endl; 
								break;
							}
///							std::cout << name_ << ": " << std::endl;
///							std::cout <<  programmer_.comment_ << std::endl;
						}
						ana_mode_ = 0;
						name_.clear();
						body_.clear();
					}
				} else if(mode == 2) {
#if 0
					if(!symbol_analize_(org)) {
						std::cerr << "(" << lno << ") ";
						std::cerr << "Device section error: " << org << "'" << std::endl; 
					}
					if(ana_mode_ == 2) {
						if(default_.device_ == name_) {
							std::cout << name_ << std::endl;
							std::cout << body_ << std::endl;
							if(!device_.analize(body_)) {
								std::cerr << "(" << lno << ") ";
								std::cerr << "Device body error: '" << org << "'";
								std::cerr << std::endl; 
								break;
							}
							std::cout << name_ << ": " << std::endl;
							std::cout << device_.comment_ << std::endl;
						}
						ana_mode_ = 0;
						name_.clear();
						body_.clear();
					}
#endif
				}
			}
			fio.close();

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	[DEFAULT]の取得
			@return [DEFAULT]情報
		*/
		//-----------------------------------------------------------------//
		const default_t& get_default() const { return default_; }
	};
}
