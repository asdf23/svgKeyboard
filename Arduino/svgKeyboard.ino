#include <Arduino.h>
#include <Bounce2.h>
//old
#include <Encoder.h>
#include <Mouse.h>
#include <SD.h>
#include "SDConfigFile.h"
#include <cstring>

#define DEBOUNCE_ROTARY_SWITCH 10
#define DEBOUNCE_SWITCH 10
#define CONFIG_FILE "config.txt"

enum OutputDeviceType {
  KEYBOARD,
  MOUSE_CLICK,
  MOUSE_MOVE,
  NOOP
};

struct SwitchInfo {
  const char* keyName;
  int pin;
  int debounce;
  OutputDeviceType deviceType;
  char* character;
  int xPos;
  int yPos;
  int wheelDelta;
  uint8_t mouseButton;
  Bounce bounce;
  SwitchInfo(const char* kName, int p, int d, OutputDeviceType dt, const char* c, int x, int y, int w, uint8_t m)
    : keyName(kName), pin(p), debounce(d), deviceType(dt), character(nullptr), xPos(x), yPos(y), wheelDelta(w), mouseButton(m), bounce() {
    bounce.attach(p);
    bounce.interval(d);
    character = strdup(c);
  }
  ~SwitchInfo() {
    if (character != nullptr) {
      free(character);
    }
  }
  void setCharacter(const char* newChar) {
    if (character != nullptr) {
      free(character);  // Free old memory
    }
    character = strdup(newChar);  // Allocate new memory and copy the value
  }
};

struct EncoderInfo {
  
  int pin1;
  int pin2;

  const char* keyNameLeft;
  OutputDeviceType deviceTypeLeft;
  char* characterLeft;
  int xPosLeft;
  int yPosLeft;
  int wheelDeltaLeft;
  uint8_t mouseButtonLeft;

  const char* keyNameRight;
  OutputDeviceType deviceTypeRight;
  char* characterRight;
  int xPosRight;
  int yPosRight;
  int wheelDeltaRight;
  uint8_t mouseButtonRight;
  
  //old
  Encoder encoder;
  long oldValue;  
  // //new
  //int oldValue;
  int oldDelta;
  EncoderInfo(uint8_t p1, uint8_t p2, 
            
            const char* kNameL, 
            OutputDeviceType dtL, 
            const char* cL, 
            int xL, 
            int yL, 
            int wL, 
            uint8_t mL, 

            const char* kNameR, 
            OutputDeviceType dtR, 
            const char* cR, 
            int xR, 
            int yR, 
            int wR, 
            uint8_t mR) : pin1(p1), pin2(p2), 

              keyNameLeft(kNameL), deviceTypeLeft(dtL), characterLeft(nullptr), xPosLeft(xL), yPosLeft(yL), wheelDeltaLeft(wL), mouseButtonLeft(mL), 
              keyNameRight(kNameR), deviceTypeRight(dtR), characterRight(nullptr),xPosRight(xR), yPosRight(yR), wheelDeltaRight(wR), mouseButtonRight(mR),
              //old
              encoder(p1,p2),
              oldValue(-999), //was -999
              oldDelta(-999) { //was -999
    characterLeft = strdup(cL);
    characterRight = strdup(cR);
  }
  ~EncoderInfo() {
    if (characterLeft != nullptr) {
      free(characterLeft);
    }
    if (characterRight != nullptr) {
      free(characterRight);
    }
  }
  void setCharacterLeft(const char* newChar) {
    if (characterLeft != nullptr) {
      free(characterLeft);  // Free old memory
    }
    characterLeft = strdup(newChar);  // Allocate new memory and copy the value
  }
  void setCharacterRight(const char* newChar) {
    if (characterRight != nullptr) {
      free(characterRight);
    }
    characterRight = strdup(newChar);
  }
};

/*
    S1     E1           [s1inc]            [e1sv w]
    S2     E2           [s2dec]            [e2vb w]
    E3  E4     E5  E6   [e3svg y] [e4vb y]           [e5vb h]  [e6sv h]
           E7                              [e7vb x]  
           E8  S3  S4                      [e8sv x]  [s3prev]  [s4next]

E1  16,17   18
E2  41,14   15
E3  36,37   35
E4  38,39   40
E5  27,28   29
E6  30,31   32
E7  24,25   26
E8  8,9     10
SW1 33  
SW2 34  
SW3 11  
SW4 12  

*/

