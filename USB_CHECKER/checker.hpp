#pragma once
//=====================================================================//
/*!	@file
	@brief	チェッカー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "common/port_map.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/format.hpp"
#include "common/trb_io.hpp"
#include "common/adc_io.hpp"
#include "common/spi_io.hpp"
#include "chip/ST7565.hpp"
#include "common/monograph.hpp"
#include "common/font6x12.hpp"
#include "common/fixed_string.hpp"

namespace app {

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  タイマーＢ、割り込みタスク
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class trb_task {
		uint32_t	time_;
		uint8_t		val_;
		uint8_t		lvl_;
		uint8_t		pos_;
		uint8_t		neg_;

	public:
		enum class type : uint8_t {
			SW_A = 0x20,	///< SW-A
			SW_B = 0x10		///< SW-B
		};

		trb_task() : time_(0), val_(0), lvl_(0), pos_(0), neg_(0) { }

		void init() const {
			utils::PORT_MAP(utils::port_map::P34::PORT);
			utils::PORT_MAP(utils::port_map::P35::PORT);
			device::PD3.B4 = 0;
			device::PD3.B5 = 0;
			device::PUR3.B4 = 1;	///< プルアップ
			device::PUR3.B5 = 1;	///< プルアップ
		}

		void operator() () {
			++time_;
			val_ = ~device::P3();
		}

		void service()
		{
			pos_ = ~lvl_ &  val_;  ///< 立ち上がりエッジ検出
			neg_ =  lvl_ & ~val_;  ///< 立ち下がりエッジ検出
			lvl_ = val_;
		}

		bool level(type t) const { return lvl_ & static_cast<uint8_t>(t); }
		bool positive(type t) const { return pos_ & static_cast<uint8_t>(t); }
		bool negative(type t) const { return neg_ & static_cast<uint8_t>(t); }

		void set_time(uint32_t v) { time_ = v; }

		uint32_t get_time() const { return time_; }
	};


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  チェッカー・クラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class checker {

	public:
		typedef device::trb_io<trb_task, uint8_t> timer_b;
		timer_b timer_b_;

#ifdef UART
		typedef utils::fifo<uint8_t, 16> buffer;
		typedef device::uart_io<device::UART0, buffer, buffer> uart;
		uart uart_;
#endif

	private:
		typedef device::adc_io<utils::null_task> adc;
		adc adc_;

		// LCD SCL: P4_2(1)
		typedef device::PORT<device::PORT4, device::bitpos::B2> SPI_SCL;
		// LCD SDA: P4_5(12)
		typedef device::PORT<device::PORT4, device::bitpos::B5> SPI_SDA;

		typedef device::spi_io<SPI_SCL, SPI_SDA, device::NULL_PORT> SPI;
		SPI		spi_;

		// LCD /CS:  P3_7(2)
		typedef device::PORT<device::PORT3, device::bitpos::B7> LCD_SEL;
		// LCD A0:   P3_3(11)
		typedef device::PORT<device::PORT3, device::bitpos::B3> LCD_A0;
		// LCD /RES: P4_7(4)
		typedef device::PORT<device::PORT4, device::bitpos::B7> LCD_RES;

		typedef chip::ST7565<SPI, LCD_SEL, LCD_A0, LCD_RES> LCD;
		LCD 	lcd_;

		typedef graphics::font6x12 afont;
		graphics::kfont_null kfont_;
		graphics::monograph<128, 24, afont> bitmap_;

		uint8_t		loop_;
		uint8_t		page_;

		float		volt_;
		float		current_;
		float		watt_;

		char		str_[32];

		enum class TASK : uint8_t {
			MAIN,		///< 電圧、電流、表示
			WATT_M,		///< 時間、電力(分）、表示
			WATT_H,		///< 時間、電力(時）、表示
			GRAPH,		///< グラフ表示
			USB_REF,	///< USB D-, D+ 差動信号電圧表示
			SETUP,		///< 設定

			limit	///< 最大値
		};

		TASK		task_;

		uint8_t		log_;
		uint8_t		log_itv_;
		uint8_t		gain_idx_;
		uint8_t		interval_;

		float		usb_m_;
		float		usb_p_;

#ifdef UART
		uint8_t		list_cnt_;
#endif

		uint8_t		buff_[128];

	public:
        //-------------------------------------------------------------//
        /*!
            @brief  コンストラクター
        */
        //-------------------------------------------------------------//
		checker() : lcd_(spi_), bitmap_(kfont_), loop_(0), page_(0),
					volt_(0.0f), current_(0.0f), watt_(0.0f),
					task_(TASK::MAIN),
					log_(0), log_itv_(0), gain_idx_(0), interval_(12),
					usb_m_(0.0f), usb_p_(0.0f)
