#pragma once
//=====================================================================//
/*!	@file
	@brief	トランジスター・テスター API @n
			Copyright 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include "common/intr_utils.hpp"
#include "common/delay.hpp"
#include "common/adc_io.hpp"

namespace trt {




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  トランジスター・テスター API
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class api {

		//Bauteile
		static const uint8_t PART_NONE  = 0;
		static const uint8_t PART_DIODE = 1;
		static const uint8_t PART_TRANSISTOR = 2;
		static const uint8_t PART_FET = 3;
		static const uint8_t PART_TRIAC = 4;
		static const uint8_t PART_THYRISTOR = 5;
		static const uint8_t PART_RESISTOR = 6;
		static const uint8_t PART_CAPACITOR = 7;

		//Ende (Bauteile)
		//Spezielle Definitionen für Bauteile
		//FETs
		static const uint8_t PART_MODE_N_E_MOS = 1;
		static const uint8_t PART_MODE_P_E_MOS = 2;
		static const uint8_t PART_MODE_N_D_MOS = 3;
		static const uint8_t PART_MODE_P_D_MOS = 4;
		static const uint8_t PART_MODE_N_JFET  = 5;
		static const uint8_t PART_MODE_P_JFET  = 6;

		//Bipolar
		static const uint8_t PART_MODE_NPN = 1;
		static const uint8_t PART_MODE_PNP = 2;


		struct port_t {
			void operator = (uint8_t v) {
			}
			void operator |= (uint8_t v) {
			}
			void operator &= (uint8_t v) {
			}
		};
		port_t R_PORT;
		port_t ADC_PORT;

		struct ddr_t {
			void operator = (uint8_t v) {
			}
			void operator |= (uint8_t v) {
			}
			void operator &= (uint8_t v) {
			}
		};
		ddr_t R_DDR;
		ddr_t ADC_DDR;

		struct inp_t {
			uint8_t operator & (uint8_t v) {
				return 0;
			}
			uint8_t operator = (uint8_t v) {
				return 0;
			}
		};
		inp_t ADC_PIN;

		typedef device::adc_io<utils::null_task> ADC;
		ADC	adc_;

		uint8_t tmpval;
		uint8_t	tmpval2;

		uint8_t	b;
		uint8_t	c;
		uint8_t	e;
		uint8_t PartMode;
		uint8_t PartFound;

		char outval[8];


		char * utoa(unsigned int val, char * s, int radix) {
			return s;
		}

		inline void _delay_ms(uint16_t ms) {
			utils::delay::micro_second(ms);
		}

		void wdt_reset() {
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  全体初期化
		*/
		//-----------------------------------------------------------------//
		void start() {
			utils::PORT_MAP(utils::port_map::P10::AN0);
			utils::PORT_MAP(utils::port_map::P11::AN1);
			utils::PORT_MAP(utils::port_map::P12::AN2);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  Ａ／Ｄ変換値を読む
			@param[in]	mux	チャネル
			@return 結果
		*/
		//-----------------------------------------------------------------//
		uint16_t ReadADC(uint8_t mux) {
			//ADC-Wert des angegebenen Kanals auslesen und als unsigned int zurückgegen
			uint16_t adcx = 0;
#if 0
			ADMUX = mux | (1 << REFS0);
			for(uint8_t j = 0; j < 20; j++) {	//20 Messungen; für bessere Genauigkeit
				ADCSRA |= (1 << ADSC);
				while (ADCSRA & (1 << ADSC));
				adcx += ADCW;
			}
#endif
			if(mux < 2) {
				adc_.start(ADC::cnv_type::CH0, ADC::ch_grp::AN0_AN1, true);
			} else {
				adc_.start(ADC::cnv_type::CH0, ADC::ch_grp::AN2_AN3, true);
			}
			for(uint8_t j = 0; j < 20; ++j) {
				adc_.scan();
				adc_.sync();
				adcx += adc_.get_value(mux & 1);
			}
			adcx /= 20;
			return adcx;
		}


		void GetGateThresholdVoltage(void) {
			uint16_t tmpint = 0;
			uint16_t tmpintb = 0;
	#ifdef UseM8
			unsigned long gcval = 0;
	#endif
	
			uint8_t extcnt = 0;
			tmpval = (1<<(2*c) | (2<<(2*b)));
			tmpval2=(1<<(2*c));
			R_DDR = tmpval;        // Drain über R_L auf Ausgang, Gate über R_H auf Ausgang
			ADC_DDR=(1<<e)|(1<<b);	//Gate und Source auf Ausgang

			if((PartFound==PART_FET) && (PartMode == PART_MODE_N_E_MOS)) {
				//Gate-Schwellspannung messen
				ADC_PORT = 0;			//Gate und Source fest auf Masse
				R_PORT = tmpval;  	   // Drain über R_L auf Plus, Gate über R_H auf Plus
				tmpval=(1<<c);
				_delay_ms(10);
				ADC_DDR=(1<<e);          // Gate über R_H langsam laden
	
				while ((ADC_PIN&tmpval)) {  // Warten, bis der MOSFET schaltet und Drain auf low geht; Schleife dauert 7 Zyklen
					wdt_reset();
					tmpint++;
					if(tmpint==0) {
						extcnt++;
						if(extcnt == 8) break; //Timeout für Gate-Schwellspannungs-Messung
					}
				}
		
				R_PORT=tmpval2;          // Gate hochohmig schalten
				R_DDR=tmpval2;          // Gate hochohmig schalten
				tmpintb=ReadADC(b);
		#ifdef UseM8
				//Gatekapazität messen
				tmpint = 0;
				extcnt = 0;
				ADC_DDR = ((1<<e) | (1<<b) | (1<<c));	//Gate, Drain und Source auf Ausgang
				ADC_PORT = 0;					//Gate, Drain und Source fest auf Masse
				tmpval = (2<<(2*b));			//Gate über R_H auf Plus
				R_DDR = tmpval;        // Drain über R_L auf Ausgang, Gate über R_H auf Ausgang
				R_PORT = tmpval;  	   // Drain über R_L auf Plus, Gate über R_H auf Plus
				tmpval=(1<<b);
				_delay_ms(10);
				ADC_DDR=((1<<e) | (1<<c));          // Gate über R_H langsam laden
				while (!(ADC_PIN & tmpval)) {  // Warten, bis Gate auf High geht; Schleife dauert 7 Zyklen
					wdt_reset();
					tmpint++;
					if(tmpint==0) {
						extcnt++;
						if(extcnt == 8) break; //Timeout für Gate-Schwellspannungs-Messung
					}
				}
				gcval = N_GATE_CAPACITY_FACTOR;	//Wert für N-Kanal-MOSFET
			#endif
			} else if((PartFound==PART_FET) && (PartMode == PART_MODE_P_E_MOS)) {
				ADC_PORT = (1<<e)|(1<<b);	//Gate und Source fest auf Plus
				R_PORT = 0;					//Drain über R_L auf Masse, Gate über R_H auf Masse
				tmpval=(1<<c);
				_delay_ms(10);
				ADC_DDR=(1<<e);          // Gate über R_H langsam laden (Gate auf Eingang)
				ADC_PORT=(1<<e);          // Gate über R_H langsam laden (Gate-Pullup aus)
				while (!(ADC_PIN&tmpval)) {  // Warten, bis der MOSFET schaltet und Drain auf high geht
					wdt_reset();
					tmpint++;
					if(tmpint==0) {
						extcnt++;
						if(extcnt == 8) break; //Timeout für Gate-Schwellspannungs-Messung
					}
				}
				R_DDR=tmpval2;          // Gate hochohmig schalten
				tmpintb=ReadADC(b);
				#ifdef UseM8
					//Gatekapazität messen
					tmpint = 0;
					extcnt = 0;
					ADC_DDR = ((1<<e) | (1<<b) | (1<<c));	//Gate, Drain und Source auf Ausgang
					ADC_PORT = ((1<<e) | (1<<b) | (1<<c));	//Gate, Drain und Source fest auf Plus
					tmpval = (2<<(2*b));			//Gate über R_H auf Masse
					R_DDR = tmpval;        			// Gate über R_H auf Ausgang
					R_PORT = 0;  	   	  			// Gate über R_H auf Masse
					tmpval=(1<<b);
					_delay_ms(10);
					tmpval2 = ((1<<e) | (1<<c));	// Gate über R_H langsam laden
					ADC_DDR=tmpval2;  
					ADC_PORT=tmpval2;         
					while (ADC_PIN & tmpval) {  // Warten, bis Gate auf High geht; Schleife dauert 7 Zyklen
						wdt_reset();
						tmpint++;
						if(tmpint==0) {
							extcnt++;
							if(extcnt == 8) break; //Timeout für Gate-Schwellspannungs-Messung
						}
					}
					gcval = P_GATE_CAPACITY_FACTOR;	//Wert für P-Kanal-MOSFET
				#endif
			}
			R_DDR = 0;
			R_PORT = 0;
			ADC_DDR = 0;
			ADC_PORT = 0;
			if((tmpint > 0) || (extcnt > 0)) {
				if(PartMode == PART_MODE_P_E_MOS) {
					tmpintb = 1023-tmpintb;
				}
				tmpintb=(tmpintb*39/8);
				utoa(tmpintb, outval, 10);
				#ifdef UseM8
					/*
					  Berechnung der Gate-Kapazität
					  Bei Vcc=5V schalten die AVR-Portpins bei 3,6V um.
					  Auf diese Spannung ist das Gate nun geladen.
					  Das sind 72% der Betriebsspannung und ergibt damit eine Zeitkonstante von 1,28 Tau (1,28 R*C).
					  Aus bisher nicht bekannten Gründen weicht der tatsächliche Wert aber deutlich davon ab.
					  Die Berechnungsfaktoren sind als defines ganz oben zu finden und müssen ggf. angepasst werden.
					*/
					gcval *= (unsigned long)(((unsigned long)extcnt * 65536) + (unsigned long)tmpint);	//Wert speichern
					gcval /= 100;
					tmpint = (unsigned int)gcval;
					if(tmpint>2) tmpint -= 3;
					utoa(tmpint, outval2, 10);
				#endif
			}
		}


		void DischargePin(uint8_t PinToDischarge, uint8_t DischargeDirection) {
			/*Anschluss eines Bauelementes kurz(10ms) auf ein bestimmtes Potenzial legen
			  Diese Funktion ist zum Entladen von MOSFET-Gates vorgesehen, um Schutzdioden u.ä.
			  in MOSFETs erkennen zu können
			Parameter:
			PinToDischarge: zu entladender Pin
			DischargeDirection: 0 = gegen Masse (N-Kanal-FET), 1= gegen Plus(P-Kanal-FET)
			*/
			uint8_t tmpval;
			tmpval = (PinToDischarge * 2);		//nötig wegen der Anordnung der Widerstände

			if(DischargeDirection) R_PORT |= (1<<tmpval);			//R_L aus
			R_DDR |= (1<<tmpval);			//Pin auf Ausgang und über R_L auf Masse
			_delay_ms(10);
			R_DDR &= ~(1<<tmpval);			//Pin wieder auf Eingang
			if(DischargeDirection) R_PORT &= ~(1<<tmpval);			//R_L aus
		}

	};
}