const int NUM_SWITCHES = 12;
SwitchInfo switches[NUM_SWITCHES] = {
  SwitchInfo("SWITCH_INCREASE",              33, DEBOUNCE_SWITCH,           KEYBOARD, "Y", 0, 0, 0, 0), //S1
  SwitchInfo("SWITCH_DECREASE",              34, DEBOUNCE_SWITCH,           KEYBOARD, "Z", 0, 0, 0, 0), //S2
  SwitchInfo("SWITCH_PREVIOUS",              11, DEBOUNCE_SWITCH,           KEYBOARD, "1", 0, 0, 0, 0), //S3
  SwitchInfo("SWITCH_NEXT",                  12, DEBOUNCE_SWITCH,           KEYBOARD, "2", 0, 0, 0, 0), //S4
  SwitchInfo("ROTARY_SVG_WIDTH_CLICK",       18, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "C", 0, 0, 0, 0), //E1
  SwitchInfo("ROTARY_VIEWBOX_WIDTH_CLICK",   15, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "F", 0, 0, 0, 0), //E2
  SwitchInfo("ROTARY_SVG_Y_CLICK",           35, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "I", 0, 0, 0, 0), //E3
  SwitchInfo("ROTARY_VIEWBOX_Y_CLICK",       40, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "L", 0, 0, 0, 0), //E4
  SwitchInfo("ROTARY_VIEWBOX_HEIGHT_CLICK",  29, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "O", 0, 0, 0, 0), //E5
  SwitchInfo("ROTARY_SVG_HEIGHT_CLICK",      32, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "R", 0, 0, 0, 0), //E6
  SwitchInfo("ROTARY_VIEWBOX_X_CLICK",       26, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "U", 0, 0, 0, 0), //E7
  SwitchInfo("ROTARY_SVG_X_CLICK",           10, DEBOUNCE_ROTARY_SWITCH,    KEYBOARD, "X", 0, 0, 0, 0)  //E8
};

const int NUM_ENCODERS = 8;
EncoderInfo encoders[NUM_ENCODERS] = {
    EncoderInfo(16, 17, "ROTARY_SVG_WIDTH_LEFT",           KEYBOARD, "A", 0, 0, 0, 0,
                        "ROTARY_SVG_WIDTH_RIGHT",          KEYBOARD, "B", 0, 0, 0, 0), //E1
    EncoderInfo(41, 14, "ROTARY_VIEWBOX_WIDTH_LEFT",       KEYBOARD, "D", 0, 0, 0, 0,
                        "ROTARY_VIEWBOX_WIDTH_RIGHT",      KEYBOARD, "E", 0, 0, 0, 0), //E2
    EncoderInfo(36, 37, "ROTARY_SVG_Y_LEFT",               KEYBOARD, "G", 0, 0, 0, 0,
                        "ROTARY_SVG_Y_RIGHT",              KEYBOARD, "H", 0, 0, 0, 0), //E3
    EncoderInfo(38, 39, "ROTARY_VIEWBOX_Y_LEFT",           KEYBOARD, "J", 0, 0, 0, 0,
                        "ROTARY_VIEWBOX_Y_RIGHT",          KEYBOARD, "K", 0, 0, 0, 0), //E4
    EncoderInfo(27, 28, "ROTARY_VIEWBOX_HEIGHT_LEFT",      KEYBOARD, "M", 0, 0, 0, 0,
                        "ROTARY_VIEWBOX_HEIGHT_RIGHT",     KEYBOARD, "N", 0, 0, 0, 0), //E5
    EncoderInfo(30, 31, "ROTARY_SVG_HEIGHT_LEFT",          KEYBOARD, "P", 0, 0, 0, 0,
                        "ROTARY_SVG_HEIGHT_RIGHT",         KEYBOARD, "Q", 0, 0, 0, 0), //E6
    EncoderInfo(24, 25, "ROTARY_VIEWBOX_X_LEFT",           KEYBOARD, "S", 0, 0, 0, 0,
                        "ROTARY_VIEWBOX_X_RIGHT",          KEYBOARD, "T", 0, 0, 0, 0), //E7
    EncoderInfo(8, 9,   "ROTARY_SVG_X_LEFT",               KEYBOARD, "V", 0, 0, 0, 0,
                        "ROTARY_SVG_X_RIGHT",              KEYBOARD, "W", 0, 0, 0, 0), //E8
};

