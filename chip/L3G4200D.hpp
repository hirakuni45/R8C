#pragma once
//=====================================================================//
/*!	@file
	@brief	L3G4200D class @n
			GY-50 Module @n
			STMicroelectronics @n
			three-axis, digital output gyroscope @n
			Interface: I2C/SPI, Vcc: 2.4V to 3.6V @n
			CS(5): 1->I2C, 0->SPI @n
			SDO/SA0(4): I2C slave address, pull-up (Default) 
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  L3G4300D テンプレートクラス (I2C)
		@param[in]	I2C		i2c I/O クラス
		@param[in]	DEV_ADR	I2C デバイス・アドレス (SDO/SA0 = 1) @n
							0b1101000: SDO/SA0 = 0
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class I2C, uint8_t DEV_ADR = 0b1101001>
	class L3G4200D {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ベクター型
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		template <typename T>
		struct vector3 {
			T	x;
			T	y;
			T	z;
			vector3(T xx = static_cast<T>(0), T yy = static_cast<T>(0), T zz = static_cast<T>(0)) :
				x(xx), y(yy), z(zz) { }
		};
		typedef vector3<int16_t> ivector3;
		typedef vector3<float> fvector3;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	スケール・レンジ [Degree Per Second]
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class SCALE : uint8_t {
			_250  = 0b00,	///<  250 DPS
			_500  = 0b01,	///<  500 DPS
			_2000 = 0b10	///< 2000 DPS
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	更新レートとカットオフ
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class ODRBW : uint8_t {
			_100HZ_12_5 = 0b0000,	///< ODR:100Hz, Cut-Off:12.5
			_100HZ_25   = 0b0001,	///< ODR:100Hz, Cut-Off:25
			_200HZ_12_5 = 0b0100,	///< ODR:200Hz, Cut-Off:12.5
			_200HZ_25   = 0b0101,	///< ODR:200Hz, Cut-Off:25
			_200HZ_50   = 0b0110,	///< ODR:200Hz, Cut-Off:50
			_200HZ_70   = 0b0111,	///< ODR:200Hz, Cut-Off:70
			_400HZ_20   = 0b1000,	///< ODR:400Hz, Cut-Off:20
			_400HZ_25   = 0b1001,	///< ODR:400Hz, Cut-Off:25
			_400HZ_50   = 0b1010,	///< ODR:400Hz, Cut-Off:50
			_400HZ_110  = 0b1011,	///< ODR:400Hz, Cut-Off:110
			_800HZ_30   = 0b1100,	///< ODR:800Hz, Cut-Off:30
			_800HZ_35   = 0b1101,	///< ODR:800Hz, Cut-Off:35
			_800HZ_50   = 0b1110,	///< ODR:800Hz, Cut-Off:50
			_800HZ_110  = 0b1111,	///< ODR:800Hz, Cut-Off:110
		};

	private:
		I2C&		i2c_;

		enum class REG : uint8_t {

			WHO_AM_I   = 0x0F,

			CTRL_REG1  = 0x20,
			CTRL_REG2  = 0x21,
			CTRL_REG3  = 0x22,
			CTRL_REG4  = 0x23,
			CTRL_REG5  = 0x24,
			REFERENCE  = 0x25,
			OUT_TEMP   = 0x26,
			STATUS_REG = 0x27,

			OUT_X_L    = 0x28,
			OUT_X_H    = 0x29,
			OUT_Y_L    = 0x2A,
			OUT_Y_H    = 0x2B,
			OUT_Z_L    = 0x2C,
			OUT_Z_H    = 0x2D,

			FIFO_CTRL_REG = 0x2E,
			FIFO_SRC_REG  = 0x2F,

			INT1_CFG      = 0x30,
			INT1_SRC      = 0x31,
			INT1_THS_XH   = 0x32,
			INT1_THS_XL   = 0x33,
			INT1_THS_YH   = 0x34,
			INT1_THS_YL   = 0x35,
			INT1_THS_ZH   = 0x36,
			INT1_THS_ZL   = 0x37,
			INT1_DURATION = 0x38,
		};

		fvector3	n_;
		fvector3	d_;
		fvector3	t_;

		bool		use_calibrate_;
		float		actual_threshold_;
		float		dps_per_digit_;
		fvector3	threshold_;


		void write_(REG reg, uint8_t data) noexcept
		{
			uint8_t tmp[2];
			tmp[0] = static_cast<uint8_t>(reg);
			tmp[1] = data;
			i2c_.send(DEV_ADR, tmp, 2);
		}


		uint8_t read_(REG reg) const noexcept
		{
			uint8_t tmp[1];
			tmp[0] = static_cast<uint8_t>(reg);
			i2c_.send(DEV_ADR, tmp, 1);
			i2c_.recv(DEV_ADR, tmp, 1);
			return tmp[0];
		}


		uint8_t fast_(REG reg) const noexcept
		{
			uint8_t tmp[1];
			tmp[0] = static_cast<uint8_t>(reg);
			i2c_.send(DEV_ADR, tmp, 1);
			i2c_.recv(DEV_ADR, tmp, 1);
			return tmp[0];
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	iica_io クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		L3G4200D(I2C& i2c) noexcept : i2c_(i2c),
			n_(), d_(), t_(),
			use_calibrate_(false),
			actual_threshold_(0.0f),
			dps_per_digit_(0.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	scale	スケール・レンジを選択
			@param[in]	odrbw	更新レートとカットオフを選択
			@return エラーなら「false」を返す
		 */
		//-----------------------------------------------------------------//
		bool start(SCALE scale = SCALE::_2000, ODRBW odrbw = ODRBW::_100HZ_12_5) noexcept
		{
			// Reset calibrate values
			d_.x = 0.0f;
			d_.y = 0.0f;
			d_.z = 0.0f;
			use_calibrate_ = false;

			// Reset threshold values
			t_.x = 0.0f;
			t_.y = 0.0f;
			t_.z = 0.0f;
			actual_threshold_ = 0.0f;

			// Check L3G4200D device
			if(fast_(REG::WHO_AM_I) != 0xD3) {
				return false;
			}

			// Enable all axis and setup normal mode + Output Data Range & Bandwidth
			uint8_t reg1 = 0x00;
			reg1 |= 0x0F; // Enable all axis and setup normal mode
			reg1 |= (static_cast<uint8_t>(odrbw) << 4); // Set output data rate & bandwidh
			write_(REG::CTRL_REG1, reg1);

			// Disable high pass filter
			write_(REG::CTRL_REG2, 0x00);

			// Generata data ready interrupt on INT2
			write_(REG::CTRL_REG3, 0x08);

			// Set full scale selection in continous mode
			write_(REG::CTRL_REG4, static_cast<uint8_t>(scale) << 4);

			switch(scale) {
			case SCALE::_250:
				dps_per_digit_ = .00875f;
				break;
			case SCALE::_500:
				dps_per_digit_ = .0175f;
				break;
			case SCALE::_2000:
				dps_per_digit_ = .07f;
				break;
			default:
				return false;
			}

			// Boot in normal mode, disable FIFO, HPF disabled
			write_(REG::CTRL_REG5, 0x00);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スケールの取得
			@return スケールを返す
		 */
		//-----------------------------------------------------------------//
		SCALE get_scale() noexcept
		{
			return static_cast<SCALE>((read_(REG::CTRL_REG4) >> 4) & 0x03);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バンド幅を返す
			@return バンド幅を返す
		 */
		//-----------------------------------------------------------------//
		ODRBW get_odr_bw() noexcept
		{
			return static_cast<ODRBW>((read_(REG::CTRL_REG1) >> 4) & 0x0F);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャリブレーションを行う
			@param[in]	snum	サンプル数
		 */
		//-----------------------------------------------------------------//
		void calibrate(uint8_t snum = 50) noexcept
		{
			// Set calibrate
			use_calibrate_ = true;

			// Reset values
			fvector3 sum;
			fvector3 sigma;

			// Read n-samples
			for(uint8_t i = 0; i < snum; ++i) {
				auto raw = get_raw();
				sum.x += static_cast<float>(raw.x);
				sum.y += static_cast<float>(raw.y);
				sum.z += static_cast<float>(raw.z);

				sigma.x += static_cast<float>(raw.x * raw.x);
				sigma.y += static_cast<float>(raw.y * raw.y);
				sigma.z += static_cast<float>(raw.z * raw.z);
	
				utils::delay::milli_second(5);
			}

			d_.x = sum.x / snum;
			d_.y = sum.y / snum;
			d_.z = sum.z / snum;

			// Calculate threshold vectors
			threshold_.x = std::sqrt((sigma.x / snum) - (d_.x * d_.x));
			threshold_.y = std::sqrt((sigma.y / snum) - (d_.y * d_.y));
			threshold_.z = std::sqrt((sigma.z / snum) - (d_.z * d_.z));

			// If already set threshold, recalculate threshold vectors
			if(actual_threshold_ > 0) {
				set_threshold(actual_threshold_);
			}
		}


		void set_threshold(uint8_t multiple = 1) noexcept
		{
			if(multiple > 0) {
				// If not calibrated, need calibrate
				if(!use_calibrate_) {
					calibrate();
				}
	
				// Calculate threshold vectors
				t_.x = threshold_.x * multiple;
				t_.y = threshold_.y * multiple;
				t_.z = threshold_.z * multiple;
			} else {
				// No threshold
				t_.x = 0.0f;
				t_.y = 0.0f;
				t_.z = 0.0f;
			}

			// Remember old threshold value
			actual_threshold_ = multiple;
		}


		uint8_t get_threshold() const noexcept
		{
			return actual_threshold_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	RAW を取得
			@return RAW
		 */
		//-----------------------------------------------------------------//
		ivector3 get_raw() const noexcept
		{
			uint8_t tmp[6];
			tmp[0] = static_cast<uint8_t>(REG::OUT_X_L) | (1 << 7);
			i2c_.send(DEV_ADR, tmp, 1);
			i2c_.recv(DEV_ADR, tmp, 6);
			ivector3 v;
			v.x = (tmp[1] << 8) | tmp[0];
			v.y = (tmp[3] << 8) | tmp[2];
			v.z = (tmp[5] << 8) | tmp[4];
			return v;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	法線を取得
			@return 法線
		 */
		//-----------------------------------------------------------------//
		const fvector3& get_normalize() noexcept
		{
			auto raw = get_raw();

			if(use_calibrate_) {
				n_.x = (raw.x - d_.x) * dps_per_digit_;
				n_.y = (raw.y - d_.y) * dps_per_digit_;
				n_.z = (raw.z - d_.z) * dps_per_digit_;
			} else {
				n_.x = raw.x * dps_per_digit_;
				n_.y = raw.y * dps_per_digit_;
				n_.z = raw.z * dps_per_digit_;
			}

			if(actual_threshold_ > 0) {
				if(std::abs(n_.x) < t_.x) n_.x = 0.0f;
				if(std::abs(n_.y) < t_.y) n_.y = 0.0f;
				if(std::abs(n_.z) < t_.z) n_.z = 0.0f;
			}

			return n_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	温度を取得
			@return 温度
		 */
		//-----------------------------------------------------------------//
		int8_t get_temperature() const noexcept
		{
			return static_cast<int8_t>(read_(REG::OUT_TEMP));
		}
	};
}
