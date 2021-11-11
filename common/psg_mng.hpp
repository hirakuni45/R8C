#pragma once
//=====================================================================//
/*!	@file
	@brief	PSG Emulations Manager @n
			ファミコン内蔵音源と同じような機能を持った波形生成 @n
			波形をレンダリングして波形バッファに生成する。 @n
			生成した波形メモリを PWM 変調などで出力する事を前提にしている。 @n
			分解能は８ビット
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "renesas.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  PSG ベース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class psg_base {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  波形タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class WTYPE : uint8_t {
			SQ25,	///< 矩形波 Duty25%
			SQ50,	///< 矩形波 Duty50%
			SQ75,	///< 矩形波 Duty75%
			TRI,	///< 三角波
			NOISE,	///< ノイズ（現在未サポート）
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  １２平均音階率によるキーテーブル @n
					(0)27.5, (1)55, (2)110, (3)220, (4)440, (5)880, (6)1760, (7)3520
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class KEY : uint8_t {
			A_1,	///< A  ラ  27.5Hz
			As_1,	///< A#
			B_1,	///< B  シ
			C_1,	///< C  ド
			Cs_1,	///< C#
			D_1,	///< D  レ
			Ds_1,	///< D#
			E_1,	///< E  ミ
			F_1,	///< F  ファ
			Fs_1,	///< F#
			G_1,	///< G  ソ
			Gs_1,	///< G#

			A_2,	///< A  ラ  55Hz
			As_2,	///< A#
			B_2,	///< B  シ
			C_2,	///< C  ド
			Cs_2,	///< C#
			D_2,	///< D  レ
			Ds_2,	///< D#
			E_2,	///< E  ミ
			F_2,	///< F  ファ
			Fs_2,	///< F#
			G_2,	///< G  ソ
			Gs_2,	///< G#

			A_3,	///< A  ラ  110Hz
			As_3,	///< A#
			B_3,	///< B  シ
			C_3,	///< C  ド
			Cs_3,	///< C#
			D_3,	///< D  レ
			Ds_3,	///< D#
			E_3,	///< E  ミ
			F_3,	///< F  ファ
			Fs_3,	///< F#
			G_3,	///< G  ソ
			Gs_3,	///< G#

			A_4,	///< A  ラ  220Hz
			As_4,	///< A#
			B_4,	///< B  シ
			C_4,	///< C  ド
			Cs_4,	///< C#
			D_4,	///< D  レ
			Ds_4,	///< D#
			E_4,	///< E  ミ
			F_4,	///< F  ファ
			Fs_4,	///< F#
			G_4,	///< G  ソ
			Gs_4,	///< G#

			A_5,	///< A  ラ  440Hz
			As_5,	///< A#
			B_5,	///< B  シ
			C_5,	///< C  ド
			Cs_5,	///< C#
			D_5,	///< D  レ
			Ds_5,	///< D#
			E_5,	///< E  ミ
			F_5,	///< F  ファ
			Fs_5,	///< F#
			G_5,	///< G  ソ
			Gs_5,	///< G#

			A_6,	///< A  ラ  880Hz
			As_6,	///< A#
			B_6,	///< B  シ
			C_6,	///< C  ド
			Cs_6,	///< C#
			D_6,	///< D  レ
			Ds_6,	///< D#
			E_6,	///< E  ミ
			F_6,	///< F  ファ
			Fs_6,	///< F#
			G_6,	///< G  ソ
			Gs_6,	///< G#

			A_7,	///< A  ラ 1760Hz
			As_7,	///< A#
			B_7,	///< B  シ
			C_7,	///< C  ド
			Cs_7,	///< C#
			D_7,	///< D  レ
			Ds_7,	///< D#
			E_7,	///< E  ミ
			F_7,	///< F  ファ
			Fs_7,	///< F#
			G_7,	///< G  ソ
			Gs_7,	///< G#

			Q,		///< 休符
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  制御コマンド
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class CTRL : uint8_t {
			TR = 84+1,	///< (2) トランスポーズ, num
			SQ25,		///< (1) 波形 SQ25
			SQ50,		///< (1) 波形 SQ50
			SQ75,		///< (1) 波形 SQ75
			TRI,		///< (1) 波形 TRI
			VOLUME,		///< (2) ボリューム
			TEMPO,		///< (2) テンポ, num
			FOR,		///< (2) ループ開始, num
			BEFORE,		///< (1) ループ終了
			END,		///< (1) 終了
			LOOP,		///< (1) 最初に戻る
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  スコア・コマンド構造 @n
					・KEY, len @n
					・TR, num @n
					・TEMPO, num @n
					・FOR, num
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct SCORE {
			union {
				KEY		key;
				CTRL	ctrl;
				uint8_t	len;
			};
			SCORE(KEY k) : key(k) { }
			SCORE(CTRL c) : ctrl(c) { }
			SCORE(uint8_t l) : len(l) { }
		};
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  PSG Manager テンプレート class @n
				PWM 変調は、(F_CLK:20MHz / 4 / 256) = 19531Hz としている。 
		@param[in] BSIZE	バッファサイズ（通常５１２）
		@param[in] CNUM		チャネル数（通常４）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint16_t BSIZE, uint16_t CNUM>
	class psg_mng : public psg_base {

		static const uint16_t BASE_FRQ = F_CLK / 4 / 256;

		// 12 平均音階率の計算：
		// 2^(1/12) の定数、１２乗すると２倍（１オクターブ上がる）となる。
		static constexpr uint16_t key_tbl_[12] = {
			static_cast<uint16_t>((3520 * 65536.0 * 1.000000000) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.059463094) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.122462048) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.189207115) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.25992105 ) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.334839854) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.414213562) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.498307077) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.587401052) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.681792831) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.781797436) / (F_CLK / 4.0 / 256.0)),
			static_cast<uint16_t>((3520 * 65536.0 * 1.887748625) / (F_CLK / 4.0 / 256.0)),
		};

		uint16_t	wav_pos_;
		uint8_t		wav_[BSIZE];

		bool		pause_;

		struct channel {
			uint8_t		vol_;  // 0 to 15
			WTYPE		wtype_;
			uint16_t	acc_;
			uint16_t	spd_;
			const SCORE*	score_org_;
			uint16_t	score_pos_;
			uint8_t		tempo_;
			uint8_t		count_;
			uint8_t		alen_;
			uint8_t		tr_;
			uint16_t	loop_org_;
			uint8_t		loop_cnt_;
			channel() : vol_(0), wtype_(WTYPE::SQ50), acc_(0), spd_(0),
				score_org_(nullptr), score_pos_(0),
				tempo_(0), count_(0), alen_(0),
				tr_(0), loop_org_(0), loop_cnt_(0)
			{ }

			void update() { acc_ += spd_; }

			int8_t get()
			{
				if(spd_ == 0) return 0;

				bool on = false;
				switch(wtype_) {
				case WTYPE::SQ25:
					if((acc_ & 0xc000) >= 0xc000) on = true;
					break;
				case WTYPE::SQ50:
					if((acc_ & 0xc000) >= 0x8000) on = true;
					break;
				case WTYPE::SQ75:
					if((acc_ & 0xc000) >= 0x4000) on = true;
					break;
				case WTYPE::TRI:
					{
						int8_t w = (acc_ >> 11) & 0x7;
						if((acc_ & 0x4000) != 0) w ^= 0x7;
						w *= vol_;
						if((acc_ & 0x8000) != 0) {
							w = -w;
						}
						return w;
					}
				case WTYPE::NOISE:
					return 0;
				}
//				int8_t w = (vol_ << 3) | (vol_ >> 1);
				// 矩形波は、三角波に比べて、音圧が高いので、バランスを取る為少し弱める。
				int8_t w = (vol_ << 2) + (vol_ << 1);
				if(!on) { w = -w; }
				return w;
			}

			void set_freq(uint16_t frq) { spd_ = (static_cast<uint32_t>(frq) << 16) / BASE_FRQ; }
//			void set_spd(uint16_t spd) { spd_ = spd; }

			// 完了なら「true」
			bool service()
			{
				if(score_org_ == nullptr) return true;

				if(alen_ > 0) {
					if(count_ > 0) {
						count_--;
					} else {
						alen_--;
					}
					return true;
				}

				auto v = score_org_[score_pos_];
				++score_pos_;
				if(v.len < 84) {
					v.len += tr_;
					if(v.len >= 0x80) v.len = 0;
					else if(v.len >= 84) v.len = 84;
					auto o = v.len / 12;
					auto k = v.len % 12;
					spd_ = key_tbl_[k] >> (7 - o);
					acc_ = 0;
					count_ = tempo_;
					alen_ = score_org_[score_pos_].len;
					++score_pos_;
					return true;
				} else if(v.len == 84) {  // 休符
					spd_ = 0;
					acc_ = 0;
					count_ = tempo_;
					alen_ = score_org_[score_pos_].len;
					++score_pos_;
					return true;
				} else {
					switch(v.ctrl) {
					case CTRL::TR:
						tr_ = score_org_[score_pos_].len;
						++score_pos_;
						break;
					case CTRL::SQ25:
						wtype_ = WTYPE::SQ25;
						break;
					case CTRL::SQ50:
						wtype_ = WTYPE::SQ50;
						break;
					case CTRL::SQ75:
						wtype_ = WTYPE::SQ75;
						break;
					case CTRL::TRI:
						wtype_ = WTYPE::TRI;
						break;
					case CTRL::VOLUME:
						vol_ = score_org_[score_pos_].len;
						++score_pos_;
						break;
					case CTRL::TEMPO:
						tempo_ = score_org_[score_pos_].len;
						++score_pos_;
						count_ = 0;
						break;
					case CTRL::FOR:
						loop_cnt_ = score_org_[score_pos_].len;
						++score_pos_;
						loop_org_ = score_pos_;
						break;
					case CTRL::BEFORE:
						if(loop_cnt_ > 1) {
							loop_cnt_--;
							score_pos_ = loop_org_;
						}
						break;
					case CTRL::END:
						score_org_ = nullptr;
						spd_ = 0;
						break;
					case CTRL::LOOP:
						score_pos_ = 0;
						alen_ = 0;
						tempo_ = 0;
						count_ = 0;
						break;
					default:
						break;
					}
					return false;
				}
			}
		};

		channel		channel_[CNUM];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		psg_mng() noexcept : wav_pos_(0), wav_{ 0x80 },
			pause_(false),
			channel_{ }
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  波形タイプの設定
			@param[in]	ch		チャネル番号
			@param[in]	wt		波形タイプ
		*/
		//-----------------------------------------------------------------//
		void set_wtype(uint8_t ch, WTYPE wt) noexcept
		{
			if(ch >= CNUM) return;
			channel_[ch].wtype_ = wt;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ボリュームの設定
			@param[in]	ch		チャネル番号
			@param[in]	vol		ボリューム（0 to 15）	
		*/
		//-----------------------------------------------------------------//
		void set_volume(uint8_t ch, uint8_t vol) noexcept
		{
			if(ch >= CNUM) return;
			channel_[ch].vol_ = vol & 15;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  周波数で指定
			@param[in]	ch		チャネル番号
			@param[in]	frq		周波数
		*/
		//-----------------------------------------------------------------//
		void set_freq(uint8_t ch, uint16_t frq) noexcept
		{
			if(ch >= CNUM) return;
			channel_[ch].set_freq(frq);
			channel_[ch].acc_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  キー、オクターブで指定
			@param[in]	ch		チャネル番号
			@param[in]	key		キー
		*/
		//-----------------------------------------------------------------//
		void set_key(uint8_t ch, KEY key) noexcept
		{
			if(ch >= CNUM) return;
			auto o = static_cast<uint8_t>(key) / 12;
			auto k = static_cast<uint8_t>(key) % 12;
			channel_[ch].spd_ = key_tbl_[k] >> (7 - o);
			channel_[ch].acc_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
			@param[in]	count	回数
		*/
		//-----------------------------------------------------------------//
		void render(uint16_t count) noexcept
		{
			for(uint16_t i = 0; i < count; ++i) {
				int16_t sum = 0;
				for(uint8_t j = 0; j < CNUM; ++j) {
					channel_[j].update();
					sum += channel_[j].get();
				}
				wav_[wav_pos_] = (sum / CNUM) + 0x80;
				++wav_pos_;
				if(wav_pos_ >= BSIZE) wav_pos_ = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形の取得
			@param[in]	idx		バッファの位置
			@return	波形（+V:0xff, 0V:0x80, -V:0x00）
		*/
		//-----------------------------------------------------------------//
		uint8_t get_wav(uint16_t idx) const noexcept { return wav_[idx]; }


		//-----------------------------------------------------------------//
		/*!
			@brief  スコアの設定
			@param[in]	ch		チャネル番号
			@param[in]	score	スコア
		*/
		//-----------------------------------------------------------------//
		void set_score(uint8_t ch, const SCORE* score)
		{
			if(ch >= CNUM) return;
			channel_[ch].score_org_ = score;
			channel_[ch].score_pos_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  演奏サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept
		{
			for(uint8_t i = 0; i < CNUM; ++i) {
				for(;;) {
					if(channel_[i].service()) {
						break;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  演奏の一時停止
			@param[in]	ena		「false」を指定するとポーズを解除
		*/
		//-----------------------------------------------------------------//
		void pause(bool ena = true) noexcept { pause_ = ena; }
	};

	template<uint16_t BSIZE, uint16_t CNUM> constexpr uint16_t psg_mng<BSIZE, CNUM>::key_tbl_[12];
}