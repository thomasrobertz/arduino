#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Vodafone-B5AA"; // Replace with your WiFi SSID
const char* password = "CJD67cJaGnqbJdub"; // Replace with your WiFi password

const char* serverUrl = "http://192.168.0.89:3000/grow_sensor_data"; // Replace with your server IP and port

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the URL to POST data
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String jsonPayload = "{\"temperature\": 25, \"humidity\": 60}";

    // Send the POST request
    int httpResponseCode = http.POST(jsonPayload);

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response:");
      Serial.println(response);
    } else {
      Serial.print("Error sending POST request: ");
      Serial.println(httpResponseCode);
    }

    // End the HTTP connection
    http.end();
  }

  // Delay between requests (e.g., send every 5 seconds)
  delay(5000);
}
