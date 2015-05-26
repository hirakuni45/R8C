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

	public:
		struct unit {
			std::string	symbol_;
			std::string body_;
			uint32_t	lno_;
			unit() : lno_(0) { }
			explicit unit(const std::string& symbol, const std::string& body, uint32_t lno) :
				symbol_(symbol), body_(body), lno_(lno) { }
		};

		typedef std::vector<unit>	units;

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

		struct programmer_t {
			std::string comment_;

			bool analize(const units& us) {
				BOOST_FOREACH(const unit& u, us) {
					if(u.symbol_ == "comment") comment_ = u.body_;
					else {
						std::cerr << boost::format("(%d) Programmer error: '") % u.lno_;
						std::cerr << u.symbol_ << "', '" << u.body_ << "'" << std::endl;
						return false;
					}
				}
				return true;
			}
		};


		struct device_t {
			std::string	group_;
			std::string ram_;
			std::string data_;
			std::string	rom_;
			std::string	comment_;
			std::string	rom_area_;
			std::string ram_area_;

			bool analize(const units& us) {
				BOOST_FOREACH(const unit& u, us) {
					if(u.symbol_ == "group") group_ = u.body_;
					else if(u.symbol_ == "rom") rom_ = u.body_;
					else if(u.symbol_ == "data") data_ = u.body_;
					else if(u.symbol_ == "ram") ram_ = u.body_;
					else if(u.symbol_ == "comment") comment_ = u.body_;
					else if(u.symbol_ == "rom-area") rom_area_ = u.body_;
					else if(u.symbol_ == "data-area") ram_area_ = u.body_;
					else {
						std::cerr << boost::format("(%d) Device error: '") % u.lno_;
						std::cerr << u.symbol_ << "', '" << u.body_ << "'" << std::endl;
						return false;
					}
				}
				return true;
			}
		};

	private:
		default_t		default_;
		programmer_t	programmer_;
		device_t		device_;

		enum class ana_mode {
			name,
			symbol,
			body,
			text,
			fin
		};

		ana_mode	ana_mode_;
		std::string	name_;
		std::string symbol_;
		std::string body_;

		units		units_;

		void reset_ana_() {
			ana_mode_ = ana_mode::name;
			name_.clear();
			symbol_.clear();
			body_.clear();

			units_.clear();
		}


		bool check_symbol_(char ch) {
			if(ch >= '0' && ch <= '9') return true;
			if(ch >= 'A' && ch <= 'Z') return true;
			if(ch >= 'a' && ch <= 'z') return true;
			if(ch == '_' || ch == '-') return true;
			return false;
		}


		bool analize_(const std::string& org, uint32_t lno) {
			if(org.empty()) return true;

			BOOST_FOREACH(char ch, org) {
				switch(ana_mode_) {
				case ana_mode::name:
					if(ch == ' ' || ch == '\t') ;
					else if(ch == '{') {
						ana_mode_ = ana_mode::symbol;
					} else if(check_symbol_(ch)) {
 						name_ += ch;
					} else {  // error name character
						return false;
					}
					break;
				case ana_mode::symbol:
					if(ch == ' ' || ch == '\t') ;
					else if(ch == '}') {
						ana_mode_ = ana_mode::fin;
					} else if(ch == '=') {
						ana_mode_ = ana_mode::body;
					} else if(check_symbol_(ch)) {
						symbol_ += ch;
					} else {
						return false; // error symbol character
					}
					break;
				case ana_mode::body:
					if(ch == ' ' || ch == '\t') ;  // TAB, Sp を無視
					else if(ch == '}') {
						ana_mode_ = ana_mode::fin;
					} else {
						if(ch == '"') {
							ana_mode_ = ana_mode::text;
						} else if(ch >= ' ') {
							body_ += ch;
						}
					}
					break;
				case ana_mode::text:
					if(ch == '"') {
						ana_mode_ = ana_mode::body;
					} else {
						body_ += ch;
					}
					break;
				default:
					break;
				}
			}

			if(ana_mode_ == ana_mode::symbol) ;
			else if(ana_mode_ == ana_mode::body || ana_mode_ == ana_mode::fin) {
				char back = 0;
				if(!body_.empty()) back = body_.back();
				if(back == 0) ;
				else if(back != ',') {  // ',' 以外なら完了

					if(!symbol_.empty() && !body_.empty()) {
///						std::cout << symbol_ << ": '" << body_ << "'" << std::endl;
						units_.emplace_back(symbol_, body_, lno);
						symbol_.clear();
						body_.clear();
						if(ana_mode_ == ana_mode::body) {
							ana_mode_ = ana_mode::symbol;
						}
					}
				}
			} else {
				return false;
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
			ana_mode_(ana_mode::name) { }


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
					reset_ana_();
					continue;
				} else if(cmd == "[DEVICE]") {
					mode = 2;
					reset_ana_();
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
					if(!analize_(org, lno)) {
						std::cerr << "(" << lno << ") ";
						std::cerr << "Programmer section error: '" << org << "'" << std::endl; 
						break;
					}
					if(ana_mode_ == ana_mode::fin) {
						if(default_.programmer_ == name_) {
							if(!programmer_.analize(units_)) {
								break;
							}
						}
						reset_ana_();
					}
				} else if(mode == 2) {
					if(!analize_(org, lno)) {
						std::cerr << "(" << lno << ") ";
						std::cerr << "Device section error: '" << org << "'" << std::endl; 
					}
					if(ana_mode_ == ana_mode::fin) {
						if(default_.device_ == name_) {
							if(!device_.analize(units_)) {
								break;
							}
						}
						reset_ana_();
					}
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


		//-----------------------------------------------------------------//
		/*!
			@brief	[PROGRAMMER]の取得
			@return [PROGRAMMER]情報
		*/
		//-----------------------------------------------------------------//
		const programmer_t& get_programmer() const { return programmer_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	[DEVICE]の取得
			@return [DEVICE]情報
		*/
		//-----------------------------------------------------------------//
		const device_t& get_device() const { return device_; }

	};
}
