#pragma once
//=====================================================================//
/*!	@file
	@brief	VL53L0X ドライバー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>
#include <cstring>
#include "common/iica_io.hpp"
#include "common/time.h"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  VL53L0X テンプレートクラス
		@param[in]	I2C_IO	i2c I/O クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class I2C_IO>
	class VL53L0X {

		// R/W ビットを含まない７ビット値
		static const uint8_t ADR_ = 0b0101001;

	    enum class reg_addr : uint8_t {
			SYSRANGE_START                              = 0x00,
			SYSTEM_THRESH_HIGH                          = 0x0C,
			SYSTEM_THRESH_LOW                           = 0x0E,

			SYSTEM_SEQUENCE_CONFIG                      = 0x01,
			SYSTEM_RANGE_CONFIG                         = 0x09,
			SYSTEM_INTERMEASUREMENT_PERIOD              = 0x04,

			SYSTEM_INTERRUPT_CONFIG_GPIO                = 0x0A,

			GPIO_HV_MUX_ACTIVE_HIGH                     = 0x84,

			SYSTEM_INTERRUPT_CLEAR                      = 0x0B,

			RESULT_INTERRUPT_STATUS                     = 0x13,
			RESULT_RANGE_STATUS                         = 0x14,

			RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       = 0xBC,
			RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        = 0xC0,
			RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       = 0xD0,
			RESULT_CORE_RANGING_TOTAL_EVENTS_REF        = 0xD4,
			RESULT_PEAK_SIGNAL_RATE_REF                 = 0xB6,

			ALGO_PART_TO_PART_RANGE_OFFSET_MM           = 0x28,

			I2C_SLAVE_DEVICE_ADDRESS                    = 0x8A,

			MSRC_CONFIG_CONTROL                         = 0x60,

			PRE_RANGE_CONFIG_MIN_SNR                    = 0x27,
			PRE_RANGE_CONFIG_VALID_PHASE_LOW            = 0x56,
			PRE_RANGE_CONFIG_VALID_PHASE_HIGH           = 0x57,
			PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          = 0x64,

			FINAL_RANGE_CONFIG_MIN_SNR                  = 0x67,
			FINAL_RANGE_CONFIG_VALID_PHASE_LOW          = 0x47,
			FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         = 0x48,
			FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,

			PRE_RANGE_CONFIG_SIGMA_THRESH_HI            = 0x61,
			PRE_RANGE_CONFIG_SIGMA_THRESH_LO            = 0x62,

			PRE_RANGE_CONFIG_VCSEL_PERIOD               = 0x50,
			PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          = 0x51,
			PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          = 0x52,

			SYSTEM_HISTOGRAM_BIN                        = 0x81,
			HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       = 0x33,
			HISTOGRAM_CONFIG_READOUT_CTRL               = 0x55,

			FINAL_RANGE_CONFIG_VCSEL_PERIOD             = 0x70,
			FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        = 0x71,
			FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        = 0x72,
			CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       = 0x20,

			MSRC_CONFIG_TIMEOUT_MACROP                  = 0x46,

			SOFT_RESET_GO2_SOFT_RESET_N                 = 0xBF,
			IDENTIFICATION_MODEL_ID                     = 0xC0,
			IDENTIFICATION_REVISION_ID                  = 0xC2,

			OSC_CALIBRATE_VAL                           = 0xF8,

			GLOBAL_CONFIG_VCSEL_WIDTH                   = 0x32,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_0            = 0xB0,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_1            = 0xB1,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_2            = 0xB2,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_3            = 0xB3,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_4            = 0xB4,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_5            = 0xB5,

			GLOBAL_CONFIG_REF_EN_START_SELECT           = 0xB6,
			DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         = 0x4E,
			DYNAMIC_SPAD_REF_EN_START_OFFSET            = 0x4F,
			POWER_MANAGEMENT_GO1_POWER_FORCE            = 0x80,

			VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           = 0x89,

			ALGO_PHASECAL_LIM                           = 0x30,
			ALGO_PHASECAL_CONFIG_TIMEOUT                = 0x30,
		};


		I2C_IO& i2c_io_;

		bool	last_status_;
		bool	stop_variable_;

		void write_(reg_addr reg, uint8_t value)
		{
			uint8_t tmp[2];
			tmp[0] = static_cast<uint8_t>(reg);
			tmp[1] = value;
			last_status_ = i2c_io_.send(ADR_, tmp, 2);
		}


		void write_(reg_addr reg, const uint8_t* src, uint8_t len)
		{
			uint8_t tmp[1 + len];
			tmp[0] = static_cast<uint8_t>(reg);
			std::memcpy(&tmp[1], src, len); 
			last_status_ = i2c_io_.send(ADR_, tmp, len + 1);
		}


		void write16_(reg_addr reg, uint16_t value)
		{
			uint8_t tmp[3];
			tmp[0] = static_cast<uint8_t>(reg);
			tmp[1] = value >> 8;
			tmp[2] = value & 0xff;
			last_status_ = i2c_io_.send(ADR_, tmp, 3);
		}


		void write32_(reg_addr reg, uint32_t value)
		{
			uint8_t tmp[5];
			tmp[0] = static_cast<uint8_t>(reg);
			tmp[1] = value >> 24;
			tmp[2] = value >> 16;
			tmp[3] = value >> 8;
			tmp[4] = value & 0xff;
			last_status_ = i2c_io_.send(ADR_, tmp, 5);
		}


		uint8_t read_(reg_addr reg)
		{
			uint8_t tmp[1];
			tmp[0] = static_cast<uint8_t>(reg);
			last_status_ = i2c_io_.send(ADR_, tmp, 1);
			if(!last_status_) return 0;

			last_status_ = i2c_io_.recv(ADR_, tmp, 1);
			return tmp[0];
		}


		bool read_(reg_addr reg, uint8_t* dst, uint8_t len)
		{
			uint8_t tmp[1];
			tmp[0] = static_cast<uint8_t>(reg);
			if(!i2c_io_.send(ADR_, tmp, 1)) {
				return false;
			}

			return i2c_io_.recv(ADR_, dst, len);
		}


		uint16_t read16_(reg_addr reg)
		{
			uint8_t tmp[2];
			tmp[0] = static_cast<uint8_t>(reg);
			last_status_ = i2c_io_.send(ADR_, tmp, 1);
			if(!last_status_) return 0;

			last_status_ =  i2c_io_.recv(ADR_, tmp, 2);
			uint16_t value = static_cast<uint16_t>(tmp[0]) << 8;
			value |= static_cast<uint16_t>(tmp[1]);
			return value;
		}


		uint32_t read32_(reg_addr reg)
		{
			uint8_t tmp[4];
			tmp[0] = static_cast<uint8_t>(reg);
			last_status_ = i2c_io_.send(ADR_, tmp, 1);
			if(!last_status_) return 0;

			last_status_ =  i2c_io_.recv(ADR_, tmp, 4);
			uint32_t value = static_cast<uint32_t>(tmp[0]) << 24;
			value |= static_cast<uint32_t>(tmp[1]) << 16;
			value |= static_cast<uint32_t>(tmp[2]) << 8;
			value |= static_cast<uint32_t>(tmp[3]);
			return value;
		}


		bool get_spad_info_(uint8_t& count, bool& type_is_aperture)
		{
			write_(static_cast<reg_addr>(0x80), 0x01);
			write_(static_cast<reg_addr>(0xFF), 0x01);
			write_(static_cast<reg_addr>(0x00), 0x00);

			write_(static_cast<reg_addr>(0xFF), 0x06);
			write_(static_cast<reg_addr>(0x83), read_(static_cast<reg_addr>(0x83)) | 0x04);
			write_(static_cast<reg_addr>(0xFF), 0x07);
			write_(static_cast<reg_addr>(0x81), 0x01);

			write_(static_cast<reg_addr>(0x80), 0x01);

			write_(static_cast<reg_addr>(0x94), 0x6b);
			write_(static_cast<reg_addr>(0x83), 0x00);

///			startTimeout();
			while(read_(static_cast<reg_addr>(0x83)) == 0x00) {
///				if(checkTimeoutExpired()) { return false; }
			}
			write_(static_cast<reg_addr>(0x83), 0x01);
			uint8_t tmp = read_(static_cast<reg_addr>(0x92));

			count = tmp & 0x7f;
			type_is_aperture = (tmp >> 7) & 0x01;

			write_(static_cast<reg_addr>(0x81), 0x00);
			write_(static_cast<reg_addr>(0xFF), 0x06);
			write_(static_cast<reg_addr>(0x83), read_(static_cast<reg_addr>(0x83))  & ~0x04);
			write_(static_cast<reg_addr>(0xFF), 0x01);
			write_(static_cast<reg_addr>(0x00), 0x01);

			write_(static_cast<reg_addr>(0xFF), 0x00);
			write_(static_cast<reg_addr>(0x80), 0x00);

			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	i2c	iica_io クラスを参照で渡す
		 */
		//-----------------------------------------------------------------//
		VL53L0X(I2C_IO& i2c) : i2c_io_(i2c),
			last_status_(true), stop_variable_(true) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	io_2v8	I/O:2.8V「true」、I/O:1.8V「false」
			@return エラーなら「false」を返す
		 */
		//-----------------------------------------------------------------//
		bool start(bool io_2v8 = true) {

			// sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
			if(io_2v8) {
				write_(reg_addr::VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
					read_(reg_addr::VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01); // set bit 0
			}

			// "Set I2C standard mode"
			write_(static_cast<reg_addr>(0x88), 0x00);

			write_(static_cast<reg_addr>(0x80), 0x01);
			write_(static_cast<reg_addr>(0xFF), 0x01);
			write_(static_cast<reg_addr>(0x00), 0x00);
			stop_variable_ = read_(static_cast<reg_addr>(0x91));
			write_(static_cast<reg_addr>(0x00), 0x01);
			write_(static_cast<reg_addr>(0xFF), 0x00);
			write_(static_cast<reg_addr>(0x80), 0x00);

			// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
			write_(reg_addr::MSRC_CONFIG_CONTROL, read_(reg_addr::MSRC_CONFIG_CONTROL) | 0x12);

			// set final range signal rate limit to 0.25 MCPS (million counts per second)
///			set_signal_rate_limit(0.25);

			write_(reg_addr::SYSTEM_SEQUENCE_CONFIG, 0xFF);

			// VL53L0X_DataInit() end
			// VL53L0X_StaticInit() begin

			uint8_t spad_count;
			bool spad_type_is_aperture;
			if(!get_spad_info_(spad_count, spad_type_is_aperture)) { return false; }

			// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
			// the API, but the same data seems to be more easily readable from
			// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
			uint8_t ref_spad_map[6];
			read_(reg_addr::GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

			// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

			write_(static_cast<reg_addr>(0xFF), 0x01);
			write_(reg_addr::DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
			write_(reg_addr::DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
			write_(static_cast<reg_addr>(0xFF), 0x00);
			write_(reg_addr::GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);
#if 0
			uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
			uint8_t spads_enabled = 0;
			for(uint8_t i = 0; i < 48; ++i) {
				if(i < first_spad_to_enable || spads_enabled == spad_count) {
					// This bit is lower than the first one that should be enabled, or
					// (reference_spad_count) bits have already been enabled, so zero this bit
					ref_spad_map[i / 8] &= ~(1 << (i % 8));
				} else if((ref_spad_map[i / 8] >> (i % 8)) & 0x1) {
					spads_enabled++;
				}
			}

			write_(reg_addr::GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

  // -- VL53L0X_set_reference_spads() end

  // -- VL53L0X_load_tuning_settings() begin
  // DefaultTuningSettings from vl53l0x_tuning.h

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x00, 0x00);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x09, 0x00);
  write_(reg_addr::0x10, 0x00);
  write_(reg_addr::0x11, 0x00);

  write_(reg_addr::0x24, 0x01);
  write_(reg_addr::0x25, 0xFF);
  write_(reg_addr::0x75, 0x00);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x4E, 0x2C);
  write_(reg_addr::0x48, 0x00);
  write_(reg_addr::0x30, 0x20);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x30, 0x09);
  write_(reg_addr::0x54, 0x00);
  write_(reg_addr::0x31, 0x04);
  write_(reg_addr::0x32, 0x03);
  write_(reg_addr::0x40, 0x83);
  write_(reg_addr::0x46, 0x25);
  write_(reg_addr::0x60, 0x00);
  write_(reg_addr::0x27, 0x00);
  write_(reg_addr::0x50, 0x06);
  write_(reg_addr::0x51, 0x00);
  write_(reg_addr::0x52, 0x96);
  write_(reg_addr::0x56, 0x08);
  write_(reg_addr::0x57, 0x30);
  write_(reg_addr::0x61, 0x00);
  write_(reg_addr::0x62, 0x00);
  write_(reg_addr::0x64, 0x00);
  write_(reg_addr::0x65, 0x00);
  write_(reg_addr::0x66, 0xA0);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x22, 0x32);
  write_(reg_addr::0x47, 0x14);
  write_(reg_addr::0x49, 0xFF);
  write_(reg_addr::0x4A, 0x00);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x7A, 0x0A);
  write_(reg_addr::0x7B, 0x00);
  write_(reg_addr::0x78, 0x21);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x23, 0x34);
  write_(reg_addr::0x42, 0x00);
  write_(reg_addr::0x44, 0xFF);
  write_(reg_addr::0x45, 0x26);
  write_(reg_addr::0x46, 0x05);
  write_(reg_addr::0x40, 0x40);
  write_(reg_addr::0x0E, 0x06);
  write_(reg_addr::0x20, 0x1A);
  write_(reg_addr::0x43, 0x40);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x34, 0x03);
  write_(reg_addr::0x35, 0x44);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x31, 0x04);
  write_(reg_addr::0x4B, 0x09);
  write_(reg_addr::0x4C, 0x05);
  write_(reg_addr::0x4D, 0x04);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x44, 0x00);
  write_(reg_addr::0x45, 0x20);
  write_(reg_addr::0x47, 0x08);
  write_(reg_addr::0x48, 0x28);
  write_(reg_addr::0x67, 0x00);
  write_(reg_addr::0x70, 0x04);
  write_(reg_addr::0x71, 0x01);
  write_(reg_addr::0x72, 0xFE);
  write_(reg_addr::0x76, 0x00);
  write_(reg_addr::0x77, 0x00);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x0D, 0x01);

  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x80, 0x01);
  write_(reg_addr::0x01, 0xF8);

  write_(reg_addr::0xFF, 0x01);
  write_(reg_addr::0x8E, 0x01);
  write_(reg_addr::0x00, 0x01);
  write_(reg_addr::0xFF, 0x00);
  write_(reg_addr::0x80, 0x00);

  // -- VL53L0X_load_tuning_settings() end

  // "Set interrupt config to new sample ready"
  // -- VL53L0X_SetGpioConfig() begin

  write_(reg_addr::SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
  write_(reg_addr::GPIO_HV_MUX_ACTIVE_HIGH, readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
  write_(reg_addr::SYSTEM_INTERRUPT_CLEAR, 0x01);

  // -- VL53L0X_SetGpioConfig() end

  measurement_timing_budget_us = getMeasurementTimingBudget();

  // "Disable MSRC and TCC by default"
  // MSRC = Minimum Signal Rate Check
  // TCC = Target CentreCheck
  // -- VL53L0X_SetSequenceStepEnable() begin

  write_(reg_addr::SYSTEM_SEQUENCE_CONFIG, 0xE8);

  // -- VL53L0X_SetSequenceStepEnable() end

  // "Recalculate timing budget"
  setMeasurementTimingBudget(measurement_timing_budget_us);

  // VL53L0X_StaticInit() end

  // VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

  // -- VL53L0X_perform_vhv_calibration() begin

  write_(reg_addr::SYSTEM_SEQUENCE_CONFIG, 0x01);
  if (!performSingleRefCalibration(0x40)) { return false; }

  // -- VL53L0X_perform_vhv_calibration() end

  // -- VL53L0X_perform_phase_calibration() begin

  write_(reg_addr::SYSTEM_SEQUENCE_CONFIG, 0x02);
  if (!performSingleRefCalibration(0x00)) { return false; }

  // -- VL53L0X_perform_phase_calibration() end

  // "restore the previous Sequence Config"
  write_(reg_addr::SYSTEM_SEQUENCE_CONFIG, 0xE8);

#endif
			/// PerformRefCalibration() end
			return true;
		}




	};
}
