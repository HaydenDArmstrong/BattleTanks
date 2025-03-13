
// Controller button constants
const uint16_t BTN_X = 0x0001;
const uint16_t BTN_CIRCLE = 0x0002;
const uint16_t BTN_SQUARE = 0x0004;
const uint16_t BTN_TRIANGLE = 0x0008;
const uint16_t BTN_L1 = 0x0010;
const uint16_t BTN_R1 = 0x0020;
const uint16_t BTN_L2 = 0x0040;
const uint16_t BTN_R2 = 0x0080;

// D-pad constants
const uint8_t DPAD_UP = 0x01;
const uint8_t DPAD_DOWN = 0x02;
const uint8_t DPAD_RIGHT = 0x04;
const uint8_t DPAD_LEFT = 0x08;

// Joystick deadzone
const int JOYSTICK_DEADZONE = 25;

// Read data from ESP32
void checkEspController() {
  while (espSerial.available() > 0) {
    char c = espSerial.read();
    if (c == '\n') {
      newEspData = true;
      return;
    } else {
      espData += c;
      // Prevent buffer overflow
      if (espData.length() > 100) {
        espData = "";
        newEspData = false;
      }
    }
  }
}

// Process controller data
void processEspData() {
  if (!newEspData) return;

  // Validate data format
  if (!espData.startsWith("BTN:")) {
    Serial.println("Error: Invalid data format");
    espData = "";
    newEspData = false;
    return;
  }
  
  // Debug: Print received data
  Serial.print("ESP32 Controller: ");
  Serial.println(espData);

  // Parse controller data
  int btnPos = espData.indexOf("BTN:") + 4;
  int dpadPos = espData.indexOf(",DPAD:") + 6;
  int lxPos = espData.indexOf(",LX:") + 4;
  int lyPos = espData.indexOf(",LY:") + 4;
  
  if (btnPos <= 3 || dpadPos <= 5 || lxPos <= 3 || lyPos <= 3) {
    Serial.println("Error: Invalid data structure");
    espData = "";
    newEspData = false;
    return;
  }

  // Extract values
  String btnStr = espData.substring(btnPos, espData.indexOf(",DPAD:"));
  String dpadStr = espData.substring(dpadPos, espData.indexOf(",LX:"));
  String lxStr = espData.substring(lxPos, espData.indexOf(",LY:"));
  String lyStr = espData.substring(lyPos);
  
  // Convert strings to values
  uint16_t btnVal = strtol(btnStr.c_str(), NULL, 16);
  uint8_t dpadVal = strtol(dpadStr.c_str(), NULL, 16);
  int lxVal = lxStr.toInt();
  int lyVal = lyStr.toInt();
  
  // Debug output
  Serial.print("Buttons: 0x");
  Serial.print(btnVal, HEX);
  Serial.print(", D-pad: 0x");
  Serial.print(dpadVal, HEX);
  Serial.print(", LX: ");
  Serial.print(lxVal);
  Serial.print(", LY: ");
  Serial.println(lyVal);
  
  // MAIN JOYSTICK CONTROL LOGIC
  // Basic movement based on left joystick (primary control method)
  if (lyVal < -JOYSTICK_DEADZONE) {
    // Forward
    Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
    Serial.println("MOVE: Forward");
  } 
  else if (lyVal > JOYSTICK_DEADZONE) {
    // Backward
    Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
    Serial.println("MOVE: Backward");
  }
  else if (lxVal < -JOYSTICK_DEADZONE) {
    // Left
    Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
    Serial.println("MOVE: Left");
  }
  else if (lxVal > JOYSTICK_DEADZONE) {
    // Right
    Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
    Serial.println("MOVE: Right");
  }
  else if (abs(lxVal) <= JOYSTICK_DEADZONE && abs(lyVal) <= JOYSTICK_DEADZONE) {
    // Stop when joystick is centered
    Motor(Stop, 0, 0, 0, 0);
    Serial.println("MOVE: Stop");
  }
  
  // D-PAD CONTROL LOGIC (alternate control)
  // Only process if joystick is centered
  if (abs(lxVal) <= JOYSTICK_DEADZONE && abs(lyVal) <= JOYSTICK_DEADZONE) {
    if (dpadVal & DPAD_UP) {
      Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
      Serial.println("DPAD: Forward");
    }
    else if (dpadVal & DPAD_DOWN) {
      Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
      Serial.println("DPAD: Backward");
    }
    else if (dpadVal & DPAD_LEFT) {
      Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
      Serial.println("DPAD: Left");
    }
    else if (dpadVal & DPAD_RIGHT) {
      Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
      Serial.println("DPAD: Right");
    }
  }
  
  // BUTTON FUNCTION PLACEHOLDERS
  // Left empty for custom programming
  if (btnVal & BTN_X) {
    // X button function
    // Your code here
  }
  else if (btnVal & BTN_CIRCLE) {
    // Circle button function
    // Your code here
  }
  else if (btnVal & BTN_SQUARE) {
    // Square button function
    // Your code here
  }
  else if (btnVal & BTN_TRIANGLE) {
    // Triangle button function
    // Your code here
  }
  else if (btnVal & BTN_L1) {
    // L1 button function
    // Your code here
  }
  else if (btnVal & BTN_R1) {
    // R1 button function
    // Your code here
  }
  else if (btnVal & BTN_L2) {
    // L2 button function
    // Your code here
  }
  else if (btnVal & BTN_R2) {
    // R2 button function
    // Your code here
  }
  
  // Reset data
  espData = "";
  newEspData = false;
}

