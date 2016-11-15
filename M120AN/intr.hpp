#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・割り込み・レジスター定義 @n
			Copyright 2015, 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  外部入力許可レジスター INTEN
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct inten_t : public rw8_t<0x0038> {
		typedef rw8_t<0x0038> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> INT0EN;
		bit_rw_t<io_, bitpos::B1> INT1EN;
		bit_rw_t<io_, bitpos::B2> INT2EN;
		bit_rw_t<io_, bitpos::B3> INT3EN;
	};
	static inten_t INTEN;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  INT 入力フィルタレジスタ０ INTF0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct intf0_t : public rw8_t<0x003A> {
		typedef rw8_t<0x003A> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> INT0F;
		bits_rw_t<io_, bitpos::B2, 2> INT1F;
		bits_rw_t<io_, bitpos::B4, 2> INT2F;
		bits_rw_t<io_, bitpos::B5, 2> INT3F;
	};
	static intf0_t INTF0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  INT 入力エッジ選択レジスタ０ ISCR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct iscr0_t : public rw8_t<0x003C> {
		typedef rw8_t<0x003C> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> INT0S;
		bits_rw_t<io_, bitpos::B2, 2> INT1S;
		bits_rw_t<io_, bitpos::B4, 2> INT2S;
		bits_rw_t<io_, bitpos::B5, 2> INT3S;
	};
	static iscr0_t ISCR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  キー入力許可レジスタ KIEN
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct kien_t : public rw8_t<0x003E> {
		typedef rw8_t<0x003E> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> KI0EN;
		bit_rw_t<io_, bitpos::B1> KI0PL;
		bit_rw_t<io_, bitpos::B2> KI1EN;
		bit_rw_t<io_, bitpos::B3> KI1PL;
		bit_rw_t<io_, bitpos::B4> KI2EN;
		bit_rw_t<io_, bitpos::B5> KI2PL;
		bit_rw_t<io_, bitpos::B6> KI3EN;
		bit_rw_t<io_, bitpos::B7> KI3PL;
	};
	static kien_t KIEN;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込み優先レベルレジスタi ILVLi (i = 0, 2, ～E)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint16_t base>
	struct ilvl_t : public rw8_t<base> {
		typedef rw8_t<base> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> B01;
		bits_rw_t<io_, bitpos::B4, 2> B45;
	};
	static ilvl_t<0x40> ILVL0;
	static ilvl_t<0x42> ILVL2;
	static ilvl_t<0x43> ILVL3;
	static ilvl_t<0x44> ILVL4;
	static ilvl_t<0x45> ILVL5;
	static ilvl_t<0x46> ILVL6;
	static ilvl_t<0x47> ILVL7;
	static ilvl_t<0x48> ILVL8;
	static ilvl_t<0x49> ILVL9;
	static ilvl_t<0x4A> ILVLA;
	static ilvl_t<0x4B> ILVLB;
	static ilvl_t<0x4C> ILVLC;
	static ilvl_t<0x4D> ILVLD;
	static ilvl_t<0x4E> ILVLE;





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ０ IRR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct irr0_t : public rw8_t<0x0050> {
		typedef rw8_t<0x0050> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> IRTJ;
		bit_rw_t<io_, bitpos::B1> IRTB;
		bit_rw_t<io_, bitpos::B2> IRTC;

		bit_rw_t<io_, bitpos::B4> IRS0T;
		bit_rw_t<io_, bitpos::B5> IRS0R;
	};
	static irr0_t IRR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ１ IRR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct irr1_t : public rw8_t<0x0051> {
		typedef rw8_t<0x0051> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> IRAD;

		bit_rw_t<io_, bitpos::B4> IRFM;
		bit_rw_t<io_, bitpos::B5> IRWD;
	};
	static irr1_t IRR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ２ IRR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct irr2_t : public rw8_t<0x0052> {
		typedef rw8_t<0x0052> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> IRCMP1;
		bit_rw_t<io_, bitpos::B3> IRCMP3;
	};
	static irr2_t IRR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ３ IRR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct irr3_t : public rw8_t<0x0053> {
		typedef rw8_t<0x0053> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> IRI0;
		bit_rw_t<io_, bitpos::B1> IRI1;
		bit_rw_t<io_, bitpos::B2> IRI2;
		bit_rw_t<io_, bitpos::B3> IRI3;

		bit_rw_t<io_, bitpos::B5> IRKI;
	};
	static irr3_t IRR3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込みレジスタi AIADRi (i = 0, 1)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static rw8_t<0x01c0> AIADR0L;
	static rw8_t<0x01c1> AIADR0M;
	static rw8_t<0x01c2> AIADR0H;

	static rw8_t<0x01c4> AIADR1L;
	static rw8_t<0x01c5> AIADR1M;
	static rw8_t<0x01c6> AIADR1H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込み許可レジスタ０ AIEN0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct aien0_t : public rw8_t<0x01C3> {
		typedef rw8_t<0x01C3> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> AIEN00;
	};
	static aien0_t AIEN0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込み許可レジスタ１ AIEN1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct aien1_t : public rw8_t<0x01C7> {
		typedef rw8_t<0x01C7> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> AIEN10;
	};
	static aien1_t AIEN1;

}
