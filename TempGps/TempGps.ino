#include "Arduino.h"
//  GPS
#include "HT_st7735.h"
#include "HT_TinyGPS++.h"
// Humid & Temp
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
// MQTT
#include <WiFi.h>
#include <PubSubClient.h>


// GPS
TinyGPSPlus GPS;
HT_st7735 st7735;
#define VGNSS_CTRL 3
// Humid & Temp
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

String lati = "none";
String longi = "none";

// Humid and Temp
String string_t = "TMP n/a";
String string_h = "HUM n/a";

// MQTT
const char* ssid = "Pixel_3392";
const char* password = "12345678mq";
const char* mqtt_server = "mqtt.eclipseprojects.io";
const int mqtt_port = 1883;
const char* mqtt_topic_dht = "CCL/fishboat";
WiFiClient espClient;
PubSubClient client(espClient);



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
    mqttSendData(string_t, string_h, lati, longi);


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
        // clear stored data in buffer from sensor
        while (Serial1.read() > 0)
          ;


        // read and write gps
        String latitude = "LAT: " + (String)GPS.location.lat();
        st7735.st7735_write_str(0, 40, latitude);
        String longitude = "LON: " + (String)GPS.location.lng();
        st7735.st7735_write_str(0, 60, longitude);

        // delay(5000);
        delay(100);
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

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi!");
}

void reconnectWifi() {
  // reconnectWifi to the MQTT broker if disconnected
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");


    // Generate a random client ID to avoid conflicts
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed. State=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void mqttSendData(String t, String h, String lati, String longi) {
  // Reconnect to WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    reconnectWifi();
  }
  client.loop();
  /* add errror handling
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  */
  // Publish sensor data to MQTT topic
  String payload = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + ", \"Latitude\": " + String(lati) + ", \"Longitude\": " + String(longi) + "}";
  client.publish("CCL/fishboat", payload.c_str());
}


void setup() {
  // init temp sensor
  dht.begin();
  // init display and wait
  st7735.st7735_init();
  delay(100);

  // Set up WiFi
  connectWiFi();
  // Set up the MQTT server
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);

  // begin running code
  GPS_test();
}

void loop() {
  delay(1000);
}
