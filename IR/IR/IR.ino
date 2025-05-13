#define IR_RECEIVE_PIN 4
#define IR_SEND_PIN 2

#include <IRremote.hpp>
#include <WiFi.h>
#include <WebServer.h>   // Lightweight HTTP server
#include <HTTPClient.h>
#include <ArduinoJson.h> // For parsing JSON payloads
#include "IRTypes.h"     // IR protocol types and IRCode struct
#include "IRCodes.h"     // IR codes map

const char* ssid = "Vodafone-B5AA"; // Replace with your WiFi SSID
const char* password = "CJD67cJaGnqbJdub"; // Replace with your WiFi password
const char* expressServerIP = "192.168.0.89"; // Replace with your Express server's IP
const int expressServerPort = 3011;

WebServer server(8077);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\n=== IR Blaster Starting ===");
  
  // Initialize IR codes
  initializeIRCodes();
  
  // Initialize IR receiver only - just like in old.ino
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver initialized on pin " + String(IR_RECEIVE_PIN));

  // Test IR sender with a known working command
  Serial.println("Testing IR sender with AVR_VOL_DOWN...");
  delay(1000);
  sendIR("AVR_VOL_DOWN");
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  delay(2000);
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  server.on("/send", HTTP_POST, handleSendCommand);
  server.begin();
  Serial.println("HTTP server started on port 8077");
  
  // Print all available IR codes for debugging
  Serial.println("\nAvailable IR commands:");
  for (const auto& pair : irCodes) {
    Serial.println("- " + pair.first);
  }
}

void sendIR(const String& name) {
  Serial.println("Attempting to send IR command: " + name);
  
  if (!irCodes.count(name)) {
    Serial.println("ERROR: Unknown IR command: " + name);
    return;
  }

  IRCode code = irCodes[name];
  Serial.println("Found IR code with protocol type: " + String(code.protocolType));
  
  // Declare variables outside the switch statement
  uint16_t address;
  uint8_t command;
  
  switch (code.protocolType) {
    case IR_PROTO_PULSE_DISTANCE:
      Serial.println("Sending PulseDistance command with data: 0x" + String((uint32_t)code.data, HEX));
      IrSender.sendPulseDistanceWidth(
        code.khz,
        code.zeroMark, code.zeroSpace,
        code.oneMark, code.oneSpace,
        code.headerMark, code.headerSpace,
        code.data,
        code.nBits,
        code.isLSBFirst,
        0, 0);
      break;

    case IR_PROTO_KASEIKYO_DENON:
      // Extract address and command from data field
      address = code.data & 0xFFFF;         // Lower 16 bits
      command = (code.data >> 16) & 0xFF;   // Next 8 bits
      
      Serial.println("Sending Denon command with address: 0x" + String(address, HEX) + 
                    ", command: 0x" + String(command, HEX));
      
      IrSender.sendKaseikyo_Denon(address, command, 0);
      break;

    case IR_PROTO_NEC:
      // Extract address and command from data field
      address = code.data & 0xFFFF;         // Lower 16 bits
      command = (code.data >> 16) & 0xFF;   // Next 8 bits
      
      Serial.println("Sending NEC command with address: 0x" + String(address, HEX) + 
                    ", command: 0x" + String(command, HEX));
      
      IrSender.sendNEC(address, command, 0);
      break;

    default:
      Serial.println("ERROR: Unsupported protocol type: " + String(code.protocolType));
      break;
  }
  
  Serial.println("IR command sent successfully");
}

void handleSendCommand() {
  Serial.println("Received HTTP request to /send endpoint");
  
  if (server.hasArg("plain") == false) {
    Serial.println("ERROR: Body missing in request");
    server.send(400, "text/plain", "Body missing");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Request body: " + body);

  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, body);

  if (err) {
    Serial.println("ERROR: Failed to parse JSON: " + String(err.c_str()));
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  String command = doc["command"];
  Serial.println("Extracted command: " + command);
  
  if (!irCodes.count(command)) {
    Serial.println("ERROR: Unknown command: " + command);
    server.send(404, "text/plain", "Unknown command: " + command);
    return;
  }

  sendIR(command);
  
  // Send acknowledgment back to client
  server.send(200, "text/plain", "Sent command: " + command);
  Serial.println("Response sent to client");
  
  // Also send ACK to Express server
  sendAckToServer(command);
}

void sendAckToServer(const String& command) {
  Serial.println("Sending ACK to Express server for command: " + command);
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://" + String(expressServerIP) + ":" + String(expressServerPort) + "/ack";
    
    Serial.println("ACK URL: " + serverPath);
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/json");
    
    String jsonPayload = "{\"command\":\"" + command + "\",\"status\":\"sent\"}";
    Serial.println("ACK payload: " + jsonPayload);
    
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("ACK sent successfully. Response code: " + String(httpResponseCode));
    } else {
      Serial.println("ERROR: Failed to send ACK. HTTP response code: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("ERROR: WiFi not connected. Cannot send ACK.");
  }
}

void loop() {
  server.handleClient();
  
  // Uncomment for debugging IR signals
  /*
  if (IrReceiver.decode()) {
    Serial.println("IR signal received:");
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();
  }
  */
}

















