/*
   Arduino Robot with ESP32 Controller Integration
   
    Laser Receiver Pins:
        VCC: +5
        GND: GND
        OUT: A5, A4

    Laser emitter out: A3

    Bluetooth Pins:
        VCC: +5
        GND: GND
        TXD: A1
        RXD: A0
  
    Servo "Head" (Ultrasonic Rotator) Pin:
        VCC: +5
        GND: GND
        SIGNAL: SERVO1 / PIN 10
        
    ESP32 Connection Pins:
        ESP32 TX (GPIO 17) -> Arduino pin 2 (RX)
        ESP32 RX (GPIO 16) -> Arduino pin 3 (TX)
        Common GND
*/

#include <Servo.h>
#include <SoftwareSerial.h>

// Bluetooth globals
SoftwareSerial btSerial(A1, A0);
char btCommand;

// ESP32 Controller communication
SoftwareSerial espSerial(2, 3); // RX, TX
String espData = "";
bool newEspData = false;

// Initialized function: Makes sure all parts work as they should
bool hasInitialized = false;

// Player "Lives"
int lives = 3;

// Servo globals
Servo headServo;
const int headServoPin = 10; // Pin for "Servo 1" on the generic motor shield

// Laser receiver globals
const int laserReceiverLeftPin = A5;
const int laserReceiverRightPin = A4;
const int laserTurret = A3;

// Motor sensor globals
const int PWM2A = 11;      // M1 motor
const int PWM2B = 3;       // M2 motor  '-pol/
const int PWM0A = 6;       // M3 motor 
const int PWM0B = 5;       // M4 motor
const int DIR_CLK = 4;     // Data input clock line
const int DIR_EN = 7;      // Enable pins
const int DATA = 8;        // USB cable
const int DIR_LATCH = 12;  // Output memory latch clock

// Directional constants
const int Move_Forward = 39;       // Move Forward
const int Move_Backward = 216;     // Move Backward
const int Left_Move = 116;         // Left translation
const int Right_Move = 139;        // Right translation
const int Right_Rotate = 149;      // Rotate Right
const int Left_Rotate = 106;       // Rotate Left
const int Stop = 0;                // Parking variable
const int Upper_Left_Move = 36;    // Upper Left Move
const int Upper_Right_Move = 3;    // Upper Right Move
const int Lower_Left_Move = 80;    // Lower Left Move
const int Lower_Right_Move = 136;  // Lower Right Move
const int Drift_Left = 20;         // Drift on Left
const int Drift_Right = 10;        // Drift on Right

// Set the default speed between 1 and 255
int Speed1 = 255;
int Speed2 = 255;
int Speed3 = 255;
int Speed4 = 255;

// Controller button constants
const uint16_t BTN_X = 0x0001;
const uint16_t BTN_CIRCLE = 0x0002;
const uint16_t BTN_SQUARE = 0x0004;
const uint16_t BTN_TRIANGLE = 0x0008;
const uint16_t BTN_L1 = 0x0010;
const uint16_t BTN_R1 = 0x0020;
const uint16_t BTN_L2 = 0x0040;
const uint16_t BTN_R2 = 0x0080;

// D-pad button constants
const uint8_t DPAD_UP = 0x01;
const uint8_t DPAD_DOWN = 0x02;
const uint8_t DPAD_RIGHT = 0x04;
const uint8_t DPAD_LEFT = 0x08;

void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
  analogWrite(PWM2A, Speed1);  // Motor PWM speed regulation
  analogWrite(PWM2B, Speed2);  // Motor PWM speed regulation
  analogWrite(PWM0A, Speed3);  // Motor PWM speed regulation
  analogWrite(PWM0B, Speed4);  // Motor PWM speed regulation

  digitalWrite(DIR_LATCH, LOW);            // DIR_LATCH sets the low level and writes the direction of motion in preparation
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Write Dir motion direction value
  digitalWrite(DIR_LATCH, HIGH);           // DIR_LATCH sets the high level and outputs the direction of motion
}

