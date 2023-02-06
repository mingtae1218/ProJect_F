#if 1
/* 스마트 화분 관리기 */
#include <Wire.h>
#include <Arduino.h>
#include <WiFiNINA.h>
#include <LiquidCrystal_I2C.h>      // LCD 모듈 라이브러리
#include <avr/wdt.h>
#include <Firmata.h>
#include "util/DHT.h"                    // 온습도 센서 모듈 라이브러리
#include "util/SuperLoop.h"
#include "util/Pitches.h"

// 센서 핀 설정
#define relayPin        3                  // 릴레이 모듈 핀 -> 생장 LED 켜기 위함
#define DHTPIN          4                    // 온습도센서 모듈 핀
#define DHTPIN2         5                    // 온습도센서 모듈 핀
#define soilmoisturePin A0                // 토양수분센서 핀
#define cdsPin          A1                   // 조도센서 모듈 핀

#define BUZ_P  8

#define RED  26
#define GREEN  25
#define BLUE  27

#define SYSEX_led_LED_START  0x11
#define SYSEX_led_LED_STOP  0x12
#define SYSEX_LED_ON  0x21
#define SYSEX_LED_OFF  0x22

// 객체 생성
LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD 초기화 (LCD 주소값, x축, y축)
DHT dht(DHTPIN, DHT11);           //온습도 센서 모듈


int MarioUW_note[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,NOTE_AS3, NOTE_AS4, 0, 0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0,0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,NOTE_DS3, NOTE_DS4, 0, 0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4, NOTE_DS4, NOTE_GS3, NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,  NOTE_AS3, NOTE_A3, NOTE_GS3,0, 0, 0
};
int MarioUW_duration[] = {
 12, 12, 12, 12,12, 12, 6,3,
 12, 12, 12, 12, 12, 12, 6, 3,
 12, 12, 12, 12, 12, 12, 6,
 3, 12, 12, 12, 12,
 12, 12, 6, 6, 18, 18, 18,
 6, 6, 6, 6,6, 6,
 18, 18, 18, 18, 18, 18, 10, 10, 10,
 10, 10, 10, 3, 3, 3
};

byte temp[] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte humi[] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B11011,
  B11111,
  B01110
};

byte C[] = {
  B10000,
  B00110,
  B01001,
  B01000,
  B01000,
  B01001,
  B00110,
  B00000
};

byte Qmark[] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B00100,
  B00000,
  B00100
};

