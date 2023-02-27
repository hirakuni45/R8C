//=====================================================================//
/*!	@file
	@brief	Renesas R8C Series Programmer (Flash Writer)
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <iomanip>
#include <random>
#include <utility>
#include <cstdlib>
#include "r8c_prog.hpp"
#include "motsx_io.hpp"
#include "conf_in.hpp"
#include "area.hpp"
#include <boost/format.hpp>

namespace {

	const std::string version_ = "0.84b";
	const std::string conf_file = "r8c_prog.conf";
	const uint32_t progress_num_ = 50;
	const char progress_cha_ = '#';

	utils::conf_in conf_in_;
	utils::motsx_io motsx_;

	const std::string get_current_path_(const std::string& exec)
	{
		std::string exec_path;
#ifdef WIN32
		{
			auto tmp = utils::sjis_to_utf8(exec);
			exec_path = utils::convert_delimiter(tmp, '\\', '/');
		}
#else
		exec_path = exec;
#endif
		std::string spch;
		std::string env;
		{
#ifdef WIN32
			auto tmp = sjis_to_utf8(getenv("PATH"));
			env = utils::convert_delimiter(tmp, '\\', '/');
			spch = ';';
#else
			env = getenv("PATH");
			spch = ':';
#endif
		}
		utils::strings ss = utils::split_text(env, spch);
		for(const auto& s : ss) {
			std::string path = s + '/' + utils::get_file_name(exec_path);
			if(utils::probe_file(path)) {
				return s;
			}
		}

		return std::string("");
	}


	struct page_t {
		uint32_t	n = 0;
		uint32_t	c = 0;
	};


	void progress_(uint32_t pageall, page_t& page)
	{
		uint32_t pos = progress_num_ * page.n / pageall;
		for(uint32_t i = 0; i < (pos - page.c); ++i) {
			std::cout << progress_cha_ << std::flush;
		}
		page.c = pos;
	}


	struct options {
		bool verbose = false;

		std::string platform;

		std::string	inp_file;

		std::string	device;
		bool	dv = false;

		std::string	com_speed = "57600";
		bool	br = false;

		std::string com_path;
		std::string com_name;
		bool	dp = false;

		std::string id_val = "ff:ff:ff:ff:ff:ff:ff";
		bool	id = false;

		utils::areas area_val;
		bool	area = false;

		bool	read = false;
		bool	erase = false;
		bool	write = false;
		bool	verify = false;
		bool	device_list = false;
		bool	progress = false;
		bool	erase_data = false;
		bool	erase_rom = false;
		bool	help = false;


		bool set_area_(const std::string& s) {
			utils::strings ss = utils::split_text(s, ",");
			std::string t;
			if(ss.empty()) t = s;
			else if(ss.size() >= 1) t = ss[0];
			uint32_t org = 0;
			bool err = false;
			if(!utils::string_to_hex(t, org)) {
				err = true;
			}
			uint32_t end = org + 256;
			if(ss.size() >= 2) {
				if(!utils::string_to_hex(ss[1], end)) {
					err = true;
				}
			}
			if(err) {
				return false;
			}
			area_val.emplace_back(org, end);
			return true;
		}


		bool set_str(const std::string& t) {
			bool ok = true;
			if(br) {
				com_speed = t;
				br = false;
			} else if(dv) {
				device = t;
				dv = false;
			} else if(dp) {
				com_path = t;
				dp = false;
			} else if(id) {
				id_val = t;
				id = false;
			} else if(area) {
				if(!set_area_(t)) {
					ok = false;
				}
				area = false;
			} else {
				inp_file = t;
			}
			return ok;
		}


		bool area_check(uint32_t adr) {
			for(const auto& t : area_val) {
				if(t.org_ <= adr && adr <= t.end_) return true;
			}
			return false;
		}
	};


	void help_(const std::string& cmd)
	{
		using namespace std;

		std::string c = utils::get_file_base(cmd);

		cout << "Renesas R8C Series Programmer Version " << version_ << endl;
		cout << "Copyright (C) 2015, Hiramatsu Kunihito (hira@rvf-rc45.net)" << endl;
		cout << "usage:" << endl;
		cout << c << "[options] [mot file] ..." << endl;
		cout << endl;
		cout << "Options :" << endl;
		cout << "-d, --device=DEVICE\t\tSpecify device name" << endl;
		cout << "-e, --erase\t\t\tPerform a device erase to a minimum" << endl;
		cout << "    --erase-all, --erase-chip\tPerform rom and data flash erase" << endl;
		cout << "    --erase-rom\t\t\tPerform rom flash erase" << endl;
		cout << "    --erase-data\t\tPerform data flash erase" << endl;
		cout << "-i, --id=xx:xx:xx:xx:xx:xx:xx\tSpecify protect ID" << endl;
//		cout << "-p, --programmer=PROGRAMMER\tSpecify programmer name" << endl;
		cout << "-P, --port=PORT\t\t\tSpecify serial port" << endl;
//		cout << "-q\t\t\t\tQuell progress output" << endl;
		cout << "-a, --area=ORG,END\t\tSpecify read area" << endl;
		cout << "-r, --read\t\t\tPerform data read" << endl;
		cout << "-s, --speed=SPEED\t\tSpecify serial speed" << endl;
		cout << "-v, --verify\t\t\tPerform data verify" << endl;
		cout << "    --device-list\t\tDisplay device list" << endl;
//		cout << "    --programmer-list\t\tDisplay programmer list" << endl;
		cout << "-V, --verbose\t\t\tVerbose output" << endl;
		cout << "-w, --write\t\t\tPerform data write" << endl;
		cout << "    --progress\t\t\tdisplay Progress output" << endl;
		cout << "-h, --help\t\t\tDisplay this" << endl;
//		cout << "    --version\t\t\tDisplay version No." << endl;
	}


	bool erase_(r8c_prog& prog, const utils::areas& as)
	{
		bool noerr = true;
		page_t page;
		for(const auto& t : as) {
   			if(!prog.erase_page(t.org_)) {
   				noerr = false;
				break;
			}
   			++page.n;
			if(prog.get_progress()) progress_(as.size(), page);
		}
		if(prog.get_progress()) std::cout << std::endl << std::flush;

		return noerr;
	}


	void dump_areas_(utils::motsx_io& motr, const utils::areas& as)
	{
		for(const auto& t : as) {
			uint32_t org = t.org_;
			uint32_t end = t.end_;
			while(org <= end) {
				const utils::motsx_io::array& a = motr.get_memory(org);
				uint32_t len = 256 - (org & 255);
				bool cr = false;
				if((org + len) > end) {
					len = end - org + 1;
					cr = true;
				}
				uint32_t ffcnt = 0;
				for(uint32_t i = 0; i < len; ++i) {
					if(a[(org + i) & 255] == 0xff) ++ffcnt;
				}
				if(ffcnt != len) {
////					dump_(org, len, &a[org & 255]);
					if(cr) std::cout << std::endl;
				}
				org |= 0xff;
				++org;
			}
		}
	}
}


int main(int argc, char* argv[])
{
	if(argc == 1) {
		help_(argv[0]);
		return 0;
	}

	options opts;

	// 設定ファイルの読み込み
	std::string conf_path;
	if(utils::probe_file(conf_file)) {  // カレントにあるか？
		conf_path = conf_file;
	} else {  // コマンド、カレントから読んでみる
		conf_path = get_current_path_(argv[0]) + '/' + conf_file;
	}

	if(conf_in_.load(conf_path)) {
		auto defa = conf_in_.get_default();
		opts.device = defa.device_;
#ifdef __CYGWIN__
		opts.platform = "Cygwin";
		opts.com_path = defa.port_win_;
		opts.com_speed = defa.speed_win_;
#endif
#ifdef __APPLE__
		opts.platform = "OS-X";
		opts.com_path = defa.port_osx_;
		opts.com_speed = defa.speed_osx_;
#endif
#ifdef __linux__
		opts.platform = "Linux";
		opts.com_path = defa.port_linux_;
		opts.com_speed = defa.speed_linux_;
#endif
		if(opts.com_path.empty()) {
			opts.com_path = defa.port_;
		}
		if(opts.com_speed.empty()) {
			opts.com_speed = defa.speed_;
		}
		opts.id_val = defa.id_;

#if 0
		if(0) {
			const utils::conf_in::programmer_t& pt = conf.get_programmer();
			std::cout << pt.comment_ << std::endl;
		}
		if(0) {
			const utils::conf_in::device_t& dt = conf.get_device();
			std::cout << dt.group_ << std::endl;
			std::cout << dt.ram_ << std::endl;
			std::cout << dt.rom_ << std::endl;
			std::cout << dt.data_ << std::endl;
			std::cout << dt.comment_ << std::endl;
			std::cout << dt.rom_area_.size() << std::endl;
			std::cout << dt.data_area_.size() << std::endl;
		}
#endif

	} else {
		std::cerr << "Configuration file can't load: '" << conf_path << '\'' << std::endl;
		return -1;
	}

   	// コマンドラインの解析
	bool opterr = false;
	for(int i = 1; i < argc; ++i) {
		const std::string p = argv[i];
		if(p[0] == '-') {
			if(p == "-V" || p == "--verbose") opts.verbose = true;
			else if(p == "-s") opts.br = true;
			else if(utils::string_strncmp(p, "--speed=", 8) == 0) { opts.com_speed = &p[8]; }
			else if(p == "-d") opts.dv = true;
			else if(utils::string_strncmp(p, "--device=", 9) == 0) { opts.device = &p[9]; }
			else if(p == "-P") opts.dp = true;
			else if(utils::string_strncmp(p, "--port=", 7) == 0) { opts.com_path = &p[7]; }
			else if(p == "-a") opts.area = true;
			else if(utils::string_strncmp(p, "--area=", 7) == 0) {
				if(!opts.set_area_(&p[7])) {
					opterr = true;
				}
			} else if(p == "-r" || p == "--read") opts.read = true;
			else if(p == "-e" || p == "--erase") opts.erase = true;
			else if(p == "-i") opts.id = true;
			else if(utils::string_strncmp(p, "--id=", 5) == 0) { opts.id_val = &p[5]; }
			else if(p == "-w" || p == "--write") opts.write = true;
			else if(p == "-v" || p == "--verify") opts.verify = true;
			else if(p == "--device-list") opts.device_list = true;
			else if(p == "--progress") opts.progress = true;
			else if(p == "--erase-rom") opts.erase_rom = true;
			else if(p == "--erase-data") opts.erase_data = true;
			else if(p == "--erase-all" || p == "--erase-chip") {
				opts.erase_rom = true;
				opts.erase_data = true;
			} else if(p == "-h" || p == "--help") opts.help = true;
			else {
				opterr = true;
			}
		} else {
			if(!opts.set_str(p)) {
				opterr = true;
			}
		}
		if(opterr) {
			std::cerr << "Option error: '" << p << "'" << std::endl;
			opts.help = true;
			opterr = false;
		}
	}
	if(opts.verbose) {
		std::cout << "# Platform: '" << opts.platform << '\'' << std::endl;
		std::cout << "# Configuration file path: '" << conf_path << '\'' << std::endl;
		std::cout << "# Device: '" << opts.device << '\'' << std::endl;
		std::cout << "# Serial port path: '" << opts.com_path << '\'' << std::endl;
		std::cout << "# Serial port speed: " << opts.com_speed << std::endl;
	}

	// HELP 表示
	if(opts.help || opts.com_path.empty() || (opts.inp_file.empty() && !opts.device_list)
///			&& opts.sequrity_set.empty() && !opts.sequrity_get && !opts.sequrity_release)
		|| opts.com_speed.empty() || opts.device.empty()) {
		if(opts.device.empty()) {
			std::cout << "Device name null." << std::endl;
		}
		if(opts.com_speed.empty()) {
			std::cout << "Serial speed none." << std::endl;
		}
		help_(argv[0]);
		return 0;
	}

	// デバイス・リスト表示
	if(opts.device_list) {
		for(const auto& s : conf_in_.get_device_list()) {
			std::cout << s << std::endl;
		}
	}

	// 入力ファイルの読み込み
	uint32_t pageall = 0;
	if(!opts.inp_file.empty()) {
		if(opts.verbose) {
			std::cout << "# Input file path: '" << opts.inp_file << '\'' << std::endl;
		}
		if(!motsx_.load(opts.inp_file)) {
			std::cerr << "Can't open input file: '" << opts.inp_file << "'" << std::endl;
			return -1;
		}
		pageall = motsx_.get_total_page();
		if(opts.verbose) {
			motsx_.list_area_map("# ");
		}
	}

    // Windwos系シリアル・ポート（COMx）の変換
    if(!opts.com_path.empty() && opts.com_path[0] != '/') {
		std::string s = utils::to_lower_text(opts.com_path);
        if(s.size() > 3 && s[0] == 'c' && s[1] == 'o' && s[2] == 'm') {
            int val;
            if(utils::string_to_int(&s[3], val)) {
                if(val >= 1 ) {
                    --val;
                    opts.com_name = opts.com_path;
                    opts.com_path = "/dev/ttyS" + (boost::format("%d") % val).str();
                }
            }
        }
		if(opts.verbose) {
			std::cout << "# Serial port alias: " << opts.com_name << " ---> " << opts.com_path << std::endl;
		}
    }
	if(opts.com_path.empty()) {
		std::cerr << "Serial port path not found." << std::endl;
		return -1;
	}

	if(opts.verbose) {
		std::cout << "# Serial port path: '" << opts.com_path << '\'' << std::endl;
	}
	int com_speed = 0;
	if(!utils::string_to_int(opts.com_speed, com_speed)) {
		std::cerr << "Serial speed conversion error: '" << opts.com_speed << '\'' << std::endl;
		return -1;		
	}

	if(!opts.erase && !opts.write && !opts.verify) return 0;
//		&& opts.sequrity_set.empty() && !opts.sequrity_get && !opts.sequrity_release) return 0;

	r8c_prog prog_(opts.verbose, opts.progress);

	if(opts.verbose) {
//		std::cout << "# Configuration file path: '" << conf_path << "'" << std::endl;
//		std::cout << "# Serial device file path: '" << opts.com_path << "'";
//		if(!opts.com_name.empty()) {
//			std::cout << "(" << opts.com_name << ")";
//		}
//		std::cout << std::endl;
//		std::cout << "# Serial device speed: " << opts.com_speed << " [bps]" << std::endl;
//		std::cout << "# Target device type: '" << opts.device << "'" << std::endl;

		std::cout << "# Device ID:";
		for(int i = 0; i < 7; ++i) {
			int v = static_cast<int>(prog_.get_id().buff[i]);
			std::cout << (boost::format(":%02X") % v).str();
		}
		std::cout << std::endl;
//		std::cout << "# Input file path: ";
//		if(opts.inp_file.empty()) {
//			std::cout << "-----" << std::endl;
//		} else {
//			std::cout << "'" << opts.inp_file << "'" << std::endl;
//		}
	}

	//=====================================
	if(!prog_.start(opts.com_path, opts.com_speed)) {
		return -1;
	}

	const utils::conf_in::device_t& devt = conf_in_.get_device();

	//===================================== リード
	if(opts.read) {
		if(opts.area_val.empty()) {  // エリア指定が無い場合
			if(devt.data_area_.size()) {
				const utils::areas& as = devt.data_area_;
				opts.area_val.emplace_back(as.front().org_, as.back().end_);
			}
			if(devt.rom_area_.size()) {
				const utils::areas& as = devt.rom_area_;
				opts.area_val.emplace_back(as.front().org_, as.back().end_);
			}
		}

		utils::motsx_io motr;
		uint32_t tpage = 0;
		const auto& as = opts.area_val;
		for(const auto& t : as) {
			tpage += ((t.end_ | 0xff) + 1 - (t.org_ & 0xffffff00)) >> 8;
		}
		if(tpage == 0) return 0;

		if(prog_.get_progress()) std::cout << "Read:   ";

		int err = 0;
		page_t page;
		for(const auto& t : as) {
			uint32_t sadr = t.org_;
			uint32_t eadr = t.end_;
			while(sadr <= eadr && err == 0) {
				uint8_t tmp[256];
				if(!prog_.read(sadr & 0xffffff00, tmp)) {
					++err;
					break;
				}
				uint32_t ofs = sadr & 255;
				motr.write(sadr, &tmp[ofs], 256 - ofs);
				sadr |= 255;
				++sadr;
				++page.n;
				if(prog_.get_progress()) progress_(tpage, page);
			}
		}
		if(prog_.get_progress()) std::cout << std::endl << std::flush;

		dump_areas_(motr, opts.area_val);
	}


	//===================================== イレース
	if(opts.erase_data || opts.erase_rom) {
		if(opts.erase_data) {
			if(opts.progress) std::cout << "Erase-data: ";
			if(!erase_(prog_, devt.data_area_)) {
				prog_.end();
				return -1;
			}
		}
		if(opts.erase_rom) {
			if(opts.progress) std::cout << "Erase-rom:  ";
			if(!erase_(prog_, devt.rom_area_)) {
				prog_.end();
				return -1;
			}
		}
	} else if(opts.erase) {  // 最適化消去（書き込むエリアのみ消去）
		auto areas = motsx_.create_area_map();

		if(opts.progress) {
			std::cout << "Erase:  " << std::flush;
		}

		page_t page;
		for(const auto& a : areas) {
			uint32_t adr = a.min_ & 0xffffff00;
			uint32_t len = 0;
			while(len < (a.max_ - a.min_ + 1)) {
				if(opts.progress) {
					progress_(pageall, page);
				}
				if(!prog_.erase_page(adr)) {  // 256 バイト単位で消去要求を送る
					prog_.end();
					return -1;
				}
				adr += 256;
				len += 256;
				++page.n;
			}
		}
		if(opts.progress) {
			std::cout << std::endl << std::flush;
		}
	}


	//===================================== 書き込み
	if(opts.write) {
		auto areas = motsx_.create_area_map();

		if(opts.progress) {
			std::cout << "Write:  " << std::flush;
		}

		page_t page;
		for(const auto& a : areas) {
			uint32_t adr = a.min_ & 0xffffff00;
			uint32_t len = 0;
			while(len < (a.max_ - a.min_ + 1)) {
				if(opts.progress) {
					progress_(pageall, page);
				}
				/// std::cout << boost::format("%08X to %08X") % adr % (adr + 255) << std::endl;
				auto mem = motsx_.get_memory(adr);
				if(!prog_.write(adr, &mem[0])) {
					prog_.end();
					return -1;
				}
				adr += 256;
				len += 256;
				++page.n;
			}
		}
		if(opts.progress) {
			std::cout << std::endl << std::flush;
		}
	}


	//===================================== verify
	if(opts.verify) {
		auto areas = motsx_.create_area_map();

		if(prog_.get_progress()) std::cout << "Verify: ";

		page_t page;
		for(const auto& a : areas) {
			uint32_t adr = a.min_ & 0xffffff00;
			uint32_t len = 0;
			while(len < (a.max_ - a.min_ + 1)) {
				if(opts.progress) {
					progress_(pageall, page);
				}
				/// std::cout << boost::format("%08X to %08X") % adr % (adr + 255) << std::endl;
				auto mem = motsx_.get_memory(adr);
				if(!prog_.verify_page(adr, &mem[0])) {
					prog_.end();
					return -1;
				}
				adr += 256;
				len += 256;
				++page.n;
			}
		}

		if(prog_.get_progress()) std::cout << std::endl << std::flush;
	}

	prog_.end();
}
