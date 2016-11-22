#pragma once
//=====================================================================//
/*!	@file
	@brief	トランジスター・テスター API @n
			Copyright 2016 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "common/intr_utils.hpp"
#include "common/delay.hpp"
#include "common/adc_io.hpp"

#define UseM8

#ifdef UseM8
const char* OrBroken;
const char* Resistor;
const char* NullDot;
const char* GateCap;
const char* Capacitor;
#endif

const char* TestRunning;
const char* Bat;
const char* BatWeak;
const char* BatEmpty;
const char* TestFailed1;
const char* TestFailed2;
const char* Bauteil;
const char* Unknown;
const char* Diode;
const char* DualDiode;
const char* TwoDiodes;
const char* Antiparallel;
const char* InSeries;
const char* mosfet;
const char* emode;
const char* dmode;
const char* jfet;
const char* Thyristor;
const char* Triac;
const char* A1;
const char* A2;
const char* K1;
const char* K2;
const char* hfestr;
const char* NPN;
const char* PNP;
const char* bstr;
const char* cstr;
const char* estr;
const char* gds;
const char* Uf;
const char* vt;
const char* mV;
const char* Anode;
const char* Gate;
const char* TestTimedOut;
const char* DiodeIcon;



namespace trt {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  トランジスター・テスター API
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class api {

		static const uint32_t N_GATE_CAPACITY_FACTOR = 387;
		static const uint32_t P_GATE_CAPACITY_FACTOR = 142;

		static const uint32_t R_L_VAL = 680;			//R_L; Norm	wert 680 Ohm
		static const uint32_t R_H_VAL = 470000UL;	//R_H; Normwert 470000 Ohm, als unsigned long angeben

		//Im Programm nötige Widerstandswerte berechnen
		static const uint32_t RH_RL_RATIO = (R_H_VAL / R_L_VAL);
		static const uint32_t R_READ_RH = (R_H_VAL / 100);


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


		static const uint32_t H_CAPACITY_FACTOR = 394;
		static const uint32_t L_CAPACITY_FACTOR = 283;


		//Eigene Zeichen
		static const uint8_t LCD_CHAR_OMEGA  = 244;	//Omega-Zeichen
		static const uint8_t LCD_CHAR_U   = 228;		//µ-Zeichen
		static const uint8_t LCD_CHAR_DIODE  = 0;	//Dioden-Icon; wird als Custom-Character erstellt


		static const uint8_t WDTO_2S = 2;


		struct port_t {
			void operator = (uint8_t v) {
//				P = v;
			}
			void operator |= (uint8_t v) {
//				P |= v;
			}
			void operator &= (uint8_t v) {
//				P &= v;
			}
		};
		port_t R_PORT;
		port_t ADC_PORT;
		port_t ON_PORT;
		port_t PORTD;

		static const uint8_t TP1 = 0;
		static const uint8_t TP2 = 1;
		static const uint8_t TP3 = 2;

		static const uint8_t ON_PIN = 4;
		static const uint8_t RST_PIN = 5;

		static const uint8_t REFS1 = 3;

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
		ddr_t ON_DDR;

		struct inp_t {
			uint8_t operator & (uint8_t v) {
				return 0;
			}
			uint8_t operator = (uint8_t v) {
				return 0;
			}
		};
		inp_t ADC_PIN;
		inp_t ON_PIN_REG;

		typedef device::adc_io<utils::null_task> ADC;
		ADC	adc_;

		struct Diode_t {
			uint8_t Anode;
			uint8_t Cathode;
			int Voltage;
		};


struct Diode_t diodes[6];
uint8_t NumOfDiodes;

uint8_t b,c,e;			//Anschlüsse des Transistors
unsigned long lhfe;		//Verstärkungsfaktor
uint8_t PartReady;		//Bauteil fertig erkannt
unsigned int hfe[2];		//Verstärkungsfaktoren
unsigned int uBE[2];	//B-E-Spannung für Transistoren
uint8_t PartMode;
uint8_t tmpval, tmpval2;
#ifdef UseM8	//Widerstands- und Kondensatormessung nur auf dem Mega8 verfügbar
	uint8_t ra, rb;				//Widerstands-Pins
	unsigned int rv[2];			//Spannungsabfall am Widerstand
	unsigned int radcmax[2];	//Maximal erreichbarer ADC-Wert (geringer als 1023, weil Spannung am Low-Pin bei Widerstandsmessung über Null liegt)
	uint8_t ca, cb;				//Kondensator-Pins
	unsigned long cv;
#endif

uint8_t PartFound, tmpPartFound;	//das gefundene Bauteil
char outval[8];
unsigned int adcv[4];

#ifdef UseM8
	char outval2[6];
#endif


		char * utoa(unsigned int val, char * s, int radix) {
			return s;
		}

		char * ultoa(unsigned long val, char * s, int radix) {
			return s;
		}

		char * itoa(int val, char * s, int radix) {
			return s;
		}

		inline void _delay_ms(uint16_t ms) {
			utils::delay::micro_second(ms);
		}

		void wdt_disable() { }
		void wdt_enable(uint8_t t) { }
		void wdt_reset() { }

		void lcd_data(unsigned char temp1) { }
		void lcd_command(unsigned char temp1) { }
		void lcd_send(unsigned char data) { }
		void lcd_string(const char* data) { }
		void lcd_enable(void) { }
		void lcd_init(void) { }
		void lcd_clear(void) { }
		void lcd_eep_string(const char* data) { }
		void Line1() {
			// SetCursor(1,0)	//An den Anfang der 1. Zeile springen
		}
		void Line2() {
			// SetCursor(2,0)	//An den Anfang der 2. Zeile springen
		}

		void SetCursor(uint8_t y, uint8_t x) {
			// lcd_command((uint8_t)(CMD_SetDDRAMAddress + (0x40*(y-1)) + x)) //An eine bestimmte Position springen
		}

		void LCDLoadCustomChar() {
			// lcd_command(CMD_SetCGRAMAddress)	//Custom-Zeichen laden
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

		int service() {
	//Einschalten
	ON_DDR = (1<<ON_PIN);
	ON_PORT = (1<<ON_PIN) | (1<<RST_PIN);	//Strom an und Pullup für Reset-Pin
	uint8_t tmp;
	//ADC-Init
///	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);	//Vorteiler=8

	lcd_init();



#if 0
	if(MCU_STATUS_REG & (1<<WDRF)) {	
		/*
		Überprüfen auf Watchdog-Reset 
		Das tritt ein, wenn der Watchdog 2s nicht zurückgesetzt wurde
		Kann vorkommen, wenn sich das Programm in einer Endlosschleife "verheddert" hat.
		*/
		lcd_eep_string(TestTimedOut);	//Timeout-Meldung
		_delay_ms(3000);
		ON_PORT = 0;	//Abschalten!
		return 0;
	}
