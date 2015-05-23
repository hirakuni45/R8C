//=====================================================================//
/*!	@file
	@brief	Renesas R8C Series Programmer (Flash Writer)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "r8c_protocol.hpp"
#include <iostream>
#include <iomanip>
#include <random>
#include <utility>
#include "motsx_io.hpp"
#include "conf_in.hpp"
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <cstdlib>

static const std::string version_ = "0.35b";
static const uint32_t progress_num_ = 50;
static const char progress_cha_ = '#';

static const std::string conf_file = "r8c_prog.conf";

void dump_(const uint8_t* top, uint32_t len, uint32_t ofs, uint32_t w = 16)
{
	using namespace std;
	cout << hex;

	uint32_t l = 0;
	for(uint32_t i = 0; i < len; ++i) {
		if(l == 0) {
			cout << uppercase << setw(4) << setfill('0') << ofs << "- ";
		}
		int d = static_cast<int>(*top);
		++top;
		cout << uppercase << setw(2) << setfill('0') << d;
		++l;
		++ofs;
		if(l >= w) {
			cout << endl;
			l = 0;
		} else {
			cout << ' ';
		}
	}

	cout << dec;
}

#if 0
   		std::mt19937 mt(0x1234);
   		protocol::array_type ar;
   		for(int i = 0; i < 256; ++i) {
   			ar.push_back(mt() & 255);
   		}
   		dump_(&ar[0], 256, 0x8000);
#endif

class r8c_prog {
	bool	verbose_;
	r8c::protocol	proto_;
	std::string		ver_;
	r8c::protocol::id_t	id_;

public:
	r8c_prog(bool verbose) : verbose_(verbose) {
		id_.fill();
	}

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


static bool read_(r8c_prog& prog)
{
	uint32_t sadr = 0x8000;
	uint32_t eadr = 0xffff;

	bool noerr = true;
	std::cout << "Read:   ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	uint32_t tpage = ((eadr + 1) - sadr) >> 8;
	for(uint32_t a = sadr; a <= eadr; a += 256) {
		uint8_t tmp[256];
   		if(!prog.read(a, tmp)) {
   			noerr = false;
   			break;
   		}
   		++n;
		progress_(tpage, n, pcn);
	}
	std::cout << std::endl << std::flush;

	return noerr;
}


static bool erase_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	std::cout << "Erase:  ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

   		if(!prog.erase(a)) {
   			noerr = false;
   			break;
   		}
   		++n;
		progress_(motf.get_total_page(), n, pcn);
	}
	std::cout << std::endl << std::flush;

	return noerr;
}


static bool write_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	std::cout << "Write:  ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

		if(!prog.write(a, motf.get_memory(a).data())) {
			noerr = false;
			break;
		}
   		++n;
		progress_(motf.get_total_page(), n, pcn);
	}
	std::cout << std::endl << std::flush;

	return noerr;
}


static bool verify_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	std::cout << "Verify: ";
	uint32_t pcn = 0;
	uint32_t n = 0;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		if(!motf.find_page(a)) continue;

		if(!prog.verify(a, motf.get_memory(a).data())) {
			noerr = false;
			break;
		}
   		++n;
		progress_(motf.get_total_page(), n, pcn);
	}
	std::cout << std::endl << std::flush;

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

	bool	read;
	bool	erase;
	bool	write;
	bool	verify;

	options() : verbose(false),
				inp_file(),
				device(), dv(false),
				speed("57600"), br(false),
				dev_path(), dp(false),
				id_val("ff:ff:ff:ff:ff:ff:ff"), id(false),
				read(false), erase(false), write(false), verify(false) { }

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
	cout << c << "[options] [mot/id/conf file] ..." << endl;
	cout << endl;
	cout << "Options :" << endl;
	cout << "-d, --device=DEVICE\t\tSpecify device name" << endl;
	cout << "-e, --erase\t\t\tPerform a device erase to a minimum" << endl;
//	cout << "-E, --erase-all, --erase-chip\tPerform rom and data flash erase" << endl;
//	cout << "    --erase-rom\t\t\tPerform rom flash erase" << endl;
//	cout << "    --erase-data\t\tPerform data flash erase" << endl;
	cout << "-i, --id=xx:xx:xx:xx:xx:xx:xx\tSpecify protect ID" << endl;
//	cout << "-p, --programmer=PROGRAMMER\tSpecify programmer name" << endl;
	cout << "-P, --port=PORT\t\t\tSpecify serial port" << endl;
//	cout << "-q\t\t\t\tQuell progress output" << endl;
	cout << "-r, --read\t\t\tPerform data read" << endl;
	cout << "-s, --speed=SPEED\t\tSpecify serial speed" << endl;
	cout << "-v, --verify\t\t\tPerform data verify" << endl;
//	cout << "    --device-list\t\tDisplay device list" << endl;
//	cout << "    --programmer-list\t\tDisplay programmer list" << endl;
	cout << "-V, --verbose\t\t\tVerbose output" << endl;
	cout << "-w, --write\t\t\tPerform data write" << endl;
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
			else if(p == "-r" || p == "--read") opt.read = true;
			else if(p == "-e" || p == "--erase") opt.erase = true;
			else if(p == "-i") opt.id = true;
			else if(utils::string_strncmp(p, "--id=", 5) == 0) { opt.id_val = &p[5]; }
			else if(p == "-w" || p == "--write") opt.write = true;
			else if(p == "-v" || p == "--verify") opt.verify = true;
		} else {
			opt.set_str(p);
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

	r8c_prog prog(opt.verbose);

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

	// リード
	if(opt.read) {
		if(!read_(prog)) {
			return -1;
		}
	}

	// イレース
	if(opt.erase) {
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
