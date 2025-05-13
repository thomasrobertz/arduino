#ifndef IR_CODES_H
#define IR_CODES_H

#include <map>
#include <Arduino.h>
#include "IRTypes.h"

// Define all IR codes in this map
extern std::map<String, IRCode> irCodes;

void initializeIRCodes();

#endif // IR_CODES_H