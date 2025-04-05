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
const int JOYSTICK_DEADZONE = 50;

// Servo position variables
int lastLRservo = 90; // Initial position for horizontal servo
int lastUDservo = 90; // Initial position for vertical servo

void smoothMoveServo(PWMServo &servo, int &currentPos, int targetPos, int easingFactor = 10) {
    targetPos = constrain(targetPos, 0, 180);
    
    // Move in small increments toward the target
    int difference = targetPos - currentPos;
    if (abs(difference) > 1) {
        currentPos += difference / easingFactor; // Smaller step toward target
        servo.write(currentPos);
    }
}

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
  int rxPos = espData.indexOf(",RX:") + 4; // Added for right thumbstick
  int ryPos = espData.indexOf(",RY:") + 4; // Added for right thumbstick
  
  if (btnPos <= 3 || dpadPos <= 5 || lxPos <= 3 || lyPos <= 3 || rxPos <= 3 || ryPos <= 3) {
    Serial.println("Error: Invalid data structure");
    espData = "";
    newEspData = false;
    return;
  }

  // Extract values
  String btnStr = espData.substring(btnPos, espData.indexOf(",DPAD:"));
  String dpadStr = espData.substring(dpadPos, espData.indexOf(",LX:"));
  String lxStr = espData.substring(lxPos, espData.indexOf(",LY:"));
  String lyStr = espData.substring(lyPos, espData.indexOf(",RX:"));
  String rxStr = espData.substring(rxPos, espData.indexOf(",RY:"));
  String ryStr = espData.substring(ryPos);

  // Convert strings to values
  uint16_t btnVal = strtol(btnStr.c_str(), NULL, 16);
  uint8_t dpadVal = strtol(dpadStr.c_str(), NULL, 16);
  int lxVal = lxStr.toInt();
  int lyVal = lyStr.toInt();
  int rxVal = rxStr.toInt();
  int ryVal = ryStr.toInt();
  
  // Debug output
  Serial.print("Buttons: 0x");
  Serial.print(btnVal, HEX);
  Serial.print(", D-pad: 0x");
  Serial.print(dpadVal, HEX);
  Serial.print(", LX: ");
  Serial.print(lxVal);
  Serial.print(", LY: ");
  Serial.println(lyVal);
  Serial.print(", RX: ");
  Serial.print(rxVal);
  Serial.print(", RY: ");
  Serial.println(ryVal);
  
  // MAIN JOYSTICK CONTROL LOGIC
  // Basic movement based on left joystick (primary control method)
  if (lyVal < -JOYSTICK_DEADZONE) {
    // Forward
    Motor(aFwd, SpeedL, SpeedR);
    Serial.println("MOVE: Forward");
  } 
  else if (lyVal > JOYSTICK_DEADZONE) {
    // Backward
    Motor(aBwd, SpeedL, SpeedR);
    Serial.println("MOVE: Backward");
  }
  else if (lxVal < -JOYSTICK_DEADZONE) {
    // Left
    Motor(LeftFwd, SpeedL, SpeedR);
    Serial.println("MOVE: Left");
  }
  else if (lxVal > JOYSTICK_DEADZONE) {
    // Right
    Motor(RightFwd, SpeedL, SpeedR);
    Serial.println("MOVE: Right");
  }
  else if (abs(lxVal) <= JOYSTICK_DEADZONE && abs(lyVal) <= JOYSTICK_DEADZONE) {
    // Stop when joystick is centered
    Motor(Stop, 0, 0);
    Serial.println("MOVE: Stop");
  }

  // Simplified right stick logic for servo control
  if (abs(rxVal) > JOYSTICK_DEADZONE) {
    int targetLRPos = map(rxVal, -512, 512, 0, 180);
    smoothMoveServo(LRServo, lastLRservo, targetLRPos);
    Serial.print("Servo Horizontal: ");
    Serial.println(lastLRservo);
  }

  if (abs(ryVal) > JOYSTICK_DEADZONE) {
    int targetUDPos = map(ryVal, -512, 512, 0, 180);
    smoothMoveServo(UDServo, lastUDservo, targetUDPos);
    Serial.print("Servo Vertical: ");
    Serial.println(lastUDservo);
  }
  
  // D-PAD CONTROL LOGIC (alternate control)
  // Only process if joystick is centered
  if (abs(lxVal) <= JOYSTICK_DEADZONE && abs(lyVal) <= JOYSTICK_DEADZONE) {
    if (dpadVal & DPAD_UP) {
      Motor(aFwd, SpeedL, SpeedR);
      Serial.println("DPAD: Forward");
    }
    else if (dpadVal & DPAD_DOWN) {
      Motor(aBwd, SpeedL, SpeedR);
      Serial.println("DPAD: Backward");
    }
    else if (dpadVal & DPAD_LEFT) {
      Motor(LeftFwd, SpeedL, SpeedR);
      Serial.println("DPAD: Left");
    }
    else if (dpadVal & DPAD_RIGHT) {
      Motor(RightFwd, SpeedL, SpeedR);
      Serial.println("DPAD: Right");
    }
  }

  int lastServoUD = 90;
  int lastServoLR = 90;
  
  // BUTTON FUNCTION PLACEHOLDERS
  // Left empty for custom programming
  if (btnVal & BTN_X) {
    // X/A button function
    //down
    UDServo.write(lastServoUD - 45);
  }
  else if (btnVal & BTN_CIRCLE) {
    //right
    LRServo.write(lastServoLR - 45);

  }
  else if (btnVal & BTN_SQUARE) {
    //left
    LRServo.write(lastServoUD + 45);
  }
  else if (btnVal & BTN_TRIANGLE) {
    //up
    UDServo.write(lastServoLR + 45);
  }
  else if (btnVal & BTN_L1) {
    LRServo.write(90); // Move to center position
  }
  else if (btnVal & BTN_R1) {
    LRServo.write(0); // Move to center position
  }
  else if (btnVal & BTN_L2) {
    UDServo.write(45);
  }
  else if (btnVal & BTN_R2) {
    UDServo.write(0);
  }
  
  // Reset data
  espData = "";
  newEspData = false;
}
