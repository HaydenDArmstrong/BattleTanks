/*
   
    Laser Reciever Pins:
        VCC: +5
        GND: GND
        OUT: A5 , a4
      *If I can find more ff duponts, BT will be reinstated*

      laser emitter out a3

    Bluetooth Pins:
        VCC: +5
        GND: GND
        TXD: A1
        RXD: A0
  
    Servo "Head" (Ultrasonic Rotator) Pin
        VCC: +5
        GND: GND
        SIGNAL: SERVO1 / PIN 10


      could also utilize laser emitter, but again... not enough ff duponts.
*/

#include <Servo.h>
#include <SoftwareSerial.h>

// bt globals

 SoftwareSerial btSerial(A1,A0);
 char btCommand;

//initialized function: Makes sure all parts work as they should
bool hasInitialized = false;

//player "Lives"
int lives = 3;

//servo globals
Servo headServo;
const int headServoPin = 10; // Pin for "Servo 1" on the generic motor shield


//laser reciever globals
const int laserReceiverLeftPin = A5;
const int laserReceiverRightPin = A4;
const int laserTurret = A3;

//motor sensor globals
const int PWM2A = 11;      // M1 motor
const int PWM2B = 3;       // M2 motor  '-pol/
const int PWM0A = 6;       // M3 motor 
const int PWM0B = 5;       // M4 motor
const int DIR_CLK = 4;     // Data input clock line
const int DIR_EN = 7;      // Enable pins
const int DATA = 8;        // USB cable
const int DIR_LATCH = 12;  // Output memory latch clock


//directional constants
const int Move_Forward = 39;    //Move Forward
const int Move_Backward = 216;      //Move Backward
const int Left_Move= 116;      //Left translation
const int Right_Move = 139;     //Right translation
const int Right_Rotate = 149;     //Rotate Left
const int Left_Rotate = 106;     //Rotate Left
const int Stop = 0;        //Parking variable
const int Upper_Left_Move = 36;    //Upper Left Move
const int Upper_Right_Move = 3;    //Upper Right Move
const int Lower_Left_Move = 80;    //Lower Left Move
const int Lower_Right_Move = 136;    //Lower Right Move
const int Drift_Left = 20;    //Drift on Left
const int Drift_Right = 10;    //Drift on Right
//Set the default speed between 1 and 255

int Speed1 = 255;
int Speed2 = 255;
int Speed3 = 255;
int Speed4 = 255;



void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
  analogWrite(PWM2A, Speed1);  //Motor PWM speed regulation
  analogWrite(PWM2B, Speed2);  //Motor PWM speed regulation
  analogWrite(PWM0A, Speed3);  //Motor PWM speed regulation
  analogWrite(PWM0B, Speed4);  //Motor PWM speed regulation

  digitalWrite(DIR_LATCH, LOW);            //DIR_LATCH sets the low level and writes the direction of motion in preparation
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  //Write Dir motion direction value
  digitalWrite(DIR_LATCH, HIGH);           //DIR_LATCH sets the high level and outputs the direction of motion
}



void setup() {
  // Start serial communication
   btSerial.begin(9600); 
  Serial.begin(9600);

  // Attach the servo to the pin
  headServo.attach(headServoPin);

  //define laser reciever
  pinMode(laserReceiverLeftPin, INPUT);
  pinMode(laserReceiverRightPin, INPUT);

  pinMode(laserTurret, OUTPUT);


  //define motor pins
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
}



void initial()
{
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

  

  //test laser detector
  Serial.print("Testing laser receiver... ");
  int laserDetected = digitalRead(laserReceiverLeftPin);
  if(laserDetected == LOW)
  {
    Serial.println("Laser Receiver OK. Laser detected");
  }
  else if (laserDetected == HIGH)
  {
    Serial.println("Laser Receiver OK. No Laser Detected");
  }
  else
  {
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
  Serial.println();    // Move to the next line

}

void displayStats() {

  // Get and display the current laser status
  int laserDetected = digitalRead(laserReceiverLeftPin || laserReceiverRightPin );
  Serial.println(laserDetected);
  if (laserDetected == LOW) {
    Serial.println("Laser detected: Yes");
  } else {
    Serial.println("Laser detected: No");
  }

  // Print a separator line
  for (int i = 0; i < 50; i++) {
    Serial.print('-');
  }
  Serial.println();
}

void laserLife()
{
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

      //games and special functions

    case '[': //runs laser game
      laserGame();
        break;
    case ']': //restore lives
      lives = 3;
      break;
    case '`':
      displayStats();
        break;
    case 't': // runs init/test function
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


void loop() {

  //  if (!hasInitialized) {
  //    initial();  // Call the initial setup function
  //    hasInitialized = true;  // Mark as initialized
  //  }



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


}
