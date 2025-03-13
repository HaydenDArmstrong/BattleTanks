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
        ESP32 TX (GPIO 17) -> Arduino pin A2 (RX)
        ESP32 RX (GPIO 16) -> Arduino pin A3 (TX)
        Common GND
*/

#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial btSerial(A1,A0);
 char btCommand;



// ESP32 Controller communication
SoftwareSerial espSerial(A2, A3); // RX, TX
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
// const int laserReceiverLeftPin = A25;
// const int laserReceiverRightPin = A26;
const int laserTurret = A5;

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





void setup() {
  // Start serial communication
  Serial.begin(9600);
  espSerial.begin(9600);

  // Attach the servo to the pin
  headServo.attach(headServoPin);

  // Define laser receiver
  // pinMode(laserReceiverLeftPin, INPUT);
  // pinMode(laserReceiverRightPin, INPUT);
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