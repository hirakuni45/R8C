#include "rs232c.hpp"
#include <iostream>

utils::rs232c rs232c_;

int main(int argc, char* argv[])
{
	std::string path = "/dev/tty.usbserial-A600e0xq";
	bool f = rs232c_.open(path, B19200);
	if(!f) {
		std::cerr << "Can't open path: '" << path << "'" << std::endl;
		return -1;
	}

	f = rs232c_.enable_RTS(false);
	if(!f) {
		return -1;
	}

	f = rs232c_.enable_DTR(false);
	if(!f) {
		return -1;
	}

	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	for(int i = 0; i < 1000; ++i) {
		char buff[1];
		size_t n = rs232c_.recv(buff, 1, tv);
		if(n == 1) {
			std::cout << buff[0] << std::flush;
		}
	}

#if 0
	size_t n = rs232c_.send("ASDFGH", 6);
	if(n != 6) {
		return -1;
	}
#endif

//	rs232c_.sync_send();

	f = rs232c_.close();
	if(!f) {
		return -1;
	}
}
