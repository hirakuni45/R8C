#pragma once
//=====================================================================//
/*!	@file
	@brief	R8C/M110AN, R8C/M120AN グループ・ポート・レジスター定義 @n
			Copyright 2014,2015 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/io_utils.hpp"

namespace device {

	/// @brief レジスタ定義
	typedef io8<0x00B9> pinsr_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート入出力機能制御レジスタ PINSR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pinsr_t : public pinsr_io {
		using pinsr_io::operator =;
		using pinsr_io::operator ();
		using pinsr_io::operator |=;
		using pinsr_io::operator &=;

		bit_t<pinsr_io, 6> TRJIOSEL;  /// TRJIO 入力信号選択（0: 外部 TRJIO端子から、1: VCOUT1 から内部入力） 
		bit_t<pinsr_io, 7> IOINSEL;   /// 端子レベル強制読み出し（0: 禁止 PDi レジスタ制御、1: 許可）
	};
	static pinsr_t PINSR;


	/// @brief レジスタ定義
	typedef io8<0x00A9> pd1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P1 方向レジスタ PD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pd1_t : public pd1_io {
		using pd1_io::operator =;
		using pd1_io::operator ();
		using pd1_io::operator |=;
		using pd1_io::operator &=;

		bit_t<pd1_io, 0> B0;
		bit_t<pd1_io, 1> B1;
		bit_t<pd1_io, 2> B2;
		bit_t<pd1_io, 3> B3;
		bit_t<pd1_io, 4> B4;
		bit_t<pd1_io, 5> B5;
		bit_t<pd1_io, 6> B6;
		bit_t<pd1_io, 7> B7;
	};
	static pd1_t PD1;


	/// @brief レジスタ定義
	typedef io8<0x00AF> p1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P1 レジスタ P1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct p1_t : public p1_io {
		using p1_io::operator =;
		using p1_io::operator ();
		using p1_io::operator |=;
		using p1_io::operator &=;

		bit_t<p1_io, 0> B0;
		bit_t<p1_io, 1> B1;
		bit_t<p1_io, 2> B2;
		bit_t<p1_io, 3> B3;
		bit_t<p1_io, 4> B4;
		bit_t<p1_io, 5> B5;
		bit_t<p1_io, 6> B6;
		bit_t<p1_io, 7> B7;
	};
	static p1_t P1;


	/// @brief レジスタ定義
	typedef io8<0x00B5> pur1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ１ PUR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pur1_t : public pur1_io {
		using pur1_io::operator =;
		using pur1_io::operator ();
		using pur1_io::operator |=;
		using pur1_io::operator &=;

		bit_t<pur1_io, 0> B0;
		bit_t<pur1_io, 1> B1;
		bit_t<pur1_io, 2> B2;
		bit_t<pur1_io, 3> B3;
		bit_t<pur1_io, 4> B4;
		bit_t<pur1_io, 5> B5;
		bit_t<pur1_io, 6> B6;
		bit_t<pur1_io, 7> B7;
	};
	static pur1_t PUR1;


	/// @brief レジスタ定義
	typedef io8<0x00BB> drr1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  駆動能力制御レジスタ１ DRR1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct drr1_t : public drr1_io {
		using drr1_io::operator =;
		using drr1_io::operator ();
		using drr1_io::operator |=;
		using drr1_io::operator &=;

		bit_t<drr1_io, 2> B2;
		bit_t<drr1_io, 3> B3;
		bit_t<drr1_io, 4> B4;
		bit_t<drr1_io, 5> B5;
	};
	static drr1_t DRR1;


	/// @brief レジスタ定義
	typedef io8<0x00C1> pod1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ１ POD1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pod1_t : public pod1_io {
		using pod1_io::operator =;
		using pod1_io::operator ();
		using pod1_io::operator |=;
		using pod1_io::operator &=;

		bit_t<pod1_io, 0> B0;
		bit_t<pod1_io, 1> B1;
		bit_t<pod1_io, 2> B2;
		bit_t<pod1_io, 3> B3;
		bit_t<pod1_io, 4> B4;
		bit_t<pod1_io, 5> B5;
		bit_t<pod1_io, 6> B6;
		bit_t<pod1_io, 7> B7;
	};
	static pod1_t POD1;


	/// @brief レジスタ定義
	typedef io8<0x00C8> pml1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピングレジスタ０ PML1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml1_t : public pml1_io {
		using pml1_io::operator =;
		using pml1_io::operator ();
		using pml1_io::operator |=;
		using pml1_io::operator &=;

		bits_t<pml1_io, 0, 2> P10SEL;
		bits_t<pml1_io, 2, 2> P11SEL;
		bits_t<pml1_io, 4, 2> P12SEL;
		bits_t<pml1_io, 6, 2> P13SEL;
	};
	static pml1_t PML1;


	/// @brief レジスタ定義
	typedef io8<0x00C9> pmh1_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピングレジスタ１ PMH1
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh1_t : public pmh1_io {
		using pmh1_io::operator =;
		using pmh1_io::operator ();
		using pmh1_io::operator |=;
		using pmh1_io::operator &=;

		bits_t<pmh1_io, 0, 2> P14SEL;
		bits_t<pmh1_io, 2, 2> P15SEL;
		bits_t<pmh1_io, 4, 2> P16SEL;
		bits_t<pmh1_io, 6, 2> P17SEL;
	};
	static pmh1_t PMH1;


	/// @brief レジスタ定義
	typedef io8<0x00D1> pmh1e_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート１機能マッピング拡張レジスタ PMH1E
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh1e_t : public pmh1e_io {
		using pmh1e_io::operator =;
		using pmh1e_io::operator ();
		using pmh1e_io::operator |=;
		using pmh1e_io::operator &=;

		bit_t<pmh1e_io, 0> P14SEL;
		bit_t<pmh1e_io, 2> P15SEL;
	};
	static pmh1e_t PMH1E;


	/// @brief レジスタ定義
	typedef io8<0x00AB> pd3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P3 方向レジスタ PD3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pd3_t : public pd3_io {
		using pd3_io::operator =;
		using pd3_io::operator ();
		using pd3_io::operator |=;
		using pd3_io::operator &=;

		bit_t<pd3_io, 3> B3;
		bit_t<pd3_io, 4> B4;
		bit_t<pd3_io, 5> B5;
		bit_t<pd3_io, 7> B7;
	};
	static pd3_t PD3;


	/// @brief レジスタ定義
	typedef io8<0x00B1> p3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P3 レジスタ P3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct p3_t : public p3_io {
		using p3_io::operator =;
		using p3_io::operator ();
		using p3_io::operator |=;
		using p3_io::operator &=;

		bit_t<p3_io, 3> B3;
		bit_t<p3_io, 4> B4;
		bit_t<p3_io, 5> B5;
		bit_t<p3_io, 7> B7;
	};
	static p3_t P3;


	/// @brief レジスタ定義
	typedef io8<0x00B1> pur3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ３ PUR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pur3_t : public pur3_io {
		using pur3_io::operator =;
		using pur3_io::operator ();
		using pur3_io::operator |=;
		using pur3_io::operator &=;

		bit_t<pur3_io, 3> B3;
		bit_t<pur3_io, 4> B4;
		bit_t<pur3_io, 5> B5;
		bit_t<pur3_io, 7> B7;
	};
	static pur3_t PUR3;


	/// @brief レジスタ定義
	typedef io8<0x00BD> drr3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  駆動能力制御レジスタ３ DRR3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct drr3_t : public drr3_io {
		using drr3_io::operator =;
		using drr3_io::operator ();
		using drr3_io::operator |=;
		using drr3_io::operator &=;

		bit_t<drr3_io, 3> B3;
		bit_t<drr3_io, 4> B4;
		bit_t<drr3_io, 5> B5;
		bit_t<drr3_io, 7> B7;
	};
	static drr3_t DRR3;


	/// @brief レジスタ定義
	typedef io8<0x00C3> pod3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ３ POD3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pod3_t : public pod3_io {
		using pod3_io::operator =;
		using pod3_io::operator ();
		using pod3_io::operator |=;
		using pod3_io::operator &=;

		bit_t<pod3_io, 3> B3;
		bit_t<pod3_io, 4> B4;
		bit_t<pod3_io, 5> B5;
		bit_t<pod3_io, 7> B7;
	};
	static pod3_t POD3;


	/// @brief レジスタ定義
	typedef io8<0x00CC> pml3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート３機能マッピングレジスタ０ PML3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml3_t : public pml3_io {
		using pml3_io::operator =;
		using pml3_io::operator ();
		using pml3_io::operator |=;
		using pml3_io::operator &=;

		bits_t<pml3_io, 6, 2> P33SEL;
	};
	static pml3_t PML3;


	/// @brief レジスタ定義
	typedef io8<0x00CD> pmh3_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート３機能マッピングレジスタ１ PMH3
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh3_t : public pmh3_io {
		using pmh3_io::operator =;
		using pmh3_io::operator ();
		using pmh3_io::operator |=;
		using pmh3_io::operator &=;

		bits_t<pmh3_io, 0, 2> P34SEL;
		bits_t<pmh3_io, 2, 2> P35SEL;
		bits_t<pmh3_io, 6, 2> P37SEL;
	};
	static pmh3_t PMH3;


	/// @brief レジスタ定義
	typedef io8<0x00AC> pd4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P4 方向レジスタ PD4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pd4_t : public pd4_io {
		using pd4_io::operator =;
		using pd4_io::operator ();
		using pd4_io::operator |=;
		using pd4_io::operator &=;

		bit_t<pd4_io, 2> B2;
		bit_t<pd4_io, 5> B5;
		bit_t<pd4_io, 6> B6;
		bit_t<pd4_io, 7> B7;
	};
	static pd4_t PD4;


	/// @brief レジスタ定義
	typedef io8<0x00B2> p4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート P4 レジスタ P4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct p4_t : public p4_io {
		using p4_io::operator =;
		using p4_io::operator ();
		using p4_io::operator |=;
		using p4_io::operator &=;

		bit_t<p4_io, 2> B2;
		bit_t<p4_io, 5> B5;
		bit_t<p4_io, 6> B6;
		bit_t<p4_io, 7> B7;
	};
	static p4_t P4;


	/// @brief レジスタ定義
	typedef io8<0x00B8> pur4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プルアップ制御レジスタ４ PUR4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pur4_t : public pur4_io {
		using pur4_io::operator =;
		using pur4_io::operator ();
		using pur4_io::operator |=;
		using pur4_io::operator &=;

		bit_t<pur4_io, 2> B2;
		bit_t<pur4_io, 5> B5;
		bit_t<pur4_io, 6> B6;
		bit_t<pur4_io, 7> B7;
	};
	static pur4_t PUR4;


	/// @brief レジスタ定義
	typedef io8<0x00C4> pod4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  オープンドレイン制御レジスタ４ POD4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pod4_t : public pod4_io {
		using pod4_io::operator =;
		using pod4_io::operator ();
		using pod4_io::operator |=;
		using pod4_io::operator &=;

		bit_t<pod4_io, 2> B2;
		bit_t<pod4_io, 5> B5;
		bit_t<pod4_io, 6> B6;
		bit_t<pod4_io, 7> B7;
	};
	static pod4_t POD4;


	/// @brief レジスタ定義
	typedef io8<0x00CE> pml4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピングレジスタ０ PML4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pml4_t : public pml4_io {
		using pml4_io::operator =;
		using pml4_io::operator ();
		using pml4_io::operator |=;
		using pml4_io::operator &=;

		bits_t<pml4_io, 4, 2> P42SEL;
	};
	static pml4_t PML4;


	/// @brief レジスタ定義
	typedef io8<0x00CF> pmh4_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピングレジスタ１ PMH4
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh4_t : public pmh4_io {
		using pmh4_io::operator =;
		using pmh4_io::operator ();
		using pmh4_io::operator |=;
		using pmh4_io::operator &=;

		bits_t<pmh4_io, 2, 2> P45SEL;
		bits_t<pmh4_io, 4, 2> P46SEL;
		bits_t<pmh4_io, 6, 2> P47SEL;
	};
	static pmh4_t PMH4;


	/// @brief レジスタ定義
	typedef io8<0x00D5> pmh4e_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート４機能マッピング拡張レジスタ PMH4E
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pmh4e_t : public pmh4e_io {
		using pmh4e_io::operator =;
		using pmh4e_io::operator ();
		using pmh4e_io::operator |=;
		using pmh4e_io::operator &=;

		bit_t<pmh4e_io, 4> P46SEL;
	};
	static pmh4e_t PMH4E;


	/// @brief レジスタ定義
	typedef io8<0x00AD> pda_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート PA 方向レジスタ PDA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pda_t : public pda_io {
		using pda_io::operator =;
		using pda_io::operator ();
		using pda_io::operator |=;
		using pda_io::operator &=;

		bit_t<pda_io, 0> B0;
	};
	static pda_t PDA;


	/// @brief レジスタ定義
	typedef io8<0x00B3> pa_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ポート PA レジスタ PA
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pa_t : public pa_io {
		using pa_io::operator =;
		using pa_io::operator ();
		using pa_io::operator |=;
		using pa_io::operator &=;

		bit_t<pa_io, 0> B0;
	};
	static pa_t PA;


	/// @brief レジスタ定義
	typedef io8<0x00C5> pamcr_io;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ポート PA 制御レジスタ PAMCR
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pamcr_t : public pamcr_io {
		using pamcr_io::operator =;
		using pamcr_io::operator ();
		using pamcr_io::operator |=;
		using pamcr_io::operator &=;

		bit_t<pamcr_io, 0> PODA;
		bit_t<pamcr_io, 4> HWRSTE;
	};
	static pamcr_t PAMCR;

}
