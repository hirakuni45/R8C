#pragma once
//=====================================================================//
/*!	@file
	@brief	16x16 マトリックス・テトリス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テトリス・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//	class tetris : public i_task {
	template <class MONOG>
	class tetris {

		MONOG&	monog_;
//		task&	task_;

		uint8_t v_ = 91;
		uint8_t m_ = 123;

		void randmize_(uint8_t v, uint8_t m)
		{
			v_ = v;
			m_ = m;
		}

		uint8_t rand_()
		{
			v_ += v_ << 2;
			++v_;
			uint8_t n = 0;
			if(m_ & 0x02) n = 1;
			if(m_ & 0x40) n ^= 1;
			m_ += m_;
			if(n == 0) ++m_;
			return v_ ^ m_;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	各スコア
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class SCORE : uint16_t {
			block_fall = 1,			///< ブロック落下
			line_erase = 20,		///< ライン消去
			block_fall_quick = 1,	///< 高速ブロック落下
		};

		///< テトリスブロック種
		static const uint8_t 	tetris_blocks_ = 7;
		static const int8_t		offset_x_ = 3;
		static const int8_t		tetris_width_ = 10;

		struct position {
			int8_t	x;
			int8_t	y;
			position() { }
			position(char x_, char y_) : x(x_), y(y_) { }
			position operator + (const position& p) const {
				position pos(x + p.x, y + p.y);
				return pos;
			} 
		};

		struct block {
			position	poss[4];
		};

		position	block_pos_;
		int16_t		v_pos_;
		int16_t		v_spd_;
		uint8_t		block_idx_;
		int8_t		angle_;
		block		blocks_[tetris_blocks_];

		class bitmap {
			uint16_t	bits_[16];
		public:
			void set(const position& pos) {
				if(pos.y < 0 || pos.y >= 16) return;
				if(pos.x < 0 || pos.x >= 10) return;
				bits_[static_cast<uint8_t>(pos.y)] |= 1 << pos.x;
			}
			void reset(const position& pos) { bits_[pos.y & 15] &= ~(1 << (pos.x & 15)); }
			void clear() { for(uint8_t i = 0; i < 16; ++i) bits_[i] = 0; }
			bool get(const position& pos) const {
				if(pos.y >= 16) return true;
				else if(pos.y < 0) return false;
				if(bits_[static_cast<uint8_t>(pos.y)] & (1 << (pos.x & 15))) return true;
				else return false;
			}
			uint16_t get_line(char y) const {
				return bits_[y & 15];
			}
			void erase_line(char y) {
				if(y < 0 || y >= 16) return;
				uint8_t i = y;
				while(i > 0) {
					bits_[i] = bits_[i - 1];
					--i;
				}
				bits_[0] = 0;
			}
		};
		bitmap	bitmap_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ゲーム・モード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class MODE : uint8_t {
			GAME,	///< 通常ゲーム
			BLINK,	///< 消去ブロック点滅
			OVER,	///< ゲーム終了
			SCORE,	///< スコア表示
		};
		MODE		mode_;
		uint8_t		count_;

		uint16_t	score_;

		bool clip_x_(const position& pos, const block& bck)
		{
			for(uint8_t i = 0; i < 4; ++i) {
				auto x = bck.poss[i].x + pos.x;
				if(x < 0 || x >= 10) return true;
			}
			return false;
		}


		bool clip_y_(const position& pos, const block& bck)
		{
			for(uint8_t i = 0; i < 4; ++i) {
				auto y = bck.poss[i].y + pos.y;
				if(y >= 16) return true;
			}
			return false;
		}


		void draw_block_(const position& pos, const block& bck)
		{
			for(uint8_t i = 0; i < 4; ++i) {
				monog_.plot(pos.x + bck.poss[i].x, pos.y + bck.poss[i].y, 1); 
			}
		}


		void rotate_(const block& in, char angle, block& out)
		{
			int8_t si;
			int8_t co;
			switch(angle) {
			case 0:	// 0
			default:
				si = 0; co = 1;
				break;
			case 1:	// 90
				si = -1; co = 0;
				break;
			case 2:	// 180
				si = 0; co = -1;
				break;
			case 3:	// 270
				si = 1; co = 0;
				break;
			}
			for(uint8_t i = 0; i < 4; ++i) {
				out.poss[i].x = in.poss[i].x * co + in.poss[i].y * si;
				out.poss[i].y = in.poss[i].y * co - in.poss[i].x * si;
			}
		}


		bool scan_map_(const position& pos, const block& in)
		{
			for(uint8_t i = 0; i < 4; ++i) {
				if(bitmap_.get(pos + in.poss[i])) return true;
			}
			return false;
		}


		void set_block_(const position& pos, const block& in)
		{
			for(uint8_t i = 0; i < 4; ++i) {
				bitmap_.set(pos + in.poss[i]);
			}
		}


		void render_block_(int8_t ofsx)
		{
			for(int8_t y = 0; y < 16; ++y) {
				for(int8_t x = 0; x < 10; ++x) {
					if(bitmap_.get(position(x, y))) {
						monog_.plot(ofsx + x, y, 1); 
					}
				}
			}
		}


		int8_t line_up_map_(int8_t sy = 15)
		{
			for(int8_t y = sy; y >= 0; --y) {
				if(bitmap_.get_line(y) == 0x3ff) {
					return y;
				}
			}
			return -1;
		}


		void line_fill_anime_();


		void task_game_()
		{
			position p = block_pos_;
			auto an = angle_;
			bool bd = false;
#if 0
			const system::switch_input& swi = task_.swi_;
			if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
				--p.x ;
			}
			if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
				++p.x;
			}

			if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
				++an;
				an &= 3;
			}

			if(swi.get_level() & system::switch_input::bits::LEFT_DOWN) {
				bd = true;
			}
#endif

			// 消去ブロック
			auto y = line_up_map_();
			if(y >= 0) {
				count_ = 60;
				mode_ = MODE::BLINK;
				return;
			}

			if(bd) {
				v_pos_ += 2048;
			} else {
				v_pos_ += v_spd_;
			}
			if(v_pos_ >= 4096) {
				v_pos_ = 0;
				++p.y;
			}

			block bk;
			// 位置の左右クリップ
			rotate_(blocks_[block_idx_], angle_, bk);
			if(clip_x_(p, bk) || scan_map_(p, bk)) {
				p.x = block_pos_.x;
			} else {
				if(block_pos_.x != p.x) {
//					task_.music_.request(sound::music::id::tetris_move, 1);
				}
				block_pos_.x = p.x;
			}

			// 回転の左右クリップ
			rotate_(blocks_[block_idx_], an, bk);
			if(clip_x_(p, bk)) {
				bk = blocks_[block_idx_];
			} else {
				if(angle_ != an) {
//					task_.music_.request(sound::music::id::tetris_rot, 1);
				}
				angle_ = an;
			}

			bool bend = false;

			// 底？
			if(clip_y_(p, bk)) {
				p.y = block_pos_.y;
				set_block_(p, bk);
				bend = true;
			}

			// 積みブロック検査
			if(scan_map_(p, bk)) {
				p.y = block_pos_.y;
				set_block_(p, bk);
				bend = true;
			}

			// 新規ブロック発生
			if(bend) {
				score_ += static_cast<uint16_t>(SCORE::block_fall);
				if(bd) score_ += static_cast<uint16_t>(SCORE::block_fall_quick);
///				task_.music_.request(sound::music::id::tetris_fall, 1);
				v_pos_ = 0;
				block_idx_ = rand_() % tetris_blocks_;
				v_spd_ += 1;
				if(v_spd_ >= 1024) v_spd_ = 1024;
				block_pos_.x = 5;	// 初期位置
				block_pos_.y = -2;
				angle_ = 0;
			} else {
				block_pos_.y = p.y;
			}

			// ゲーム終了判定
			if(bitmap_.get_line(0)) {
				mode_ = MODE::OVER;
				count_ = 120 + 60;
			}

			// フレームの描画
			monog_.line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
			monog_.line(offset_x_ + tetris_width_, 0, offset_x_ + tetris_width_, 15, 1);

			// 積みブロックの描画
			render_block_(offset_x_);

			// 落下ブロックの描画
			if(!bend) {
				position pp(p.x + offset_x_, p.y);
				draw_block_(pp, bk);
			}
		}


		void task_blink_()
		{
			int8_t y = 15;
			int8_t tmp[4];
			uint8_t dc = 0;
			while(1) {
				y = line_up_map_(y);
				if(y >= 0) {
					tmp[dc] = y;
					++dc;
					if(dc >= 4) break;
					--y;
				} else {
					break;
				}
			}

			// フレームの描画
			monog_.line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
			monog_.line(offset_x_ + tetris_width_, 0, offset_x_ + tetris_width_, 15, 1);

			// 積みブロックの描画
			render_block_(offset_x_);

			// 消去ラインアニメーション
			if(count_) {
				if((count_ % 10) > 4) {
					for(uint8_t i = 0; i < dc; ++i) {
						monog_.line(offset_x_, tmp[i], offset_x_ + tetris_width_ - 1, tmp[i], 0);
					}
				}
				--count_;
			} else {
				for(uint8_t i = 0; i < dc; ++i) {
					bitmap_.erase_line(tmp[i]);
					score_ += static_cast<uint16_t>(SCORE::line_erase);
				}
				mode_ = MODE::GAME;
				/// task_.music_.request(sound::music::id::tetris_erase, 1);
			}
		}


		void task_over_()
		{
			if(count_) {
				--count_;
			} else {
				mode_ = MODE::SCORE;
				count_ = 128;
			}			

			if(count_ > 60) {
				if((count_ % 30) >= 15) {
					return;
				}
			}
			// フレームの描画
			monog_.line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
			monog_.line(offset_x_ + tetris_width_, 0, offset_x_ + tetris_width_, 15, 1);

			// 積みブロックの描画
			render_block_(offset_x_);

			if(count_ <= 60) {
				uint8_t n = (60 - count_) / 8;
				for(uint8_t h = 0; h < n; ++h) {
					monog_.line(0, h, 15, h, 1);
					monog_.line(0, 15 - h, 15, 15 - h, 1);
				}
			}
		}


		void task_score_()
		{
#if 0
			uint16_t sc = static_cast<uint16_t>(score_);
			int8_t y = 5;
			if(sc >= 10000) {
				y = 2 + 7;
			}
///			task_.dm_.draw_3x5(13, y, sc % 10);
			sc /= 10;
///			task_.dm_.draw_3x5(9,  y, sc % 10);
			sc /= 10;
///			task_.dm_.draw_3x5(5,  y, sc % 10);
			sc /= 10;
///			task_.dm_.draw_3x5(1,  y, sc % 10);
			if(sc >= 10000) {
				y = 2;
				sc /= 10;
///				task_.dm_.draw_3x5(9,  y, sc % 10);
				sc /= 10;
///				task_.dm_.draw_3x5(5,  y, sc % 10);
			}
#endif
#if 0
			{
				randmize_((score_ % 10) * 67, (score_ / 10) * 27);
				for(uint8_t y = 0; y < 16; ++y) {
					for(uint8_t x = 0; x < 16; ++y) {
						if(count_ < (rand_() & 127)) {
							task_.mng_.point_reset(x, y);
						}
					}
				}
				if(count_) --count_;
			}
#endif


///			const system::switch_input& swi = task_.swi_;
///			if(swi.get_positive()) {
///				task_.start<menu>();
///			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
//		tetris(task& t) : task_(t),
		tetris(MONOG& monog) : monog_(monog), 
			block_pos_(0, 0), v_pos_(0), v_spd_(0),
			block_idx_(0), angle_(0),
			mode_(MODE::GAME), count_(0), score_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
//		virtual ~tetris() { }
		~tetris() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void init()
		{
			// ブロックの定義(0)
			blocks_[0].poss[0] = position(-2,  0);	//
			blocks_[0].poss[1] = position(-1,  0);	//
			blocks_[0].poss[2] = position( 0,  0);	// [][]<>[]
			blocks_[0].poss[3] = position( 1,  0);	//
			// ブロックの定義(1)
			blocks_[1].poss[0] = position(-1, -1);	//
			blocks_[1].poss[1] = position(-1,  0);	// 
			blocks_[1].poss[2] = position( 0,  0);	// []<>[]
			blocks_[1].poss[3] = position( 1,  0);	// []
			// ブロックの定義(2)
			blocks_[2].poss[0] = position(-1,  0);	// 
			blocks_[2].poss[1] = position( 0,  0);	// 
			blocks_[2].poss[2] = position( 1,  0);	// []<>[]
			blocks_[2].poss[3] = position( 1, -1);	//     []
			// ブロックの定義(3)
			blocks_[3].poss[0] = position(-1,  0);	// 
			blocks_[3].poss[1] = position( 0,  0);	// []<>[]
			blocks_[3].poss[2] = position( 0, -1);	//   []
			blocks_[3].poss[3] = position( 1,  0);	// 
			// ブロックの定義(4)
			blocks_[4].poss[0] = position(-1,  0);	// 
			blocks_[4].poss[1] = position( 0,  0);	// []<>
			blocks_[4].poss[2] = position( 0, -1);	//   [][]
			blocks_[4].poss[3] = position( 1, -1);	// 
			// ブロックの定義(5)
			blocks_[5].poss[0] = position(-1, -1);	// 
			blocks_[5].poss[1] = position( 0, -1);	// 
			blocks_[5].poss[2] = position( 0,  0);	//   <>[]
			blocks_[5].poss[3] = position( 1,  0);	// [][]
			// ブロックの定義(6)
			blocks_[6].poss[0] = position(-1, -1);	// 
			blocks_[6].poss[1] = position( 0, -1);	// 
			blocks_[6].poss[2] = position( 0,  0);	// []<>
			blocks_[6].poss[3] = position(-1,  0);	// [][]

			v_spd_ = 128;
			v_pos_ = 0;
			block_pos_.x = 5;	/// 初期ブロック位置
			block_pos_.y = -2;
			bitmap_.clear();
			score_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			switch(mode_) {
			case MODE::GAME:
				task_game_();
				break;
			case MODE::BLINK:
				task_blink_();
				break;
			case MODE::OVER:
				task_over_();
				break;
			case MODE::SCORE:
				task_score_();
				break;
			default:
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() { }
	};
}