struct SpecialCharacterValue {
  char* key;
  uint8_t value;
  SpecialCharacterValue(const char* k, uint8_t v)
    : key(nullptr), value(v) {
      key = strdup(k);
    }
  ~SpecialCharacterValue() {
    if(key != nullptr) {
      free(key);
    }
  }
};
const int NUM_SPECIAL_KEYBOARD_CHARACTERS = 57;
SpecialCharacterValue specialKeyboardCharacters[NUM_SPECIAL_KEYBOARD_CHARACTERS] = {
  SpecialCharacterValue("KEY_LEFT_CTRL}", 0x80),
  SpecialCharacterValue("{KEY_LEFT_SHIFT}", 0x81),
  SpecialCharacterValue("{KEY_LEFT_ALT}", 0x82),
  SpecialCharacterValue("{KEY_LEFT_GUI}", 0x83),
  SpecialCharacterValue("{KEY_RIGHT_CTRL}", 0x84),
  SpecialCharacterValue("{KEY_RIGHT_SHIFT}", 0x85),
  SpecialCharacterValue("{KEY_RIGHT_ALT}", 0x86),
  SpecialCharacterValue("{KEY_RIGHT_GUI}", 0x87),
  SpecialCharacterValue("{KEY_UP_ARROW}", 0xDA),
  SpecialCharacterValue("{KEY_DOWN_ARROW}", 0xD9),
  SpecialCharacterValue("{KEY_LEFT_ARROW}", 0xD8),
  SpecialCharacterValue("{KEY_RIGHT_ARROW}", 0xD7),
  SpecialCharacterValue("{KEY_BACKSPACE}", 0xB2),
  SpecialCharacterValue("{KEY_TAB}", 0xB3),
  SpecialCharacterValue("{KEY_RETURN}", 0xB0),
  SpecialCharacterValue("{KEY_ESC}", 0xB1),
  SpecialCharacterValue("{KEY_INSERT}", 0xD1),
  SpecialCharacterValue("{KEY_DELETE}", 0xD4),
  SpecialCharacterValue("{KEY_PAGE_UP}", 0xD3),
  SpecialCharacterValue("{KEY_PAGE_DOWN}", 0xD6),
  SpecialCharacterValue("{KEY_HOME}", 0xD2),
  SpecialCharacterValue("{KEY_END}", 0xD5),
  SpecialCharacterValue("{KEY_CAPS_LOCK}", 0xC1),
  SpecialCharacterValue("{KEY_F1}", 0xC2),
  SpecialCharacterValue("{KEY_F2}", 0xC3),
  SpecialCharacterValue("{KEY_F3}", 0xC4),
  SpecialCharacterValue("{KEY_F4}", 0xC5),
  SpecialCharacterValue("{KEY_F5}", 0xC6),
  SpecialCharacterValue("{KEY_F6}", 0xC7),
  SpecialCharacterValue("{KEY_F7}", 0xC8),
  SpecialCharacterValue("{KEY_F8}", 0xC9),
  SpecialCharacterValue("{KEY_F9}", 0xCA),
  SpecialCharacterValue("{KEY_F10}", 0xCB),
  SpecialCharacterValue("{KEY_F11}", 0xCC),
  SpecialCharacterValue("{KEY_F12}", 0xCD),
  SpecialCharacterValue("{KEY_PRINT_SCREEN}", 0xCE),
  SpecialCharacterValue("{KEY_SCROLL_LOCK}", 0xCF),
  SpecialCharacterValue("{KEY_PAUSE}", 0xD0),
  SpecialCharacterValue("{KEY_NUM_LOCK}", 0xDB),
  SpecialCharacterValue("{KEYPAD_DIVIDE}", 0xDC),
  SpecialCharacterValue("{KEYPAD_MULTIPLY}", 0xDD),
  SpecialCharacterValue("{KEYPAD_SUBTRACT}", 0xDE),
  SpecialCharacterValue("{KEYPAD_ADD}", 0xDF),
  SpecialCharacterValue("{KEYPAD_ENTER}", 0xE0),
  SpecialCharacterValue("{KEYPAD_1}", 0xE1),
  SpecialCharacterValue("{KEYPAD_2}", 0xE2),
  SpecialCharacterValue("{KEYPAD_3}", 0xE3),
  SpecialCharacterValue("{KEYPAD_4}", 0xE4),
  SpecialCharacterValue("{KEYPAD_5}", 0xE5),
  SpecialCharacterValue("{KEYPAD_6}", 0xE6),
  SpecialCharacterValue("{KEYPAD_7}", 0xE7),
  SpecialCharacterValue("{KEYPAD_8}", 0xE8),
  SpecialCharacterValue("{KEYPAD_9}", 0xE9),
  SpecialCharacterValue("{KEYPAD_0}", 0xEA),
  SpecialCharacterValue("{KEYPAD_DECIMAL}", 0xEB),
  SpecialCharacterValue("{KEY_APPLICATION}", 0xED),
  SpecialCharacterValue("{KEY_MENU}", 0xED),
};
const int NUM_SPECIAL_MOUSE_CHARACTERS = 3;
SpecialCharacterValue specialMouseCharacters[NUM_SPECIAL_MOUSE_CHARACTERS] = {
  SpecialCharacterValue("{MOUSE_LEFT}", MOUSE_LEFT),
  SpecialCharacterValue("{MOUSE_RIGHT}", MOUSE_RIGHT),
  SpecialCharacterValue("{MOUSE_MIDDLE}", MOUSE_MIDDLE)
};

