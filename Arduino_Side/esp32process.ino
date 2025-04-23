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

// Joystick constants
const int JOYSTICK_DEADZONE = 60; //adjust for amount of the joysticks should move in xy plane before being detected as real movement.
const float smoothRateUD = 6; // Adjust for speed of movement UP DOWN
const float smoothRateLR = 8.5; //adjust for speed of movement LEFT RIGHT
const int updateInterval = 10; // every ____ miliseconds, update. 

const int startingPosLR = 90; //starting position in degrees for Left-Right servo. 90 is recommended so servo is centered in left right
const int startingPosUD = 145; //starting position in degrees for Up-Right servo. 145 is recommended due to map range of 115-180


void flashLaserTurret() {
    digitalWrite(laserTurret, LOW); // Turn off the turret momentarily
    delay(400);                     // Wait 100ms (adjust duration as needed)
    digitalWrite(laserTurret, HIGH); // Turn the turret back on
}

void laserLife() {
    if (lives > 0) {
        lives--;
        Serial.print("Laser hit! Lives remaining: ");
        Serial.println(lives);

    } else {
        Serial.println("No lives left. Game Over!");
        digitalWrite(laserTurret, LOW);
        Motor (Stop, 0,0);
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
    digitalWrite(laserTurret, HIGH); // Ensure turret on before game start
    if (lives > 0) {
        int laserLeft = digitalRead(laserReceiverLeftPin);
        int laserRight = digitalRead(laserReceiverRightPin);

        if (laserLeft == LOW || laserRight == LOW) {
            Serial.println("Laser detected!");
            Motor(Stop, 0, 0);      // Stop  robot when hit
            flashLaserTurret();     // Flash turret to indicate a hit
            laserLife();            // Decrement lives
        } else {
            //  Serial.println("No laser detected.");
        }
        delay(3); // Small delay, sensor stability
    }

    if (!newEspData) {
        return;
    }

    // Validate data format
    if (!espData.startsWith("BTN:")) {
        Serial.println("Error: Invalid data format");
        espData = "";
        newEspData = false;
        return;
    }
  
    // Parse controller data 
    // lx: left stick x axis graph rx: right stick x axis
    //ly: left stick y axis graph ry: right stick y axis
    int btnPos = espData.indexOf("BTN:") + 4;
    int dpadPos = espData.indexOf(",DPAD:") + 6;
    int lxPos = espData.indexOf(",LX:") + 4;  
    int lyPos = espData.indexOf(",LY:") + 4;
    int rxPos = espData.indexOf(",RX:") + 4; 
    int ryPos = espData.indexOf(",RY:") + 4;
  
    if (btnPos <= 3 || dpadPos <= 5 || lxPos <= 3 || lyPos <= 3 || rxPos <= 3 || ryPos <= 3) {
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

    if (lxVal > 512 || lyVal > 512 || rxVal > 512 || ryVal > 512) {
        Serial.println("Error: Invalid joystick values (greater than 512)");
        espData = "";
        newEspData = false;
        return;
    }
  
    // Flags to track if joysticks are active
    bool leftJoystickActive = abs(lxVal) > JOYSTICK_DEADZONE || abs(lyVal) > JOYSTICK_DEADZONE;
    bool rightJoystickActive = abs(rxVal) > JOYSTICK_DEADZONE || abs(ryVal) > JOYSTICK_DEADZONE;

    // Left Joystick Control Logic (LY, LX)
    if (leftJoystickActive) {
        if (lyVal < -JOYSTICK_DEADZONE) {
            // Forward
            Motor(aFwd, SpeedL, SpeedR);
            Serial.println("MOVE: Forward");
        } else if (lyVal > JOYSTICK_DEADZONE) {
            // Backward
            Motor(aBwd, SpeedL, SpeedR);
        } else if (lxVal < -JOYSTICK_DEADZONE) {
            // Left
            Motor(LeftFwd, SpeedL, SpeedR);
        } else if (lxVal > JOYSTICK_DEADZONE) {
            // Right
            Motor(RightFwd, SpeedL, SpeedR);
        }
    } else {
        // Stop only if neither joystick is active
        Motor(Stop, 0, 0);
    }

    // Right Joystick Control Logic (RY, RX)
    if (rightJoystickActive) {
        static int currentLRPos = startingPosLR; // Starting position for horizontal servo. declared in top, global constants
        static int currentUDPos = startingPosUD; // Starting position for vertical servo. declared in top, global constants
        static unsigned long lastUpdate = millis();
    
        if (millis() - lastUpdate >= updateInterval) { // Update per milisecond amount given in updateInterval
            lastUpdate = millis();

            if (abs(rxVal / 2) > JOYSTICK_DEADZONE) {
                int targetLRPos = map(rxVal, -512, 512, 180, 0); // map range(-512,512 is the joystick map. 180 to 0 is degree of movement.)
                currentLRPos += (targetLRPos - currentLRPos) / smoothRateLR; // Smooth transition
                LRServo.write(currentLRPos);
                Serial.print("MOVE Servo Horizontal: ");
                Serial.println(currentLRPos);
            }

            if (abs(ryVal / 2) > JOYSTICK_DEADZONE) {
                int targetUDPos = map(ryVal, -512, 512, 115, 180); // map range (-512,512 is the joystick map. 115 to 180 is degree of movement.)
                currentUDPos += (targetUDPos - currentUDPos) / smoothRateLR; // Smooth transition 
                UDServo.write(currentUDPos);
                Serial.print("MOVE Servo Vertical: ");
                Serial.println(currentUDPos);
            }
        }
    }

    //D-PAD LOGIC
    if (!leftJoystickActive) {
        if (dpadVal & DPAD_UP) {
            Motor(aFwd, SpeedL, SpeedR);
            Serial.println("DPAD: Forward");
        } else if (dpadVal & DPAD_DOWN) {
            Motor(aBwd, SpeedL, SpeedR);
            Serial.println("DPAD: Backward");
        } else if (dpadVal & DPAD_LEFT) {
            Motor(LeftFwd, SpeedL, SpeedR);
            Serial.println("DPAD: Left");
        } else if (dpadVal & DPAD_RIGHT) {
            Motor(RightFwd, SpeedL, SpeedR);
            Serial.println("DPAD: Right");
        } else {
            // Stop motors if no left thumbstickk movement AND no D-pad direction is pressed 
            Motor(Stop, 0, 0);
        }
    }

    if (btnVal & BTN_X) {
        // Down button pressed
        UDServo.write(180);
    } else if (btnVal & BTN_TRIANGLE) {
        // Up button pressed
        UDServo.write(115);
    } else if (btnVal & BTN_SQUARE) {
        // Left button pressed
        LRServo.write(180);
    } else if (btnVal & BTN_CIRCLE) {
        // Right button pressed
        LRServo.write(0);
    } else if (btnVal & BTN_L1) {
        //left bumper
    } else if (btnVal & BTN_R1) {
        //right bumper
    } else if (btnVal & BTN_L2) {
        // left trigger
    } else if (btnVal & BTN_R2) {
        // right trigger
    }
    printControllerDebug(btnVal, dpadVal, lxVal, lyVal, rxVal, ryVal);
    // Reset data
    espData = "";
    newEspData = false;
}

// Debug Output: Display Controller State
void printControllerDebug(uint16_t btnVal, uint8_t dpadVal, int lxVal, int lyVal, int rxVal, int ryVal) {
    Serial.print("Controller Debug | Buttons: ");
    if (btnVal & BTN_X) Serial.print("X ");
    if (btnVal & BTN_CIRCLE) Serial.print("Circle ");
    if (btnVal & BTN_SQUARE) Serial.print("Square ");
    if (btnVal & BTN_TRIANGLE) Serial.print("Triangle ");
    if (btnVal & BTN_L1) Serial.print("L1 ");
    if (btnVal & BTN_R1) Serial.print("R1 ");
    if (btnVal & BTN_L2) Serial.print("L2 ");
    if (btnVal & BTN_R2) Serial.print("R2 ");
    if (btnVal == 0) Serial.print("None ");
    Serial.print("| D-Pad: ");
    if (dpadVal & DPAD_UP) Serial.print("Up ");
    if (dpadVal & DPAD_DOWN) Serial.print("Down ");
    if (dpadVal & DPAD_LEFT) Serial.print("Left ");
    if (dpadVal & DPAD_RIGHT) Serial.print("Right ");
    if (dpadVal == 0) Serial.print("None ");
    Serial.print("| Left Joystick: LX=");
    Serial.print(lxVal);
    Serial.print(", LY=");
    Serial.print(lyVal);
    Serial.print(" | Right Joystick: RX=");
    Serial.print(rxVal);
    Serial.print(", RY=");
    Serial.println(ryVal);
}
