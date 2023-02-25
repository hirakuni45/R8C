EESchema Schematic File Version 4
LIBS:rc_servo_tester-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L rc_servo_tester-rescue:R8C_M110AN U?
U 1 1 58CF79CD
P 6000 3550
F 0 "U?" H 4450 4700 60  0000 C CNN
F 1 "R8C/M110AN" H 4700 2400 60  0000 C CNN
F 2 "" H 6000 3550 60  0000 C CNN
F 3 "" H 6000 3550 60  0000 C CNN
	1    6000 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 58CF7A14
P 3900 2550
F 0 "R?" V 3980 2550 50  0000 C CNN
F 1 "10K" V 3900 2550 50  0000 C CNN
F 2 "" V 3830 2550 50  0000 C CNN
F 3 "" H 3900 2550 50  0000 C CNN
	1    3900 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 58CF7A95
P 3900 4150
F 0 "R?" V 3980 4150 50  0000 C CNN
F 1 "10K" V 3900 4150 50  0000 C CNN
F 2 "" V 3830 4150 50  0000 C CNN
F 3 "" H 3900 4150 50  0000 C CNN
	1    3900 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 58CF7B9A
P 8250 3850
F 0 "R?" V 8330 3850 50  0000 C CNN
F 1 "5.6K" V 8250 3850 50  0000 C CNN
F 2 "" V 8180 3850 50  0000 C CNN
F 3 "" H 8250 3850 50  0000 C CNN
	1    8250 3850
	0    1    1    0   
$EndComp
$Comp
L rc_servo_tester-rescue:CONN_01X06 P?
U 1 1 58CF7C23
P 1950 3300
F 0 "P?" H 1950 3650 50  0000 C CNN
F 1 "Program" V 2050 3300 50  0000 C CNN
F 2 "" H 1950 3300 50  0000 C CNN
F 3 "" H 1950 3300 50  0000 C CNN
	1    1950 3300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3900 2850 3900 2700
Connection ~ 3900 2850
Wire Wire Line
	3900 2400 3900 2250
$Comp
L power:+5V #PWR?
U 1 1 58CF7DF1
P 3900 2250
F 0 "#PWR?" H 3900 2100 50  0001 C CNN
F 1 "+5V" H 3900 2390 50  0000 C CNN
F 2 "" H 3900 2250 50  0000 C CNN
F 3 "" H 3900 2250 50  0000 C CNN
	1    3900 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4450 3900 4450
Wire Wire Line
	3900 4450 3900 4300
Wire Wire Line
	3500 4450 3500 3250
Wire Wire Line
	3500 3250 2150 3250
Connection ~ 3900 4450
Wire Wire Line
	2150 3350 2250 3350
Wire Wire Line
	2250 3350 2250 3850
$Comp
L power:GND #PWR?
U 1 1 58CF7E6E
P 2250 3850
F 0 "#PWR?" H 2250 3600 50  0001 C CNN
F 1 "GND" H 2250 3700 50  0000 C CNN
F 2 "" H 2250 3850 50  0000 C CNN
F 3 "" H 2250 3850 50  0000 C CNN
	1    2250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 4950 6000 5100
$Comp
L power:GND #PWR?
U 1 1 58CF7EBB
P 6000 5100
F 0 "#PWR?" H 6000 4850 50  0001 C CNN
F 1 "GND" H 6000 4950 50  0000 C CNN
F 2 "" H 6000 5100 50  0000 C CNN
F 3 "" H 6000 5100 50  0000 C CNN
	1    6000 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 2150 6000 1650
$Comp
L power:+5V #PWR?
U 1 1 58CF806C
P 6000 1550
F 0 "#PWR?" H 6000 1400 50  0001 C CNN
F 1 "+5V" H 6000 1690 50  0000 C CNN
F 2 "" H 6000 1550 50  0000 C CNN
F 3 "" H 6000 1550 50  0000 C CNN
	1    6000 1550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 58CF808A
P 6400 1850
F 0 "C?" H 6425 1950 50  0000 L CNN
F 1 "0.1uF" H 6425 1750 50  0000 L CNN
F 2 "" H 6438 1700 50  0000 C CNN
F 3 "" H 6400 1850 50  0000 C CNN
	1    6400 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C?
