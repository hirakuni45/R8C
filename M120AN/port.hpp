#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・ポート・レジスター定義 @n
			Copyright 2014,2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct port1_t {
		static const uint16_t base_address_ = 0x00A9;
	};
	typedef port1_t PORT1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート３定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct port3_t {
		static const uint16_t base_address_ = 0x00AB;
	};
	typedef port3_t PORT3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct port4_t {
		static const uint16_t base_address_ = 0x00AC;
	};
	typedef port4_t PORT4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポートＡ定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct porta_t {
		static const uint16_t base_address_ = 0x00AD;
	};
	typedef porta_t PORTA;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート入出力機能制御レジスタ PINSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pinsr_t : public rw8_t<0x00B9> {
		typedef rw8_t<0x00B9> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		/// TRJIO 入力信号選択（0: 外部 TRJIO端子から、1: VCOUT1 から内部入力） 
		bit_rw_t<io_, bitpos::B6> TRJIOSEL;
		/// 端子レベル強制読み出し（0: 禁止 PDi レジスタ制御、1: 許可）
		bit_rw_t<io_, bitpos::B7> IOINSEL;
	};
	static pinsr_t PINSR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P1 方向レジスタ PD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static basic_rw_t<rw8_t<0x00A9> >  PD1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P1 レジスタ P1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static basic_rw_t<rw8_t<0x00AF> >  P1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ１ PUR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static basic_rw_t<rw8_t<0x00B5> >  PUR1; 


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  駆動能力制御レジスタ１ DRR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct drr1_t : public rw8_t<0x00BB> {
		typedef rw8_t<0x00BB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> B2;
		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
	};
	static drr1_t DRR1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ１ POD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static basic_rw_t<rw8_t<0x00C1> >  POD1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピングレジスタ０ PML1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml1_t : public rw8_t<0x00C8> {
		typedef rw8_t<0x00C8> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> P10SEL;
		bits_rw_t<io_, bitpos::B2, 2> P11SEL;
		bits_rw_t<io_, bitpos::B4, 2> P12SEL;
		bits_rw_t<io_, bitpos::B6, 2> P13SEL;
	};
	static pml1_t PML1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピングレジスタ１ PMH1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh1_t : public rw8_t<0x00C9> {
		typedef rw8_t<0x00C9> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> P14SEL;
		bits_rw_t<io_, bitpos::B2, 2> P15SEL;
		bits_rw_t<io_, bitpos::B4, 2> P16SEL;
		bits_rw_t<io_, bitpos::B6, 2> P17SEL;
	};
	static pmh1_t PMH1;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピング拡張レジスタ PMH1E
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh1e_t : public rw8_t<0x00D1> {
		typedef rw8_t<0x00D1> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> P14SEL;
		bit_rw_t<io_, bitpos::B2> P15SEL;
	};
	static pmh1e_t PMH1E;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P3 方向レジスタ PD3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pd3_t : public rw8_t<0x00AB> {
		typedef rw8_t<0x00AB> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pd3_t PD3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P3 レジスタ P3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct p3_t : public rw8_t<0x00B1> {
		typedef rw8_t<0x00B1> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static p3_t P3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ３ PUR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pur3_t : public rw8_t<0x00B7> {
		typedef rw8_t<0x00B7> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pur3_t PUR3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  駆動能力制御レジスタ３ DRR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct drr3_t : public rw8_t<0x00BD> {
		typedef rw8_t<0x00BD> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static drr3_t DRR3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ３ POD3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pod3_t : public rw8_t<0x00C3> {
		typedef rw8_t<0x00C3> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B3> B3;
		bit_rw_t<io_, bitpos::B4> B4;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pod3_t POD3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート３機能マッピングレジスタ０ PML3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml3_t : public rw8_t<0x00CC> {
		typedef rw8_t<0x00CC> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B6, 2> P33SEL;
	};
	static pml3_t PML3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート３機能マッピングレジスタ１ PMH3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh3_t : public rw8_t<0x00CD> {
		typedef rw8_t<0x00CD> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B0, 2> P34SEL;
		bits_rw_t<io_, bitpos::B2, 2> P35SEL;
		bits_rw_t<io_, bitpos::B6, 2> P37SEL;
	};
	static pmh3_t PMH3;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P4 方向レジスタ PD4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pd4_t : public rw8_t<0x00AC> {
		typedef rw8_t<0x00AC> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> B2;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B6> B6;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pd4_t PD4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P4 レジスタ P4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct p4_t : public rw8_t<0x00B2> {
		typedef rw8_t<0x00B2> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> B2;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B6> B6;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static p4_t P4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ４ PUR4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pur4_t : public rw8_t<0x00B8> {
		typedef rw8_t<0x00B8> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> B2;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B6> B6;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pur4_t PUR4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ４ POD4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pod4_t : public rw8_t<0x00C4> {
		typedef rw8_t<0x00C4> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B2> B2;
		bit_rw_t<io_, bitpos::B5> B5;
		bit_rw_t<io_, bitpos::B6> B6;
		bit_rw_t<io_, bitpos::B7> B7;
	};
	static pod4_t POD4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピングレジスタ０ PML4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml4_t : public rw8_t<0x00CE> {
		typedef rw8_t<0x00CE> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B4, 2> P42SEL;
	};
	static pml4_t PML4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピングレジスタ１ PMH4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh4_t : public rw8_t<0x00CF> {
		typedef rw8_t<0x00CF> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bits_rw_t<io_, bitpos::B2, 2> P45SEL;
		bits_rw_t<io_, bitpos::B4, 2> P46SEL;
		bits_rw_t<io_, bitpos::B6, 2> P47SEL;
	};
	static pmh4_t PMH4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピング拡張レジスタ PMH4E
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh4e_t : public rw8_t<0x00D5> {
		typedef rw8_t<0x00D5> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B4> P46SEL;
	};
	static pmh4e_t PMH4E;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート PA 方向レジスタ PDA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pda_t : public rw8_t<0x00AD> {
		typedef rw8_t<0x00AD> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> B0;
	};
	static pda_t PDA;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート PA レジスタ PA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pa_t : public rw8_t<0x00B3> {
		typedef rw8_t<0x00B3> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> B0;
	};
	static pa_t PA;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ポート PA 制御レジスタ PAMCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pamcr_t : public rw8_t<0x00C5> {
		typedef rw8_t<0x00C5> io_;
		using io_::operator =;
		using io_::operator ();
		using io_::operator |=;
		using io_::operator &=;

		bit_rw_t<io_, bitpos::B0> PODA;
		bit_rw_t<io_, bitpos::B4> HWRSTE;
	};
	static pamcr_t PAMCR;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  単ポート定義テンプレート
		@param[in]	PORTx	ポート定義
		@param[in]	bpos	ビット位置	
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class PORTx, bitpos bpos>
	struct PORT {

		//-----------------------------------------------------------------//
		/*!
			@brief  ポート方向レジスタ
		*/
		//-----------------------------------------------------------------//
		static bit_rw_t<rw8_t<PORTx::base_address_ + 0x00>, bpos> DIR;


		//-----------------------------------------------------------------//
		/*!
			@brief  プルアップ制御・レジスタ
		*/
		//-----------------------------------------------------------------//
		static bit_rw_t<rw8_t<PORTx::base_address_ + 0x0C>, bpos> PU;


		//-----------------------------------------------------------------//
		/*!
			@brief  オープンドレイン制御・レジスタ
		*/
		//-----------------------------------------------------------------//
		static bit_rw_t<rw8_t<PORTx::base_address_ + 0x18>, bpos> OD;


		//-----------------------------------------------------------------//
		/*!
			@brief  ポート・レジスタ
		*/
		//-----------------------------------------------------------------//
		static bit_rw_t<rw8_t<PORTx::base_address_ + 0x06>, bpos> P;

	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  無効ポート定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct NULL_PORT {

		struct null_t {
			void operator = (bool f) { }
			bool operator () () const { return 0; } 
		};

		//-----------------------------------------------------------------//
		/*!
			@brief  ポート方向レジスタ
		*/
		//-----------------------------------------------------------------//
		static null_t DIR;


		//-----------------------------------------------------------------//
		/*!
			@brief  プルアップ制御・レジスタ
		*/
		//-----------------------------------------------------------------//
		static null_t PU;


		//-----------------------------------------------------------------//
		/*!
			@brief  オープンドレイン制御・レジスタ
		*/
		//-----------------------------------------------------------------//
		static null_t OD;


		//-----------------------------------------------------------------//
		/*!
			@brief  ポート・レジスタ
		*/
		//-----------------------------------------------------------------//
		static null_t P;

	};
}
