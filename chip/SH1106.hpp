#pragma once
//=====================================================================//
/*!	@file
	@brief	SH1106 OLED ドライバー @n
			Copyright 2022 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  SH1106 テンプレートクラス
		@param[in]	CSI_IO	CSI(SPI) 制御クラス
		@param[in]	CS	デバイス選択、レジスター選択、制御クラス
		@param[in]	DC	データ、コマンド切り替え、制御クラス
		@param[in]	RES	リセット、制御クラス
		@param[in]	EXTVCC EXTERNAL VCC の場合「true」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class CSI_IO, class CS, class DC, class RES, bool EXT_VCC = false>
	class SH1106 {

		CSI_IO&	csi_;

		enum class CMD : uint8_t {
			SETCONTRAST			= 0x81,
			DISPLAYALLON_RESUME	= 0xA4,
			DISPLAYALLON		= 0xA5,
			NORMALDISPLAY		= 0xA6,
			INVERTDISPLAY		= 0xA7,
			DISPLAYOFF			= 0xAE,
			DISPLAYON			= 0xAF,

			SETDISPLAYOFFSET	= 0xD3,
			SETCOMPINS			= 0xDA,

			SETVCOMDETECT		= 0xDB,

			SETDISPLAYCLOCKDIV	= 0xD5,
			SETPRECHARGE		= 0xD9,

			SETMULTIPLEX		= 0xA8,

			SETLOWCOLUMN		= 0x00,
			SETHIGHCOLUMN		= 0x10,

			SETSTARTLINE		= 0x40,

			MEMORYMODE			= 0x20,
			COLUMNADDR			= 0x21,
			PAGEADDR			= 0x22,

			COMSCANINC			= 0xC0,
			COMSCANDEC			= 0xC8,

			SEGREMAP0			= 0xA0,
			SEGREMAP1			= 0xA1,

			CHARGEPUMP			= 0x8D,

			EXTERNALVCC			= 0x1,
			SWITCHCAPVCC		= 0x2,

			// Scrolling
			ACTIVATE_SCROLL		= 0x2F,
			DEACTIVATE_SCROLL	= 0x2E,
			SET_VERTICAL_SCROLL_AREA = 0xA3,
			RIGHT_HORIZONTAL_SCROLL  = 0x26,
			LEFT_HORIZONTAL_SCROLL   = 0x27,
			VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29,
			VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A,
		};


		void write_cmd_(CMD c)
		{
			//digitalWrite(dc, LOW);
			DC::P = 0;
			//digitalWrite(cs, LOW);
			CS::P = 0;
			//fastSPIwrite(c);
			csi_.xchg(static_cast<uint8_t>(c));
			//digitalWrite(cs, HIGH);
			CS::P = 1;
#if 0
			// I2C
			uint8_t control = 0x00;   // Co = 0, D/C = 0
			Wire.beginTransmission(_i2caddr);
			WIRE_WRITE(control);
			WIRE_WRITE(c);
			Wire.endTransmission();
#endif
		}


		void write_data_(uint8_t c)
		{
			// SPI
			//digitalWrite(dc, HIGH);
			DC::P = 1;
			//digitalWrite(cs, LOW);
			CS::P = 0;
			//fastSPIwrite(c);
			csi_.xchg(c);
			//digitalWrite(cs, HIGH);
			CS::P = 1;
#if 0
			// I2C
			uint8_t control = 0x40;   // Co = 0, D/C = 1
			Wire.beginTransmission(_i2caddr);
			WIRE_WRITE(control);
			WIRE_WRITE(c);
			Wire.endTransmission();
#endif  
		}


#if 0

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
//void Adafruit_SH1106::startscrollright(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_RIGHT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X00);
  SH1106_command(0XFF);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}
// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SH1106::startscrollleft(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_LEFT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X00);
  SH1106_command(0XFF);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}
// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SH1106::startscrolldiagright(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_SET_VERTICAL_SCROLL_AREA);  
  SH1106_command(0X00);
  SH1106_command(SH1106_LCDHEIGHT);
  SH1106_command(SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X01);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}
// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SH1106::startscrolldiagleft(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_SET_VERTICAL_SCROLL_AREA);  
  SH1106_command(0X00);
  SH1106_command(SH1106_LCDHEIGHT);
  SH1106_command(SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X01);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}
void Adafruit_SH1106::stopscroll(void){
  SH1106_command(SH1106_DEACTIVATE_SCROLL);
}

   #if defined SH1106_128_32
    // Init sequence for 128x32 OLED module
    SH1106_command(SH1106_DISPLAYOFF);                    // 0xAE
    SH1106_command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
    SH1106_command(0x80);                                  // the suggested ratio 0x80
    SH1106_command(SH1106_SETMULTIPLEX);                  // 0xA8
    SH1106_command(0x1F);
    SH1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3
    SH1106_command(0x0);                                   // no offset
    SH1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0 
    SH1106_command(SH1106_CHARGEPUMP);                    // 0x8D
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x10); }
    else 
      { SH1106_command(0x14); }
    SH1106_command(SH1106_MEMORYMODE);                    // 0x20
    SH1106_command(0x00);                                  // 0x0 act like ks0108
    SH1106_command(SH1106_SEGREMAP | 0x1);
    SH1106_command(SH1106_COMSCANDEC);
    SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
    SH1106_command(0x02);
    SH1106_command(SH1106_SETCONTRAST);                   // 0x81
    SH1106_command(0x8F);
    SH1106_command(SH1106_SETPRECHARGE);                  // 0xd9
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x22); }
    else 
      { SH1106_command(0xF1); }
    SH1106_command(SH1106_SETVCOMDETECT);                 // 0xDB
    SH1106_command(0x40);
    SH1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4
    SH1106_command(SH1106_NORMALDISPLAY);                 // 0xA6
  #endif

  #if defined SH1106_128_64
    // Init sequence for 128x64 OLED module
    SH1106_command(SH1106_DISPLAYOFF);                    // 0xAE
    SH1106_command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
    SH1106_command(0x80);                                  // the suggested ratio 0x80
    SH1106_command(SH1106_SETMULTIPLEX);                  // 0xA8
    SH1106_command(0x3F);
    SH1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3
    SH1106_command(0x00);                                   // no offset
	
    SH1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0 0x40
    SH1106_command(SH1106_CHARGEPUMP);                    // 0x8D
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x10); }
    else 
      { SH1106_command(0x14); }
    SH1106_command(SH1106_MEMORYMODE);                    // 0x20
    SH1106_command(0x00);                                  // 0x0 act like ks0108
    SH1106_command(SH1106_SEGREMAP | 0x1);
    SH1106_command(SH1106_COMSCANDEC);
    SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
    SH1106_command(0x12);
    SH1106_command(SH1106_SETCONTRAST);                   // 0x81
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x9F); }
    else 
      { SH1106_command(0xCF); }
    SH1106_command(SH1106_SETPRECHARGE);                  // 0xd9
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x22); }
    else 
      { SH1106_command(0xF1); }
    SH1106_command(SH1106_SETVCOMDETECT);                 // 0xDB
    SH1106_command(0x40);
    SH1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4
    SH1106_command(SH1106_NORMALDISPLAY);                 // 0xA6
  #endif
  
  #if defined SH1106_96_16
    // Init sequence for 96x16 OLED module
    SH1106_command(SH1106_DISPLAYOFF);                    // 0xAE
    SH1106_command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
    SH1106_command(0x80);                                  // the suggested ratio 0x80
    SH1106_command(SH1106_SETMULTIPLEX);                  // 0xA8
    SH1106_command(0x0F);
    SH1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3
    SH1106_command(0x00);                                   // no offset
    SH1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0
    SH1106_command(SH1106_CHARGEPUMP);                    // 0x8D
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x10); }
    else 
      { SH1106_command(0x14); }
    SH1106_command(SH1106_MEMORYMODE);                    // 0x20
    SH1106_command(0x00);                                  // 0x0 act like ks0108
    SH1106_command(SH1106_SEGREMAP | 0x1);
    SH1106_command(SH1106_COMSCANDEC);
    SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
    SH1106_command(0x2);	//ada x12
    SH1106_command(SH1106_SETCONTRAST);                   // 0x81
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x10); }
    else 
      { SH1106_command(0xAF); }
    SH1106_command(SH1106_SETPRECHARGE);                  // 0xd9
    if (vccstate == SH1106_EXTERNALVCC) 
      { SH1106_command(0x22); }
    else 
      { SH1106_command(0xF1); }
    SH1106_command(SH1106_SETVCOMDETECT);                 // 0xDB
    SH1106_command(0x40);
    SH1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4
    SH1106_command(SH1106_NORMALDISPLAY);                 // 0xA6
  #endif

  SH1106_command(SH1106_DISPLAYON);//--turn on oled panel


#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		SH1106(CSI_IO& csi) : csi_(csi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
			@param[in]	contrast コントラスト
			@param[in]	comrvs	コモンライン・リバースの場合：true
			@param[in]	bias9	BIAS9 選択の場合「true」
			@param[in]	vratio	ボルテージ・レシオ
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			CS::DIR  = 1;  // (/CS) output
			DC::DIR  = 1;  // (A0) output
			RES::DIR = 1; // (/RES) output

			CS::P = 1;
			DC::P = 0;

			RES::P = 0;  // assert /RES signal

			utils::delay::milli_second(100);

			RES::P = 1;  // negate /RES signal

			utils::delay::milli_second(10);

			// 128 x 64 pixel
			write_cmd_(CMD::DISPLAYOFF);
			write_cmd_(CMD::SETDISPLAYCLOCKDIV);
			write_cmd_(static_cast<CMD>(0x80));       // the suggested ratio 0x80
			write_cmd_(CMD::SETMULTIPLEX);

			write_cmd_(static_cast<CMD>(0x3F));
			write_cmd_(CMD::SETDISPLAYOFFSET);
			write_cmd_(static_cast<CMD>(0x00));

			write_cmd_(CMD::SETSTARTLINE);            // line #0 0x40
			write_cmd_(CMD::CHARGEPUMP);
			if(EXT_VCC) {
				write_cmd_(static_cast<CMD>(0x10));
			} else {
				write_cmd_(static_cast<CMD>(0x14));
			}
			write_cmd_(CMD::MEMORYMODE);
			write_cmd_(static_cast<CMD>(0x00));       // 0x0 act like ks0108
			write_cmd_(CMD::SEGREMAP1);
			write_cmd_(CMD::COMSCANDEC);
			write_cmd_(CMD::SETCOMPINS);
			write_cmd_(static_cast<CMD>(0x12));
			write_cmd_(CMD::SETCONTRAST);
			if(EXT_VCC) {
				write_cmd_(static_cast<CMD>(0x9F));
			} else {
				write_cmd_(static_cast<CMD>(0xCF));
			}
			write_cmd_(CMD::SETPRECHARGE);
			if(EXT_VCC) {
				write_cmd_(static_cast<CMD>(0x22));
			} else {
				write_cmd_(static_cast<CMD>(0xF1));
			}
			write_cmd_(CMD::SETVCOMDETECT);
			write_cmd_(static_cast<CMD>(0x40));
			write_cmd_(CMD::DISPLAYALLON_RESUME);
			write_cmd_(CMD::NORMALDISPLAY);
			// end of 128 x 64 pixel

			write_cmd_(CMD::DISPLAYON);

			write_cmd_(CMD::SETLOWCOLUMN);  // low col = 0
			write_cmd_(CMD::SETHIGHCOLUMN);  // hi col = 0
			write_cmd_(CMD::SETSTARTLINE);  // line #0
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  画面反転の許可
			@param[in]	ena		画面反転をしない場合「false」
		*/
		//-----------------------------------------------------------------//
		void invert_enable(bool ena = true)
		{
			if(ena) {
				write_cmd_(CMD::INVERTDISPLAY);
			} else {
				write_cmd_(CMD::NORMALDISPLAY);
			}
		}
	};
}
