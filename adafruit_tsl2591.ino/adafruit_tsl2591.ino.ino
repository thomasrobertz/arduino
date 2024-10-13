#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

// Create TSL2591 sensor object
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

void configureSensor() {
  // You can change the gain and integration time as needed
  tsl.setGain(TSL2591_GAIN_MED);      // Medium gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // 100ms integration time
}

void setup() {
  Serial.begin(115200);
  
  // Initialize sensor
  if (!tsl.begin()) {
    Serial.println("Could not find a valid TSL2591 sensor, check wiring!");
    while (1);
  }

  Serial.println("TSL2591 Light Sensor Test");
  configureSensor();
}

void loop() {
  // Get raw light readings (visible + IR, IR, and full spectrum)
  uint16_t fullLuminosity = tsl.getFullLuminosity();
  uint16_t ir = fullLuminosity >> 16;
  uint16_t visible = fullLuminosity & 0xFFFF;

  // Display readings
  Serial.print("Visible: ");
  Serial.print(visible);
  Serial.print("\tInfrared: ");
  Serial.print(ir);
  Serial.print("\tFull Spectrum: ");
  Serial.println(fullLuminosity);
  
  // Wait a bit before taking another reading
  delay(1000);
}
