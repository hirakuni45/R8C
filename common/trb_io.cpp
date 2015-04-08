//=====================================================================//
/*!	@file
	@brief	R8C グループ・TimerRB I/O 制御 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "trb_io.hpp"

namespace device {

	volatile uint16_t trb_io::count_ = 0;
	void (*trb_io::task_)(void);
}
