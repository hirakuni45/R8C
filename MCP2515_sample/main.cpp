//=====================================================================//
/*!	@file
	@brief	R8C MCP2515・メイン @n
			MCP2515(SCK)  ---> SPI_SCK (P1_1:19) @n
			MCP2515(MOSI) ---> SPI_MOSI(P1_2:18) @n
			MCP2515(MISO) ---> SPI_MISO(P1_3:17) @n
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2021 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/R8C/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

#include "common/format.hpp"
#include "common/fifo.hpp"
#include "common/uart_io.hpp"
#include "common/adc_io.hpp"
#include "common/trb_io.hpp"
#include "common/spi_io.hpp"
#include "chip/MCP2515.hpp"

namespace {

	typedef device::trb_io<utils::null_task, uint8_t> timer_b;
	timer_b timer_b_;

	typedef utils::fifo<uint8_t, 16> buffer;
	typedef device::uart_io<device::UART0, buffer, buffer> uart;
	uart uart_;

	// ポートの定義と接続
	// P1_0(20):
	typedef device::PORT<device::PORT1, device::bitpos::B0> MCP_CS;
	// P1_1(19):
	typedef device::PORT<device::PORT1, device::bitpos::B1> SPI_SCK;
	// P1_2(18):
	typedef device::PORT<device::PORT1, device::bitpos::B2> SPI_MOSI;
	// P1_3(17):
	typedef device::PORT<device::PORT1, device::bitpos::B3> SPI_MISO;
	// P1_4(16)
	typedef device::PORT<device::PORT1, device::bitpos::B4> MCP_INT;

	typedef device::spi_io<SPI_MISO, SPI_MOSI, SPI_SCK, device::soft_spi_mode::CK01_> SPI;
	SPI		spi_;

	typedef chip::MCP2515<SPI, MCP_CS> MCP;
	MCP		mcp_(spi_);
}

extern "C" {

	void sci_putch(char ch) {
		uart_.putch(ch);
	}


	char sci_getch(void) {
		return uart_.getch();
	}


	uint16_t sci_length() {
		return uart_.length();
	}


	void sci_puts(const char* str) {
		uart_.puts(str);
	}


	void TIMER_RB_intr(void) {
		timer_b_.itask();
	}


	void UART0_TX_intr(void) {
		uart_.isend();
	}


	void UART0_RX_intr(void) {
		uart_.irecv();
	}
}


// __attribute__ ((section (".exttext")))
int main(int argc, char *argv[])
{
	using namespace device;

	// クロック関係レジスタ・プロテクト解除
	PRCR.PRC0 = 1;

	// 高速オンチップオシレーターへ切り替え(20MHz)
	// ※ F_CLK を設定する事（Makefile内）
	OCOCR.HOCOE = 1;
	utils::delay::micro_second(1);  // >=30us(125KHz)
	SCKCR.HSCKSEL = 1;
	CKSTPR.SCKSEL = 1;

	// タイマーＢ初期化
	{
		uint8_t ir_level = 2;
		timer_b_.start(60, ir_level);
	}

	// UART の設定 (P1_4: TXD0[out], P1_5: RXD0[in])
	// ※シリアルライターでは、RXD 端子は、P1_6 となっているので注意！
	{
		utils::PORT_MAP(utils::port_map::P14::TXD0);
		utils::PORT_MAP(utils::port_map::P15::RXD0);
		uint8_t intr_level = 1;
		uart_.start(57600, intr_level);
	}

	uart_.puts("Start R8C MCP2515 sample\n");

	// SPI 開始
	{
		uint32_t speed = 1000000;  // 1M bps
		spi_.start(speed);
	}

	// MFRC522 開始 (send)
	{
		if(mcp_.start(MCP::ID_MODE::ANY, MCP::SPEED::BPS_500K)) {
			utils::format("MCP2515 Start OK\n");
		} else {
			utils::format("MCP2515 Start NG\n");
		}
		mcp_.set_mode(MCP::MODE::NORMAL);
		MCP_INT::DIR = 0;  // input
		MCP_INT::PU = 1;
	}

	using namespace utils;

	while(1) {
		timer_b_.sync();

		const uint8_t data[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
		// send data:  ID = 0x100, Standard CAN Frame, Data length = 8
		// bytes, 'data' = array of data bytes to send
		if(!mcp_.send(0x100, 0, data, 8)) {
			utils::format("Send: NG\n");
		}

		if(!MCP_INT::P()) {
			uint32_t id;
			uint8_t ext;
			uint8_t len;
			uint8_t tmp[8];
			if(mcp_.recv(id, ext, tmp, len)) {
				utils::format("ID: %8X\n") % id;
			} else {
				utils::format("Recv fail\n");
			}
		}
	}
}




#if 0
// recv
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }
}
#endif


// loopback
#if 0
// CAN TX Variables
unsigned long prevTX = 0;                                        // Variable to store last execution time
const unsigned int invlTX = 1000;                                // One second interval constant
byte data[] = {0xAA, 0x55, 0x01, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send

// CAN RX Variables
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];

// Serial Output String Buffer
char msgString[128];

// CAN0 INT and CS
#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);  // CAN is running at 500,000BPS; 115,200BPS is SLOW, not FAST, thus 9600 is crippling.
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  // Since we do not set NORMAL mode, we are in loopback mode by default.
  //CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Loopback Example...");
}

void loop()
{
  if(!digitalRead(CAN0_INT))                          // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)             // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){            // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }
  
  if(millis() - prevTX >= invlTX){                    // Send this at a one second interval. 
    prevTX = millis();
    byte sndStat = CAN0.sendMsgBuf(0x100, 8, data);
    
    if(sndStat == CAN_OK)
      Serial.println("Message Sent Successfully!");
    else
      Serial.println("Error Sending Message...");

  }
}
#endif