struct SpecialMouseMoveValue {
  char* key;
  int xDelta;
  int yDelta;
  int wDelta;
  SpecialMouseMoveValue(const char* k, int x, int y, int w)
    : key(nullptr), xDelta(x), yDelta(y), wDelta(w) {
      key = strdup(k);
    }
  ~SpecialMouseMoveValue() {
    if(key != nullptr) {
      free(key);
    }
  }
};
const int NUM_SPECIAL_MOUSE_MOVE_CHARACTERS = 6;
SpecialMouseMoveValue specialMouseMoves[NUM_SPECIAL_MOUSE_MOVE_CHARACTERS] = {
  SpecialMouseMoveValue("{MOUSE_MOVE_X_DECREMENT}", -1,0,0),
  SpecialMouseMoveValue("{MOUSE_MOVE_X_INCREMENT}", 1,0,0),
  SpecialMouseMoveValue("{MOUSE_MOVE_Y_DECREMENT}", 0,-1,0),
  SpecialMouseMoveValue("{MOUSE_MOVE_Y_INCREMENT}", 0,1,0),
  SpecialMouseMoveValue("{MOUSE_WHEEL_UP}", 0,0,-1),
  SpecialMouseMoveValue("{MOUSE_WHEEL_DOWN}", 0,0,1)
};

