#include <Wire.h>
#include <RTClib.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_timer.h>

RTC_DS3231 rtc;

//TLC Levels
#define highLevel 103 //82 103
#define lowLevel 23 //44 23
#define groundLevel 63 //63
//#define framerate 25

//Pins
#define tlcpin 4
#define twentyfour 35
#define twentyfive 34
#define thirty 33
#define RTCvcc 32
#define LEDR 13
#define LEDG 12
#define LEDB 14

//Framerate
int framerate = 25;

//DEBUG
#define DEBUG false

//conditional debugging
#if DEBUG 
  #define beginDebug()  do {  Serial.begin (115200); } while (0)
  #define DebugLog(x)         Serial.println(x);
#else
  #define beginDebug()       ((void) 0)
  #define DebugLog(x)        ((void) 0)
#endif

//Funktionsvariabeln

volatile unsigned char hourCount = 0;
volatile unsigned char minuteCount = 0;
volatile unsigned char secondCount = 0;
volatile unsigned char frameCount = 0;

volatile unsigned char bitCount = 0;
volatile unsigned char updateCnt = 0;
volatile unsigned char currentBit = 0;

volatile unsigned char lastLevel = 0;
volatile unsigned char polarBit = 0;

hw_timer_t * timer = NULL;
bool kannsenden = false;
bool kannempfangen = true;

//Network
// Datenstruktur für die Übertragungsvariablen
struct Data {
  int hourCountSend = hourCount;
  int minuteCountSend = minuteCount;
  int secondCountSend = secondCount;
  int frameCountSend = frameCount;
};

Data data;

unsigned long previousMillis = 0;
unsigned long previousMicros = 0;
unsigned long previousSecond = 0;
unsigned long rightMicros = 0;
unsigned long adjustMicros = 0;
unsigned long newTimer = 0;
unsigned long secondCounter = 0;
unsigned long secondnow = 0;

unsigned long long adjustedMicros = 0;
unsigned long long rtcSyncInterval = 1000000ULL; // 1 Sekunde
unsigned long long lastMicros = 0;
DateTime lastRtcSyncTime; // Time of last RTC synchronization

//Synchronisierungsinterval
unsigned long interval = 60 * 1000; //In Millisekunden
unsigned long tempinterval = 0; //damit die Uhr sich beim Hochfahren einmal selbst stellt

// The code of function setLevel(), timeUpdate() and timerInterrupt() is licenced and modified under MIT Licence:
// Copyright (c) 2022 lydasia

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

