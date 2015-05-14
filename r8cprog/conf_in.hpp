#pragma once
//=====================================================================//
/*!	@file
	@brief	conf ファイルのパース
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "string_utils.hpp"
#include "file_io.hpp"

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
		};

	private:
		default_t	default_;


		void default_analize_(const std::string& org, const std::string& cmd) {
			utils::strings ss = utils::split_text(cmd, "=");
			if(ss.size() == 2) {
//				std::cout << ss[0] << " : " << ss[1] << std::endl;
				if(ss[0] == "programmer") default_.programmer_ = ss[1];
				else if(ss[0] == "device") default_.device_ = ss[1];
				else if(ss[0] == "port") default_.port_ = ss[1];
				else if(ss[0] == "speed") default_.speed_ = ss[1];
			}
		}

		void programmer_analize_(const std::string& org, const std::string& cmd) {
		}

		void device_analize_(const std::string& org, const std::string& cmd) {
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		conf_in() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	conf ファイルの読み込みとパース
			@param[in]	file	ファイル名
			@return 読み込み成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& file) {
			utils::file_io fio;

//			std::cout << "Conf: '" << file << "'" << std::endl;

			if(!fio.open(file, "rb")) {
				return false;
			}

			std::string line;
			int mode = -1;
			uint32_t lno = 0;
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
//					std::cout << "DEFAULT: " << lno << std::endl;
					continue;
				} else if(cmd == "[PROGRAMMER]") {
					mode = 1;
//					std::cout << "PROGRAMMER: " << lno << std::endl;
					continue;
				} else if(cmd == "[DEVICE]") {
					mode = 2;
//					std::cout << "DEVICE: " << lno << std::endl;
					continue;
				}

				if(mode == 0) {
					default_analize_(org, cmd);
				} else if(mode == 1) {
					programmer_analize_(org, cmd);
				} else if(mode == 2) {
					device_analize_(org, cmd);
				}
			}
			fio.close();

			return true;
		}

		const default_t& get_default() const { return default_; }
	};
}
