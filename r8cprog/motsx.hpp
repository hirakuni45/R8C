#pragma once
//=====================================================================//
/*!	@file
	@brief	モトローラーＳフォーマット入出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Motolora Sx I/O クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class motsx_io {


#if 0
//-----------------------------------------------------------------//
/*!
	@brief	モトローラー S フォーマットの読み込み
	@return	エラーが無ければ「０」が返る
 */
//-----------------------------------------------------------------//
int read_mot(void)
{
	unsigned int total = 0;
	int mode = 0;
	unsigned int value = 0;
	int vcnt = 0;
	unsigned int type = 0;
	unsigned int length = 0;
	unsigned int address = 0;
	unsigned int sum = 0;
	char ch;
	unsigned int min, max;
	min = 0xffffffff;
	max = 0x00000000;
	char toend = 0;
	while((ch = sh7125_sci_getch(1)) != 0) {

		if(ch == ('C' - 0x40)) {
			sh7125_sci_puts(1, "\nbreak !\n");
			break;
		} else if(ch == ' ') ;
		else if(ch == 0x0d || ch == 0x0a) {
			if(toend) break;
		} else if(mode == 0 && ch == 'S') {
			mode = 1;
			value = vcnt = 0;
		} else if(ch >= '0' && ch <= '9') {
			value <<= 4;
			value |= ch - '0';
			++vcnt;
		} else if(ch >= 'A' && ch <= 'F') {
			value <<= 4;
			value |= ch - 'A' + 10;
			++vcnt;
		} else {
			sh7125_sci_puts(1, "\nS format illegual character: '");
			if(ch >= 0x20 && ch <= 0x7f) {
				sh7125_sci_putch(1, ch);
			} else {
				sh7125_sci_puts(1, "0x");
				put_hexadecimal(ch, 2);
			}
			sh7125_sci_puts(1, "'\n");
			return -1;
		}

		if(mode == 1) {		// タイプ取得
			if(vcnt == 1) {
				type = value;
				mode = 2;
				value = vcnt = 0;
			}
		} else if(mode == 2) {	// レングス取得
			if(vcnt == 2) {
				length = value;
				sum = value;
				mode = 3;
				value = vcnt = 0;
			}
		} else if(mode == 3) {	// アドレス取得
			int alen = 0;
			if(type == 0) {
				alen = 4;
			} else if(type == 1) {
				alen = 4;
			} else if(type == 2) {
				alen = 6;
			} else if(type == 3) {
				alen = 8;
			} else if(type == 5) {
				alen = 4;
			} else if(type == 7) {
				alen = 8;
			} else if(type == 8) {
				alen = 6;
			} else if(type == 9) {
				alen = 4;
			} else {
				return -1;
			}
			if(vcnt == alen) {
				address = value;
				if(type >= 1 && type <= 3) {
					if(min > address) min = address;
				}
				alen >>= 1;
				length -= alen;
				length -= 1;	// SUM の分サイズを引く
				sh7125_sci_putch(1, '.');
				while(alen > 0) {
					sum += value;
					value >>= 8;
					--alen;
				}
				if(type >= 1 && type <= 3) {
					mode = 4;
				} else if(type >= 7 && type <= 9) {
					mode = 5;
				} else {
					mode = 4;
				}
				value = vcnt = 0;
			}
		} else if(mode == 4) {	// データ・レコード
			if(vcnt >= 2) {
				if(type >= 1 && type <= 3) {
					total += write_memory_byte(address, value);
					if(max < address) max = address;
					++address;
				}
				sum += value;
				value = vcnt = 0;
				--length;
				if(length == 0) {
					mode = 5;
				}
			}
		} else if(mode == 5) {	// SUM
			if(vcnt >= 2) {
				value &= 0xff;
				sum ^= 0xff;
				sum &= 0xff;
				if(sum != value) {	// SUM エラー
					sh7125_sci_puts(1, "\nS format SUM error: ");
					put_hexadecimal(value, 2);
					sh7125_sci_puts(1, " -> ");
					put_hexadecimal(sum, 2);
					sh7125_sci_puts(1, "\n");
					return -1;
				} else {
					if(type >= 7 && type <= 9) {
						toend = 1;
					}
					mode = 0;
					value = vcnt = 0;
				}
			}
		}
	}

	if(total) {
		sh7125_sci_puts(1, "\nS format downloaded: 0x");
		put_hexadecimal(min, 8);
		sh7125_sci_puts(1, " to 0x");
		put_hexadecimal(max, 8);
		sh7125_sci_puts(1, " ");
		put_decimal(total);
		sh7125_sci_puts(1, " bytes\n");
	}

	return 0;
}
#endif



	};

}

