#pragma once
//=====================================================================//
/*! @file
    @brief  R8C 起動前初期化
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2015, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//

#ifdef __cplusplus
extern "C" {
#endif
	//-----------------------------------------------------------------//
	/*!
		@brief  メイン関数起動前初期化
	*/
	//-----------------------------------------------------------------//
	void _init(void);
#ifdef __cplusplus
};
#endif
