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
#include "common/format.hpp"

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
			A_0,		///< A  ラ  27.5Hz (0)
			As0,		///< A#
			Bb0 = As0,	///< Bb
			B_0,		///< B  シ
			C_1,		///< C  ド
			Cs1,		///< C#
			Db1 = Cs1,	///< Db
			D_1,		///< D  レ
			Ds1,		///< D#
			Eb1 = Ds1,	///< Eb
			E_1,		///< E  ミ
			F_1,		///< F  ファ
			Fs1,		///< F#
			Gb1 = Fs1,	///< Gb
			G_1,		///< G  ソ
			Gs1,		///< G#

			Ab1 = Gs1,	///< Ab
			A_1,		///< A  ラ  55Hz (1)
			As1,		///< A#
			Bb1 = As1,	///< Bb
			B_1,		///< B  シ
			C_2,		///< C  ド
			Cs2,		///< C#
			Db2 = Cs2,	///< Db
			D_2,		///< D  レ
			Ds2,		///< D#
			Eb2 = Ds2,	///< Eb
			E_2,		///< E  ミ
			F_2,		///< F  ファ
			Fs2,		///< F#
			Gb2 = Fs2,	///< Gb
			G_2,		///< G  ソ
			Gs2,		///< G#

			Ab2 = Gs2,	///< Ab
			A_2,		///< A  ラ  110Hz (2)
			As2,		///< A#
			Bb2 = As2,	///< Bb
			B_2,		///< B  シ
			C_3,		///< C  ド
			Cs3,		///< C#
			Db3 = Cs3,	///< Db
			D_3,		///< D  レ
			Ds3,		///< D#
			Eb3 = Ds3,	///< Eb
			E_3,		///< E  ミ
			F_3,		///< F  ファ
			Fs3,		///< F#
			Gb3 = Fs3,	///< Gb
			G_3,		///< G  ソ
			Gs3,		///< G#

			Ab3 = Gs3,	///< Ab
			A_3,		///< A  ラ  220Hz (3)
			As3,		///< A#
			Bb3 = As3,	///< Bb
			B_3,		///< B  シ
			C_4,		///< C  ド
			Cs4,		///< C#
			Db4 = Cs4,	///< Db
			D_4,		///< D  レ
			Ds4,		///< D#
			Eb4 = Ds4,	///< Eb
			E_4,		///< E  ミ
			F_4,		///< F  ファ
			Fs4,		///< F#
			Gb4 = Fs4,	///< Gb
			G_4,		///< G  ソ
			Gs4,		///< G#

			Ab4 = Gs4,	///< Ab
			A_4,		///< A  ラ  440Hz (4)
			As4,		///< A#
			Bb4 = As4,	///< Bb
			B_4,		///< B  シ
			C_5,		///< C  ド
			Cs5,		///< C#
			Db5 = Cs5,	///< Db
			D_5,		///< D  レ
			Ds5,		///< D#
			Eb5 = Ds5,	///< Eb
			E_5,		///< E  ミ
			F_5,		///< F  ファ
			Fs5,		///< F#
			Gb5 = Fs5,	///< Gb
			G_5,		///< G  ソ
			Gs5,		///< G#

			Ab5 = Gs5,	///< Ab
			A_5,		///< A  ラ  880Hz (5)
			As5,		///< A#
			Bb5 = As5,	///< Bb
			B_5,		///< B  シ
			C_6,		///< C  ド
			Cs6,		///< C#
			Db6 = Cs6,	///< Db
			D_6,		///< D  レ
			Ds6,		///< D#
			Eb6 = Ds6,	///< Eb
			E_6,		///< E  ミ
			F_6,		///< F  ファ
			Fs6,		///< F#
			Gb6 = Fs6,	///< Gb
			G_6,		///< G  ソ
			Gs6,		///< G#

			Ab6 = Gs6,	///< Ab
			A_6,		///< A  ラ 1760Hz (6)
			As6,		///< A#
			Bb6 = As6,	///< Bb
			B_6,		///< B  シ
			C_7,		///< C  ド
			Cs7,		///< C#
			Db7 = Cs7,	///< Db
			D_7,		///< D  レ
			Ds7,		///< D#
			Eb7 = Ds7,	///< Eb
			E_7,		///< E  ミ
			F_7,		///< F  ファ
			Fs7,		///< F#
			Gb7 = Fs7,	///< Gb
			G_7,		///< G  ソ
			Gs7,		///< G#

			Ab7 = Gs7,	///< Ab
			A_7,		///< A  ラ 3520Hz (7)
			As7,		///< A#
			B_7,		///< B  シ
			C_8,		///< C  ド

			Q,		///< 休符
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  制御コマンド
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class CTRL : uint8_t {
			TR = 89,	///< (2) トランスポーズ, num
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
			TEST,
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
			constexpr SCORE(KEY k) noexcept : key(k) { }
			constexpr SCORE(CTRL c) noexcept : ctrl(c) { }
			constexpr SCORE(uint8_t l) noexcept : len(l) { }
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

		static constexpr uint16_t BASE_FRQ = F_CLK / 4 / 256;

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

		static uint16_t	frame_;

		struct channel {
			uint8_t		vol_;  // 0 to 15
			WTYPE		wtype_;
			uint16_t	acc_;
			uint16_t	spd_;
			const SCORE*	score_org_;
			uint16_t	score_pos_;
			uint8_t		tempo_;
			uint16_t	count_;
			uint8_t		tr_;
			uint16_t	loop_org_;
			uint8_t		loop_cnt_;
			channel() noexcept : vol_(0), wtype_(WTYPE::SQ50), acc_(0), spd_(0),
				score_org_(nullptr), score_pos_(0),
				tempo_(0), count_(0),
				tr_(0), loop_org_(0), loop_cnt_(0)
			{ }

			void update() noexcept { acc_ += spd_; }

			int8_t get() noexcept
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

			void set_freq(uint16_t frq) noexcept { spd_ = (static_cast<uint32_t>(frq) << 16) / BASE_FRQ; }
//			void set_spd(uint16_t spd) noexcept { spd_ = spd; }

			// 完了なら「true」
			bool service() noexcept
			{
				if(score_org_ == nullptr) return true;

				if(count_ > 0) {
					count_--;
					if(count_ > 0) return true;
				}

				auto v = score_org_[score_pos_];
				++score_pos_;
				if(v.len < 88) {
					v.len += tr_;
					if(v.len >= 0x80) v.len = 0;
					else if(v.len >= 88) v.len = 87;
					auto o = v.len / 12;
					auto k = v.len % 12;
					spd_ = key_tbl_[k] >> (7 - o);
					acc_ = 0;
					count_ = score_org_[score_pos_].len * tempo_;
					++score_pos_;
					return true;
				} else if(v.len == 88) {  // 休符
					spd_ = 0;
					acc_ = 0;
					count_ = score_org_[score_pos_].len * tempo_;
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
						tempo_ = 0;
						count_ = 0;
						break;
					case CTRL::TEST:
//						utils::format("%d\n") % frame_;
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
			channel_[ch].count_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  演奏サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept
		{
			frame_++;

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
	template<uint16_t BSIZE, uint16_t CNUM> uint16_t psg_mng<BSIZE, CNUM>::frame_;
}