#ifdef UART
					, list_cnt_(0)
#endif
			{ }


        //-------------------------------------------------------------//
        /*!
            @brief  初期化
        */
        //-------------------------------------------------------------//
		void init()
		{
			// タイマーＢ初期化
			{
		   		timer_b::task_.init();
				uint8_t ir_level = 2;
				timer_b_.start_timer(50, ir_level);
			}
#ifdef UART
			// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
			// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
			{
				utils::PORT_MAP(utils::port_map::P14::TXD0);
				utils::PORT_MAP(utils::port_map::P15::RXD0);
				uint8_t ir_level = 1;
				uart_.start(57600, ir_level);
			}
#endif

			// ADC の設定
			{
				utils::PORT_MAP(utils::port_map::P10::AN0);
				utils::PORT_MAP(utils::port_map::P11::AN1);
				utils::PORT_MAP(utils::port_map::P12::AN2);
				utils::PORT_MAP(utils::port_map::P13::AN3);
			}

			// SPI を開始
			{
				spi_.start(0);
			}

			// LCD を開始
			{
				// contrast, reverse=yes, BIAS9=false(BIAS7), Voltage-Ratio: 3
				lcd_.start(0x1C, true, false, 3);
				bitmap_.flash(0);
				bitmap_.enable_2x();
			}

#ifdef UART
			utils::format("Start USB Checker\n");
#endif
		}


        //-------------------------------------------------------------//
        /*!
            @brief  電圧、電流表示
        */
        //-------------------------------------------------------------//
		void vc()
		{
			// 400mV/A * 3
			if(page_ == 0) {
				utils::format("%3.2fV", str_, sizeof(str_)) % volt_;
				bitmap_.draw_text(0, 0, str_);
			} else {
				utils::format("%3.2fA", str_, sizeof(str_)) % current_;
				bitmap_.draw_text(0, 0, str_);
			}
		}


        //-------------------------------------------------------------//
        /*!
            @brief  経過時間、電力
			@param[in]	ch		表示単位
			@param[in]	wdiv	積算電力の母数
        */
        //-------------------------------------------------------------//
		void watt(const char* form, float watt)
		{
			if(page_ == 0) {
				auto s = timer_b::task_.get_time() / 50;
				auto m = s / 60;
				auto h = m / 60;
				utils::format("%02d:%02d:%02d", str_, sizeof(str_)) % (h % 24) % (m % 60) % (s % 60);
				bitmap_.draw_text(0, 0, str_);
			} else {
				utils::format(form, str_, sizeof(str_)) % watt;
				bitmap_.draw_text(0, 0, str_);
			}
		}


        //-------------------------------------------------------------//
        /*!
            @brief  グラフ表示
			@param[in]	o	表示オフセット
			@param[in]	w	表示範囲
        */
        //-------------------------------------------------------------//
		void graph(uint8_t o = 0, uint8_t w = 127)
		{
			static const int8_t gain_tbl[] = { 1, 2, 3, 4, 6, 8, 12, 16 };
			int16_t gain = gain_tbl[gain_idx_];
			if(page_ == 0) {
 				int16_t v = w - gain * w / 16;
				bitmap_.line(o, 0, o + v, 0, true);
				bitmap_.line(o, 1, o + v, 1, true);
			}

			uint8_t pos = log_ - w - 1;
			pos &= 127;
			int16_t v0 = static_cast<int16_t>(buff_[pos]);
			v0 *= 3;
			v0 /= gain;
			for(uint8_t x = 0; x < w; ++x) {
				++pos;
				pos &= 127;
				int16_t y0 = 47 - v0;
				int16_t v1 = static_cast<int16_t>(buff_[pos]);
				v1 *= 3;
				v1 /= gain;
				int16_t y1 = 47 - v1;
				if(page_) {
					y0 -= 24;
					y1 -= 24;
				}
				int16_t x0 = static_cast<int16_t>(x) + o;
				int16_t x1 = x0 + 1;
				bitmap_.line(x0, y0, x1, y1, true);
				v0 = v1;
			}
		}


        //-------------------------------------------------------------//
        /*!
            @brief  USB 信号電圧表示
        */
        //-------------------------------------------------------------//
		void usb_ref()
		{
			if(page_ == 0) {
				utils::format("-D: %3.2fV", str_, sizeof(str_)) % usb_m_;
				bitmap_.draw_text(0, 0, str_);
			} else {
				utils::format("+D: %3.2fV", str_, sizeof(str_)) % usb_p_;
				bitmap_.draw_text(0, 0, str_);
			}
		}


        //-------------------------------------------------------------//
        /*!
            @brief  設定
        */
        //-------------------------------------------------------------//
		void setup()
		{
			if(page_ == 0) {

			} else {

			}
		}


        //-------------------------------------------------------------//
        /*!
            @brief  サービス
        */
        //-------------------------------------------------------------//
		void service()
		{
			timer_b_.sync();

			timer_b_.task_.service();

			adc_.start(adc::cnv_type::CH0_CH1, adc::ch_grp::AN0_AN1, false);
			adc_.scan(); // AN0, AN1 A/D scan start

			if(timer_b::task_.positive(timer_b::task_type::type::SW_A)) {
				auto n = static_cast<uint8_t>(task_) + 1;
				if(n >= static_cast<uint8_t>(TASK::limit)) n = 0;
				task_ = static_cast<TASK>(n);
			}

			switch(task_) {
			case TASK::WATT_M:
			case TASK::WATT_H:
				if(timer_b::task_.positive(timer_b::task_type::type::SW_B)) {
					timer_b::task_.set_time(0);
					watt_ = 0;
				}
				break;
			case TASK::MAIN:
			case TASK::GRAPH:
				if(timer_b::task_.positive(timer_b::task_type::type::SW_B)) {
					++gain_idx_;
					if(gain_idx_ >= 8) gain_idx_ = 0;  // 0 to 7
				}
				break;
			case TASK::SETUP:
				if(timer_b::task_.positive(timer_b::task_type::type::SW_B)) {
				}
				break;
			default:
				break;
			}

			adc_.sync(); // A/D scan sync
			{
				uint32_t i = adc_.get_value(0);
				uint32_t v = adc_.get_value(1);

				adc_.start(adc::cnv_type::CH0_CH1, adc::ch_grp::AN2_AN3, false);
				adc_.scan(); // AN2, AN3 A/D scan start

				if(i <= 3) i = 0;  // noise bias
				if(log_itv_ == 0) {
					buff_[log_] = (i * v) >> 12;
					++log_;
					log_ &= 127;
					log_itv_ = interval_;
				} else {
					--log_itv_;
				}
				current_ = static_cast<float>(i) / 1023.0f * 3.3f / (0.4f * 3.0f);
				volt_    = static_cast<float>(v) / 1023.0f * 3.3f * 6.0f;
				watt_ += volt_ * current_ / 50.0f;
			}

			{
				adc_.sync(); // A/D scan sync
				usb_m_ = static_cast<float>(adc_.get_value(0)) / 1023.0f * 3.3f;
				usb_p_ = static_cast<float>(adc_.get_value(1)) / 1023.0f * 3.3f;
			}

#ifdef UART
			++list_cnt_;
			if(list_cnt_ >= 50) {
				list_cnt_ = 0;
				utils::format("Vol: %3.2f [V], Cur: %3.2f [A]\n") % volt_ % current_;
				utils::format("Watt: %8.5f [Wh]\n") % (watt_ / 3600.0f);
			}
#endif

			if(loop_ == 0) {
				bitmap_.flash(0);
			} else if(loop_ == 1) {
				switch(task_) {
				case TASK::MAIN:
					vc();
					graph(64, 64 - 1);
					break;

				case TASK::WATT_M:
					watt("%8.3fWm", watt_ / 60.0f);
					break;
				case TASK::WATT_H:
					watt("%8.5fWh", watt_ / 3600.0f);
					break;

				case TASK::GRAPH:
					graph();
					break;

				case TASK::USB_REF:
					usb_ref();
					break;

				case TASK::SETUP:
					setup();
					break;

				default:
					break;
				}
			} else if(loop_ == 2) {
				lcd_.copy(bitmap_.fb(), bitmap_.page_num(), page_ * 3);
				++page_;
				page_ &= 1;
			}

			++loop_;
			if(loop_ >= 3) {
				loop_ = 0;
			}
		}
	};
}
