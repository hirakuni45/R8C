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

		void default_(const std::string& org) {
		}

		void programmer_(const std::string& org) {
		}

		void device_(const std::string& org) {
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		conf_in() { }


		bool load(const std::string& file) {
			utils::file_io fio;

			if(!fio.open(file, "rb")) {
				return false;
			}

			std::string line;
			int mode = -1;
			uint32_t lno = 0;
			while(fio.get_line(line)) {
				++lno;
//				std::cout << line << std::endl;

				strings ss = split_text(line, " \t", 1);
				std::string org = line;
				line.clear();

				if(ss.size() == 0) {
					continue;
				}

				const std::string& s = ss[0];
				if(s.empty()) {
					continue;
				}

				if(s[0] == '#') {
					continue;
				} else if(s == "[DEFAULT]") {
					mode = 0;
					std::cout << "DEFAULT: " << lno << std::endl;
				} else if(s == "[PROGRAMMER]") {
					mode = 1;
					std::cout << "PROGRAMMER: " << lno << std::endl;
				} else if(s == "[DEVICE]") {
					mode = 2;
					std::cout << "DEVICE: " << lno << std::endl;
				}

				if(mode == 0) {
					default_(org);
				} else if(mode == 1) {
					programmer_(org);
				} else if(mode == 2) {
					device_(org);
				}

				line.clear();
			}
			fio.close();

			return true;
		}
	};
}
