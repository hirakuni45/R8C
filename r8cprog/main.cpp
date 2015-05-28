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
#include <set>
#include <cstdlib>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "r8c_protocol.hpp"
#include "motsx_io.hpp"
#include "conf_in.hpp"

static const std::string version_ = "0.50b";
static const uint32_t progress_num_ = 50;
static const char progress_cha_ = '#';
static const std::string conf_file = "r8c_prog.conf";


#if 0
   		std::mt19937 mt(0x1234);
   		protocol::array_type ar;
   		for(int i = 0; i < 256; ++i) {
   			ar.push_back(mt() & 255);
   		}
   		dump_(&ar[0], 256, 0x8000);
#endif


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
			std::cerr << "Can't open path: '" << path << "'" << std::endl;
			return false;
		}

		// コネクション
		if(!proto_.connection()) {
			proto_.end();
			std::cerr << "Connection device error..." << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "Connection OK." << std::endl;
		}

		// ボーレート変更
		int val;
		if(!utils::string_to_int(brate, val)) {
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
			std::cerr << "Get version error..." << std::endl;
			return false;
		}
		if(verbose_) {
			std::cout << "Version: '" << ver_ << "'" << std::endl;
		}

		// ID チェック認証
		if(!proto_.id_inspection(id_)) {
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
			std::cerr << "Read error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
			return false;
		}
		return true;
	}


	bool erase(uint32_t top) {
		uint32_t area = 1024;
		if(top >= 0x8000) area = 4096;

		uint32_t adr = top & ~(area - 1);
		if(set_.find(adr) != set_.end()) {
			return true;
		}
		set_.insert(adr);

		// イレース
		if(!proto_.erase_page(top)) {
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
   			std::cerr << "Write error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
			return false;
		}
		return true;
   	}


	bool verify(uint32_t top, const uint8_t* data) {
		// ページ読み込み
		uint8_t tmp[256];
   		if(!proto_.read_page(top, tmp)) {
   			std::cerr << "Read error: " << std::hex << std::setw(6)
					  << static_cast<int>(top) << " to " << static_cast<int>(top + 255)
					  << std::endl;
   			return false;
   		}

		for(int i = 0; i < 256; ++i) {
			if(data[i] != tmp[i]) {
   			std::cerr << "Verify error: " << std::hex << std::setw(6)
					  << "0x" << static_cast<int>(top)
					  << std::setw(2) << static_cast<int>(data[top + i]) << " -> "
					  << static_cast<int>(tmp[i])
					  << std::endl;
				return false;
			}
		}
		return true;
	}

	void end() {
		proto_.end();
	}
};


static void progress_(uint32_t page, uint32_t n, uint32_t& pcn)
{
	uint32_t pos = progress_num_ * n / page;
	for(int i = 0; i < (pos - pcn); ++i) {
		std::cout << progress_cha_ << std::flush;
	}
	pcn = pos;
}


static bool read_(r8c_prog& prog, utils::motsx_io& motr, uint32_t sadr, uint32_t eadr)
{
	bool noerr = true;
	if(prog.get_progress()) std::cout << "Read:   ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	uint32_t tpage = ((eadr | 0xff) + 1 - (sadr & 0xffff00)) >> 8;
	while(sadr <= eadr) {
		uint8_t tmp[256];
   		if(!prog.read(sadr & 0xffff00, tmp)) {
   			noerr = false;
   			break;
   		}
		uint32_t ofs = sadr & 255;
		motr.write(sadr, &tmp[ofs], 256 - ofs);
		sadr |= 255;
		++sadr;
   		++n;
		if(prog.get_progress()) progress_(tpage, n, pcn);
	}
	if(prog.get_progress()) std::cout << std::endl << std::flush;

	return noerr;
}


static bool erase_(r8c_prog& prog, const utils::conf_in::device_t::areas& as)
{
	bool noerr = true;
	uint32_t pcn = 0;
	uint32_t n = 0;
	BOOST_FOREACH(const utils::conf_in::device_t::area a, as) {
   		if(!prog.erase(a.top_)) {
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

	std::string	area_val;
	bool	area;

	bool	read;
	bool	erase;
	bool	write;
	bool	verify;
	bool	device_list;
	bool	progress;
	bool	erase_data;
	bool	erase_rom;

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
				erase_data(false), erase_rom(false) { }

	void set_str(const std::string& t) {
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
			area_val = t;
			area = false;
		} else {
			inp_file = t;
		}
	}
};


static void title_(const std::string& cmd)
{
	using namespace std;

	std::string c = utils::get_file_base(cmd);

	cout << "Renesas R8C Series Programmer Version" << version_ << endl;
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
	cout << "-A, --area=ORG,END\t\tSpecify read area" << endl;
	cout << "-r, --read\t\t\tPerform data read" << endl;
	cout << "-s, --speed=SPEED\t\tSpecify serial speed" << endl;
	cout << "-v, --verify\t\t\tPerform data verify" << endl;
	cout << "    --device-list\t\tDisplay device list" << endl;
//	cout << "    --programmer-list\t\tDisplay programmer list" << endl;
	cout << "-V, --verbose\t\t\tVerbose output" << endl;
	cout << "-w, --write\t\t\tPerform data write" << endl;
	cout << "    --progress\t\t\tdisplay Progress output" << endl;
//	cout << "-h, --help\t\t\tDisplay this" << endl;
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
			else if(p == "-A") opt.area = true;
			else if(utils::string_strncmp(p, "--area=", 7) == 0) { opt.area_val = &p[7]; }
			else if(p == "-r" || p == "--read") opt.read = true;
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
			}
		} else {
			opt.set_str(p);
		}
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
	}

	if(!prog.start(opt.dev_path, opt.speed)) {
		return -1;
	}

	const utils::conf_in::device_t& devt = conf.get_device();

	// リード
	if(opt.read) {
		uint32_t adr = 0x17002;
		uint32_t end = 0x173fc;

		utils::motsx_io motr;
		if(!read_(prog, motr, adr, end)) {
			return -1;
		}

		while(adr <= end) {
			const utils::motsx_io::array& a = motr.get_memory(adr);
			uint32_t len = 256 - (adr & 255);
			bool cr = false;
			if((adr + len) > end) {
				len = end - adr;
				cr = true;
			}
			dump_(adr, len, &a[adr & 255]);
			if(cr) std::cout << std::endl;
			adr |= 0xff;
			++adr;
		}
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
