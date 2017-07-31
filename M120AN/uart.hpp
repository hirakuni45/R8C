#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・シリアルインターフェース定義 @n
   			※複数チャネルを持つデバイスを考慮している為、シングルチャネル@n
			デバイスでは、レジスター名を読み替える必要があります。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2014, 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	template <uint16_t base>
	struct uart {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信モードレジスタ UMR
			@param[in]	ofs	オフセット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <uint16_t ofs>
		struct umr_t : public rw8_t<ofs> {
			typedef rw8_t<ofs> io_;
			using io_::operator =;
			using io_::operator ();
			using io_::operator |=;
			using io_::operator &=;

			bits_rw_t<io_, bitpos::B0, 3> SMD;
			bit_rw_t <io_, bitpos::B3>    CKDIR;
			bit_rw_t <io_, bitpos::B4>    STPS;
			bit_rw_t <io_, bitpos::B5>    PRY;
			bit_rw_t <io_, bitpos::B6>    PRYE;
		};
		static umr_t<base + 0x00> UMR;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART ビットレートレジスタ UBRG
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static rw8_t<base + 0x01> UBRG;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送信バッファレジスタ UTBL
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static wo8_t<base + 0x02> UTBL;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送信バッファレジスタ UTBH
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static wo8_t<base + 0x03> UTBH;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信制御レジスタ０ UC0
			@param[in]	ofs	オフセット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <uint16_t ofs>
		struct uc0_t : public rw8_t<ofs> {
			typedef rw8_t<ofs> io_;
			using io_::operator =;
			using io_::operator ();
			using io_::operator |=;
			using io_::operator &=;

			bits_rw_t<io_, bitpos::B0, 2> CLK;
			bit_rw_t <io_, bitpos::B3>    TXEPT;
			bit_rw_t <io_, bitpos::B4>    DFE;
			bit_rw_t <io_, bitpos::B5>    NCH;
			bit_rw_t <io_, bitpos::B6>    CKPOL;
			bit_rw_t <io_, bitpos::B7>    UFORM;
		};
		static uc0_t<base + 0x04> UC0;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信制御レジスタ１ UC1
			@param[in]	ofs	オフセット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <uint16_t ofs>
		struct uc1_t : public rw8_t<ofs> {
			typedef rw8_t<ofs> io_;
			using io_::operator =;
			using io_::operator ();
			using io_::operator |=;
			using io_::operator &=;

			bit_rw_t<io_, bitpos::B0> TE;
			bit_rw_t<io_, bitpos::B1> TI;
			bit_rw_t<io_, bitpos::B2> RE;
			bit_rw_t<io_, bitpos::B3> RI;
			bit_rw_t<io_, bitpos::B4> UIRS;
			bit_rw_t<io_, bitpos::B5> URPM;
		};
		static uc1_t<base + 0x05> UC1;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 受信バッファレジスタ URB
			@param[in]	ofs	オフセット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <uint16_t ofs>
		struct urb_t : public rw16_t<ofs> {
			typedef rw16_t<ofs> io_;
			using io_::operator =;
			using io_::operator ();
			using io_::operator |=;
			using io_::operator &=;

			bits_rw_t<io_, bitpos::B0, 8> URBL;
			bits_rw_t<io_, bitpos::B8, 8> URBH;

			bit_rw_t <io_, bitpos::B12> OER;
			bit_rw_t <io_, bitpos::B13> FER;
			bit_rw_t <io_, bitpos::B14> PER;
			bit_rw_t <io_, bitpos::B15> SUM;
		};
		static urb_t<base + 0x06> URB;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 割り込みフラグと許可レジスタ UIR
			@param[in]	ofs	オフセット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <uint16_t ofs>
		struct uir_t : public rw8_t<ofs> {
			typedef rw8_t<ofs> io_;
			using io_::operator =;
			using io_::operator ();
			using io_::operator |=;
			using io_::operator &=;

			bit_rw_t<io_, bitpos::B2> URIE;
			bit_rw_t<io_, bitpos::B3> UTIE;
			bit_rw_t<io_, bitpos::B6> URIF;
			bit_rw_t<io_, bitpos::B7> UTIF;
		};
		static uir_t<base + 0x08> UIR;

	};

	typedef uart<0x0080> UART0;
}
