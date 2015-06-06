//=====================================================================//
/*!	@file
	@brief	Renesas R8C Series Programmer (Flash Writer)
	@author	平松邦仁 (hira@rvf-rc45.net)
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
#include <boost/foreach.hpp>

static const std::string version_ = "0.72b";
static const uint32_t progress_num_ = 50;
static const char progress_cha_ = '#';
static const std::string conf_file = "r8c_prog.conf";

static void dump_(uint32_t adr, uint32_t len, const uint8_t* top, uint32_t w = 16)
{
	using namespace std;

	uint32_t l = adr % w;
	for(uint32_t i = 0; i < len; ++i) {
		if(l == 0 || i ==0) {
			cout << boost::format("%06X: ") % adr;
		}
		int d = static_cast<int>(*top);
		++top;
		cout << boost::format("%02X") % d;
		++l;
		++adr;
		if(l >= w) {
			cout << endl;
			l = 0;
		} else {
			cout << ' ';
		}
	}
}


static void progress_(uint32_t page, uint32_t n, uint32_t& pcn)
{
	uint32_t pos = progress_num_ * n / page;
	for(uint32_t i = 0; i < (pos - pcn); ++i) {
		std::cout << progress_cha_ << std::flush;
	}
	pcn = pos;
}


static bool read_(r8c_prog& prog, utils::motsx_io& motr, const utils::areas& as)
{
	uint32_t tpage = 0;
	BOOST_FOREACH(const utils::area_t& t, as) {
		tpage += ((t.end_ | 0xff) + 1 - (t.org_ & 0xffffff00)) >> 8;
	}
	if(tpage == 0) return true;

	if(prog.get_progress()) std::cout << "Read:   ";

	int err = 0;
	uint32_t pcn = 0;
	uint32_t n = 0;
	BOOST_FOREACH(const utils::area_t& t, as) {
		uint32_t sadr = t.org_;
		uint32_t eadr = t.end_;
		while(sadr <= eadr && err == 0) {
			uint8_t tmp[256];
			if(!prog.read(sadr & 0xffffff00, tmp)) {
				++err;
				break;
			}
			uint32_t ofs = sadr & 255;
			motr.write(sadr, &tmp[ofs], 256 - ofs);
			sadr |= 255;
			++sadr;
			++n;
			if(prog.get_progress()) progress_(tpage, n, pcn);
		}
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return err == 0;
}


static bool erase_(r8c_prog& prog, const utils::areas& as)
{
	bool noerr = true;
	uint32_t pcn = 0;
	uint32_t n = 0;
	BOOST_FOREACH(const utils::area_t& a, as) {
   		if(!prog.erase(a.org_)) {
   			noerr = false;
   			break;
   		}
   		++n;
		if(prog.get_progress()) progress_(as.size(), n, pcn);
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return noerr;
}


static bool erase_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	if(prog.get_progress()) std::cout << "Erase:  ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

   		if(!prog.erase(a)) {
   			noerr = false;
   			break;
   		}

   		++n;
		if(prog.get_progress()) progress_(motf.get_total_page(), n, pcn);
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return noerr;
}


static bool write_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	if(prog.get_progress()) std::cout << "Write:  ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

		if(!prog.write(a, motf.get_memory(a).data())) {
			noerr = false;
			break;
		}
   		++n;
		if(prog.get_progress()) progress_(motf.get_total_page(), n, pcn);
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return noerr;
}


static bool verify_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	if(prog.get_progress()) std::cout << "Verify: ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

		if(!prog.verify(a, motf.get_memory(a).data())) {
			noerr = false;
			break;
		}
   		++n;
		if(prog.get_progress()) progress_(motf.get_total_page(), n, pcn);
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return noerr;
}


struct options {
	bool verbose;

	std::string	inp_file;

	std::string	device;
	bool	dv;

	std::string	speed;
	bool	br;

	std::string dev_path;
	std::string dev_comx;
	bool	dp;

	std::string id_val;
	bool	id;

	utils::areas area_val;
	bool	area;

	bool	read;
	bool	erase;
	bool	write;
	bool	verify;
	bool	device_list;
	bool	progress;
	bool	erase_data;
	bool	erase_rom;
	bool	help;

	options() : verbose(false),
				inp_file(),
				device(), dv(false),
				speed("57600"), br(false),
				dev_path(), dp(false),
				id_val("ff:ff:ff:ff:ff:ff:ff"), id(false),
				area_val(), area(false),
				read(false), erase(false), write(false), verify(false),
				device_list(false),
				progress(false),
				erase_data(false), erase_rom(false),
				help(false) { }


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
			speed = t;
			br = false;
		} else if(dv) {
			device = t;
			dv = false;
		} else if(dp) {
			dev_path = t;
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
		BOOST_FOREACH(const utils::area_t& t, area_val) {
			if(t.org_ <= adr && adr <= t.end_) return true;
		}
		return false;
	}
};


static void dump_areas_(utils::motsx_io& motr, const utils::areas& as)
{
	BOOST_FOREACH(const utils::area_t& t, as) {
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
				dump_(org, len, &a[org & 255]);
				if(cr) std::cout << std::endl;
			}
			org |= 0xff;
			++org;
		}
	}
}


static void title_(const std::string& cmd)
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
//	cout << "-p, --programmer=PROGRAMMER\tSpecify programmer name" << endl;
	cout << "-P, --port=PORT\t\t\tSpecify serial port" << endl;
//	cout << "-q\t\t\t\tQuell progress output" << endl;
	cout << "-a, --area=ORG,END\t\tSpecify read area" << endl;
	cout << "-r, --read\t\t\tPerform data read" << endl;
	cout << "-s, --speed=SPEED\t\tSpecify serial speed" << endl;
	cout << "-v, --verify\t\t\tPerform data verify" << endl;
	cout << "    --device-list\t\tDisplay device list" << endl;
//	cout << "    --programmer-list\t\tDisplay programmer list" << endl;
	cout << "-V, --verbose\t\t\tVerbose output" << endl;
	cout << "-w, --write\t\t\tPerform data write" << endl;
	cout << "    --progress\t\t\tdisplay Progress output" << endl;
	cout << "-h, --help\t\t\tDisplay this" << endl;
//	cout << "    --version\t\t\tDisplay version No." << endl;
}

static const std::string get_current_path_(const std::string& exec)
{
	std::string exec_path;
#ifdef WIN32
	{
		std::string tmp;
		utils::sjis_to_utf8(exec, tmp);
		utils::convert_delimiter(tmp, '\\', '/', exec_path);
	}
#else
	exec_path = exec;
#endif
	std::string spch;
	std::string base = utils::get_file_name(exec_path);
	std::string env;
	{
#ifdef WIN32
		std::string tmp;
		sjis_to_utf8(getenv("PATH"), tmp);
		utils::convert_delimiter(tmp, '\\', '/', env);
		spch = ";";
#else
		env = getenv("PATH");
		spch = ":";
#endif
	}
	utils::strings ss = utils::split_text(env, spch); 
	BOOST_FOREACH(const std::string& s, ss) {
		std::string path = s + '/' + base;
		if(utils::probe_file(path)) {
			return std::move(s);
		}
	}

	return std::string("");
}


int main(int argc, char* argv[])
{
	if(argc == 1) {
		title_(argv[0]);
		return 0;
	}

	options opt;

	// 設定ファイルの読み込み
	std::string conf_path;
	if(utils::probe_file(conf_file)) {  // カレントにあるか？
		conf_path = conf_file;
	} else {  // コマンド、カレントから読んでみる
		conf_path = get_current_path_(argv[0]) + '/' + conf_file;
	}

	// 設定ファイルの読み込み及びパース
	utils::conf_in conf;
	if(conf.load(conf_path)) {
		const utils::conf_in::default_t& dt = conf.get_default();
		opt.speed    = dt.speed_;
		opt.dev_path = dt.port_;
		opt.device   = dt.device_;
		opt.id_val   = dt.id_;
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
	}

   	// コマンドラインの解析
	bool opterr = false;
	for(int i = 1; i < argc; ++i) {
		const std::string p = argv[i];
		if(p[0] == '-') {
			if(p == "-V" || p == "-verbose") opt.verbose = true;
			else if(p == "-s") opt.br = true;
			else if(utils::string_strncmp(p, "--speed=", 8) == 0) { opt.speed = &p[8]; }
			else if(p == "-d") opt.dv = true;
			else if(utils::string_strncmp(p, "--device=", 9) == 0) { opt.device = &p[9]; }
			else if(p == "-P") opt.dp = true;
			else if(utils::string_strncmp(p, "--port=", 7) == 0) { opt.dev_path = &p[7]; }
			else if(p == "-a") opt.area = true;
			else if(utils::string_strncmp(p, "--area=", 7) == 0) {
				if(!opt.set_area_(&p[7])) {
					opterr = true;
				}
			} else if(p == "-r" || p == "--read") opt.read = true;
			else if(p == "-e" || p == "--erase") opt.erase = true;
			else if(p == "-i") opt.id = true;
			else if(utils::string_strncmp(p, "--id=", 5) == 0) { opt.id_val = &p[5]; }
			else if(p == "-w" || p == "--write") opt.write = true;
			else if(p == "-v" || p == "--verify") opt.verify = true;
			else if(p == "--device-list") opt.device_list = true;
			else if(p == "--progress") opt.progress = true;
			else if(p == "--erase-rom") opt.erase_rom = true;
			else if(p == "--erase-data") opt.erase_data = true;
			else if(p == "--erase-all" || p == "--erase-chip") {
				opt.erase_rom = true;
				opt.erase_data = true;
			} else if(p == "-h" || p == "--help") opt.help = true;
			else {
				opterr = true;
			}
		} else {
			if(!opt.set_str(p)) {
				opterr = true;
			}
		}
		if(opterr) {
			std::cerr << "Option error: '" << p << "'" << std::endl;
			opt.help = true;
		}
	}

	// HELP 表示
	if(opt.help) {
		title_(argv[0]);
		return 0;
	}

	// デバイス・リスト表示
	if(opt.device_list) {
		BOOST_FOREACH(const std::string& s, conf.get_device_list()) {
			std::cout << s << std::endl;
		}
	}

	// Windwos系シリアル・ポート（COMx）の変換
	if(!opt.dev_path.empty() && opt.dev_path[0] != '/') {
		std::string s = utils::to_lower_text(opt.dev_path);
		if(s.size() > 3 && s[0] == 'c' && s[1] == 'o' && s[2] == 'm') {
			int val;
			if(utils::string_to_int(&s[3], val)) {
				if(val >= 1 ) {
					--val;
					opt.dev_comx = opt.dev_path;
					opt.dev_path = "/dev/ttyS" + (boost::format("%d") % val).str();
				}
			}
		}
	}

	r8c_prog prog(opt.verbose, opt.progress);

	if(opt.verbose) {
		std::cout << "Configuration file path: '" << conf_path << "'" << std::endl;
		std::cout << "Serial device file path: '" << opt.dev_path << "'";
		if(!opt.dev_comx.empty()) {
			std::cout << "(" << opt.dev_comx << ")";
		}
		std::cout << std::endl;
		std::cout << "Serial device speed: " << opt.speed << " [bps]" << std::endl;
		std::cout << "Target device type: '" << opt.device << "'" << std::endl;
		std::cout << "Device id:";
		for(int i = 0; i < 7; ++i) {
			int v = static_cast<int>(prog.get_id().buff[i]);
			std::cout << (boost::format(":%02X") % v).str();
		}
		std::cout << std::endl;
		std::cout << "Input file path: ";
		if(opt.inp_file.empty()) {
			std::cout << "-----" << std::endl;
		} else {
			std::cout << "'" << opt.inp_file << "'" << std::endl;
		}
	}

	// モトローラーSフォーマットファイルの読み込み
	utils::motsx_io motf;
	if(!opt.inp_file.empty()) {
		if(!motf.load(opt.inp_file)) {
			std::cerr << "Can't load input file: '" << opt.inp_file << "'" << std::endl; 
			return -1;
		}
		if(opt.verbose) {
			motf.list_memory_map();
		}
		// エリア判定


	}

	if(!prog.start(opt.dev_path, opt.speed)) {
		return -1;
	}

	const utils::conf_in::device_t& devt = conf.get_device();

	// リード
	if(opt.read) {
		if(opt.area_val.empty()) {  // エリア指定が無い場合
			if(devt.data_area_.size()) {
				const utils::areas& as = devt.data_area_;
				opt.area_val.emplace_back(as.front().org_, as.back().end_);
			}
			if(devt.rom_area_.size()) {
				const utils::areas& as = devt.rom_area_;
				opt.area_val.emplace_back(as.front().org_, as.back().end_);
			}
		}
		utils::motsx_io motr;
		if(!read_(prog, motr, opt.area_val)) {
			return -1;
		}
		dump_areas_(motr, opt.area_val);
	}

	// イレース
	if(opt.erase_data || opt.erase_rom) {
		if(opt.erase_data) {
			if(opt.progress) std::cout << "Erase-data: ";
			if(!erase_(prog, devt.data_area_)) {
				return -1;
			}
		}
		if(opt.erase_rom) {
			if(opt.progress) std::cout << "Erase-rom:  ";
			if(!erase_(prog, devt.rom_area_)) {
				return -1;
			}
		}
	} else if(opt.erase) {
		if(!erase_(prog, motf)) {
			return -1;
		}
	}

	// 書き込み
	if(opt.write) {
		if(!write_(prog, motf)) {
			return -1;
		}
	}

	// verify
	if(opt.verify) {
		if(!verify_(prog, motf)) {
			return -1;
		}
	}
}
