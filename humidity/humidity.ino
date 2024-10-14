#include "DHT.h"
#include <Wire.h> // Required for I2C communication
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h> // For BMP280 air pressure sensor, assuming you're using this model

#define DHTPIN 3  // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

struct SensorData {
    float temperature;
    float humidity;
    unsigned long timestamp;
};

const int maxReadings = 10;
SensorData readings[maxReadings];
int currentReading = 0;

Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  dht.begin();
  if (!bmp.begin(0x77)) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }
}

void loop() {
  delay(2000);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  float pressure = bmp.readPressure(); // Read the air pressure

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  collectData(temperature, humidity);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C ");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  if (currentReading == maxReadings) {
      for (int i = 0; i < maxReadings; i++) {
          Serial.print("Temp: ");
          Serial.print(readings[i].temperature);
          Serial.print(", Humidity: ");
          Serial.print(readings[i].humidity);
          Serial.print(", Time: ");
          Serial.println(readings[i].timestamp);
      }
  }
}

void collectData(float temperature, float humidity) {
    if (currentReading < maxReadings) {
        readings[currentReading].temperature = temperature;
        readings[currentReading].humidity = humidity;
        readings[currentReading].timestamp = millis();
        currentReading++;
    } else {
        // Array is full, you can handle it or reset for simplicity
        currentReading = 0; // Reset index to start overwriting old data
    }
}