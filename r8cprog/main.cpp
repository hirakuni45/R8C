//=====================================================================//
/*!	@file
	@brief	R8C Flash Programmer
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "r8c_protocol.hpp"
#include <iostream>
#include <iomanip>
#include <random>
#include "motsx_io.hpp"

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


class r8c_prog {
	bool	verbose_;
	r8c::protocol	proto_;
	std::string		ver_;
	r8c::protocol::id_t	id_;

public:
	r8c_prog(bool verbose) : verbose_(verbose) {
		id_.fill();
	}


	bool start(const std::string& path, uint32_t brate) {
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
		speed_t speed;
		switch(brate) {
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


#if 0
   		std::mt19937 mt(0x1234);
   		protocol::array_type ar;
   		for(int i = 0; i < 256; ++i) {
   			ar.push_back(mt() & 255);
   		}
   		dump_(&ar[0], 256, 0x8000);
#endif

	void end() {
		proto_.end();
	}
};


static bool erase_(r8c_prog& prog, utils::motsx_io& motf)
{
	std::cout << "Erase:  ";
	bool noerr = true;
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		const std::vector<uint8_t>& mem = motf.get_memory();
		bool erase = false;
		for(int i = 0; i < 256; ++i) {
			if(mem[a + i] != 0xff) {
				erase = true;
				break;
			}
		}
		if(erase) {
			if(!prog.erase(a)) {
				noerr = false;
				break;
			}
			std::cout << '#' << std::flush;
		}
	}
	std::cout << std::endl;

	return noerr;
}


static bool write_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	std::cout << "Write:  ";
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		const std::vector<uint8_t>& mem = motf.get_memory();
		bool skip = true;
		for(int i = 0; i < 256; ++i) {
			if(mem[a + i] != 0xff) {
				skip = false;
				break;
			}
		}
		if(skip) continue;

		if(!prog.write(a, &mem[a])) {
			noerr = false;
			break;
		}
		std::cout << '#' << std::flush;
	}
	std::cout << std::endl;

	return noerr;
}


static bool verify_(r8c_prog& prog, utils::motsx_io& motf)
{
	bool noerr = true;
	std::cout << "Verify: ";
	for(uint32_t a = motf.get_min(); a <= motf.get_max(); a += 256) {
		const std::vector<uint8_t>& mem = motf.get_memory();
		bool skip = true;
		for(int i = 0; i < 256; ++i) {
			if(mem[a + i] != 0xff) {
				skip = false;
				break;
			}
		}
		if(skip) continue;

		if(!prog.verify(a, &mem[a])) {
			noerr = false;
			break;
		}
		std::cout << '#' << std::flush;
	}
	std::cout << std::endl;

	return noerr;
}


struct options {
	bool verbose;
	std::string	inp_file;
	std::string	device;
	bool	dv;
	uint32_t	baud_rate;
	bool	br;
	std::string dev_path;
	bool	dp;

	bool	erase;
	bool	write;
	bool	verify;

	options() : verbose(false),
				inp_file(),
				device(), dv(false),
				baud_rate(57600), br(false),
				dev_path(), dp(false),
				erase(false), write(false), verify(false) { }

	void set_str(const std::string& t) {
		if(br) {
			int val;
			if(utils::string_to_int(t, val)) {
				baud_rate = val;
			} else {
				std::cerr << "Options error: baud rate: " << t << std::endl;
			}
			br = false;
		} else if(dv) {
			device = t;
			dv = false;
		} else if(dp) {
			dev_path = t;
			dp = false;
		} else {
			inp_file = t;
		}
	}
};

static void title_(const std::string& cmd)
{
	std::cout << "R8C programmer" << std::endl;
	std::cout << cmd << std::endl;
}


int main(int argc, char* argv[])
{
	if(argc == 1) {
		title_(argv[0]);
		return 0;
	}

	options opt;
	for(int i = 1; i < argc; ++i) {
		const std::string p = argv[i];
		if(p[0] == '-') {
			if(p == "-verbose") opt.verbose = true;
			else if(p == "-s") opt.br = true;
			else if(p == "-d") opt.dv = true;
			else if(p == "-P") opt.dp = true;
			else if(p == "-e") opt.erase = true;
			else if(p == "-w") opt.write = true;
			else if(p == "-v") opt.verify = true;
		} else {
			opt.set_str(p);
		}
	}

	
	utils::motsx_io motf;
	if(!motf.load(opt.inp_file)) {
		std::cerr << "Can't load input file: '" << opt.inp_file << "'" << std::endl; 
		return -1;
	}
	if(opt.verbose) {
		std::cout << "Load data: ";
		std::cout << std::hex << std::uppercase << std::setfill('0')
				  << std::setw(6) << static_cast<int>(motf.get_min()) << " to "
				  << std::setw(6) << static_cast<int>(motf.get_max())
				  << std::dec << std::endl;
	}

	// test
	if(opt.dev_path.empty()) {
		opt.dev_path = "/dev/tty.usbserial-A600e0xq";
	}
	r8c_prog prog(opt.verbose);
	if(!prog.start(opt.dev_path, opt.baud_rate)) {
		return -1;
	}

	if(opt.erase) {
		if(!erase_(prog, motf)) {
			return -1;
		}
	}

	if(opt.write) {
		if(!write_(prog, motf)) {
			return -1;
		}
	}

	if(opt.verify) {
		if(!verify_(prog, motf)) {
			return -1;
		}
	}

}
