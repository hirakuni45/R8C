#pragma once
//=====================================================================//
/*!	@file
	@brief	MFRC522 クラス @n
			NXP Semiconductors @n
			Interface: SPI, Vcc: 3.3V
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/delay.hpp"

namespace chip {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  MFRC522 テンプレートクラス
		@param[in]	SPI	spi クラス
		@param[in]	CS	チップ・セレクト
		@param[in]	RES	リセット信号
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class SPI, class CS, class RES>
	class MFRC522 {
	public:


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  ステータス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class status : uint8_t {
			OK,				///< Success
			ERROR,			///< Error in communication
			COLLISION,		///< Collission detected
			TIMEOUT,		///< Timeout in communication.
			NO_ROOM,		///< A buffer is not big enough.
			INTERNAL_ERROR,	///< Internal error in the code. Should not happen ;-)
			INVALID,		///< Invalid argument.
			CRC_WRONG,		///< The CRC_A does not match
			MIFARE_NACK	= 0xff	///< A MIFARE PICC responded with NAK.
		};


		// MFRC522 RxGain[2:0] masks, defines the receiver's signal voltage gain factor (on the PCD).
		// Described in 9.3.3.6 / table 98 of the datasheet at
		// http://www.nxp.com/documents/data_sheet/MFRC522.pdf
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  アンテナ・ゲイン
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class RxGain {
			v_18dB		= 0x00 << 4,	// 000b - 18 dB, minimum
			v_23dB		= 0x01 << 4,	// 001b - 23 dB
			v_18dB_2	= 0x02 << 4,	// 010b - 18 dB, it seems 010b is a duplicate for 000b
			v_23dB_2	= 0x03 << 4,	// 011b - 23 dB, it seems 011b is a duplicate for 001b
			v_33dB		= 0x04 << 4,	// 100b - 33 dB, average, and typical default
			v_38dB		= 0x05 << 4,	// 101b - 38 dB
			v_43dB		= 0x06 << 4,	// 110b - 43 dB
			v_48dB		= 0x07 << 4,	// 111b - 48 dB, maximum
			min			= 0x00 << 4,	// 000b - 18 dB, minimum, convenience for RxGain_18dB
			avg			= 0x04 << 4,	// 100b - 33 dB, average, convenience for RxGain_33dB
			max			= 0x07 << 4		// 111b - 48 dB, maximum, convenience for RxGain_48dB
		};

	private:
		SPI&	spi_;

		enum class reg_adr : uint8_t {
			// Page 0: Command and status
			//            0x00
			Command		= 0x01 << 1,	// starts and stops command execution
			ComIEn		= 0x02 << 1,	// enable and disable interrupt request control bits
			DivIEn		= 0x03 << 1,	// enable and disable interrupt request control bits
			ComIrq		= 0x04 << 1,	// interrupt request bits
			DivIrq		= 0x05 << 1,	// interrupt request bits

			Error		= 0x06 << 1,	// error bits showing the error status of the last command executed 
			Status1		= 0x07 << 1,	// communication status bits
			Status2		= 0x08 << 1,	// receiver and transmitter status bits
			FIFOData	= 0x09 << 1,	// input and output of 64 byte FIFO buffer
			FIFOLevel	= 0x0A << 1,	// number of bytes stored in the FIFO buffer
			WaterLevel	= 0x0B << 1,	// level for FIFO underflow and overflow warning
			Control		= 0x0C << 1,	// miscellaneous control registers
			BitFraming	= 0x0D << 1,	// adjustments for bit-oriented frames
			Coll		= 0x0E << 1,	// bit position of the first bit-collision detected on the RF interface
			//			  0x0F			// reserved for future use

			// Page 1: Command
			//            0x10
			Mode		= 0x11 << 1,	// defines general modes for transmitting and receiving 
			TxMode		= 0x12 << 1,	// defines transmission data rate and framing
			RxMode		= 0x13 << 1,	// defines reception data rate and framing
			TxControl	= 0x14 << 1,	// controls the logical behavior of the antenna driver pins TX1 and TX2
			TxASK		= 0x15 << 1,	// controls the setting of the transmission modulation
			TxSel		= 0x16 << 1,	// selects the internal sources for the antenna driver
			RxSel		= 0x17 << 1,	// selects internal receiver settings
			RxThreshold	= 0x18 << 1,	// selects thresholds for the bit decoder
			Demod		= 0x19 << 1,	// defines demodulator settings
			// 			  0x1A			// reserved for future use
			// 			  0x1B			// reserved for future use
			MfTx		= 0x1C << 1,	// controls some MIFARE communication transmit parameters
			MfRx		= 0x1D << 1,	// controls some MIFARE communication receive parameters
			// 			  0x1E			// reserved for future use
			SerialSpeed	= 0x1F << 1,	// selects the speed of the serial UART interface
		
			// Page 2: Configuration
			// 			  0x20			// reserved for future use
			CRCResultH	= 0x21 << 1,	// shows the MSB and LSB values of the CRC calculation
			CRCResultL	= 0x22 << 1,
			// 			  0x23			// reserved for future use
			ModWidth	= 0x24 << 1,	// controls the ModWidth setting?
			// 			  0x25			// reserved for future use
			RFCfg		= 0x26 << 1,	// configures the receiver gain
			GsN			= 0x27 << 1,	// selects the conductance of the antenna driver pins TX1 and TX2 for modulation 
			CWGsP		= 0x28 << 1,	// defines the conductance of the p-driver output during periods of no modulation
			ModGsP		= 0x29 << 1,	// defines the conductance of the p-driver output during periods of modulation
			TMode		= 0x2A << 1,	// defines settings for the internal timer
			TPrescaler	= 0x2B << 1,	// the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
			TReloadH	= 0x2C << 1,	// defines the 16-bit timer reload value
			TReloadL	= 0x2D << 1,
			TCounterValueH = 0x2E << 1,	// shows the 16-bit timer value
			TCounterValueL = 0x2F << 1,
		
			// Page 3: Test Registers
			// 			  0x30			// reserved for future use
			TestSel1	= 0x31 << 1,	// general test signal configuration
			TestSel2	= 0x32 << 1,	// general test signal configuration
			TestPinEn	= 0x33 << 1,	// enables pin output driver on pins D1 to D7
			TestPinValue= 0x34 << 1,	// defines the values for D1 to D7 when it is used as an I/O bus
			TestBus		= 0x35 << 1,	// shows the status of the internal test bus
			AutoTest	= 0x36 << 1,	// controls the digital self-test
			Version		= 0x37 << 1,	// shows the software version
			AnalogTest	= 0x38 << 1,	// controls the pins AUX1 and AUX2
			TestDAC1	= 0x39 << 1,	// defines the test value for TestDAC1
			TestDAC2	= 0x3A << 1,	// defines the test value for TestDAC2
			TestADC		= 0x3B << 1		// shows the value of ADC I and Q channels
			// 			  0x3C			// reserved for production tests
			// 			  0x3D			// reserved for production tests
			// 			  0x3E			// reserved for production tests
			// 			  0x3F			// reserved for production tests
		};


		// MFRC522 commands. Described in chapter 10 of the datasheet.
		enum class Command : uint8_t {
			Idle			= 0x00,		// no action, cancels current command execution
			Mem				= 0x01,		// stores 25 bytes into the internal buffer
			GenerateRandomID= 0x02,		// generates a 10-byte random ID number
			CalcCRC			= 0x03,		// activates the CRC coprocessor or performs a self-test
			Transmit		= 0x04,		// transmits data from the FIFO buffer
			NoCmdChange		= 0x07,		// no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
			Receive			= 0x08,		// activates the receiver circuits
			Transceive 		= 0x0C,		// transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
			MFAuthent 		= 0x0E,		// performs the MIFARE standard authentication as a reader
			SoftReset		= 0x0F		// resets the MFRC522
		};


		void write_reg_(reg_adr reg, uint8_t value) noexcept
		{
			CS::P = 0;  // enable device
			// MSB == 0 is for writing. LSB is not used in address.
			spi_.xchg(static_cast<uint8_t>(reg) & 0x7E);
			spi_.xchg(value);
			CS::P = 1;  // disable device
		}


		inline void write_reg_(reg_adr reg, Command value) noexcept
		{
			write_reg_(reg, static_cast<uint8_t>(value));
		}


		void write_reg_(reg_adr reg, const void* data, uint8_t size) noexcept
		{
			CS::P = 0;  // enable device
			// MSB == 0 is for writing. LSB is not used in address.
			spi_.xchg(static_cast<uint8_t>(reg) & 0x7E);
			spi_.send(data, size);
			CS::P = 1;  // disable device
		}


		uint8_t read_reg_(reg_adr reg) noexcept
		{
			CS::P = 0;  // enable device
			// MSB == 0 is for writing. LSB is not used in address.
			spi_.xchg(0x80 | (static_cast<uint8_t>(reg) & 0x7E));
			auto data = spi_.xchg(0);
			CS::P = 1;  // disable device
			return data;
		}


		void read_reg_(reg_adr reg, void* data, uint8_t size, uint8_t rx_align = 0) noexcept
		{
			if(size == 0) {
				return;
			}

			// MSB == 1 is for reading. LSB is not used in address.
			uint8_t adr = 0x80 | (static_cast<uint8_t>(reg) & 0x7E);
			CS::P = 0;  // enable device
			spi_.xchg(adr);
			size--;
			if(rx_align > 0) {
				// Create bit mask for bit positions rx_align..7
				uint8_t mask = 0;
				for(uint8_t i = rx_align; i <= 7; i++) {
					mask |= (1 << i);
				}
				uint8_t idx = 0;
				uint8_t* p = static_cast<uint8_t*>(data);
				while(idx < size) {
					// Only update bit positions rx_align..7 in values[0]
					if(idx == 0) {
						uint8_t value = spi_.xchg(adr);
						// Apply mask to both current value of values[0] and the new data in value.
						p[0] = (p[idx] & ~mask) | (value & mask);
					} else {
						p[idx] = spi_.xchg(adr);
					}
					idx++;
				}
				p[idx] = spi_.xchg(0);
			} else {
				spi_.recv(data, size);
			}

			CS::P = 1;  // disable device
		}


		void set_reg_bit_(reg_adr reg, uint8_t mask, bool ena = true)
		{
			auto tmp = read_reg_(reg);
			if(ena) {
				write_reg_(reg, tmp | mask);
			} else {
				write_reg_(reg, tmp & (~mask));
			}
		}


		status calculate_CRC_(const void* data, uint8_t size, uint16_t& result)
		{
			write_reg_(reg_adr::Command, Command::Idle);	// Stop any active command.
			write_reg_(reg_adr::DivIrq,  0x04);				// Clear the CRCIRq interrupt request bit
			set_reg_bit_(reg_adr::FIFOLevel, 0x80);		// FlushBuffer = 1, FIFO initialization
			write_reg_(reg_adr::FIFOData, data, size);		// Write data to the FIFO
			write_reg_(reg_adr::Command, Command::CalcCRC);	// Start the calculation

			// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73μs.
			// TODO check/modify for other architectures than Arduino Uno 16bit
			uint16_t loop = 5000;
			while(1) {
				// DivIrqReg[7..0] bits are:
				//     Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
				auto n = read_reg_(reg_adr::DivIrq);
				if(n & 0x04) {  // CRCIRq bit set - calculation done
					break;
				}
				--loop;
				// The emergency break.
				// We will eventually terminate on this one after 89ms.
				// Communication with the MFRC522 might be down.
				if(loop == 0) {
					return status::TIMEOUT;
				}
			}
			// Stop calculating CRC for new content in the FIFO.
			write_reg_(reg_adr::Command, Command::Idle);

			// Transfer the result from the registers to the result buffer
			uint16_t d = static_cast<uint16_t>(read_reg_(reg_adr::CRCResultH)) << 8;
			result = d | static_cast<uint16_t>(read_reg_(reg_adr::CRCResultL));

			return status::OK;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクタ
		 */
		//-----------------------------------------------------------------//
		MFRC522(SPI& spi) noexcept : spi_(spi) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ソフト・リセット
		 */
		//-----------------------------------------------------------------//
		void reset() noexcept
		{
			write_reg_(reg_adr::Command, Command::SoftReset);	// Issue the SoftReset command.
			// The datasheet does not mention how long the SoftRest command takes to complete.
			// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
			// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of
			// the crystal + 37,74μs. Let us be generous: 50ms.
			utils::delay::milli_second(50);
			// Wait for the PowerDown bit in CommandReg to be cleared
			while(read_reg_(reg_adr::Command) & (1 << 4)) {
				// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アンテナを許可（無効）
			@param[in]	ena	無効にする場合「false」
		 */
		//-----------------------------------------------------------------//
		void antenna_enable(bool ena = true) noexcept
		{
			if(ena) {
				auto value = read_reg_(reg_adr::TxControl);
				if((value & 0x03) != 0x03) {
					write_reg_(reg_adr::TxControl, value | 0x03);
				}
			} else {
				set_reg_bit_(reg_adr::TxControl, 0x03);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
		 */
		//-----------------------------------------------------------------//
		void start() noexcept
		{
			CS::DIR = 1;
			CS::P = 1;  // disable device
			RES::DIR = 1;
			RES::P = 0;  // device reset

			/// SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0));
			if(RES::port_no == 0xff) {
				reset();
			} else {  // Let us be generous: 50ms.
				utils::delay::milli_second(50);
				RES::P = 1;
			}


			// When communicating with a PICC we need a timeout if something goes wrong.
			// f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
			// TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.

			// TAuto=1; timer starts automatically at the end of the transmission in all
			// communication modes at all speeds
			write_reg_(reg_adr::TMode, 0x80);
			// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz,
			// ie a timer period of 25μs.
			write_reg_(reg_adr::TPrescaler, 0xA9);
			// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
			write_reg_(reg_adr::TReloadH, 0x03);
			write_reg_(reg_adr::TReloadL, 0xE8);

			// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
			write_reg_(reg_adr::TxASK, 0x40);
			// Default 0x3F. Set the preset value for the CRC coprocessor for
			// the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
			write_reg_(reg_adr::Mode, 0x3D);
			// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
			antenna_enable();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アンテナ・ゲイン取得
			@return アンテナ・ゲイン
		 */
		//-----------------------------------------------------------------//
		RxGain get_antenna_gain() noexcept
		{
			return static_cast<RxGain>((read_reg_(reg_adr::RFCfg) >> 4) & 0x07);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アンテナ・ゲイン取得
			@param[in]	gain	RxGain 型
		 */
		//-----------------------------------------------------------------//
		void set_antenna_gain(RxGain gain) noexcept
		{
			if(get_antenna_gain() != gain) {		// only bother if there is a change
				set_reg_bit_(reg_adr::RFCfg, (0x07 << 4), false);		// clear bits
				set_reg_bit_(reg_adr::RFCfg, static_cast<uint8_t>(gain) << 4);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セルフ・テスト
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool perform_selftest()
		{
			reset();
#if 0	
	// 2. Clear the internal buffer by writing 25 bytes of 00h
	byte ZEROES[25] = {0x00};
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);	// flush the FIFO buffer
	PCD_WriteRegister(FIFODataReg, 25, ZEROES);	// write 25 bytes of 00h to FIFO
	PCD_WriteRegister(CommandReg, PCD_Mem);		// transfer to internal buffer
	
	// 3. Enable self-test
	PCD_WriteRegister(AutoTestReg, 0x09);
	
	// 4. Write 00h to FIFO buffer
	PCD_WriteRegister(FIFODataReg, 0x00);
	
	// 5. Start self-test by issuing the CalcCRC command
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);
	
	// 6. Wait for self-test to complete
	byte n;
	for (uint8_t i = 0; i < 0xFF; i++) {
		// The datasheet does not specify exact completion condition except
		// that FIFO buffer should contain 64 bytes.
		// While selftest is initiated by CalcCRC command
		// it behaves differently from normal CRC computation,
		// so one can't reliably use DivIrqReg to check for completion.
		// It is reported that some devices does not trigger CRCIRq flag
		// during selftest.
		n = PCD_ReadRegister(FIFOLevelReg);
		if (n >= 64) {
			break;
		}
	}
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	
	// 7. Read out resulting 64 bytes from the FIFO buffer.
	byte result[64];
	PCD_ReadRegister(FIFODataReg, 64, result, 0);
	
	// Auto self-test done
	// Reset AutoTestReg register to be 0 again. Required for normal operation.
	PCD_WriteRegister(AutoTestReg, 0x00);
	
	// Determine firmware version (see section 9.3.4.8 in spec)
	byte version = PCD_ReadRegister(VersionReg);
	
	// Pick the appropriate reference values
	const byte *reference;
	switch (version) {
		case 0x88:	// Fudan Semiconductor FM17522 clone
			reference = FM17522_firmware_reference;
			break;
		case 0x90:	// Version 0.0
			reference = MFRC522_firmware_referenceV0_0;
			break;
		case 0x91:	// Version 1.0
			reference = MFRC522_firmware_referenceV1_0;
			break;
		case 0x92:	// Version 2.0
			reference = MFRC522_firmware_referenceV2_0;
			break;
		default:	// Unknown version
			return false; // abort test
	}
	
	// Verify that the results match up to our expectations
	for (uint8_t i = 0; i < 64; i++) {
		if (result[i] != pgm_read_byte(&(reference[i]))) {
			return false;
		}
	}
#endif
			return true;
		}


#if 0
	StatusCode PCD_TransceiveData(byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits = NULL, byte rxAlign = 0, bool checkCRC = false);
	StatusCode PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData = NULL, byte *backLen = NULL, byte *validBits = NULL, byte rxAlign = 0, bool checkCRC = false);
	StatusCode PICC_RequestA(byte *bufferATQA, byte *bufferSize);
	StatusCode PICC_WakeupA(byte *bufferATQA, byte *bufferSize);
	StatusCode PICC_REQA_or_WUPA(byte command, byte *bufferATQA, byte *bufferSize);
	StatusCode PICC_Select(Uid *uid, byte validBits = 0);
	StatusCode PICC_HaltA();
#endif

	};
}