void setup() {
  // Start serial communication
  btSerial.begin(9600);
  Serial.begin(9600);
  espSerial.begin(9600);

  // Attach the servo to the pin
  headServo.attach(headServoPin);

  // Define laser receiver
  pinMode(laserReceiverLeftPin, INPUT);
  pinMode(laserReceiverRightPin, INPUT);
  pinMode(laserTurret, OUTPUT);

  // Define motor pins
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWM0B, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);

  // Initial debug message
  Serial.println("Setup complete. Starting loop...");
  Serial.println("ESP32 controller integration ready!");
}

void initial() {
  Serial.println("Initializing system components...");
  delay(100);

  // Test servo
  Serial.print("Testing servo... ");
  headServo.attach(headServoPin);
  headServo.write(90); // Move to center position
  delay(500);
  headServo.write(0); // Move to left position
  delay(500);
  headServo.write(180); // Move to right position
  delay(500);
  headServo.write(90); // Reset to center
  Serial.println("Servo OK.");

  // Test laser detector
  Serial.print("Testing laser receiver... ");
  int laserDetected = digitalRead(laserReceiverLeftPin);
  if (laserDetected == LOW) {
    Serial.println("Laser Receiver OK. Laser detected");
  } else if (laserDetected == HIGH) {
    Serial.println("Laser Receiver OK. No Laser Detected");
  } else {
    Serial.println("Laser Receiver FAILED");
  }

  // Test motors
  Serial.print("Testing motors... ");
  Motor(Move_Forward, 150, 150, 150, 150); // Run motors at half speed forward
  delay(500);
  Motor(Stop, 0, 0, 0, 0); // Stop motors
  Serial.println("Motors OK.");

  Serial.println("System initialization complete.");
  hasInitialized = true; // Mark system as initialized

  // Print a dashed line
  for (int i = 0; i < 50; i++) {
    Serial.print('-');
  }
  Serial.println(); // Move to the next line
}

void displayStats() {
  // Get and display the current laser status
  int laserDetected = digitalRead(laserReceiverLeftPin) || digitalRead(laserReceiverRightPin);
  Serial.println(laserDetected);
  if (laserDetected == LOW) {
    Serial.println("Laser detected: Yes");
  } else {
    Serial.println("Laser detected: No");
  }

  // Print lives
  Serial.print("Lives remaining: ");
  Serial.println(lives);

  // Print a separator line
  for (int i = 0; i < 50; i++) {
    Serial.print('-');
  }
  Serial.println();
}

void laserLife() {
  if (lives > 0) {
    lives--;
    Serial.print("Laser hit! Lives remaining: ");
    Serial.print(lives);
    Serial.println();

    if (lives == 0) {
      Serial.println("Game Over!");
      // Add game-over logic here
    }
  } else {
    Serial.println("No lives left. Game Over!");
  }
}

// Constants for delays
const int turretFlashOnDelay = 300;
const int turretFlashOffDelay = 700;
const int pauseAfterHit = 2000;

void flashLaserTurret() {
  for (int i = 0; i < 2; ++i) {
    digitalWrite(laserTurret, LOW);
    delay(turretFlashOffDelay);
    digitalWrite(laserTurret, HIGH);
    delay(turretFlashOnDelay);
  }
}

void moveServoHead() {
  headServo.write(90); // Move to center position
  delay(1000);
  headServo.write(45); // Move to left position
  delay(1000);
  headServo.write(135); // Move to right position
  delay(1000);
  headServo.write(90); // Reset to center
  delay(1000);
}

