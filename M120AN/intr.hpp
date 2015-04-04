#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・割り込み・レジスター定義 @n
			Copyright 2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  外部入力許可レジスター INTEN
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0038> inten_io;
	struct inten_t : public inten_io {
		using inten_io::operator =;
		using inten_io::operator ();
		using inten_io::operator |=;
		using inten_io::operator &=;

		bit_t<inten_io, 0> INT0EN;
		bit_t<inten_io, 1> INT1EN;
		bit_t<inten_io, 2> INT2EN;
		bit_t<inten_io, 3> INT3EN;
	};
	static inten_t INTEN;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  INT 入力フィルタレジスタ０ INTF0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x003A> intf0_io;
	struct intf0_t : public intf0_io {
		using intf0_io::operator =;
		using intf0_io::operator ();
		using intf0_io::operator |=;
		using intf0_io::operator &=;

		bits_t<intf0_io, 0, 2> INT0F;
		bits_t<intf0_io, 2, 2> INT1F;
		bits_t<intf0_io, 4, 2> INT2F;
		bits_t<intf0_io, 5, 2> INT3F;
	};
	static intf0_t INTF0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  INT 入力エッジ選択レジスタ０ ISCR0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x003C> iscr0_io;
	struct iscr0_t : public iscr0_io {
		using iscr0_io::operator =;
		using iscr0_io::operator ();
		using iscr0_io::operator |=;
		using iscr0_io::operator &=;

		bits_t<iscr0_io, 0, 2> INT0S;
		bits_t<iscr0_io, 2, 2> INT1S;
		bits_t<iscr0_io, 4, 2> INT2S;
		bits_t<iscr0_io, 5, 2> INT3S;
	};
	static iscr0_t ISCR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  キー入力許可レジスタ KIEN
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x003E> kien_io;
	struct kien_t : public kien_io {
		using kien_io::operator =;
		using kien_io::operator ();
		using kien_io::operator |=;
		using kien_io::operator &=;

		bit_t<kien_io, 0> KI0EN;
		bit_t<kien_io, 1> KI0PL;
		bit_t<kien_io, 2> KI1EN;
		bit_t<kien_io, 3> KI1PL;
		bit_t<kien_io, 4> KI2EN;
		bit_t<kien_io, 5> KI2PL;
		bit_t<kien_io, 6> KI3EN;
		bit_t<kien_io, 7> KI3PL;
	};
	static kien_t KIEN;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込み優先レベルレジスタi ILVLi (i = 0, 2, ～E)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint16_t base>
	struct ilvl_t : public io8<base> {
		typedef io8<base> rw_io;
		using rw_io::operator =;
		using rw_io::operator ();
		using rw_io::operator |=;
		using rw_io::operator &=;

		bits_t<rw_io, 0, 2> B01;
		bits_t<rw_io, 4, 2> B45;
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
	typedef io8<0x0050> irr0_io;
	struct irr0_t : public irr0_io {
		using irr0_io::operator =;
		using irr0_io::operator ();
		using irr0_io::operator |=;
		using irr0_io::operator &=;

		bit_t<irr0_io, 0> IRTJ;
		bit_t<irr0_io, 1> IRTB;
		bit_t<irr0_io, 2> IRTC;

		bit_t<irr0_io, 4> IRS0T;
		bit_t<irr0_io, 5> IRS0R;
	};
	static irr0_t IRR0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ１ IRR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0051> irr1_io;
	struct irr1_t : public irr1_io {
		using irr1_io::operator =;
		using irr1_io::operator ();
		using irr1_io::operator |=;
		using irr1_io::operator &=;

		bit_t<irr1_io, 2> IRAD;

		bit_t<irr1_io, 4> IRFM;
		bit_t<irr1_io, 5> IRWD;
	};
	static irr1_t IRR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ２ IRR2
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0052> irr2_io;
	struct irr2_t : public irr2_io {
		using irr2_io::operator =;
		using irr2_io::operator ();
		using irr2_io::operator |=;
		using irr2_io::operator &=;

		bit_t<irr2_io, 2> IRCMP1;
		bit_t<irr2_io, 3> IRCMP3;
	};
	static irr2_t IRR2;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  割り込みモニタフラグレジスタ３ IRR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x0053> irr3_io;
	struct irr3_t : public irr3_io {
		using irr3_io::operator =;
		using irr3_io::operator ();
		using irr3_io::operator |=;
		using irr3_io::operator &=;

		bit_t<irr3_io, 0> IRI0;
		bit_t<irr3_io, 1> IRI1;
		bit_t<irr3_io, 2> IRI2;
		bit_t<irr3_io, 3> IRI3;

		bit_t<irr3_io, 5> IRKI;
	};
	static irr3_t IRR3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込みレジスタi AIADRi (i = 0, 1)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static io8<0x01c0> AIADR0L;
	static io8<0x01c1> AIADR0M;
	static io8<0x01c2> AIADR0H;

	static io8<0x01c4> AIADR1L;
	static io8<0x01c5> AIADR1M;
	static io8<0x01c6> AIADR1H;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込み許可レジスタ０ AIEN0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01c3> aien0_io;
	struct aien0_t : public aien0_io {
		using aien0_io::operator =;
		using aien0_io::operator ();
		using aien0_io::operator |=;
		using aien0_io::operator &=;

		bit_t<aien0_io, 0> AIEN00;
	};
	static aien0_t AIEN0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アドレス一致割り込み許可レジスタ１ AIEN1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef io8<0x01c7> aien1_io;
	struct aien1_t : public aien1_io {
		using aien1_io::operator =;
		using aien1_io::operator ();
		using aien1_io::operator |=;
		using aien1_io::operator &=;

		bit_t<aien1_io, 0> AIEN10;
	};
	static aien1_t AIEN1;

}
