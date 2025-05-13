#include "IRCodes.h"

// Global map of IR codes
std::map<String, IRCode> irCodes;

// Initialize all IR codes
void initializeIRCodes() {
  // Screen controls
  irCodes["SCREEN_UP"] = IRCode(0, 0x5FAAAA, 23, true, 38, 1350, 450, 450, 1350, 550, 1250);
  irCodes["SCREEN_DOWN"] = IRCode(0, 0x6FAAAA, 23, true, 38, 1350, 450, 450, 1350, 500, 1300);
  
  // Beamer controls
  irCodes["BEAMER_ON"] = IRCode(1, 0x00FDCD32, 32, true, 38);  // address 0xCD32, command 0xFD
  irCodes["BEAMER_OFF"] = IRCode(1, 0x00D1CD32, 32, true, 38); // address 0xCD32, command 0xD1
  
  // AVR controls - we'll keep these in IRCodes.cpp even though we use hardcoded values in sendIR
  irCodes["AVR_POWER"] = IRCode(2, 0x110051403254, 48, true, 38);
  irCodes["AVR_PC"] = IRCode(2, 0xC2D61403254, 48, true, 38);
  irCodes["AVR_PS5"] = IRCode(2, 0x3C2D51403254, 48, true, 38);
  irCodes["AVR_VOL_UP"] = IRCode(2, 0x561701403254ULL, 48, true, 38);   // Use ULL suffix for 64-bit literal
  irCodes["AVR_VOL_DOWN"] = IRCode(2, 0x461711403254ULL, 48, true, 38); // Use ULL suffix for 64-bit literal
  irCodes["AVR_MUTE"] = IRCode(2, 0x761721403254, 48, true, 38);
  
  // TV controls
  irCodes["TV_POWER"] = IRCode(1, 0x00F57F00, 32, true, 38);  // address 0x7F00, command 0xF5
  irCodes["TV_MUTE"] = IRCode(1, 0x00AF7F00, 32, true, 38);   // address 0x7F00, command 0xAF
  irCodes["TV_VOL_UP"] = IRCode(1, 0x00E17F00, 32, true, 38); // address 0x7F00, command 0xE1
  irCodes["TV_VOL_DOWN"] = IRCode(1, 0x00A07F00, 32, true, 38); // address 0x7F00, command 0xA0
}