void laserGame() {
  while (lives > 0) {
    int laserLeft = digitalRead(laserReceiverLeftPin);
    int laserRight = digitalRead(laserReceiverRightPin);
    digitalWrite(laserTurret, HIGH); // Ensure turret is on

    if (laserLeft == LOW || laserRight == LOW) {
      Serial.println("Laser detected!");
      Motor(Stop, 0, 0, 0, 0); // Stop robot
      flashLaserTurret();     // Flash turret to indicate hit
      delay(pauseAfterHit);   // Pause for effect
      laserLife();            // Decrement lives
    } else {
      Serial.println("No laser detected.");
      delay(100);
      // moveServoHead(); // Execute servo movement
    }

    // Check for command to end game
    if (Serial.available() > 0) {
      char cmd = Serial.read();
      if (cmd == ']' || cmd == 'S') {
        break; // Exit game
      }
    }

    // Check ESP32 controller
    checkEspController();
    if (newEspData) {
      processEspData();
      if (espData.indexOf("]") >= 0 || espData.indexOf("S") >= 0) {
        break; // Exit game
      }
    }
  }

  // Game over logic
  Motor(Stop, 0, 0, 0, 0);        // Ensure the robot is stopped
  digitalWrite(laserTurret, LOW); // Turn off turret
  Serial.println("Robot has died. Game Over!");
}

