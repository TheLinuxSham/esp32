#include "Arduino.h"
//  GPS
#include "HT_st7735.h"
#include "HT_TinyGPS++.h"
// Humid & Temp
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

// GPS
TinyGPSPlus GPS;
HT_st7735 st7735;
#define VGNSS_CTRL 3
// Humid & Temp
#define DHTPIN 4  // GPIO 4 for DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

String string_t = "TMP n/a";
String string_h = "HUM n/a";



void GPS_test(void) {
  pinMode(VGNSS_CTRL, OUTPUT);
  digitalWrite(VGNSS_CTRL, HIGH);
  Serial1.begin(115200, SERIAL_8N1, 33, 34);
  Serial.println("GPS_test");
  st7735.st7735_fill_screen(ST7735_BLACK);
  delay(100);
  st7735.st7735_write_str(0, 0, (String) "Starting...");


  while (1) {
    sensorTempAndHumid();
    st7735.st7735_write_str(0, 40, "Await Geo Sen.");

    if (Serial1.available() > 0) {
      if (Serial1.peek() != '\n') {
        GPS.encode(Serial1.read());
      } else {
        Serial1.read();
        if (GPS.time.second() == 0 || GPS.time.second() == 30) {
          continue;
        }

        // set base color of display
        st7735.st7735_fill_screen(ST7735_BLACK);

        // section for display
        // st7735.st7735_write_str(leftalign, topalign, argument);

        // read and write gps
        String latitude = "LAT: " + (String)GPS.location.lat();
        st7735.st7735_write_str(0, 40, latitude);
        String longitude = "LON: " + (String)GPS.location.lng();
        st7735.st7735_write_str(0, 60, longitude);

        // delay(5000);
        delay(100);
        // clear stored data in buffer from sensor
        while (Serial1.read() > 0)
          ;
      }
    }
  }
}

void sensorTempAndHumid() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // read and write temperature if available
  if (!isnan(t) || (String)t != string_t) {
    string_t = "TMP " + (String)t;
  };
  st7735.st7735_write_str(0, 0, string_t);

  // read and write humidity if available
  if (!isnan(h) || (String)h != string_h) {
    string_h = "HUM " + (String)h;
  };
  st7735.st7735_write_str(0, 20, string_h);
}

void setup() {
  // init temp sensor
  dht.begin();
  // init display and wait
  st7735.st7735_init();
  delay(100);
  // begin running code
  GPS_test();
}

void loop() {
  delay(1000);
}
