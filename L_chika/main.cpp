#include "port.hpp"

int main(int argc, char *ragv[])
{
	using namespace device;

	PD1.B0 = 1;

	while(1) {
		for(int i = 0; i < 1000; ++i) P1.B0 = 0;
		for(int i = 0; i < 1000; ++i) P1.B0 = 1;
	}

}