void IRAM_ATTR setLevel(void)
{
  int dutyCycle;
  switch(bitCount)
  {
    case 0:
      polarBit = 0;
      currentBit = (((frameCount % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 1:
      currentBit = (((frameCount % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 2:
      currentBit = (((frameCount % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 3:
      currentBit = (((frameCount % 10) >> (4 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 4:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 5:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 6:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 7:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 8:
      currentBit = (((frameCount / 10 % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;  
    case 9:
      currentBit = (((frameCount / 10 % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;  
    case 10:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 11:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 12:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 13:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 14:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 15:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 16:
      currentBit = (((secondCount % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 17:
      currentBit = (((secondCount % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 18:
      currentBit = (((secondCount % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 19:
      currentBit = (((secondCount % 10) >> (4 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 20:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 21:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 22:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 23:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 24:
      currentBit = (((secondCount /10 % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 25:
      currentBit = (((secondCount /10 % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break; 
    case 26:
      currentBit = (((secondCount /10 % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 27:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 28:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 29:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 30:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 31:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 32:
      currentBit = (((minuteCount % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 33:
      currentBit = (((minuteCount % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 34:
      currentBit = (((minuteCount % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 35:
      currentBit = (((minuteCount % 10) >> (4 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 36:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 37:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 38:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 39:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 40:
      currentBit = (((minuteCount / 10 % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 41:
      currentBit = (((minuteCount / 10 % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 42:
      currentBit = (((minuteCount / 10 % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 43:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 44:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break; 
    case 45:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 46:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 47:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break; 
    case 48:
      currentBit = (((hourCount % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 49:
      currentBit = (((hourCount % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 50:
      currentBit = (((hourCount % 10) >> (3 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 51:
      currentBit = (((hourCount % 10) >> (4 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 52:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 53:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 54:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 55:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 56:
      currentBit = (((hourCount / 10 % 10) >> (1 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 57:
      currentBit = (((hourCount / 10 % 10) >> (2 - 1)) & 1);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break; 
    case 58:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 59:
      currentBit = (polarBit);
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 60:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 61:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 62:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 63:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 64:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 65:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 66:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 67:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 68:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 69:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 70:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 71:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 72:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 73:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 74:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 75:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 76:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 77:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 78:
      currentBit = 0;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    case 79:
      currentBit = 1;
      dutyCycle = (lastLevel = (updateCnt?(currentBit?(!lastLevel):(lastLevel)):(!lastLevel))?(1):(0))?(highLevel):(lowLevel);
      ledcWrite(0, dutyCycle);
      if((updateCnt)&(!currentBit)){polarBit=!polarBit;};
      break;
    default:
      ledcWrite(0, groundLevel);
  }
}

// Zeit anpassen / hochzählen
void IRAM_ATTR timeUpdate(void)
{
  if(bitCount < 79)
    {
      bitCount ++;
    }
    else
    {
      bitCount = 0;
      if(frameCount < framerate-1)
      {
        frameCount ++;
      }
      else
      {
        frameCount = 0;
        if(secondCount < 59)
        {
          secondCount ++;
        }
        else
        {
          secondCount = 0;
          if(minuteCount < 59)
          {
            minuteCount ++;
          }
          else
          {
            minuteCount = 0;
            if(hourCount < 23)
            {
              hourCount ++;
            }
            else
            {
              hourCount = 0;
            }
          }
        }
      }      
    }
    
}


void IRAM_ATTR timerInterrupt()
{
  setLevel();

  //1/2 Bit Counter
  if(updateCnt == 0)
  {
    updateCnt ++;
  }
  else
  {
    updateCnt = 0;
    timeUpdate();
  }
}

//Timecode wird empfangen
void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  /*Serial.printf("Empfangen von: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);*/
  DebugLog("Daten erfolgreich erhalten");

  //Timecode empfangen
  hourCount = data.hourCountSend;
  minuteCount = data.minuteCountSend;
  secondCount = data.secondCountSend;
  frameCount = data.frameCountSend;

  //RTC nach dem empfangenen Timecode synchronisieren
  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  rtc.adjust(DateTime(year, month, day, hourCount, minuteCount, secondCount));
  DebugLog("E Timecode: " + String(data.hourCountSend) + ":" + String(data.minuteCountSend) + ":" + String(data.secondCountSend) + ":" + String(data.frameCountSend));

  //LED Grün
  LEDdisplay(0,255,0);

  //Ab jetzt empfängt der ESP32 bis er nichts mehr empfängt, dann sendet er
  kannsenden = false;
  kannempfangen = true;
}

void setup()
{
  beginDebug();

  //Power On RTC
  pinMode(RTCvcc, OUTPUT);
  digitalWrite(RTCvcc, HIGH);

  //WIFI Broadcast Setup
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    DebugLog("ESP-NOW Initialisierung fehlgeschlagen");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    DebugLog("Peer hinzufügen fehlgeschlagen");
    return;
  }
  
  // RTC Setup
  if (! rtc.begin()) {
    DebugLog("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    DebugLog("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DebugLog("RTC adjusted!");
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  //Zeit erstmalig setzen
  DateTime now = rtc.now();
  hourCount = now.hour();
  minuteCount = now.minute();
  secondCount = now.second();
  data.hourCountSend = hourCount;
  data.minuteCountSend = minuteCount; 
  data.secondCountSend = secondCount;
  data.frameCountSend = frameCount;
  DebugLog(String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
  
  //Set Output
  pinMode(tlcpin, OUTPUT);
  pinMode(twentyfour, INPUT);
  pinMode(twentyfive, INPUT);
  pinMode(thirty, INPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // 50% PWM Ground Level
  ledcSetup(0, 1000000/framerate , 8);
  ledcAttachPin(tlcpin, 0);
  ledcWrite(0, groundLevel);

  // LED Setup
  ledcSetup(1, 5000 , 8);
  ledcAttachPin(LEDR, 1);
  ledcSetup(2, 5000 , 8);
  ledcAttachPin(LEDG, 2);
  ledcSetup(3, 5000 , 8);
  ledcAttachPin(LEDB, 3);
  
  //LED Rot
  LEDdisplay(255,0,0);

  lastRtcSyncTime = rtc.now();
  lastMicros = micros();

  

  /*while(1)
  {
  }*/
}

void loop()
{
  if (digitalRead(twentyfour) == HIGH) {
    framerate = 24;
  } else if (digitalRead(twentyfive) == HIGH) {
    framerate = 25;
  } else if (digitalRead(thirty) == HIGH) {
    framerate = 30;
  }

  //unsigned long long currentTime = getAdjustedMicros();
  //DebugLog(String(currentTime) + ' ' + String(micros()));
  
  //Wann startet neue Sekunde
  unsigned long currentMicros = getAdjustedMicros();
  if (currentMicros - previousMicros >= 1000000) {
    previousMicros = currentMicros;
    //DateTime now = rtc.now();
    newTimer = currentMicros;
    //Korrigiere die Frequenz
    ledcChangeFrequency(0, 1000000/framerate + 1/framerate, 8); //1000000/framerate
    secondCounter = 0;
  }

  //Synchronisieren
  unsigned long currentMillis = millis();
  char str[20];
  if (currentMillis - previousMillis >= tempinterval) {
    previousMillis = currentMillis;
    if (kannsenden && !kannempfangen) {
      //Timecode senden
      uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      
      DateTime now = rtc.now();
      hourCount = now.hour();
      minuteCount = now.minute();
      secondCount = now.second();

      data.hourCountSend = hourCount;
      data.minuteCountSend = minuteCount; 
      data.secondCountSend = secondCount;
      data.frameCountSend = frameCount;

      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
      if (result == ESP_OK) {
        DebugLog("Daten erfolgreich übertragen");
      } else {
        DebugLog("Übertragung fehlgeschlagen");
      }

      //LED Blau
      LEDdisplay(0,0,255);

      DebugLog("S Timecode: " + String(data.hourCountSend) + ":" + String(data.minuteCountSend) + ":" + String(data.secondCountSend) + ":" + String(data.frameCountSend));
    }

    
    kannempfangen = false;
    kannsenden = true;
    tempinterval = interval;
  }

  //Frame Timer
  if (getAdjustedMicros() >= newTimer) {
      newTimer += 1000000 / framerate / 80 / 2 + 0; //1000000 / framerate / 80 / 2
      // Bit Timer
      if (secondCounter < framerate * 80 * 2) {
        timerInterrupt();
      }
      secondCounter++;
  }

  //DebugLog(String(getAdjustedMicros()) + ' ' + String(micros()));
}

void LEDdisplay(int R, int G, int B) {
  R = abs(255-R);
  G = abs(255-G);
  B = abs(255-B);
  ledcWrite(1, R);
  ledcWrite(2, G);
  ledcWrite(3, B);
}

unsigned long long getAdjustedMicros() {
  unsigned long long currentMicros = micros();
  unsigned long long elapsedMicros = currentMicros - lastMicros;
  adjustedMicros += elapsedMicros;
  lastMicros = currentMicros;
  
  // Check if it's time to synchronize with RTC
  if (elapsedMicros >= rtcSyncInterval) {
    unsigned long long rtcElapsedMicros = (rtc.now() - lastRtcSyncTime).totalseconds() * 1000000ULL;
    adjustedMicros += rtcElapsedMicros;
    lastRtcSyncTime = rtc.now();
  }
  
  return adjustedMicros;
}