boolean readConfiguration() {
  const uint8_t CONFIG_LINE_LENGTH = 127;
  SDConfigFile cfg;
  if (!cfg.begin(CONFIG_FILE, CONFIG_LINE_LENGTH)) {
    Serial.print("Failed to open configuration file: ");
    Serial.println(CONFIG_FILE);
    return false;
  }

  while (cfg.readNextSetting()) {
    const char* cfgName = cfg.getName();
    boolean foundConfig = false;
    for (int i = 0; i < NUM_SWITCHES && !foundConfig; i++) {
      if (strcmp(cfgName, switches[i].keyName) == 0) {
        foundConfig = true;
        boolean foundEscape = false;
        const char* value = cfg.getValue();

        if( strcmp(value, "") == 0 ) {
          switches[i].deviceType = NOOP;
          foundEscape = true;
        }

        if(! foundEscape) {
          for (int j = 0; j < NUM_SPECIAL_KEYBOARD_CHARACTERS && !foundEscape; j++) {
            if (strcmp(specialKeyboardCharacters[j].key, value) == 0) {
              char valueStr[4];
              sprintf(valueStr, "%u", specialKeyboardCharacters[j].value);
              switches[i].deviceType = KEYBOARD;
              switches[i].setCharacter(valueStr);
              foundEscape = true;
              break;
            }
          }
        }
        
        if(! foundEscape) {
          for(int k = 0; k < NUM_SPECIAL_MOUSE_CHARACTERS && !foundEscape; k++) {
            if(strcmp(specialMouseCharacters[k].key, value) == 0) {
              switches[i].deviceType = MOUSE_CLICK;
              switches[i].mouseButton = specialMouseCharacters[k].value;
              foundEscape = true;
            }
          }
        }
        if(! foundEscape) {
          for(int m = 0; m < NUM_SPECIAL_MOUSE_MOVE_CHARACTERS && !foundEscape; m++) {
            if(strcmp(specialMouseMoves[m].key, value) == 0) {
              switches[i].deviceType = MOUSE_MOVE;
              switches[i].xPos = specialMouseMoves[m].xDelta;
              switches[i].yPos = specialMouseMoves[m].yDelta;
              switches[i].wheelDelta = specialMouseMoves[m].wDelta;
              foundEscape = true;
            }
          }
        }
        if(! foundEscape) {
          switches[i].deviceType = KEYBOARD;
          switches[i].setCharacter(value);
        }
      } //if cfgName==switches[i].keyName
    } //for each switches switches[i]
    if(!foundConfig) {
      for (int i = 0; i < NUM_ENCODERS && !foundConfig; i++) {
        if(strcmp(cfgName, encoders[i].keyNameLeft) == 0) {
          foundConfig = true;
          boolean foundEscape = false;
          const char* value = cfg.getValue();

          if( strcmp(value, "") == 0 ) {
            encoders[i].deviceTypeLeft = NOOP;
            foundEscape = true;
          }

          if(! foundEscape) {
            for (int j = 0; j < NUM_SPECIAL_KEYBOARD_CHARACTERS && !foundEscape; j++) {
              if (strcmp(specialKeyboardCharacters[j].key, value) == 0) {
                char valueStr[4];
                sprintf(valueStr, "%u", specialKeyboardCharacters[j].value);
                encoders[i].deviceTypeLeft = KEYBOARD;
                encoders[i].setCharacterLeft(valueStr);
                foundEscape = true;
                break;
              }
            }
          }
          
          if(! foundEscape) {
            for(int k = 0; k < NUM_SPECIAL_MOUSE_CHARACTERS && !foundEscape; k++) {
              if(strcmp(specialMouseCharacters[k].key, value) == 0) {
                encoders[i].deviceTypeLeft = MOUSE_CLICK;
                encoders[i].mouseButtonLeft = specialMouseCharacters[k].value;
                foundEscape = true;
              }
            }
          }
          if(! foundEscape) {
            for(int m = 0; m < NUM_SPECIAL_MOUSE_MOVE_CHARACTERS && !foundEscape; m++) {
              if(strcmp(specialMouseMoves[m].key, value) == 0) {
                encoders[i].deviceTypeLeft = MOUSE_MOVE;
                encoders[i].xPosLeft = specialMouseMoves[m].xDelta;
                encoders[i].yPosLeft = specialMouseMoves[m].yDelta;
                encoders[i].wheelDeltaLeft = specialMouseMoves[m].wDelta;
                foundEscape = true;
              }
            }
          }
          if(! foundEscape) {
            encoders[i].deviceTypeLeft = KEYBOARD;
            encoders[i].setCharacterLeft(value);
          }
        }
      } //for each encoders encoders[i]
    }
    if(!foundConfig) {
      for (int i = 0; i < NUM_ENCODERS && !foundConfig; i++) {
        if(strcmp(cfgName, encoders[i].keyNameRight) == 0) {
          foundConfig = true;
          boolean foundEscape = false;
          const char* value = cfg.getValue();

          if( strcmp(value, "") == 0 ) {
            encoders[i].deviceTypeRight = NOOP;
            foundEscape = true;
          }

          if(! foundEscape) {
            for (int j = 0; j < NUM_SPECIAL_KEYBOARD_CHARACTERS && !foundEscape; j++) {
              if (strcmp(specialKeyboardCharacters[j].key, value) == 0) {
                char valueStr[4];
                sprintf(valueStr, "%u", specialKeyboardCharacters[j].value);
                encoders[i].deviceTypeRight = KEYBOARD;
                encoders[i].setCharacterRight(valueStr);
                foundEscape = true;
                break;
              }
            }
          }
          
          if(! foundEscape) {
            for(int k = 0; k < NUM_SPECIAL_MOUSE_CHARACTERS && !foundEscape; k++) {
              if(strcmp(specialMouseCharacters[k].key, value) == 0) {
                encoders[i].deviceTypeRight = MOUSE_CLICK;
                encoders[i].mouseButtonRight = specialMouseCharacters[k].value;
                foundEscape = true;
              }
            }
          }
          if(! foundEscape) {
            for(int m = 0; m < NUM_SPECIAL_MOUSE_MOVE_CHARACTERS && !foundEscape; m++) {
              if(strcmp(specialMouseMoves[m].key, value) == 0) {
                encoders[i].deviceTypeRight = MOUSE_MOVE;
                encoders[i].xPosRight = specialMouseMoves[m].xDelta;
                encoders[i].yPosRight = specialMouseMoves[m].yDelta;
                encoders[i].wheelDeltaRight = specialMouseMoves[m].wDelta;
                foundEscape = true;
              }
            }
          }
          if(! foundEscape) {
            encoders[i].deviceTypeRight = KEYBOARD;
            encoders[i].setCharacterRight(value);
          }
        }
      } //for each encoders encoders[i]
    }
  } //while each config setting cfgName

  cfg.end();
  return true;
}

