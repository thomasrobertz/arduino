#include "IRCodes.h"

// Global map of IR codes
std::map<String, IRCode> irCodes;

// Initialize all IR codes
void initializeIRCodes() {
  // Screen controls
  irCodes["SCREEN_UP"] = IRCode(0, 0x5FAAAA, 23, true, 38, 1350, 450, 450, 1350, 550, 1250);
  irCodes["SCREEN_DOWN"] = IRCode(0, 0x6FAAAA, 23, true, 38, 1350, 450, 450, 1350, 500, 1300);
  
  // Beamer controls
  irCodes["BEAMER_ON"] = IRCode(1, 0xFD02CD32, 32, true, 38);
  irCodes["BEAMER_OFF"] = IRCode(1, 0xD12ECD32, 32, true, 38);
  
  // AVR controls
  irCodes["AVR_POWER"] = IRCode(2, 0x110051403254, 48, true, 38);
  irCodes["AVR_PC"] = IRCode(2, 0xC2D61403254, 48, true, 38);
  irCodes["AVR_PS5"] = IRCode(2, 0x3C2D51403254, 48, true, 38);
  irCodes["AVR_VOL_UP"] = IRCode(2, 0x561701403254, 48, true, 38);
  irCodes["AVR_VOL_DOWN"] = IRCode(2, 0x00170114, 32, true, 38); // address 0x114, command 0x17
  irCodes["AVR_MUTE"] = IRCode(2, 0x761721403254, 48, true, 38);
  
  // TV controls
  irCodes["TV_POWER"] = IRCode(1, 0xF50A7F00, 32, true, 38);
  irCodes["TV_MUTE"] = IRCode(1, 0xAF507F00, 32, true, 38);
  irCodes["TV_VOL_UP"] = IRCode(1, 0xE11E7F00, 32, true, 38);
  irCodes["TV_VOL_DOWN"] = IRCode(1, 0xA05F7F00, 32, true, 38);
}

