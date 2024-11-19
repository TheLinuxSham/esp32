#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

// DHT22 settings
#define DHTPIN 4  // GPIO 4 for DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
 
  // Initialize the DHT22 sensor
  Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop() {
  // === DHT22 Sensor Readings ===
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Print humidity and temperatures
  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F(" °C, "));
  Serial.print(F("Temperature: "));
  Serial.print(f);
  Serial.println(F(" °F"));

  // Wait 10 seconds before the next reading
  delay(10000);
}
