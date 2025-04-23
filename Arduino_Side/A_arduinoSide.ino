#include <PWMServo.h>
#include <SoftwareSerial.h>

// ESP32 Controller communication
SoftwareSerial espSerial(A4, A5); // RX, TX
String espData = "";
bool newEspData = false;

// Initialized function: Makes sure all parts work as they should
bool hasInitialized = false;

// Player "Lives". Always starts at 5. If you want to change the number of lives, change startingLives
const int startingLives = 5;
int lives = startingLives;

// Servo globals
PWMServo LRServo;
const int LRServoPin = 10; // Pin for "Servo 1" on l293d

PWMServo UDServo;
const int UDServoPin = 9; // Pin for "Servo 1" on l293d

// Laser receiver globals
const int laserReceiverLeftPin = A2;
const int laserReceiverRightPin = A1;
const int laserTurret = A3;

// Motor sensor globals
const int PWML = 11;      // M1 motor left side pwm2a
const int PWMR = 5;       // M2 motor right side pwm0b
const int DIR_CLK = 4;    // Data input clock line
const int DIR_EN = 7;     // Enable pins
const int DATA = 8;       // USB cable
const int DIR_LATCH = 12; // Output memory latch clock

// Directional constants
const int aFwd = 39;      // Move Forward
const int aBwd = 216;     // Move Backward
const int CW = 116;       // Left translation (L moves up, R moves down)
const int CCW = 139;      // Right translation (L moves down, R moves up)
const int Stop = 0;       // Parking variable
const int LeftFwd = 36;   // Upper Left Move
const int RightFwd = 3;   // Upper Right Move
const int RightBwd = 80;  // Lower Left Move (right backward)
const int LeftBwd = 136;  // Lower Right Move

// extra movement options: obsolete due to two motor design.
// const int Right_Rotate = 149;      // Rotate Right
// const int Left_Rotate = 106;       // Rotate Left
// const int Drift_Left = 20;         // Drift on Left
// const int Drift_Right = 10;        // Drift on Right

// Set the default speed between 1 and 255
int SpeedL = 255;
int SpeedR = 255;

void setup() {
  // Serial communication
  Serial.begin(9600);
  espSerial.begin(9600);

  // Attach servo to pin
  LRServo.attach(LRServoPin);
  UDServo.attach(UDServoPin);

  // Define laser receiver and emitters
  pinMode(laserReceiverLeftPin, INPUT);
  pinMode(laserReceiverRightPin, INPUT);
  pinMode(laserTurret, OUTPUT);

  // Define motor pins
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWMR, OUTPUT);
  pinMode(PWML, OUTPUT);

  // Initial debug
  Serial.println("Setup complete. Starting loop...");
  Serial.println("ESP32 controller integration ready...");
}

void loop() {
  if (lives > 0) {

     if (!hasInitialized) {
       initial();  // Call the initial setup function
     }

    // Check for new controller data
    checkEspController();

    // Process controller data if new data is available
    processEspData();

  } else if (lives == 0) {
    Serial.println("No lives left. Game Over!");
    digitalWrite(laserTurret, LOW);
    Motor(Stop, 0, 0);
    lives = -1;

  } else {
    // Effectively stop Arduino and loop. Except an if statement where if '5' is put in serial terminal, the lives refill
    if (Serial.available() > 0) {
      int command = Serial.parseInt(); // Read the command from Serial
      if (command == 5) {
        lives = startingLives; // Refill lives to 5
        Serial.println("Lives refilled to 5!");
      }
    }
  }
}
