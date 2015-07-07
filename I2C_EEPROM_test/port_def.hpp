#pragma once
//=====================================================================//
/*!	@file
	@brief	ポート定義クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "port.hpp"
#include "common/port_map.hpp"

// ポートの配置
// P4_2(1):   LCD_SCK  ,SD_CLK(5)
// P3_7(2):   LCD_/CS
// /RES(3):  (System reset)
// P4_7(4):            ,SD_DO/DAT0(7)
// VSS:(5)   (Power GND)
// P4_6(6):   XIN (高精度なクロック用)
// VCC(7):   (Power +V)
// MODE(8):  (System mode)
// P3_5(9):   I2C_SDA
// P3_4(10):           ,SD_/CS(1)
// P1_0(20):  AN0 (keep)
// P1_1(19):  AN1 (keep)
// P1_2(18):  AN2 (keep)
// P1_3(17):  AN3 (keep)
// P1_4(16):  TXD0 (keep)
// P1_5(15):  RXD0 (keep)
// P1_6(14):  LCD_A0 (share)
// P1_7(13):  TRJIO (keep)
// P4_5(12):  LCD_SDA  ,SD_DI/CMD(2)
// P3_3(11):  I2C_SCL

// I2C ポートの定義クラス
struct scl_sda {
	void init() const {  // オープン・ドレイン設定
		utils::PORT_MAP(utils::port_map::P35::PORT);
		utils::PORT_MAP(utils::port_map::P33::PORT);
		device::POD3.B5 = 1;  // SDA
		device::POD3.B3 = 1;  // SCL
	}
	void scl_dir(bool b) const { device::PD3.B3 = b; }  // SCL 方向 (0:in, 1:out)
	void scl_out(bool b) const { device::P3.B3 = b; }   // SCL 出力
	bool scl_inp() const { return device::P3.B3(); }    // SCL 入力
	void sda_dir(bool b) const { device::PD3.B5 = b; }  // SDA 方向 (0:in, 1:out)
	void sda_out(bool b) const { device::P3.B5 = b; }   // SDA 出力
	bool sda_inp() const { return device::P3.B5(); }    // SDA 入力
};

// SPI ベース・ポートの定義クラス
struct spi_base {
	void init() const {
		utils::PORT_MAP(utils::port_map::P42::PORT);
		utils::PORT_MAP(utils::port_map::P45::PORT);
		utils::PORT_MAP(utils::port_map::P47::PORT);
		device::PD4.B2 = 1;  // LCD_SCK  ,SD_CLK 
		device::PD4.B5 = 1;  // LCD_SDA  ,SD_DI/CMD
		device::PD4.B7 = 0;  //          ,SD_DO/DAT0
	}
	void scl_out(bool b) const { device::P4.B2 = b; }
	void sda_out(bool b) const { device::P4.B5 = b; }
	bool sda_inp() const { return device::P4.B7(); }
};

// SPI コントロール・ポートの定義クラス
struct spi_ctrl {
	void init() const {
		utils::PORT_MAP(utils::port_map::P16::PORT);
		utils::PORT_MAP(utils::port_map::P37::PORT);
		utils::PORT_MAP(utils::port_map::P34::PORT);
		device::PD1.B6 = 1;  // LCD_A0
		device::PD3.B7 = 1;  // LCD_/CS
		device::PD3.B4 = 1;  //         ,SD_/CS
	}
	void a0_out(bool b) const { device::P1.B6 = b; }
	void lcd_sel(bool b) const { device::P3.B7 = b; }
	void sd_sel(bool b) const { device::P3.B4 = b; }
};
