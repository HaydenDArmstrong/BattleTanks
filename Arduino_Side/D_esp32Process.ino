// Controller button constants
namespace ControllerButtons {
    const uint16_t X = 0x0001;
    const uint16_t CIRCLE = 0x0002;
    const uint16_t SQUARE = 0x0004;
    const uint16_t TRIANGLE = 0x0008;
    const uint16_t L1 = 0x0010;
    const uint16_t R1 = 0x0020;
    const uint16_t L2 = 0x0040;
    const uint16_t R2 = 0x0080;
}

// D-pad constants
namespace DPad {
    const uint8_t UP = 0x01;
    const uint8_t DOWN = 0x02;
    const uint8_t RIGHT = 0x04;
    const uint8_t LEFT = 0x08;
}

// Joystick configuration
namespace JoystickConfig {
    const int DEADZONE = 80;                  // Joystick movement threshold
    const float SMOOTH_RATE_UD = 25;        // Up-Down movement smoothing
    const float SMOOTH_RATE_LR = 9.0;        // Left-Right movement smoothing
    const int UPDATE_INTERVAL_MS = 10;        // Update interval in milliseconds
    const unsigned long DEBOUNCE_TIME_MS = 10;// Joystick debounce time
    const unsigned long DEBOUNCE_DELAY_MS = 5;// Debounce delay
    
    const int STARTING_POS_LR = 80;           // Left-Right starting position (centered)
    const int STARTING_POS_UD = 155;          // Up-Down starting position
    const int UD_MIN = 140;                   // Up-Down minimum position
    const int UD_MAX = 180;                   // Up-Down maximum position
    const int LR_MIN = 10;                     // Left-Right minimum position
    const int LR_MAX = 170;                   // Left-Right maximum position
}

// Turret control
void flashLaserTurret() {
    digitalWrite(laserTurret, LOW);           // Turn off momentarily
    delay(400);                               // Flash duration
    digitalWrite(laserTurret, HIGH);          // Turn back on
}

// Life management
void handleLaserHit() {
    if (lives > 0) {
        lives--;
        Serial.print("Laser hit! Lives remaining: ");
        Serial.println(lives);
    } else {
        Serial.println("No lives left. Game Over!");
        digitalWrite(laserTurret, LOW);
        Motor(Stop, 0, 0);
    }
}

// ESP32 data reading
void readEspControllerData() {
    static const size_t MAX_DATA_LENGTH = 100;
    
    while (espSerial.available() > 0) {
        char c = espSerial.read();
        if (c == '\n') {
            newEspData = true;
            return;
        }
        
        espData += c;
        if (espData.length() > MAX_DATA_LENGTH) {
             espData = "";
            newEspData = false;
        }
    }
}

// Data parsing helper
bool parseControllerData(const String& data, uint16_t& btnVal, uint8_t& dpadVal, 
                        int& lxVal, int& lyVal, int& rxVal, int& ryVal) {
    // Validate data format
    if (!data.startsWith("BTN:")) {
        Serial.println("Error: Invalid data format");
        return false;
    }

    // Find all field positions
    const int btnPos = data.indexOf("BTN:") + 4;
    const int dpadPos = data.indexOf(",DPAD:") + 6;
    const int lxPos = data.indexOf(",LX:") + 4;  
    const int lyPos = data.indexOf(",LY:") + 4;
    const int rxPos = data.indexOf(",RX:") + 4; 
    const int ryPos = data.indexOf(",RY:") + 4;

    // Validate positions
    if (btnPos <= 3 || dpadPos <= 5 || lxPos <= 3 || lyPos <= 3 || rxPos <= 3 || ryPos <= 3) {
        return false;
    }

    // Extract and convert values
    btnVal = strtol(data.substring(btnPos, data.indexOf(",DPAD:")).c_str(), nullptr, 16);
    dpadVal = strtol(data.substring(dpadPos, data.indexOf(",LX:")).c_str(), nullptr, 16);
    lxVal = data.substring(lxPos, data.indexOf(",LY:")).toInt();
    lyVal = data.substring(lyPos, data.indexOf(",RX:")).toInt();
    rxVal = data.substring(rxPos, data.indexOf(",RY:")).toInt();
    ryVal = data.substring(ryPos).toInt();

    // Validate joystick values
    if (lxVal > 512 || lyVal > 512 || rxVal > 512 || ryVal > 512) {
        Serial.println("Error: Invalid joystick values (greater than 512)");
        return false;
    }

    return true;
}

