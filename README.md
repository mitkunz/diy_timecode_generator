# diy_timecode_generator
SMPTE standard-compliant DIY LTC timecode generator, that supports framerates of 24, 25, or 30 fps (NDF).

## Features
•	SMPTE standard-compliant LTC timecode generator

•	Supports framerates of 24, 25, or 30 fps (NDF)

•	Capability to synchronize multiple generators through WIFI broadcasting

•	Constructed with cost-effective components

•	Time accuracy with a drift of no more than 1 frames per 8h

•	Integrated temperature-compensated crystal oscillator (TCXO)

•	Equipped with BNC and 3.5mm audio jack connectivity options

## Hardware
•	ESP32 (eg. Adafruit Feather, Dealikee ESP-32S or similar)

	Important note: Make sure that the board has a LiPo Controller (and connector) and runs on 3V3.
    Tested with Dealike ESP-32S.


 
•	3,7 V 2000mAh 1S Lithium Battery (for ESP32)

•	DS3231 Breakout Board (eg. Wishiot DS-3231 RTC, AZDelivery DS3231 RTC or similar)

	Important note: Also make sure it runs on 3V3 (they usually do).
    Tested with Wishiot DS3231 RTC.
 
•	Perfboard 5x9cm (16x30 perforations)

•	3,5mm Stereo Audio Jack for Perfboard

•	BNC Jack for Perfboard

•	3 Position Switch for Perfboard (Type: 2P3T)

•	2 Position Switch for Perfboard (Type: SPDT)

•	RGB LED

	Make sure it has the following specs:
	red = 400 mcd
	green = 1500 mcd
	blue = 700 mcd
  
•	3x 220Ω Resistors

•	Copper Wire & Solder

•	Jumpercables / Cables


## Required Boards / Libraries
•	ESP32 Boards (https://github.com/espressif/arduino-esp32)

•	Wire

•	RTClib (https://github.com/adafruit/RTClib)

•	esp_now 

•	WiFi

•	esp_timer (https://playground.arduino.cc/Code/Timer1/)

### Modified Code
This project uses code sections from this project: (https://github.com/lydasia/openltc), which are marked by comments.

## Pin Definitions
•	tlcpin : Pin for Timecode Output (default: 4 )

•	twentyfour : Switch Input for 24fps (default: 35 )

•	twentyfive : Swicth Input for 25fps (default: 34 )

•	thirty : Switch Input for 30fps (default: 33 )

• RTCvcc : Power Pin for the RTC (default: 32 )

•	LEDR : Output Pin for LED Red Channel (default: 13 )

•	LEDG : Output Pin for LED Green Channel (default: 12 )

•	LEDB : Output Pin for LED Blue Channel (default: 14 )

## Circuit Board Layout

![Timecode-Generator-Platine double](https://github.com/mitkunz/diy_timecode_generator/assets/143692878/513d358a-6c0e-43d6-ad08-cae10a0f1842)

	
    I've made a little change to this layout by connecting the VCC of the RTC to Pin 32 instead of going straight to 3v3. 
    This allows for a disabled RTC whenever the device is connected to a lipo battery and not turned on. 
    Pin 32 gets HIGH when the ESP32 is booted. Otherwise the RTC would stay on and drain the battery faster.

I cannot guarantee that this code will work for everyone and with different components than the ones I use.
Use this code at your own risk.

## Housing and final build
I will post my design for the housing box for 3d printing as soon as they are finished.
