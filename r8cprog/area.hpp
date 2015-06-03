#pragma once
//=====================================================================//
/*!	@file
	@brief	領域クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>

namespace utils {

	struct area_t {
		uint32_t	org_;
		uint32_t	end_;
		area_t(uint32_t o = 0, uint32_t e = 0) : org_(o), end_(e) { }
	};

	typedef std::vector<area_t>	areas;

}