byte water[] = {
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

byte good[] = {
  B00000,
  B10001,
  B10001,
  B00000,
  B00000,
  B10001,
  B10001,
  B01110
};

byte wind[] = {
  B00000,
  B10011,
  B11010,
  B00100,
  B01011,
  B11001,
  B00000,
  B00000
};

char str_M[10];
char str_T[10];
char str_H[10];
bool turn =true;
bool ledturn = false;

void Play_MarioUW()
{
    for (int thisNote = 0; thisNote < (sizeof(MarioUW_note)/sizeof(int)); thisNote++) {

    int noteDuration = 1000 / MarioUW_duration[thisNote];//convert duration to time delay
    tone(BUZ_P, MarioUW_note[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.80;
    delay(pauseBetweenNotes);
    noTone(BUZ_P); //stop music on buzzer pin
    }
}

class lcd_opening : public SuperLoop {
private:
   void job() override {


   }
public:
   void begin(uint32_t cycle) override {
      SuperLoop::begin(cycle);
      lcd.init();
      lcd.clear();
      lcd.noBacklight();
      delay(500);
      lcd.backlight();
      delay(500);
      lcd.setCursor(2, 0);
      lcd.print("SMART");
      delay(1000);
      lcd.setCursor(8, 0);
      lcd.print("GARDEN");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("Wo Woong Cooper");
      delay(1000);
      lcd.clear();

      Play_MarioUW();
   }
};

lcd_opening lcd_open;

class Cds : public SuperLoop {
private:
   void job() override {
      int cdsValue = analogRead(cdsPin);
      int cdsValue_per = map(cdsValue, 0, 1023, 100, 0);
      Serial.println(cdsValue);
      Firmata.sendAnalog(cdsPin%A0, cdsValue_per);
        if(cdsValue_per > 94){
          digitalWrite(relayPin, LOW);
          turn = true;
        }
        else if(cdsValue_per <= 94){
          digitalWrite(relayPin, HIGH);
          turn = false;
        }

   }
public:
   void start() {
      setStop(false);
   }

   void stop() {
      setStop(true);
   }

};

Cds cds;

class Moisture : public SuperLoop {
private:
	void job() override {
		int value = analogRead(soilmoisturePin);
		int soilmoisture_per = map(value, 170, 1023, 100, 0);  // 센서 값을 퍼센트로 변경
		Firmata.sendAnalog(soilmoisturePin%A0, soilmoisture_per);
		Serial.println(value);

		  //LCD에 토양 습도값 출력
		  lcd.setCursor(1,0);
		  lcd.print("MOIST:");
		  sprintf(str_M, "%03d", soilmoisture_per);
		  lcd.print(str_M);
		  lcd.setCursor(10,0);
		  lcd.print("%");

		//토양습도 값에 따른 LCD에 이모티콘 띄우기
		if (soilmoisture_per >= 0 && soilmoisture_per < 30) {
			lcd.setCursor(13, 0);
			lcd.write(3);
			lcd.setCursor(14, 0);
			lcd.write(4);
		} else if (soilmoisture_per >= 30 && soilmoisture_per < 70) {
			lcd.setCursor(13, 0);
			lcd.print(" ");
			lcd.setCursor(14, 0);
			lcd.write(5);
		} else if (soilmoisture_per >= 70) {
			lcd.setCursor(13, 0);
			lcd.write(3);
			lcd.setCursor(14, 0);
			lcd.write(6);
		}

   }
public:
   void start() {
      setStop(false);
   }

   void stop() {
      setStop(true);
   }
};

Moisture moi;


class Heatindex: public SuperLoop {
private:
   void job() override {
       unsigned char h_Value = dht.readHumidity();                        // 공기 중 습도 값 측정
       unsigned char t_Value = dht.readTemperature();                     // 공기 중 온도 값 측정

      Firmata.sendDigital(DHTPIN2, h_Value);
      Firmata.sendDigital(DHTPIN, t_Value);

      //LCD에 온도값 출력
      lcd.setCursor(1,1);
      lcd.write(0);
      sprintf(str_T, "%02d", t_Value);
      lcd.setCursor(3,1);
      lcd.print(str_T);
      lcd.write(1);

      //LCD에 습도값 출력
      lcd.setCursor(7,1);
      lcd.write(2);
      sprintf(str_H, "%02d", h_Value);
      lcd.setCursor(9,1);
      lcd.print(str_H);
      lcd.print("%");


   }

public:
   void begin(uint32_t cycle) override {
      SuperLoop::begin(cycle);
   }

   void start() {
      setStop(false);
   }

   void stop() {
      setStop(true);
   }

};

Heatindex heat;

void setup() {
	Serial.begin(115200);
	Serial1.begin(115200);
   Firmata.begin(Serial1);
   lcd_open.begin(500);

   pinMode(relayPin, OUTPUT);
   pinMode(cdsPin, INPUT);
   pinMode(soilmoisturePin, INPUT);
   WiFiDrv::pinMode(RED, OUTPUT);
   WiFiDrv::pinMode(GREEN, OUTPUT);
   WiFiDrv::pinMode(BLUE, OUTPUT);

   dht.begin();
   heat.begin(500);
   moi.begin(500);
   cds.begin(500);

   lcd.createChar(0, temp);
   lcd.createChar(1, C);
   lcd.createChar(2, humi);
   lcd.createChar(3, Qmark);
   lcd.createChar(4, water);
   lcd.createChar(5, good);
   lcd.createChar(6, wind);

	Firmata.attach(SYSTEM_RESET, []() {
		wdt_enable (WDTO_1S);
	});

   Firmata.attach(START_SYSEX, [](uint8_t command, uint8_t argc, uint8_t *argv){
      switch (command) {
         case SYSEX_led_LED_STOP:
        	 cds.stop();
        	 ledturn = true;
            break;
         case SYSEX_led_LED_START:
        	 cds.start();
        	 ledturn = false;
            break;
         case SYSEX_LED_ON:
        	 if(ledturn == true) {
        		 digitalWrite(relayPin, HIGH);
        	 }
            break;
         case SYSEX_LED_OFF:
        	 if(ledturn == true) {
        		 digitalWrite(relayPin, LOW);
        	 }
            break;
         default:
            break;
      }
   });

   Firmata.attach(ANALOG_MESSAGE, [](uint8_t pin, int value){
      switch (pin) {
         case RED%A0:
		 	 WiFiDrv::analogWrite(RED, value);
            break;
         case GREEN%A0:
		 	 WiFiDrv::analogWrite(GREEN, value);
            break;
         case BLUE%A0:
		 	 WiFiDrv::analogWrite(BLUE, value);
            break;
         default:
            break;
      }
   });

   Firmata.attach(REPORT_ANALOG, [](uint8_t pin, int tf){
      switch (pin) {
         case cdsPin%A0:
            tf? cds.start() : cds.stop();
            break;
         case soilmoisturePin%A0:
            tf? moi.start() : moi.stop();
            break;
         default:
            break;
      }
   });

   Firmata.attach(REPORT_DIGITAL, [](uint8_t pin, int tf){
      switch (pin) {
         case DHTPIN%A0:
            tf? heat.start() : heat.stop();
            break;
         case DHTPIN2%A0:
            tf? heat.start() : heat.stop();
            break;
         default:
            break;
      }
   });

}

void loop() {
   lcd_open.loop();
   heat.loop();
   moi.loop();
   cds.loop();
   while(Firmata.available())
      Firmata.processInput();
}


#endif
