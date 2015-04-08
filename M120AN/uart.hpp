#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・シリアルインターフェース定義 @n
   			※複数チャネルを持つデバイスを考慮している為、シングルチャネル@n
			デバイスでは、レジスター名を読み替える必要があります。@n
			Copyright 2014,2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	template <uint16_t base>
	struct uart {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信モードレジスタ UMR
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base> umr_io;
		struct umr_t : public umr_io {
			using umr_io::operator =;
			using umr_io::operator ();
			using umr_io::operator |=;
			using umr_io::operator &=;

			bits_t<umr_io, 0, 3> SMD;
			bit_t <umr_io, 3>    CKDIR;
			bit_t <umr_io, 4>    STPS;
			bit_t <umr_io, 5>    PRY;
			bit_t <umr_io, 6>    PRYE;
		};
		static umr_t UMR;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART ビットレートレジスタ UBRG
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 1> ubrg_io;
		struct ubrg_t : public ubrg_io {
			using ubrg_io::operator =;
			using ubrg_io::operator ();
			using ubrg_io::operator |=;
			using ubrg_io::operator &=;
		};
		static ubrg_t UBRG;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送信バッファレジスタ UTBL
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 2> utbl_io;
		struct utbl_t : public utbl_io {
			using utbl_io::operator =;
		};
		static utbl_t UTBL;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送信バッファレジスタ UTBH
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 3> utbh_io;
		struct utbh_t : public utbh_io {
			using utbh_io::operator =;
		};
		static utbh_t UTBH;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信制御レジスタ０ UC0
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 4> uc0_io;
		struct uc0_t : public uc0_io {
			using uc0_io::operator =;
			using uc0_io::operator ();
			using uc0_io::operator |=;
			using uc0_io::operator &=;

			bits_t<uc0_io, 0, 2> CLK;
			bit_t <uc0_io, 3>    TXEPT;
			bit_t <uc0_io, 4>    DFE;
			bit_t <uc0_io, 5>    NCH;
			bit_t <uc0_io, 6>    CKPOL;
			bit_t <uc0_io, 7>    UFORM;
		};
		static uc0_t UC0;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 送受信制御レジスタ１ UC1
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 5> uc1_io;
		struct uc1_t : public uc1_io {
			using uc1_io::operator =;
			using uc1_io::operator ();
			using uc1_io::operator |=;
			using uc1_io::operator &=;

			bit_t <uc1_io, 0> TE;
			bit_t <uc1_io, 1> TI;
			bit_t <uc1_io, 2> RE;
			bit_t <uc1_io, 3> RI;
			bit_t <uc1_io, 4> UIRS;
			bit_t <uc1_io, 5> URPM;
		};
		static uc1_t UC1;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 受信バッファレジスタ URB
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io16_ro<base + 6> urb_io;
		struct urb_t : public urb_io {
			using urb_io::operator ();

			bits_t<urb_io, 0, 8> URBL;
			bits_t<urb_io, 8, 8> URBH;

			bit_t <urb_io, 12> OER;
			bit_t <urb_io, 13> FER;
			bit_t <urb_io, 14> PER;
			bit_t <urb_io, 15> SUM;
		};
		static urb_t URB;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	UART 割り込みフラグと許可レジスタ UIR
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef io8<base + 8> uir_io;
		struct uir_t : public uir_io {
			using uir_io::operator =;
			using uir_io::operator ();
			using uir_io::operator |=;
			using uir_io::operator &=;

			bit_t <uir_io, 2> URIE;
			bit_t <uir_io, 3> UTIE;
			bit_t <uir_io, 6> URIF;
			bit_t <uir_io, 7> UTIF;
		};
		static uir_t UIR;

	};

	typedef uart<0x80> UART0;
}
