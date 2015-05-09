#include "r8c_protocol.hpp"
#include <iostream>

r8c::protocol proto_;

int main(int argc, char* argv[])
{
	using namespace r8c;

	// 開始
	std::string path = "/dev/tty.usbserial-A600e0xq";
	if(!proto_.start(path)) {
		std::cerr << "Can't open path: '" << path << "'" << std::endl;
		return -1;
	}

	// コネクション
	if(!proto_.connection()) {
		proto_.end();
		std::cerr << "Connection device error..." << std::endl;
		return -1;
	}
	std::cout << "Connection OK." << std::endl;

	// ボーレート変更
	if(!proto_.change_speed(B57600)) {
		proto_.end();
		std::cerr << "Change speed error: " << std::endl;
		return -1;
	}
	std::cout << "Change speed OK. (57600)" << std::endl;

	// バージョンの取得
	std::string ver = proto_.get_version();
	if(ver.empty()) {
		std::cerr << "Get version error..." << std::endl;
		return -1;
	}
	std::cout << "Version: '" << ver << "'" << std::endl;

	// ID チェック
	protocol::id_t id;
	id.fill();
	if(!proto_.id_inspection(id)) {
		std::cerr << "ID error: ";
		for(int i = 0; i < 7; ++i) {
			std::cerr << std::hex << static_cast<int>(id.buff[i]) << ' ';
		}
		std::cerr << std::dec << std::endl;
		return -1;
	}
	std::cout << "ID OK." << std::endl;

	// ステートの取得
	protocol::status st;
	if(!proto_.get_status(st)) {
		std::cerr << "Get status error..." << std::endl;
		return -1;
	}
	std::cout << "ID state: " << st.get_id_state() << std::endl;





	proto_.end();
}