bool isSingleCharacter(const char* str) {
    if (str == nullptr || strlen(str) == 0) {
        return false; // Empty or null string
    }

    size_t byteCount = 0;
    size_t charCount = 0;

    while (str[byteCount] != '\0') {
        // Check the start of a UTF-8 character
        if ((str[byteCount] & 0xC0) != 0x80) {
            charCount++; // Increment character count for each new character
            if (charCount > 1) {
                return false; // More than one character found
            }
        }
        byteCount++;
    }
    return charCount == 1; // Ensure exactly one character
}

uint16_t utf8ToUint16(const char* str) {
    uint16_t unicodeChar = 0;

    if ((str[0] & 0x80) == 0) {
        // Single-byte ASCII (0xxxxxxx)
        unicodeChar = str[0];
    } else if ((str[0] & 0xE0) == 0xC0) {
        // Two-byte character (110xxxxx 10xxxxxx)
        unicodeChar = ((str[0] & 0x1F) << 6) | (str[1] & 0x3F);
    } else if ((str[0] & 0xF0) == 0xE0) {
        // Three-byte character (1110xxxx 10xxxxxx 10xxxxxx)
        unicodeChar = ((str[0] & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else {
        // UTF-8 sequences larger than three bytes don't fit in uint16_t
        unicodeChar = 0xFFFF; // Use an error code or handle as needed
    }

    return unicodeChar;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switches[i].pin, INPUT_PULLUP);
  }
  // //new
  // for (int i = 0; i < NUM_ENCODERS; i++) {
  //   pinMode(encoders[i].pin1, INPUT);
  //   encoders[i].oldValue = digitalRead(encoders[i].pin1);
  //   pinMode(encoders[i].pin2, INPUT);
  // }
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Error reading SD card. Using default configuration v12");
  } else {
    Serial.println("SD card found. Using configuration");
    readConfiguration();
  }
  Keyboard.begin();
  Mouse.begin();  //if mouse exists
}

