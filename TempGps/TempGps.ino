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


void GPS_test(void) {
  pinMode(VGNSS_CTRL, OUTPUT);
  digitalWrite(VGNSS_CTRL, HIGH);
  Serial1.begin(115200, SERIAL_8N1, 33, 34);  // Temp used 9600
  Serial.println("GPS_test");
  st7735.st7735_fill_screen(ST7735_BLACK);
  delay(100);
  st7735.st7735_write_str(0, 0, (String) "Starting...");

  while (1) {
    if (Serial1.available() > 0) {
      if (Serial1.peek() != '\n') {
        GPS.encode(Serial1.read());
      } else {
        Serial1.read();
        if (GPS.time.second() % 10 == 0) {
          continue;
        }

        // temp
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        /* TODO output error string on display
        if (isnan(h) || isnan(t) || isnan(f)) {
          Serial.println(F("Failed to read from DHT sensor!"));
          return;
        }
        */

        // output data on display
        st7735.st7735_fill_screen(ST7735_BLACK);
        String temp_c = "Temp " + (String)t;
        st7735.st7735_write_str(0, 0, temp_c);
        String humid = "Hum " + (String)h;
        st7735.st7735_write_str(0, 20, humid);
        String latitude = "LAT: " + (String)GPS.location.lat();
        st7735.st7735_write_str(0, 40, latitude);
        String longitude = "LON: " + (String)GPS.location.lng();
        st7735.st7735_write_str(0, 60, longitude);


        delay(5000);
        while (Serial1.read() > 0)
          ;
      }
    }
  }
}

void setup() {
  // init temp sensor
  dht.begin();
  // run everything else
  delay(100);
  st7735.st7735_init();
  GPS_test();
}

void loop() {
  delay(100);
}