// Motor control based on joystick input
void handleLeftJoystick(int lxVal, int lyVal) {
    static unsigned long lastJoystickAction = 0;
    static bool joystickActive = false;
    
    const bool leftJoystickActive = abs(lxVal) > JoystickConfig::DEADZONE || 
                                   abs(lyVal) > JoystickConfig::DEADZONE;

    if (leftJoystickActive) {
        if (!joystickActive || (millis() - lastJoystickAction > JoystickConfig::DEBOUNCE_DELAY_MS)) {
            joystickActive = true;
            lastJoystickAction = millis();

            if (abs(lyVal) > abs(lxVal)) {
                if (lyVal < -JoystickConfig::DEADZONE) {
                    Motor(aFwd, SpeedL, SpeedR);
                    Serial.print("FWD ");
                } else if (lyVal > JoystickConfig::DEADZONE) {
                    Motor(aBwd, SpeedL, SpeedR);
                    Serial.print("BWD ");
                }
            } else {
                if (lxVal < -JoystickConfig::DEADZONE) {
                    Motor(LeftFwd, SpeedL, SpeedR);
                    Serial.print("LEFT ");
                } else if (lxVal > JoystickConfig::DEADZONE) {
                    Motor(RightFwd, SpeedL, SpeedR);
                    Serial.print("RIGHT ");
                }
            }
        }
    } else {
        joystickActive = false;
        Motor(Stop, 0, 0);
    }
}

// Turret control based on right joystick
void handleRightJoystick(int rxVal, int ryVal) {
    static unsigned long lastJoystickAction = 0;
    static bool joystickActive = false;
    static int currentLRPos = JoystickConfig::STARTING_POS_LR;
    static int currentUDPos = JoystickConfig::STARTING_POS_UD;
    
    const bool rightJoystickActive = abs(rxVal) > JoystickConfig::DEADZONE || 
                                    abs(ryVal) > JoystickConfig::DEADZONE;

    if (rightJoystickActive) {
        if (!joystickActive || (millis() - lastJoystickAction > JoystickConfig::DEBOUNCE_DELAY_MS)) {
            joystickActive = true;
            lastJoystickAction = millis();

            if (abs(ryVal) > abs(rxVal)) {
                // Vertical movement
                const int targetUDPos = map(ryVal, -512, 512, JoystickConfig::UD_MIN, JoystickConfig::UD_MAX);
                currentUDPos += (targetUDPos - currentUDPos) / JoystickConfig::SMOOTH_RATE_LR;
                UDServo.write(currentUDPos);
                LRServo.write(currentLRPos);
                // Servo positions
   
    Serial.print(" | Servos: LR=");
    Serial.print(currentLRPos);
    Serial.print("°, UD=");
    Serial.print(currentUDPos);
    Serial.print("°");

            } else {
                // Horizontal movement
                const int targetLRPos = map(rxVal, -512, 512, JoystickConfig::LR_MAX, JoystickConfig::LR_MIN);
                currentLRPos += (targetLRPos - currentLRPos) / JoystickConfig::SMOOTH_RATE_LR;
                LRServo.write(currentLRPos);
                UDServo.write(currentUDPos);

    Serial.print(" | Servos: LR=");
    Serial.print(currentLRPos);
    Serial.print("°, UD=");
    Serial.print(currentUDPos);
    Serial.print("° | ");

            }
        }
    } else {
        joystickActive = false;
    }
}

