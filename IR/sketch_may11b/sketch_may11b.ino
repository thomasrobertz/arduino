#define IR_RECEIVE_PIN 4
#define IR_SEND_PIN 2

#include <map>
#include <IRremote.hpp>
#include <WiFi.h>
#include <WebServer.h>   // Lightweight HTTP server
#include <HTTPClient.h>
#include <ArduinoJson.h> // For parsing JSON payloads

const char* ssid = "Vodafone-B5AA"; // Replace with your WiFi SSID
const char* password = "CJD67cJaGnqbJdub"; // Replace with your WiFi password
const char* expressServerIP = "192.168.0.89"; // Replace with your Express server's IP
const int expressServerPort = 3011;

WebServer server(8077);

enum IRProtocolType {
  IR_PROTO_UNKNOWN = 0,
  IR_PROTO_PULSE_DISTANCE,
  IR_PROTO_KASEIKYO_DENON,
  IR_PROTO_NEC
};

struct IRCode {
  IRProtocolType protocol;
  uint64_t data;        // Raw or packed (e.g. Address/Command combined)
  uint16_t address;     // For NEC/Kaseikyo
  uint8_t command;      // For NEC/Kaseikyo
  uint8_t nBits;
  bool isLSBFirst;

  // Only for PulseDistance
  uint8_t khz;
  uint16_t headerMark, headerSpace;
  uint16_t oneMark, oneSpace;
  uint16_t zeroMark, zeroSpace;
};

std::map<String, IRCode> irCodes = {
  { "SCREEN_LOWER", {
      .protocol = IR_PROTO_PULSE_DISTANCE,
      .data = 0x5FAAAA,
      .nBits = 23,
      .isLSBFirst = true,
      .khz = 38,
      .headerMark = 1350, .headerSpace = 450,
      .oneMark = 450, .oneSpace = 1350,
      .zeroMark = 500, .zeroSpace = 1300
    }
  },
  { "AVR_VOL_DOWN", {
      .protocol = IR_PROTO_KASEIKYO_DENON,
      .address = 0x114,
      .command = 0x17
    }
  },
  { "NEC_DEVICE_1", {
      .protocol = IR_PROTO_NEC,
      .address = 0x32,
      .command = 0x2E
    }
  },
  { "NEC_DEVICE_2", {
      .protocol = IR_PROTO_NEC,
      .address = 0x7F00,
      .command = 0x50
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(2000);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver ready");

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

/*
  Serial.println("Sending Kaseikyo_Denon VOL DOWN (0x114, 0x17) 10 times...");

  for (int i = 0; i < 20; i++) {
    IrSender.sendKaseikyo_Denon(0x114, 0x17, 0);
    Serial.print("Sent ");
    Serial.println(i + 1);
    delay(1000);  // 1 second delay
  }

  Serial.println("Done.");
  */
}

void sendIR(String key) {
  if (!irCodes.count(key)) {
    Serial.println("Code not found.");
    return;
  }

  IRCode code = irCodes[key];

  switch (code.protocol) {
    case IR_PROTO_PULSE_DISTANCE:
      IrSender.sendPulseDistanceWidth(
        code.khz,
        code.zeroMark, code.zeroSpace,
        code.oneMark, code.oneSpace,
        code.headerMark, code.headerSpace,
        code.data,
        code.nBits,
        code.isLSBFirst ? PROTOCOL_IS_LSB_FIRST : PROTOCOL_IS_MSB_FIRST,
        0, 0);
      break;

    case IR_PROTO_KASEIKYO_DENON:
      IrSender.sendKaseikyo_Denon(code.address, code.command, 0);
      break;

    case IR_PROTO_NEC:
      IrSender.sendNEC(code.address, code.command, 0);
      break;

    default:
      Serial.println("Unsupported protocol.");
  }

  Serial.print("Sent: ");
  Serial.println(key);
  sendAckToServer(key);
}

void handleSendCommand() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }

  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));

  if (err) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  String command = doc["command"];
  if (!irCodes.count(command)) {
    server.send(404, "text/plain", "Unknown command: " + command);
    return;
  }

  sendIR(command);
  server.send(200, "text/plain", "Sent command: " + command);
}

void sendAckToServer(const String& command) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://" + String(expressServerIP) + ":" + String(expressServerPort) + "/ack";
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/json");
    String jsonPayload = "{\"command\":\"" + command + "\",\"status\":\"sent\"}";
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.print("ACK sent. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending ACK. HTTP response code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send ACK.");
  }
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    IrReceiver.resume();
  }
  server.handleClient();
}
