#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Replace with your WiFi credentials
//const char* ssid = "Dropitlikeitshotspot";
//const char* password = "fopx8054";

const char* ssid = "VodafoneMobileWiFi-82F409";
const char* password = "416383635";

// MQTT Broker details
const char* mqtt_server = "mqtt.eclipseprojects.io";
const int mqtt_port = 1883;
const char* mqtt_topic_dht = "CCL/fishboat";

// DHT sensor setup
#define DHTPIN 4  // GPIO where DHT22 is connected
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Create MQTT and WiFi clients
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  // Connect to WiFi
  connectWiFi();

  // Set up the MQTT server
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);
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

void reconnect() {
  // Reconnect to the MQTT broker if disconnected
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

void loop() {
  // Reconnect to WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // add lat and lon

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Publish sensor data to MQTT topic
  String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  client.publish("home/room/dht_sensor", payload.c_str());
  
  Serial.println("Data sent to MQTT: " + payload);
  delay(10000);  // Send data every 5 seconds
}
