#ifndef IR_TYPES_H
#define IR_TYPES_H

#include <Arduino.h>

enum IRProtocolType {
  IR_PROTO_UNKNOWN = 0,
  IR_PROTO_PULSE_DISTANCE,
  IR_PROTO_KASEIKYO_DENON,
  IR_PROTO_NEC
};

struct IRCode {
  uint8_t protocolType;
  uint64_t data;
  uint8_t nBits;
  bool isLSBFirst;
  uint8_t khz;
  uint16_t headerMark;
  uint16_t headerSpace;
  uint16_t oneMark;
  uint16_t oneSpace;
  uint16_t zeroMark;
  uint16_t zeroSpace;

  IRCode() :
    protocolType(0), data(0), nBits(0), isLSBFirst(true), khz(38),
    headerMark(0), headerSpace(0), oneMark(0), oneSpace(0), zeroMark(0), zeroSpace(0) {}

  // Your actual useful constructor
  IRCode(
    uint8_t proto, uint64_t d, uint8_t bits, bool lsb,
    uint8_t f, uint16_t hM = 0, uint16_t hS = 0,
    uint16_t oM = 0, uint16_t oS = 0,
    uint16_t zM = 0, uint16_t zS = 0
  ) :
    protocolType(proto), data(d), nBits(bits), isLSBFirst(lsb), khz(f),
    headerMark(hM), headerSpace(hS),
    oneMark(oM), oneSpace(oS),
    zeroMark(zM), zeroSpace(zS) {}
};

#endif // IR_TYPES_H