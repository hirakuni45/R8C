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

		float		volt_;
		float		current_;
		float		watt_;

		char		str_[32];

		enum class TASK : uint8_t {
			ROOT,	///< 電圧、電流、表示
			WATT_M,	///< 時間、電力(分）、表示
			WATT_H,	///< 時間、電力(時）、表示

			limit	///< 最大値
		};

		TASK		task_;

        //-------------------------------------------------------------//
        /*!
            @brief  コンストラクター
        */
        //-------------------------------------------------------------//
		checker() : lcd_(spi_), bitmap_(kfont_), loop_(0), volt_(0), current_(0), watt_(0),
					task_(TASK::ROOT) { }


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
				adc_.start(adc::cnv_type::CH0_CH1, adc::ch_grp::AN0_AN1, true);
			}

			// SPI を開始
			{
				spi_.start(0);
			}

			// LCD を開始
			{
				// contrast, reverse=yes, BIAS9=false(BIAS7), Voltage-Ratio: 3
				lcd_.start(0x1C, true, false, 3);
				bitmap_.clear(0);
				bitmap_.enable_2x();
			}

#ifdef UART
			utils::format("Start USB Checker\n");
#endif
		}


        //-------------------------------------------------------------//
        /*!
            @brief  電圧、電流
        */
        //-------------------------------------------------------------//
		void root()
		{
			// 400mV/A * 3
#if 0
			uint32_t i = adc_.get_value(0);
			if(i <= 3) i = 0;  // noise bias
			i <<= 8;
			i /= (124 * 3);

			uint32_t v = adc_.get_value(1);
			v *= 845 * 6;
#endif
			bitmap_.clear(0);
///			utils::format("%1.2:8yV", str_, sizeof(str_)) % (v >> 10);
			utils::format("%3.2fV", str_, sizeof(str_)) % volt_;
			bitmap_.draw_text(0, 0, str_);

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 0);

			bitmap_.clear(0);

///			utils::format("%1.2:8yA", str_, sizeof(str_)) % i;
			utils::format("%3.2fA", str_, sizeof(str_)) % current_;
			bitmap_.draw_text(0, 0, str_);

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 3);

#ifdef UART
			utils::format("Vol: %1.2:8y [V], Cur: %1.2:8y [A]\n") % (v >> 10) % i;
#endif
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
			bitmap_.clear(0);

			auto s = timer_b::task_.get_time() / 50;
			auto m = s / 60;
			auto h = m / 60;
			utils::format("%02d:%02d:%02d", str_, sizeof(str_)) % (h % 24) % (m % 60) % (s % 60);
			bitmap_.draw_text(0, 0, str_);

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 0);

			bitmap_.clear(0);
			utils::format(form, str_, sizeof(str_)) % watt;
			bitmap_.draw_text(0, 0, str_);

			lcd_.copy(bitmap_.fb(), bitmap_.page_num(), 3);
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

			adc_.scan(); // A/D scan start

			if(timer_b::task_.positive(timer_b::task_type::type::SW_A)) {
				auto n = static_cast<uint8_t>(task_) + 1;
				if(n >= static_cast<uint8_t>(TASK::limit)) n = 0;
				task_ = static_cast<TASK>(n);
			}

			if(task_ == TASK::WATT_M || task_ == TASK::WATT_H) {
				if(timer_b::task_.positive(timer_b::task_type::type::SW_B)) {
					timer_b::task_.set_time(0);
					watt_ = 0;
				}
			}

			adc_.sync(); // A/D scan sync

			auto i = adc_.get_value(0);
			if(i <= 3) i = 0;  // noise bias
			current_ = static_cast<float>(i) / 1024.0f * 3.3f / (0.4f * 3.0f);
			volt_ = static_cast<float>(adc_.get_value(1)) / 1024.0f * 3.3f * 6.0f;
			watt_ += volt_ * current_ / 50.0f;

			if(loop_ >= 25) {
				loop_ = 0;

				switch(task_) {
				case TASK::ROOT:
					root();
					break;

				case TASK::WATT_M:
					watt("%8.3fWm", watt_ / 60.0f);
					break;
				case TASK::WATT_H:
					watt("%8.5fWh", watt_ / 3600.0f);
					break;

				default:
					break;
				}
			}
			++loop_;
		}
	};
}