// Button controls for turret
void handleButtons(uint16_t btnVal) {
    if (btnVal & ControllerButtons::X) {
        UDServo.write(JoystickConfig::UD_MAX);
    } else if (btnVal & ControllerButtons::TRIANGLE) {
        UDServo.write(JoystickConfig::UD_MIN);
    } else if (btnVal & ControllerButtons::SQUARE) {
        LRServo.write(JoystickConfig::LR_MAX);

        LRServo.write(JoystickConfig::LR_MIN);
    }
}

// Process controller data
void processControllerData() {
    digitalWrite(laserTurret, HIGH); // Ensure turret is on
    
    // Check for laser hits
    if (lives > 0) {
        const int laserLeft = digitalRead(laserReceiverLeftPin);
        const int laserMiddle = digitalRead(laserReceiverMiddlePin);
        const int laserRight = digitalRead(laserReceiverRightPin);

        //laserlefte has to be high on grey turret for whatever reason
         if (laserLeft == LOW5|| laserRight == LOW || laserMiddle == LOW) {
             Serial.println("Laser detected!");
             Motor(Stop, 0, 0);
             flashLaserTurret();
             handleLaserHit();
         }
        delay(3); // Small delay for sensor stability
    }

    if (!newEspData) return;

    // Parse controller data
    uint16_t btnVal;
    uint8_t dpadVal;
    int lxVal, lyVal, rxVal, ryVal;
    
    if (!parseControllerData(espData, btnVal, dpadVal, lxVal, lyVal, rxVal, ryVal)) {
        espData = "";
        newEspData = false;
        return;
    }

    // Handle controls
    handleLeftJoystick(lxVal, lyVal);
    handleRightJoystick(rxVal, ryVal);
    handleButtons(btnVal);
    
    // Debug output
    printControllerDebug(btnVal, dpadVal, lxVal, lyVal, rxVal, ryVal);
    
    // Reset data flags
     espData = "";
    newEspData = false;
}


void printControllerDebug(uint16_t btnVal, uint8_t dpadVal, int lxVal, int lyVal, int rxVal, int ryVal) {
    static int currentLRPos = JoystickConfig::STARTING_POS_LR;
    static int currentUDPos = JoystickConfig::STARTING_POS_UD;

    Serial.print("Controller Debug:");
    Serial.print("Buttons: ");
    if (btnVal & ControllerButtons::X) Serial.print("X ");
    if (btnVal & ControllerButtons::CIRCLE) Serial.print("CIRCLE ");
    if (btnVal & ControllerButtons::SQUARE) Serial.print("SQUARE ");
    if (btnVal & ControllerButtons::TRIANGLE) Serial.print("TRIANGLE ");
    if (btnVal & ControllerButtons::L1) Serial.print("L1 ");
    if (btnVal & ControllerButtons::R1) Serial.print("R1 ");
    if (btnVal & ControllerButtons::L2) Serial.print("L2 ");
    if (btnVal & ControllerButtons::R2) Serial.print("R2 ");
  

    Serial.print("D-Pad: ");
    if (dpadVal & DPad::UP) Serial.print("UP ");
    if (dpadVal & DPad::DOWN) Serial.print("DOWN ");
    if (dpadVal & DPad::LEFT) Serial.print("LEFT ");
    if (dpadVal & DPad::RIGHT) Serial.print("RIGHT ");

    Serial.print("Joysticks:");
    Serial.print("  Left  (LX, LY): (");
    Serial.print(lxVal);
    Serial.print(", ");
    Serial.print(lyVal);
    Serial.print(")");
    Serial.print("  Right (RX, RY): (");
    Serial.print(rxVal);
    Serial.print(", ");
    Serial.print(ryVal);
    Serial.print(")");
    Serial.println(".");
}