U 1 1 58CF80C1
P 6800 1850
F 0 "C?" H 6825 1950 50  0000 L CNN
F 1 "10uF" H 6825 1750 50  0000 L CNN
F 2 "" H 6838 1700 50  0000 C CNN
F 3 "" H 6800 1850 50  0000 C CNN
	1    6800 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 1700 6800 1650
Wire Wire Line
	6800 1650 6400 1650
Connection ~ 6000 1650
Wire Wire Line
	6400 1700 6400 1650
Connection ~ 6400 1650
Wire Wire Line
	6400 2000 6400 2100
Wire Wire Line
	6400 2100 6800 2100
Wire Wire Line
	6800 2000 6800 2100
Connection ~ 6800 2100
$Comp
L power:GND #PWR?
U 1 1 58CF816D
P 6800 2200
F 0 "#PWR?" H 6800 1950 50  0001 C CNN
F 1 "GND" H 6800 2050 50  0000 C CNN
F 2 "" H 6800 2200 50  0000 C CNN
F 3 "" H 6800 2200 50  0000 C CNN
	1    6800 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 3150 2450 3150
Wire Wire Line
	2450 3150 2450 2850
Wire Wire Line
	2450 2850 3900 2850
$Comp
L rc_servo_tester-rescue:LED D?
U 1 1 58CF838F
P 2950 1950
F 0 "D?" H 2950 2050 50  0000 C CNN
F 1 "RED-LED(VF:1.9V)" H 2950 1850 50  0000 C CNN
F 2 "" H 2950 1950 50  0000 C CNN
F 3 "" H 2950 1950 50  0000 C CNN
	1    2950 1950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 58CF83E0
P 2950 2400
F 0 "R?" V 3030 2400 50  0000 C CNN
F 1 "660" V 2950 2400 50  0000 C CNN
F 2 "" V 2880 2400 50  0000 C CNN
F 3 "" H 2950 2400 50  0000 C CNN
	1    2950 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 2150 2950 2250
Wire Wire Line
	2950 2550 2950 3050
Wire Wire Line
	2950 3050 3950 3050
Wire Wire Line
	3950 3050 3950 3250
Wire Wire Line
	3950 3250 4100 3250
Wire Wire Line
	2950 1750 2950 1600
$Comp
L power:+5V #PWR?
U 1 1 58CF85B4
P 2950 1600
F 0 "#PWR?" H 2950 1450 50  0001 C CNN
F 1 "+5V" H 2950 1740 50  0000 C CNN
F 2 "" H 2950 1600 50  0000 C CNN
F 3 "" H 2950 1600 50  0000 C CNN
	1    2950 1600
	1    0    0    -1  
$EndComp
$Comp
L rc_servo_tester-rescue:POT RV?
U 1 1 58CF86A0
P 8750 2100
F 0 "RV?" H 8750 2000 50  0000 C CNN
F 1 "10K" H 8750 2100 50  0000 C CNN
F 2 "" H 8750 2100 50  0000 C CNN
F 3 "" H 8750 2100 50  0000 C CNN
	1    8750 2100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8750 1850 8750 1700
Wire Wire Line
	8750 2350 8750 2500
$Comp
L power:+5V #PWR?
U 1 1 58CF87AE
P 8750 1700
F 0 "#PWR?" H 8750 1550 50  0001 C CNN
F 1 "+5V" H 8750 1840 50  0000 C CNN
F 2 "" H 8750 1700 50  0000 C CNN
F 3 "" H 8750 1700 50  0000 C CNN
	1    8750 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 58CF87D6
P 8750 2500
F 0 "#PWR?" H 8750 2250 50  0001 C CNN
F 1 "GND" H 8750 2350 50  0000 C CNN
F 2 "" H 8750 2500 50  0000 C CNN
F 3 "" H 8750 2500 50  0000 C CNN
	1    8750 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 2100 8450 2100
Wire Wire Line
	8450 2100 8450 2850