#endif



	LCDLoadCustomChar();	//Custom-Zeichen
	//Diodensymbol in LCD laden
	lcd_eep_string(DiodeIcon);
	Line1();	//1. Zeile
	//Einsprungspunkt, wenn Start-Taste im Betrieb erneut gedrückt wird
	start:
	wdt_enable(WDTO_2S);	//Watchdog an
	PartFound = PART_NONE;
	tmpPartFound = PART_NONE;
	NumOfDiodes = 0;
	PartReady = 0;
	PartMode = 0;
	#ifdef UseM8
	ca = 0;
	cb = 0;
	#endif
	lcd_clear();
	//Versorgungsspannung messen
	ReadADC(5 | (1<<REFS1));	//Dummy-Readout
	hfe[0] = ReadADC(5 | (1<<REFS1)); 	//mit interner Referenz
	if (hfe[0] < 650) {			//Vcc < 7,6V; Warnung anzeigen
		lcd_eep_string(Bat);		//Anzeige: "Batterie"
		if(hfe[0] < 600) {					//Vcc <7,15V; zuverlässiger Betrieb nicht mehr möglich
			lcd_eep_string(BatEmpty);		//Batterie leer!
			_delay_ms(1000);
			PORTD = 0;	//abschalten
			return 0;
		}
		lcd_eep_string(BatWeak);		//Batterie schwach
		Line2();
	}
	//Test beginnen
	lcd_eep_string(TestRunning);	//String: Test läuft
	//Alle 6 Kombinationsmöglichkeiten für die 3 Pins prüfen
	CheckPins(TP1, TP2, TP3);
	CheckPins(TP1, TP3, TP2);
	CheckPins(TP2, TP1, TP3);
	CheckPins(TP2, TP3, TP1);
	CheckPins(TP3, TP2, TP1);
	CheckPins(TP3, TP1, TP2);
	#ifdef UseM8
		//Separate Messung zum Test auf Kondensator
		if((PartFound == PART_NONE) || (PartFound == PART_RESISTOR) || (PartFound == PART_DIODE)) {
			//Kondensator entladen; sonst ist evtl. keine Messung möglich
			R_PORT = 0;
			R_DDR = (1<<(TP1 * 2)) | (1<<(TP2 * 2)) | (1<<(TP3 * 2));
			_delay_ms(50);
			R_DDR = 0;
			//Kapazität in allen 6 Pin-Kompinationen messen
			ReadCapacity(TP3, TP1);
			ReadCapacity(TP3, TP2);
			ReadCapacity(TP2, TP3);
			ReadCapacity(TP2, TP1);
			ReadCapacity(TP1, TP3);
			ReadCapacity(TP1, TP2);	
		}
	#endif
	//Fertig, jetzt folgt die Auswertung
	GetGateThresholdVoltage();
	lcd_clear();
	if(PartFound == PART_DIODE) {
		if(NumOfDiodes == 1) {
			//Standard-Diode
			lcd_eep_string(Diode);	//"Diode: "
			lcd_eep_string(Anode);
			lcd_data(diodes[0].Anode + 49);
			lcd_string(";K=");
			lcd_data(diodes[0].Cathode + 49);
			Line2();	//2. Zeile
			lcd_eep_string(Uf);	//"Uf = "
			lcd_string(itoa(diodes[0].Voltage, outval, 10));
			lcd_eep_string(mV);
			goto end;
		} else if(NumOfDiodes == 2) {
		//Doppeldiode
			if(diodes[0].Anode == diodes[1].Anode) {
				//Common Anode
				lcd_eep_string(DualDiode);	//Doppeldiode
				lcd_string("CA");
				Line2(); //2. Zeile
				lcd_eep_string(Anode);
				lcd_data(diodes[0].Anode + 49);
				lcd_eep_string(K1);	//";K1="
				lcd_data(diodes[0].Cathode + 49);
				lcd_eep_string(K2);	//";K2="
				lcd_data(diodes[1].Cathode + 49);
				goto end;
			} else if(diodes[0].Cathode == diodes[1].Cathode) {
				//Common Cathode
				lcd_eep_string(DualDiode);	//Doppeldiode
				lcd_string("CC");
				Line2(); //2. Zeile
				lcd_string("K=");
				lcd_data(diodes[0].Cathode + 49);
				lcd_eep_string(A1);		//";A1="
				lcd_data(diodes[0].Anode + 49);
				lcd_eep_string(A2);		//";A2="
				lcd_data(diodes[1].Anode + 49);
				goto end;
			} else if ((diodes[0].Cathode == diodes[1].Anode) && (diodes[1].Cathode == diodes[0].Anode)) {
				//Antiparallel
				lcd_eep_string(TwoDiodes);	//2 Dioden
				Line2(); //2. Zeile
				lcd_eep_string(Antiparallel);	//Antiparallel
				goto end;
			}
		} else if(NumOfDiodes == 3) {
			//Serienschaltung aus 2 Dioden; wird als 3 Dioden erkannt
			b = 3;
			c = 3;
			/* Überprüfen auf eine für eine Serienschaltung von 2 Dioden mögliche Konstellation
				Dafür müssen 2 der Kathoden und 2 der Anoden übereinstimmen.
				Das kommmt daher, dass die Dioden als 2 Einzeldioden und ZUSÄTZLICH als eine "große" Diode erkannt werden.
			*/
			if((diodes[0].Anode == diodes[1].Anode) || (diodes[0].Anode == diodes[2].Anode)) b = diodes[0].Anode;
			if(diodes[1].Anode == diodes[2].Anode) b = diodes[1].Anode;

			if((diodes[0].Cathode == diodes[1].Cathode) || (diodes[0].Cathode == diodes[2].Cathode)) c = diodes[0].Cathode;
			if(diodes[1].Cathode == diodes[2].Cathode) c = diodes[1].Cathode;
			if((b<3) && (c<3)) {
				lcd_eep_string(TwoDiodes);//2 Dioden
				Line2(); //2. Zeile
				lcd_eep_string(InSeries); //"in Serie A="
				lcd_data(b + 49);
				lcd_string(";K=");
				lcd_data(c + 49);
				goto end;
			}
		}
	} else if (PartFound == PART_TRANSISTOR) {
		if(PartReady == 0) {	//Wenn 2. Prüfung nie gemacht, z.B. bei Transistor mit Schutzdiode
			hfe[1] = hfe[0];
			uBE[1] = uBE[0];
		}
		if((hfe[0]>hfe[1])) {	//Wenn der Verstärkungsfaktor beim ersten Test höher war: C und E vertauschen!
			hfe[1] = hfe[0];
			uBE[1] = uBE[0];
			tmp = c;
			c = e;
			e = tmp;
		}

		if(PartMode == PART_MODE_NPN) {
			lcd_eep_string(NPN);
		} else {
			lcd_eep_string(PNP);
		}
		lcd_eep_string(bstr);	//B=
		lcd_data(b + 49);
		lcd_eep_string(cstr);	//;C=
		lcd_data(c + 49);
		lcd_eep_string(estr);	//;E=
		lcd_data(e + 49);
		Line2(); //2. Zeile
		//Verstärkungsfaktor berechnen
		//hFE = Emitterstrom / Basisstrom
		lhfe = hfe[1];
		lhfe *= RH_RL_RATIO;	//Verhältnis von High- zu Low-Widerstand
		if(uBE[1]<11) uBE[1] = 11;
		lhfe /= uBE[1];
		hfe[1] = (unsigned int) lhfe;
		lcd_eep_string(hfestr);	//"hFE="
		lcd_string(utoa(hfe[1], outval, 10));
		SetCursor(2,7);			//Cursor auf Zeile 2, Zeichen 7
		if(NumOfDiodes > 2) {	//Transistor mit Schutzdiode
			lcd_data(LCD_CHAR_DIODE);	//Diode anzeigen
		} else {
			#ifdef UseM8
				lcd_data(' ');
			#endif
		}
		#ifdef UseM8
			for(c=0;c<NumOfDiodes;c++) {
				if(((diodes[c].Cathode == e) && (diodes[c].Anode == b) && (PartMode == PART_MODE_NPN)) || ((diodes[c].Anode == e) && (diodes[c].Cathode == b) && (PartMode == PART_MODE_PNP))) {
					lcd_eep_string(Uf);	//"Uf="
					lcd_string(itoa(diodes[c].Voltage, outval, 10));
					lcd_data('m');
					goto end;
				}
			}
		#endif
		goto end;
	} else if (PartFound == PART_FET) {	//JFET oder MOSFET
		if(PartMode&1) {	//N-Kanal
			lcd_data('N');
		} else {
			lcd_data('P');	//P-Kanal
		}
		if((PartMode==PART_MODE_N_D_MOS) || (PartMode==PART_MODE_P_D_MOS)) {
			lcd_eep_string(dmode);	//"-D"
			lcd_eep_string(mosfet);	//"-MOS"
		} else {
			if((PartMode==PART_MODE_N_JFET) || (PartMode==PART_MODE_P_JFET)) {
				lcd_eep_string(jfet);	//"-JFET"
			} else {
				lcd_eep_string(emode);	//"-E"
				lcd_eep_string(mosfet);	//"-MOS"
			}
		}
		#ifdef UseM8	//Gatekapazität
			if(PartMode < 3) {	//Anreicherungs-MOSFET
				lcd_eep_string(GateCap);	//" C="
				tmpval = strlen(outval2);
				tmpval2 = tmpval;
				if(tmpval>4) tmpval = 4;	//bei Kapazität >100nF letze Nachkommastelle nicht mehr angeben (passt sonst nicht auf das LCD)
				lcd_show_format_cap(outval2, tmpval, tmpval2);
				lcd_data('n');
			}
		#endif
		Line2(); //2. Zeile
		lcd_eep_string(gds);	//"GDS="
		lcd_data(b + 49);
		lcd_data(c + 49);
		lcd_data(e + 49);
		if((NumOfDiodes > 0) && (PartMode < 3)) {	//MOSFET mit Schutzdiode; gibt es nur bei Anreicherungs-FETs
			lcd_data(LCD_CHAR_DIODE);	//Diode anzeigen
		} else {
			lcd_data(' ');	//Leerzeichen
		}
		if(PartMode < 3) {	//Anreicherungs-MOSFET
			lcd_eep_string(vt);
			lcd_string(outval);	//Gate-Schwellspannung, wurde zuvor ermittelt
			lcd_data('m');
		}
		goto end;
	} else if (PartFound == PART_THYRISTOR) {
		lcd_eep_string(Thyristor);	//"Thyristor"
		Line2(); //2. Zeile
		lcd_string("GAK=");
		lcd_data(b + 49);
		lcd_data(c + 49);
		lcd_data(e + 49);
		goto end;
	} else if (PartFound == PART_TRIAC) {
		lcd_eep_string(Triac);	//"Triac"
		Line2(); //2. Zeile
		lcd_eep_string(Gate);
		lcd_data(b + 49);
		lcd_eep_string(A1);		//";A1="
		lcd_data(e + 49);
		lcd_eep_string(A2);		//";A2="
		lcd_data(c + 49);
		goto end;
	#ifdef UseM8	//Widerstandsmessung nur mit Mega8 verfügbar
		} else if(PartFound == PART_RESISTOR) {
			lcd_eep_string(Resistor); //"Widerstand: "
			lcd_data(ra + 49);	//Pin-Angaben
			lcd_data('-');
			lcd_data(rb + 49);
			Line2(); //2. Zeile
			lcd_string ("R = ");
			if(rv[0]>512) {		//Überprüfen, wie weit die an den Testwiderständen anliegenden Spannungen von 512 abweichen
				hfe[0] = (rv[0] - 512);
			} else {
				hfe[0] = (512 - rv[0]);
			}
			if(rv[1]>512) {
				hfe[1] = (rv[1] - 512);
			} else {
				hfe[1] = (512 - rv[1]);
			}
			if(hfe[0] > hfe[1])  {
				radcmax[0] = radcmax[1];
				rv[0] = rv[1];	//Ergebnis verwenden, welches näher an 512 liegt (bessere Genauigkeit)
				rv[1] = R_READ_RH;	//470k-Testwiderstand	
			} else {
				rv[1] = R_L_VAL;	//680R-Testwiderstand
			}
			if(rv[0]==0) rv[0] = 1;
			lhfe = (unsigned long)((unsigned long)((unsigned long)rv[1] * (unsigned long)rv[0]) / (unsigned long)((unsigned long)radcmax[0] - (unsigned long)rv[0]));	//Widerstand berechnen
			ultoa(lhfe,outval,10);

			if(rv[1]==R_READ_RH) {	//470k-Widerstand?
				ra = strlen(outval);	//Nötig, um Komma anzuzeigen
				for(rb=0;rb<ra;rb++) {
					lcd_data(outval[rb]);
					if(rb==(ra-2)) lcd_data('.');	//Komma
				}
				lcd_data ('k'); //Kilo-Ohm, falls 470k-Widerstand verwendet
			} else {
				lcd_string(outval);
			}
			lcd_data(LCD_CHAR_OMEGA);	//Omega für Ohm 
			goto end;

		} else if(PartFound == PART_CAPACITOR) {	//Kapazitätsmessung auch nur auf Mega8 verfügbar
			lcd_eep_string(Capacitor);
			lcd_data(ca + 49);	//Pin-Angaben
			lcd_data('-');
			lcd_data(cb + 49);
			Line2(); //2. Zeile
			tmpval2 = 'n';
			if(cv > 99999) {	//ab 1µF
				cv /= 1000;
				tmpval2 = LCD_CHAR_U;
			}
			ultoa(cv, outval, 10);
			tmpval = strlen(outval);
			lcd_show_format_cap(outval, tmpval, tmpval);
			lcd_data(tmpval2);
			lcd_data('F');
			goto end;
	#endif
	}
	#ifdef UseM8	//Unterscheidung, ob Dioden gefunden wurden oder nicht nur auf Mega8
		if(NumOfDiodes == 0) {
			//Keine Dioden gefunden
			lcd_eep_string(TestFailed1); //"Kein,unbek. oder"
			Line2(); //2. Zeile
			lcd_eep_string(TestFailed2); //"defektes "
			lcd_eep_string(Bauteil);
		} else {
			lcd_eep_string(Bauteil);
			lcd_eep_string(Unknown); //" unbek."
			Line2(); //2. Zeile
			lcd_eep_string(OrBroken); //"oder defekt"
			lcd_data(NumOfDiodes + 48);
			lcd_data(LCD_CHAR_DIODE);
		}
	#else	//auf Mega48 keine Anzeige der evtl. gefundenen Dioden
		lcd_eep_string(TestFailed1); //"Kein,unbek. oder"
		Line2(); //2. Zeile
		lcd_eep_string(TestFailed2); //"defektes "
		lcd_eep_string(Bauteil);
	#endif
	end:
	while(!(ON_PIN_REG & (1<<RST_PIN)));		//warten ,bis Taster losgelassen
	_delay_ms(200);
	for(hfe[0] = 0;hfe[0]<10000;hfe[0]++) {
		if(!(ON_PIN_REG & (1<<RST_PIN))) {
			/*Wenn der Taster wieder gedrückt wurde...
			wieder zum Anfang springen und neuen Test durchführen
			*/
			goto start;
		}
		wdt_reset();
		_delay_ms(1);
	}
	ON_PORT &= ~(1<<ON_PIN);	//Abschalten
	wdt_disable();	//Watchdog aus
	//Endlosschleife
	while(1) {
		if(!(ON_PIN_REG & (1<<RST_PIN))) {	
			/* wird nur erreicht,
		 	wenn die automatische Abschaltung nicht eingebaut wurde */
			goto start;
		}
	}
	return 0;
}

