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
  server.on("/test", HTTP_GET, handleTestMode); // Add a test endpoint
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

  // Special case for AVR_VOL_DOWN and AVR_VOL_UP
  if (name == "AVR_VOL_DOWN") {
    Serial.println("Using hardcoded values for AVR_VOL_DOWN");
    Serial.println("Sending Denon command with address: 0x114, command: 0x17");
    IrSender.sendKaseikyo_Denon(0x114, 0x17, 0);
    Serial.println("IR command sent successfully");
    return;
  }
  
  if (name == "AVR_VOL_UP") {
    Serial.println("Using hardcoded values for AVR_VOL_UP");
    Serial.println("Sending Denon command with address: 0x14, command: 0x17");
    IrSender.sendKaseikyo_Denon(0x14, 0x17, 0);
    Serial.println("IR command sent successfully");
    return;
  }

  // Special case for AVR_POWER - use the exact signal from the remote
  if (name == "AVR_POWER") {
    Serial.println("Using exact signal from remote for AVR_POWER");
    
    // Use the Kaseikyo_Denon protocol with the exact values from the remote
    uint16_t address = 0x514;
    uint8_t command = 0x0;
    
    Serial.println("Sending Kaseikyo_Denon command with address: 0x" + String(address, HEX) + 
                  ", command: 0x" + String(command, HEX));
    
    IrSender.sendKaseikyo_Denon(address, command, 0);
    
    Serial.println("IR command sent");
    return;
  }
  
  // Special case for AVR_PC
  if (name == "AVR_PC") {
    Serial.println("Using hardcoded values for AVR_PC");
    // Try different combinations
    Serial.println("Attempt 1: address: 0x140, command: 0xC2");
    IrSender.sendKaseikyo_Denon(0x140, 0xC2, 0);
    delay(100);
    
    Serial.println("Attempt 2: address: 0x114, command: 0xC2");
    IrSender.sendKaseikyo_Denon(0x114, 0xC2, 0);
    
    Serial.println("IR command attempts completed");
    return;
  }
  
  // Special case for AVR_PS5
  if (name == "AVR_PS5") {
    Serial.println("Using hardcoded values for AVR_PS5");
    // Try different combinations
    Serial.println("Attempt 1: address: 0x140, command: 0x3C");
    IrSender.sendKaseikyo_Denon(0x140, 0x3C, 0);
    delay(100);
    
    Serial.println("Attempt 2: address: 0x114, command: 0x3C");
    IrSender.sendKaseikyo_Denon(0x114, 0x3C, 0);
    
    Serial.println("IR command attempts completed");
    return;
  }
  
  // Special case for AVR_MUTE
  if (name == "AVR_MUTE") {
    Serial.println("Using hardcoded values for AVR_MUTE");
    // Try different combinations
    Serial.println("Attempt 1: address: 0x140, command: 0x76");
    IrSender.sendKaseikyo_Denon(0x140, 0x76, 0);
    delay(100);
    
    Serial.println("Attempt 2: address: 0x114, command: 0x76");
    IrSender.sendKaseikyo_Denon(0x114, 0x76, 0);
    
    Serial.println("IR command attempts completed");
    return;
  }
  
  // Special case for TV_MUTE
  if (name == "TV_MUTE") {
    Serial.println("Using exact signal from remote for TV_MUTE");
    
    // Use the NEC protocol with the exact values from the remote
    uint16_t address = 0x7F00;
    uint8_t command = 0x50;
    
    Serial.println("Sending NEC command with address: 0x" + String(address, HEX) + 
                  ", command: 0x" + String(command, HEX));
    
    IrSender.sendNEC(address, command, 0);
    
    Serial.println("IR command sent");
    return;
  }
  
  // For other commands, use the general approach
  IRCode code = irCodes[name];
  Serial.println("Found IR code with protocol type: " + String(code.protocolType));
  Serial.println("Data: 0x" + String((uint32_t)(code.data & 0xFFFFFFFF), HEX) + String((uint16_t)(code.data >> 32), HEX));
  Serial.println("nBits: " + String(code.nBits));
  
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
      // For 48-bit codes, we need to extract address and command differently
      if (code.nBits == 48) {
        // Extract from the raw data format seen in the debug output
        address = (code.data >> 16) & 0xFFFF;  // Bits 16-31
        command = (code.data >> 24) & 0xFF;    // Bits 24-31 (overlapping with address)
        
        Serial.println("Sending 48-bit Denon command with address: 0x" + String(address, HEX) + 
                      ", command: 0x" + String(command, HEX));
      } else {
        // Standard 32-bit format
        address = code.data & 0xFFFF;         // Lower 16 bits
        command = (code.data >> 16) & 0xFF;   // Next 8 bits
        
        Serial.println("Sending 32-bit Denon command with address: 0x" + String(address, HEX) + 
                      ", command: 0x" + String(command, HEX));
      }
      
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
  
  if (IrReceiver.decode()) {
    Serial.println("IR signal received:");
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();
  }
  
}

void handleTestMode() {
  Serial.println("Entering test mode...");
  Serial.println("Listening for IR signals. Press any IR remote button...");
  
  // Send a response to the client
  server.send(200, "text/plain", "Test mode activated. Check serial monitor for results.");
  
  // Listen for IR signals for 30 seconds
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    if (IrReceiver.decode()) {
      Serial.println("IR signal received:");
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
      
      // Extract the protocol, address, and command
      Serial.println("Protocol: " + String(IrReceiver.decodedIRData.protocol));
      Serial.println("Address: 0x" + String(IrReceiver.decodedIRData.address, HEX));
      Serial.println("Command: 0x" + String(IrReceiver.decodedIRData.command, HEX));
      Serial.println("Raw Data: 0x" + String(IrReceiver.decodedIRData.decodedRawData, HEX));
      
      IrReceiver.resume();
    }
    
    // Handle client requests
    server.handleClient();
  }
  
  Serial.println("Test mode completed.");
}