Wire Wire Line
	8450 2850 7900 2850
$Comp
L rc_servo_tester-rescue:CONN_01X03 P?
U 1 1 58CF891E
P 9850 3150
F 0 "P?" H 9850 3350 50  0000 C CNN
F 1 "Servo" V 9950 3150 50  0000 C CNN
F 2 "" H 9850 3150 50  0000 C CNN
F 3 "" H 9850 3150 50  0000 C CNN
	1    9850 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 3050 9650 3050
Wire Wire Line
	8800 3150 9400 3150
Wire Wire Line
	9650 3250 9400 3250
Wire Wire Line
	9400 3250 9400 3600
$Comp
L power:GND #PWR?
U 1 1 58CF8A11
P 9400 3600
F 0 "#PWR?" H 9400 3350 50  0001 C CNN
F 1 "GND" H 9400 3450 50  0000 C CNN
F 2 "" H 9400 3600 50  0000 C CNN
F 3 "" H 9400 3600 50  0000 C CNN
	1    9400 3600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 58CF8A3B
P 9400 2300
F 0 "#PWR?" H 9400 2150 50  0001 C CNN
F 1 "+5V" H 9400 2440 50  0000 C CNN
F 2 "" H 9400 2300 50  0000 C CNN
F 3 "" H 9400 2300 50  0000 C CNN
	1    9400 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 3650 8600 3650
Wire Wire Line
	8600 3650 8600 3850
Wire Wire Line
	8600 3850 8400 3850
Wire Wire Line
	8100 3850 7900 3850
Wire Wire Line
	7900 3450 8700 3450
Wire Wire Line
	8700 3450 8700 5500
Wire Wire Line
	8700 5500 2800 5500
Wire Wire Line
	2800 5500 2800 3550
Wire Wire Line
	2800 3550 2150 3550
Wire Wire Line
	2150 3450 2900 3450
Wire Wire Line
	2900 3450 2900 5400
Wire Wire Line
	2900 5400 8600 5400
Connection ~ 8600 3850
Text Notes 1350 3050 0    60   ~ 0
+V
Text Notes 1350 3150 0    60   ~ 0
/RESET
Text Notes 1400 3250 0    60   ~ 0
MODE
Text Notes 1400 3350 0    60   ~ 0
GND
Text Notes 1400 3450 0    60   ~ 0
RXD
Text Notes 1400 3550 0    60   ~ 0
TXD
Text Notes 10100 3050 0    60   ~ 0
SIGNAL
Text Notes 10100 3150 0    60   ~ 0
+V
Text Notes 10100 3250 0    60   ~ 0
GND
Wire Wire Line
	3900 4000 3900 3850
$Comp
L power:+5V #PWR?
U 1 1 58CF973D
P 3900 3850
F 0 "#PWR?" H 3900 3700 50  0001 C CNN
F 1 "+5V" H 3900 3990 50  0000 C CNN
F 2 "" H 3900 3850 50  0000 C CNN
F 3 "" H 3900 3850 50  0000 C CNN
	1    3900 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky D?
U 1 1 58CF8FEF
P 9400 2700
F 0 "D?" H 9400 2800 50  0000 C CNN
F 1 "RB751S-40" H 9400 2600 50  0000 C CNN
F 2 "" H 9400 2700 50  0000 C CNN
F 3 "" H 9400 2700 50  0000 C CNN
	1    9400 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 2550 9400 2300
Wire Wire Line
	9400 3150 9400 2850
Wire Wire Line
	8800 3150 8800 5600
Wire Wire Line
	8800 5600 2700 5600
Wire Wire Line
	2700 5600 2700 3050
Wire Wire Line
	2700 3050 2150 3050
Connection ~ 9400 3150
Wire Wire Line
	3900 2850 4100 2850
Wire Wire Line
	3900 4450 4100 4450
Wire Wire Line
	6000 1650 6000 1550
Wire Wire Line
	6400 1650 6000 1650
Wire Wire Line
	6800 2100 6800 2200
Wire Wire Line
	8600 3850 8600 5400
Wire Wire Line
	9400 3150 9650 3150
$EndSCHEMATC