void CheckPins(uint8_t HighPin, uint8_t LowPin, uint8_t TristatePin) {
	/*
	Funktion zum Testen der Eigenschaften des Bauteils bei der angegebenen Pin-Belegung
	Parameter:
	HighPin: Pin, der anfangs auf positives Potenzial gelegt wird
	LowPin: Pin, der anfangs auf negatives Potenzial gelegt wird
	TristatePin: Pin, der anfangs offen gelassen wird

	Im Testverlauf wird TristatePin natürlich auch positiv oder negativ geschaltet.
	*/
	unsigned int adcv[6];
	uint8_t tmpval, tmpval2;
	/*
		HighPin wird fest auf Vcc gelegt
		LowPin wird über R_L auf GND gelegt
		TristatePin wird hochohmig geschaltet, dafür ist keine Aktion nötig
	*/
	wdt_reset();
	//Pins setzen
	tmpval = (LowPin * 2);			//nötig wegen der Anordnung der Widerstände
	R_DDR = (1<<tmpval);			//Low-Pin auf Ausgang und über R_L auf Masse
	R_PORT = 0;
	ADC_DDR = (1<<HighPin);			//High-Pin auf Ausgang
	ADC_PORT = (1<<HighPin);		//High-Pin fest auf Vcc
	_delay_ms(5);
	//Bei manchen MOSFETs muss das Gate (TristatePin) zuerst entladen werden
	//N-Kanal:
	DischargePin(TristatePin,0);
	//Spannung am Low-Pin ermitteln
	adcv[0] = ReadADC(LowPin);
	if(adcv[0] < 20) goto next;	//Sperrt das Bauteil jetzt?
	//sonst: Entladen für P-Kanal (Gate auf Plus)
	DischargePin(TristatePin,1);
	//Spannung am Low-Pin ermitteln
	adcv[0] = ReadADC(LowPin);

	next:
	if(adcv[0] < 20) {	//Wenn das Bauteil keinen Durchgang zwischen HighPin und LowPin hat
		tmpval2 = (TristatePin * 2);		//nötig wegen der Anordnung der Widerstände
		R_DDR |= (1<<tmpval2);			//Tristate-Pin über R_L auf Masse, zum Test auf pnp
		_delay_ms(2);
		adcv[0] = ReadADC(LowPin);		//Spannung messen
		if(adcv[0] > 700) {
			//Bauteil leitet => pnp-Transistor o.ä.
			//Verstärkungsfaktor in beide Richtungen messen
			R_DDR = (1<<tmpval);		//Tristate-Pin (Basis) hochohmig
			tmpval2++;
			R_DDR |= (1<<tmpval2);		//Tristate-Pin (Basis) über R_H auf Masse

			_delay_ms(10);
			adcv[0] = ReadADC(LowPin);		//Spannung am Low-Pin (vermuteter Kollektor) messen
			adcv[2] = ReadADC(TristatePin);	//Basisspannung messen
			R_DDR = (1<<tmpval);		//Tristate-Pin (Basis) hochohmig
			//Prüfen, ob Test schon mal gelaufen
			if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) PartReady = 1;
			hfe[PartReady] = adcv[0];
			uBE[PartReady] = adcv[2];

			if(adcv[2] > 200) {
				if(PartFound != PART_THYRISTOR) {
					PartFound = PART_TRANSISTOR;	//PNP-Transistor gefunden (Basis wird "nach oben" gezogen)
					PartMode = PART_MODE_PNP;
				}
			} else {
				if(PartFound != PART_THYRISTOR) {
				 	PartFound = PART_FET;			//P-Kanal-MOSFET gefunden (Basis/Gate wird NICHT "nach oben" gezogen)
					PartMode = PART_MODE_P_E_MOS;
				}
			}
			if(PartFound != PART_THYRISTOR) {
				b = TristatePin;
				c = LowPin;
				e = HighPin;
			}
		}

		//Tristate (vermutete Basis) auf Plus, zum Test auf npn
		ADC_PORT = 0;					//Low-Pin fest auf Masse
		tmpval = (TristatePin * 2);		//nötig wegen der Anordnung der Widerstände
		tmpval2 = (HighPin * 2);		//nötig wegen der Anordnung der Widerstände
		R_DDR = (1<<tmpval) | (1<<tmpval2);			//High-Pin und Tristate-Pin auf Ausgang
		R_PORT = (1<<tmpval) | (1<<tmpval2);		//High-Pin und Tristate-Pin über R_L auf Vcc
		ADC_DDR = (1<<LowPin);			//Low-Pin auf Ausgang
		_delay_ms(10);
		adcv[0] = ReadADC(HighPin);		//Spannung am High-Pin messen
		if(adcv[0] < 500) {
			if(PartReady==1) goto testend;
			//Bauteil leitet => npn-Transistor o.ä.

			//Test auf Thyristor:
			//Gate entladen
			
			R_PORT = (1<<tmpval2);			//Tristate-Pin (Gate) über R_L auf Masse
			_delay_ms(10);
			R_DDR = (1<<tmpval2);			//Tristate-Pin (Gate) hochohmig
			//Test auf Thyristor
			_delay_ms(5);
			adcv[1] = ReadADC(HighPin);		//Spannung am High-Pin (vermutete Anode) erneut messen
			
			R_PORT = 0;						//High-Pin (vermutete Anode) auf Masse
			_delay_ms(5);
			R_PORT = (1<<tmpval2);			//High-Pin (vermutete Anode) wieder auf Plus
			_delay_ms(5);
			adcv[2] = ReadADC(HighPin);		//Spannung am High-Pin (vermutete Anode) erneut messen
			if((adcv[1] < 500) && (adcv[2] > 900)) {	//Nach Abschalten des Haltestroms muss der Thyristor sperren
				//war vor Abschaltung des Triggerstroms geschaltet und ist immer noch geschaltet obwohl Gate aus => Thyristor
				PartFound = PART_THYRISTOR;
				//Test auf Triac
				R_DDR = 0;
				R_PORT = 0;
				ADC_PORT = (1<<LowPin);	//Low-Pin fest auf Plus
				_delay_ms(5);
				R_DDR = (1<<tmpval2);	//HighPin über R_L auf Masse
				_delay_ms(5);
				if(ReadADC(HighPin) > 50) goto savenresult;	//Spannung am High-Pin (vermuteter A2) messen; falls zu hoch: Bauteil leitet jetzt => kein Triac
				R_DDR |= (1<<tmpval);	//Gate auch über R_L auf Masse => Triac müsste zünden
				_delay_ms(5);
				if(ReadADC(TristatePin) < 200) goto savenresult; //Spannung am Tristate-Pin (vermutetes Gate) messen; Abbruch falls Spannung zu gering
				if(ReadADC(HighPin) < 150) goto savenresult; //Bauteil leitet jetzt nicht => kein Triac => Abbruch
				R_DDR = (1<<tmpval2);	//TristatePin (Gate) wieder hochohmig
				_delay_ms(5);
				if(ReadADC(HighPin) < 150) goto savenresult; //Bauteil leitet nach Abschalten des Gatestroms nicht mehr=> kein Triac => Abbruch
				R_PORT = (1<<tmpval2);	//HighPin über R_L auf Plus => Haltestrom aus
				_delay_ms(5);
				R_PORT = 0;				//HighPin wieder über R_L auf Masse; Triac müsste jetzt sperren
				_delay_ms(5);
				if(ReadADC(HighPin) > 50) goto savenresult;	//Spannung am High-Pin (vermuteter A2) messen; falls zu hoch: Bauteil leitet jetzt => kein Triac
				PartFound = PART_TRIAC;
				PartReady = 1;
				goto savenresult;
			}
			//Test auf Transistor oder MOSFET
			tmpval++;
			R_DDR |= (1<<tmpval);		//Tristate-Pin (Basis) auf Ausgang
			R_PORT |= (1<<tmpval);		//Tristate-Pin (Basis) über R_H auf Plus
			_delay_ms(50);
			adcv[0] = ReadADC(HighPin);		//Spannung am High-Pin (vermuteter Kollektor) messen
			adcv[2] = ReadADC(TristatePin);	//Basisspannung messen
			R_PORT = (1<<tmpval2);			//Tristate-Pin (Basis) hochohmig
			R_DDR = (1<<tmpval2);			//Tristate-Pin (Basis) auf Eingang

			if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) PartReady = 1;	//prüfen, ob Test schon mal gelaufen
			hfe[PartReady] = 1023 - adcv[0];
			uBE[PartReady] = 1023 - adcv[2];
			if(adcv[2] < 500) {
				PartFound = PART_TRANSISTOR;	//NPN-Transistor gefunden (Basis wird "nach unten" gezogen)
				PartMode = PART_MODE_NPN;
			} else {
				PartFound = PART_FET;			//N-Kanal-MOSFET gefunden (Basis/Gate wird NICHT "nach unten" gezogen)
				PartMode = PART_MODE_N_E_MOS;
			}
			savenresult:
			b = TristatePin;
			c = HighPin;
			e = LowPin;
		}
		ADC_DDR = 0;
		ADC_PORT = 0;
		//Fertig
	} else {	//Durchgang
		//Test auf N-JFET oder selbstleitenden N-MOSFET
		R_DDR |= (2<<(TristatePin*2));	//Tristate-Pin (vermutetes Gate) über R_H auf Masse
		_delay_ms(20);
		adcv[0] = ReadADC(LowPin);		//Spannung am vermuteten Source messen
		R_PORT |= (2<<(TristatePin*2));	//Tristate-Pin (vermutetes Gate) über R_H auf Plus
		_delay_ms(20);
		adcv[1] = ReadADC(LowPin);		//Spannung am vermuteten Source erneut messen
		//Wenn es sich um einen selbstleitenden MOSFET oder JFET handelt, müsste adcv[1] > adcv[0] sein
		if(adcv[1]>(adcv[0]+100)) {
			//Spannung am Gate messen, zur Unterscheidung zwischen MOSFET und JFET
			ADC_PORT = 0;
			ADC_DDR = (1<<LowPin);	//Low-Pin fest auf Masse
			tmpval = (HighPin * 2);		//nötig wegen der Anordnung der Widerstände
			R_DDR |= (1<<tmpval);			//High-Pin auf Ausgang
			R_PORT |= (1<<tmpval);			//High-Pin über R_L auf Vcc
			_delay_ms(20);
			adcv[2] = ReadADC(TristatePin);		//Spannung am vermuteten Gate messen
			if(adcv[2]>800) {	//MOSFET
				PartFound = PART_FET;			//N-Kanal-MOSFET
				PartMode = PART_MODE_N_D_MOS;	//Verarmungs-MOSFET
			} else {	//JFET (pn-Übergang zwischen G und S leitet)
				PartFound = PART_FET;			//N-Kanal-JFET
				PartMode = PART_MODE_N_JFET;
			}
			PartReady = 1;
			b = TristatePin;
			c = HighPin;
			e = LowPin;
		}
		ADC_PORT = 0;

		//Test auf P-JFET oder selbstleitenden P-MOSFET
		ADC_DDR = (1<<LowPin);	//Low-Pin (vermuteter Drain) fest auf Masse, Tristate-Pin (vermutetes Gate) ist noch über R_H auf Plus
		tmpval = (HighPin * 2);			//nötig wegen der Anordnung der Widerstände
		R_DDR |= (1<<tmpval);			//High-Pin auf Ausgang
		R_PORT |= (1<<tmpval);			//High-Pin über R_L auf Vcc
		_delay_ms(20);
		adcv[0] = ReadADC(HighPin);		//Spannung am vermuteten Source messen
		R_PORT = (1<<tmpval);			//Tristate-Pin (vermutetes Gate) über R_H auf Masse
		_delay_ms(20);
		adcv[1] = ReadADC(HighPin);		//Spannung am vermuteten Source erneut messen
		//Wenn es sich um einen selbstleitenden P-MOSFET oder P-JFET handelt, müsste adcv[0] > adcv[1] sein
		if(adcv[0]>(adcv[1]+100)) {
			//Spannung am Gate messen, zur Unterscheidung zwischen MOSFET und JFET
			ADC_PORT = (1<<HighPin);	//High-Pin fest auf Plus
			ADC_DDR = (1<<HighPin);		//High-Pin auf Ausgang
			_delay_ms(20);
			adcv[2] = ReadADC(TristatePin);		//Spannung am vermuteten Gate messen
			if(adcv[2]<200) {	//MOSFET
				PartFound = PART_FET;			//P-Kanal-MOSFET
				PartMode = PART_MODE_P_D_MOS;	//Verarmungs-MOSFET
			} else {	//JFET (pn-Übergang zwischen G und S leitet)
				PartFound = PART_FET;			//P-Kanal-JFET
				PartMode = PART_MODE_P_JFET;
			}
			PartReady = 1;
			b = TristatePin;
			c = LowPin;
			e = HighPin;
		}

		tmpval2 = (2<<(2*HighPin));	//R_H
		tmpval = (1<<(2*HighPin));	//R_L
		ADC_PORT = 0;
		//Test auf Diode
		ADC_DDR = (1<<LowPin);	//Low-Pin fest auf Masse, High-Pin ist noch über R_L auf Vcc
		DischargePin(TristatePin,1);	//Entladen für P-Kanal-MOSFET
		_delay_ms(5);
		adcv[0] = ReadADC(HighPin) - ReadADC(LowPin);
		R_DDR = tmpval2;	//High-Pin über R_H auf Plus
		R_PORT = tmpval2;
		_delay_ms(5);
		adcv[2] = ReadADC(HighPin) - ReadADC(LowPin);
		R_DDR = tmpval;	//High-Pin über R_L auf Plus
		R_PORT = tmpval;
		DischargePin(TristatePin,0);	//Entladen für N-Kanal-MOSFET
		_delay_ms(5);
		adcv[1] = ReadADC(HighPin) - ReadADC(LowPin);
		R_DDR = tmpval2;	//High-Pin über R_H  auf Plus
		R_PORT = tmpval2;
		_delay_ms(5);
		adcv[3] = ReadADC(HighPin) - ReadADC(LowPin);
		/*Ohne das Entladen kann es zu Falscherkennungen kommen, da das Gate eines MOSFETs noch geladen sein kann.
			Die zusätzliche Messung mit dem "großen" Widerstand R_H wird durchgeführt, um antiparallele Dioden von
			Widerständen unterscheiden zu können.
			Eine Diode hat eine vom Durchlassstrom relativ unabhängige Durchlassspg.
			Bei einem Widerstand ändert sich der Spannungsabfall stark (linear) mit dem Strom.
		*/
		if(adcv[0] > adcv[1]) {
			adcv[1] = adcv[0];	//der höhere Wert gewinnt
			adcv[3] = adcv[2];
		}

		if((adcv[1] > 30) && (adcv[1] < 950)) { //Spannung liegt über 0,15V und unter 4,64V => Ok
			if((PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) PartFound = PART_DIODE;	//Diode nur angeben, wenn noch kein anderes Bauteil gefunden wurde. Sonst gäbe es Probleme bei Transistoren mit Schutzdiode
			diodes[NumOfDiodes].Anode = HighPin;
			diodes[NumOfDiodes].Cathode = LowPin;
			diodes[NumOfDiodes].Voltage = (adcv[1]*54/11);	// ca. mit 4,9 multiplizieren, um aus dem ADC-Wert die Spannung in Millivolt zu erhalten
			NumOfDiodes++;
			for(uint8_t i=0;i<NumOfDiodes;i++) {
				if((diodes[i].Anode == LowPin) && (diodes[i].Cathode == HighPin)) {	//zwei antiparallele Dioden: Defekt oder Duo-LED
					if((adcv[3]*64) < (adcv[1] / 5)) {	//Durchlassspannung fällt bei geringerem Teststrom stark ab => Defekt
						if(i<NumOfDiodes) {
							for(uint8_t j=i;j<(NumOfDiodes-1);j++) {
								diodes[j].Anode = diodes[j+1].Anode;
								diodes[j].Cathode = diodes[j+1].Cathode;
								diodes[j].Voltage = diodes[j+1].Voltage;
							}
						}
						NumOfDiodes -= 2;
					}
				}
			}
		}
	}
	#ifdef UseM8	//Widerstandsmessung nur auf dem Mega8 verfügbar
		//Test auf Widerstand
		tmpval2 = (2<<(2*HighPin));	//R_H
		tmpval = (1<<(2*HighPin));	//R_L
		ADC_PORT = 0;
		ADC_DDR = (1<<LowPin);	//Low-Pin fest auf Masse
		R_DDR = tmpval;	//High-Pin über R_L auf Plus
		R_PORT = tmpval;
		adcv[2] = ReadADC(LowPin);
		adcv[0] = ReadADC(HighPin) - adcv[2];
		R_DDR = tmpval2;	//High-Pin über R_H auf Plus
		R_PORT = tmpval2;
		adcv[3] = ReadADC(LowPin);
		adcv[1] = ReadADC(HighPin) - adcv[3];

		//Messung der Spannungsdifferenz zwischen dem Pluspol von R_L und R_H und Vcc
		tmpval2 = (2<<(2*LowPin));	//R_H
		tmpval = (1<<(2*LowPin));	//R_L
		ADC_DDR = (1<<HighPin);		//High-Pin auf Ausgang
		ADC_PORT = (1<<HighPin);	//High-Pin fest auf Plus
		R_PORT = 0;
		R_DDR = tmpval;				//Low-Pin über R_L auf Masse
		adcv[2] += (1023 - ReadADC(HighPin));
		R_DDR = tmpval2;				//Low-Pin über R_H auf Masse
		adcv[3] += (1023 - ReadADC(HighPin));
		
		if(((adcv[0] - adcv[2]) < 900) && ((adcv[1] - adcv[3]) > 20)) goto testend; 	//Spannung fällt bei geringem Teststrom nicht weit genug ab
		if(((adcv[1] * 32) / 31) < adcv[0]) {	//Abfallende Spannung fällt bei geringerem Teststrom stark ab und es besteht kein "Beinahe-Kurzschluss" => Widerstand
			if((PartFound == PART_DIODE) || (PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) {
				if((tmpPartFound == PART_RESISTOR) && (ra == LowPin) && (rb == HighPin)) {
					/* Das Bauteil wurde schon einmal mit umgekehrter Polarität getestet.
					Jetzt beide Ergebnisse miteinander vergleichen. Wenn sie recht ähnlich sind,
					handelt es sich (höchstwahrscheinlich) um einen Widerstand. */
					if(!((((adcv[0] + 100) * 11) >= ((rv[0] + 100) * 10)) && (((rv[0] + 100) * 11) >= ((adcv[0] + 100) * 10)) && (((adcv[1] + 100) * 11) >= ((rv[1] + 100) * 10)) && (((rv[1] + 100) * 11) >= ((adcv[1] + 100) * 10)))) {
						//min. 10% Abweichung => kein Widerstand
						tmpPartFound = PART_NONE;
						goto testend;
					}
					PartFound = PART_RESISTOR;
				}
				rv[0] = adcv[0];
				rv[1] = adcv[1];

				radcmax[0] = 1023 - adcv[2];	//Spannung am Low-Pin ist nicht ganz Null, sondern rund 0,1V (wird aber gemessen). Der dadurch entstehende Fehler wird hier kompenisert
				radcmax[1] = 1023 - adcv[3];
				ra = HighPin;
				rb = LowPin;
				tmpPartFound = PART_RESISTOR;
			}
		}
	#endif
	testend:
	ADC_DDR = 0;
	ADC_PORT = 0;
	R_DDR = 0;
	R_PORT = 0;
}


void ReadCapacity(uint8_t HighPin, uint8_t LowPin) {
	//Test auf Kondensator (auch nur auf ATMega8 möglich)
	if((HighPin == cb) && (LowPin == ca)) return;	//Test schon mal mit umgekehrter Polung gelaufen
	unsigned long gcval = 0;
	unsigned int tmpint = 0;
	uint8_t extcnt = 0;
	uint8_t tmpx = 0;
	
	tmpval2 = (2<<(2*HighPin));	//R_H
	tmpval = (1<<(2*HighPin));	//R_L
	ADC_PORT = 0;
	R_PORT = 0;
	R_DDR = 0;
	ADC_DDR = (1<<LowPin);	//Low-Pin fest auf Masse
	R_DDR = tmpval2;		//HighPin über R_H auf Masse
	_delay_ms(5);
	adcv[0] = ReadADC(HighPin);
	DischargePin(HighPin,1);
	adcv[2] = ReadADC(HighPin);
	_delay_ms(5);
	adcv[1] = ReadADC(HighPin);
	wdt_reset();
	if((adcv[1] > (adcv[0] + 1)) || (adcv[2] > (adcv[0] + 1))) {	//Spannung ist gestiegen
		R_DDR = tmpval;			//High-Pin über R_L auf Masse
		while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
			wdt_reset();
			tmpint++;
			if(tmpint==0) {
				extcnt++;
				if(extcnt == 30) break; //Timeout für Entladung
			}
		}
		tmpint = 0;
		extcnt = 0;
		R_PORT = tmpval;			//High-Pin über R_L auf Plus
		_delay_ms(5);
		adcv[2] = ReadADC(HighPin);
		_delay_ms(80);
		adcv[3] = ReadADC(HighPin);
		if((adcv[3] < (adcv[2] + 3)) && (adcv[3] < 850)) return;	//Spannung ist nicht nennenswert gestiegen => Abbruch
		if((NumOfDiodes > 0) && (adcv[3] > 950)) return; //höchstwahrscheinlich eine (oder mehrere) Diode(n) in Sperrrichtung, die sonst fälschlicherweise als Kondensator erkannt wird
		R_PORT = 0;
		while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
			wdt_reset();
			tmpint++;
			if(tmpint==0) {
				extcnt++;
				if(extcnt == 30) break; //Timeout für Entladung
			}
		}
		tmpint = 0;
		extcnt = 0;
		ADC_DDR = 7;					//alle Pins auf Ausgang und aus Masse
		R_PORT = tmpval;  	   			// HighPin über R_L auf Plus
		tmpval=(1<<HighPin);
		_delay_ms(2);
		ADC_DDR=(1<<LowPin);          // Kondensator über R_H langsam laden
		while (!(ADC_PIN & tmpval)) {  // Warten, bis HighPin auf High geht; Schleife dauert 7 Zyklen
			wdt_reset();
			tmpint++;
			if(tmpint==0) {
				extcnt++;
				if(extcnt == 30) break; //Timeout für Ladung
			}
		}
		if((extcnt == 0) && (tmpint<256)) {	//Niedrige Kapazität
			//mit R_H erneut messen
			R_PORT = 0;
			tmpint = 0;
			extcnt = 0;
			while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
				wdt_reset();
				tmpint++;
				if(tmpint==0) {
					extcnt++;
					if(extcnt == 30) break; //Timeout für Entladung
				}
			}
			tmpint = 0;
			extcnt = 0;
			ADC_DDR = 7;					//alle Pins auf Ausgang
			ADC_PORT = 0;					//alle Pins fest auf Masse
			R_DDR = tmpval2;        		// HighPin über R_H auf Ausgang
			R_PORT = tmpval2;  	   			// HighPin über R_H auf Plus
			_delay_ms(2);
			ADC_DDR=(1<<LowPin);          // Kondensator über R_H langsam laden
			while (!(ADC_PIN & tmpval)) {  // Warten, bis HighPin auf High geht; Schleife dauert 7 Zyklen
				wdt_reset();
				tmpint++;
				if(tmpint==0) {
					extcnt++;
					if(extcnt == 30) break; //Timeout für Kapazitätsmessung
				}
			}
			tmpx = 1;
		}
		if(tmpx) {
			gcval = H_CAPACITY_FACTOR;
			if((extcnt == 0) && (tmpint < 5)) goto end;	//Kapazität zu gering
			cv = 1;
		} else {
			gcval = L_CAPACITY_FACTOR;
			cv = 1000;
		}

		gcval *= (unsigned long)(((unsigned long)extcnt * 65536) + (unsigned long)tmpint);	//Wert speichern
		gcval /= 100;
		cv *= gcval;

		PartFound = PART_CAPACITOR;	//Kondensator gefunden

		ca = HighPin;
		cb = LowPin;
		//Kondensator wieder entladen
		tmpint = 0;
		extcnt = 0;
		R_DDR = (1<<(2*HighPin));			//High-Pin über R_L auf Masse
		R_PORT = 0;
		while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
			wdt_reset();
			tmpint++;
			if(tmpint==0) {
				extcnt++;
				if(extcnt == 30) break; //Timeout für Entladung
			}
		}
		ADC_DDR = 7;	//komplett entladen
		ADC_PORT = 7;
		_delay_ms(10);
		//Fertig
	}
	end:
	ADC_DDR = 0;				
	ADC_PORT = 0;
	R_DDR = 0;
	R_PORT = 0; 
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


#ifdef UseM8
void lcd_show_format_cap(char outval[], uint8_t strlength, uint8_t CommaPos) {
	if(strlength < 3) {
		if(strlength==1) {
			lcd_string("0.");
			lcd_data('0');
			lcd_data(outval[0]);
		} else {
			lcd_string("0.");
			lcd_data(outval[0]);
			lcd_data(outval[1]);
		}
	} else {
		for(PartReady=0;PartReady<strlength;PartReady++) {
			if((PartReady + 2) == CommaPos) lcd_data('.');
			lcd_data(outval[PartReady]);
		}
	}
}
#endif

	};
}