void loop() {
  for (int i = 0; i < NUM_SWITCHES; i++) {
    if (switches[i].deviceType != NOOP) {
      switches[i].bounce.update();
      if (switches[i].bounce.fallingEdge()) {
        if (switches[i].deviceType == KEYBOARD) {
          if(isSingleCharacter(switches[i].character)) {
            Serial.println(String("fe Switch SingleCharPrint switch i:") + i);
            Keyboard.press(utf8ToUint16(switches[i].character)); //single character are pressed on keydown
          } //else //longer strings will be printed on keyup
        } else {
          if(switches[i].mouseButton != 0) {
            Serial.println(String("fe Switch Mouse click i:") + i);
            Mouse.press(switches[i].mouseButton);
          } else {
            Serial.println(String("fe Switch Mouse move i:") + i);
            Mouse.move(switches[i].xPos, switches[i].yPos, switches[i].wheelDelta);
          }
        }
      } else if (switches[i].bounce.risingEdge()) {
        if (switches[i].deviceType == KEYBOARD) {
          if(isSingleCharacter(switches[i].character)) { //single character strings are released on keyup
            Serial.println(String("re Switch SingleCharRelease i:") + i);
            Keyboard.release(utf8ToUint16(switches[i].character));
          } else {
            Serial.println(String("re Switch MultiChar i:") + i);
            Keyboard.print(switches[i].character); //longer strings are printed on keyup
          }
        } else {
          if(switches[i].mouseButton != 0) {
            Serial.println(String("re Mouse releae i:") + i);
            Mouse.release(switches[i].mouseButton);
          } // else mouse already moved
        }
      }
    }
  }
  for (int i = 0; i < NUM_ENCODERS; i++) {
    //old
    long newValue = encoders[i].encoder.read(); 
    ////new
    ////int newValue = digitalRead(encoders[i].pin1);
    int newDelta;
    boolean isTurned = false;
    boolean isLeft = false;


    //old
    if( newValue != encoders[i].oldValue && (encoders[i].oldValue != -999) ) {
      Serial.println(String("encoder turned ") + i + String(" old: ") + encoders[i].oldValue + String(" new: ") + newValue);
      delay(10);
      uint8_t remainder = newValue % 4;
      if(remainder == 0) {
        isTurned = true;
        newDelta = newValue / 4;
        if(newDelta > encoders[i].oldDelta) {
          isLeft = true;
        }
        encoders[i].oldDelta = newDelta;
      }
    }
    encoders[i].oldValue = newValue;

    // //new
    // if(newValue != encoders[i].oldValue) {
    //   isTurned = true;
    //   Serial.println(String("encoder turned ") + i + String(" old: ") + encoders[i].oldValue + String(" new: ") + newValue);
    //   if(digitalRead(encoders[i].pin2) == newValue) {
    //     isLeft = true;
    //   }
    // }
    // encoders[i].oldValue = newValue;

    if( isTurned && isLeft ) {
      if(encoders[i].deviceTypeLeft == KEYBOARD) {
        Serial.println(String("encoder turned+left Keyboard i:") + i);
        Keyboard.print(encoders[i].characterLeft);
      } else if(encoders[i].deviceTypeLeft == MOUSE_CLICK) {
        Serial.println(String("encoder turned+left Mouse click i:") + i);
        Mouse.click(encoders[i].mouseButtonLeft);
      } else if(encoders[i].deviceTypeLeft == MOUSE_MOVE) {
        Serial.println(String("encoder turned+left Mouse move i:") + i);
        Mouse.move(encoders[i].xPosLeft, encoders[i].yPosLeft, encoders[i].wheelDeltaLeft);
      }
    } else if( isTurned && !isLeft ) {
      if(encoders[i].deviceTypeRight == KEYBOARD) {
        Serial.println(String("encoder turned+right Keyboard i:") + i);
        Keyboard.print(encoders[i].characterRight);
      } else if(encoders[i].deviceTypeRight == MOUSE_CLICK) {
        Serial.println(String("encoder turned+right Mouse click i:") + i);
        Mouse.click(encoders[i].mouseButtonRight);
      } else if(encoders[i].deviceTypeRight == MOUSE_MOVE) {
        Serial.println(String("encoder turned+right Mouse move i:") + i);
        Mouse.move(encoders[i].xPosRight, encoders[i].yPosRight, encoders[i].wheelDeltaRight);
      }
      //encoders[i].oldValue = newValue; //the old mod 4 code might have been working but bug here this was left in
    }
  }
}

