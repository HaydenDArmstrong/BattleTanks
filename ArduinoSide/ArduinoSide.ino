#include <PWMServo.h>
#include <SoftwareSerial.h>





// ESP32 Controller communication
SoftwareSerial espSerial(A4, A5); // RX, TX
String espData = "";
bool newEspData = false;

// Initialized function: Makes sure all parts work as they should
bool hasInitialized = false;


// Player "Lives"
int lives = 3;

// Servo globals
PWMServo LRServo;
const int LRServoPin = 10; // Pin for "Servo 1" on the generic motor shield

PWMServo UDServo;
const int UDServoPin = 9; // Pin for "Servo 1" on the generic motor shield

// Laser receiver globals
// const int laserReceiverLeftPin = A25;
// const int laserReceiverRightPin = A26;
const int laserTurret = A3;

// Motor sensor globals
const int PWML = 11;      // M1 motor left side pwm2a
const int PWMR = 5;       // M2 motor right side pwm0b
const int DIR_CLK = 4;     // Data input clock line
const int DIR_EN = 7;      // Enable pins
const int DATA = 8;        // USB cable
const int DIR_LATCH = 12;  // Output memory latch clock

// Directional constants
const int aFwd = 39;       // Move Forward
const int aBwd = 216;     // Move Backward
const int CW = 116;         // Left translation l moves up r moves down
const int CCW = 139;        // Right translation l moves down r moves up
// const int Right_Rotate = 149;      // Rotate Right
// const int Left_Rotate = 106;       // Rotate Left
const int Stop = 0;                // Parking variable
const int LeftFwd = 36;    // Upper Left Move
const int RightFwd = 3;    // Upper Right Move
const int RightBwd = 80;    // Lower Left Move right bckwd
const int LeftBwd = 136;  // Lower Right Move
// const int Drift_Left = 20;         // Drift on Left
// const int Drift_Right = 10;        // Drift on Right

// Set the default speed between 1 and 255
int SpeedL = 255;
int SpeedR = 255;





void setup() {
  // Start serial communication
  Serial.begin(9600);
  espSerial.begin(9600);

  // Attach the servo to the pin
  LRServo.attach(LRServoPin);
  UDServo.attach(UDServoPin);

  // Define laser receiver
  // pinMode(laserReceiverLeftPin, INPUT);
  // pinMode(laserReceiverRightPin, INPUT);
  pinMode(laserTurret, OUTPUT);

  // Define motor pins
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWMR, OUTPUT);
   pinMode(PWML, OUTPUT); 

  // Initial debug message
  Serial.println("Setup complete. Starting loop...");
  Serial.println("ESP32 controller integration ready!");
}




const int turretFlashOnDelay = 300;
const int turretFlashOffDelay = 700;



void loop() {

  
  // Check for controller initialization on first run
     if (!hasInitialized) {
       initial();  // Call the initial setup function
     }




  
  // Check input from ESP32 Controller
  checkEspController();
  if (newEspData) {
    processEspData();
  }
}