void processCommand(char command) {
  switch (command) {
    case 'w': // Move Forward
      Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
      delay(1000);
      break;
    case 's': // Move Backward
      Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
      delay(200);
      break;
    case 'a': // Move Left
      Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
      delay(200);
      break;
    case 'd': // Move Right
      Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
      delay(200);
      break;
    case 'f': // Rotate Left
      Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
      delay(100);
      break;
    case 'j': // Rotate Right
      Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
      delay(100);
      break;

    // Additional cases
    case 'q': // Move Upper Left
      Motor(Upper_Left_Move, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;
    case 'e': // Move Upper Right
      Motor(Upper_Right_Move, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;
    case 'z': // Move Lower Left
      Motor(Lower_Left_Move, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;
    case 'c': // Move Lower Right
      Motor(Lower_Right_Move, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;
    case 'n': // Drift Left
      Motor(Drift_Left, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;
    case 'm': // Drift Right
      Motor(Drift_Right, Speed1, Speed2, Speed3, Speed4);
      delay(300);
      break;

    // Games and special functions
    case '[': // Runs laser game
      laserGame();
      break;
    case ']': // Restore lives
      lives = 3;
      break;
    case '`': // Display stats
      displayStats();
      break;
    case 't': // Runs init/test function
      initial();
      break;
    case 'S': // Stop All Motors
      Motor(Stop, 0, 0, 0, 0);
      break;
    default: // Default to Stop
      Motor(Stop, 0, 0, 0, 0);
      break;
  }
}

// Function to check for ESP32 controller data
void checkEspController() {
  while (espSerial.available() > 0) {
    char c = espSerial.read();
    
    // If end of line, set flag to process data
    if (c == '\n') {
      newEspData = true;
      return;
    } else {
      // Add character to incoming data
      espData += c;
    }
  }
}

// Parse the controller data and control the robot
void processEspData() {
  if (!newEspData) return;
  
  Serial.print("ESP32 Controller: ");
  Serial.println(espData);
  
  // Process specific button messages
  if (espData.indexOf("X button pressed") >= 0) {
    processCommand('w'); // Forward
  } 
  else if (espData.indexOf("Circle button pressed") >= 0) {
    processCommand('S'); // Stop
  }
  else if (espData.indexOf("Square button pressed") >= 0) {
    processCommand('a'); // Left
  }
  else if (espData.indexOf("Triangle button pressed") >= 0) {
    processCommand('d'); // Right
  }
  
  // Process BTN data format
  if (espData.startsWith("BTN:")) {
    // Parse the button data
    int btnPos = espData.indexOf("BTN:") + 4;
    int dpadPos = espData.indexOf(",DPAD:") + 6;
    int lxPos = espData.indexOf(",LX:") + 4;
    int lyPos = espData.indexOf(",LY:") + 4;
    
    if (btnPos > 3 && dpadPos > 5 && lxPos > 3 && lyPos > 3) {
      // Extract substrings
      String btnStr = espData.substring(btnPos, espData.indexOf(",DPAD:"));
      String dpadStr = espData.substring(dpadPos, espData.indexOf(",LX:"));
      String lxStr = espData.substring(lxPos, espData.indexOf(",LY:"));
      String lyStr = espData.substring(lyPos);
      
      // Convert to values
      uint16_t btnVal = strtol(btnStr.c_str(), NULL, 16);
      uint8_t dpadVal = strtol(dpadStr.c_str(), NULL, 16);
      int lxVal = lxStr.toInt();
      int lyVal = lyStr.toInt();
      
      // Debug
      Serial.print("Buttons: 0x");
      Serial.print(btnVal, HEX);
      Serial.print(", D-pad: 0x");
      Serial.print(dpadVal, HEX);
      Serial.print(", LX: ");
      Serial.print(lxVal);
      Serial.print(", LY: ");
      Serial.println(lyVal);
      
      // Handle joystick for movement
      if (lyVal < -100) {
        // Forward
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Forward");
      } 
      else if (lyVal > 100) {
        // Backward
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Backward");
      }
      else if (lxVal < -100) {
        // Left
        Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Left");
      }
      else if (lxVal > 100) {
        // Right
        Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Right");
      }
      else if (abs(lxVal) < 25 && abs(lyVal) < 25) {
        // Stop when joystick is in deadzone
        Motor(Stop, 0, 0, 0, 0);
        Serial.println("Controller: Stop (joystick center)");
      }
      
      // Handle D-pad
      if (dpadVal & DPAD_UP) {
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: D-pad Up");
      }
      else if (dpadVal & DPAD_DOWN) {
        Motor(Move_Backward, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: D-pad Down");
      }
      else if (dpadVal & DPAD_LEFT) {
        Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: D-pad Left");
      }
      else if (dpadVal & DPAD_RIGHT) {
        Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: D-pad Right");
      }
      
      // Handle special buttons
      if (btnVal & BTN_X) {
        // X button: Forward
        Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: X Button (Forward)");
      }
      else if (btnVal & BTN_CIRCLE) {
        // Circle button: Stop
        Motor(Stop, 0, 0, 0, 0);
        Serial.println("Controller: Circle Button (Stop)");
      }
      else if (btnVal & BTN_SQUARE) {
        // Square button: Left
        Motor(Left_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Square Button (Left)");
      }
      else if (btnVal & BTN_TRIANGLE) {
        // Triangle button: Right
        Motor(Right_Move, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: Triangle Button (Right)");
      }
      else if (btnVal & BTN_L1) {
        // L1 button: Rotate Left
        Motor(Left_Rotate, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: L1 Button (Rotate Left)");
      }
      else if (btnVal & BTN_R1) {
        // R1 button: Rotate Right
        Motor(Right_Rotate, Speed1, Speed2, Speed3, Speed4);
        Serial.println("Controller: R1 Button (Rotate Right)");
      }
      else if (btnVal & BTN_L2) {
        // L2 button: Toggle laser game
        Serial.println("Controller: L2 Button (Toggle Laser Game)");
        laserGame();
      }
      else if (btnVal & BTN_R2) {
        // R2 button: Reset lives
        lives = 3;
        Serial.println("Controller: R2 Button (Reset Lives)");
      }
    }
  }
  
  // Clear the data for next message
  espData = "";
  newEspData = false;
}

void loop() {
  // Check for controller initialization on first run
  if (!hasInitialized) {
    initial();  // Call the initial setup function
  }

  // Check input from Serial Monitor
  if (Serial.available() > 0) {
    char command = Serial.read();
    processCommand(command);
  }

  // Check input from Bluetooth
  if (btSerial.available()) {
    btCommand = btSerial.read(); // Read the incoming char
    
    // Debug: echo command to Serial Monitor
    Serial.print("Bluetooth Command Received: ");
    Serial.println(btCommand);
    
    // Process the command
    processCommand(btCommand);
  }
  
  // Check input from ESP32 Controller
  checkEspController();
  if (newEspData) {
    processEspData();
  }
}
