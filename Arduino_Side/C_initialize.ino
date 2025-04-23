void initial() {
  Serial.println("Initializing system components...");
  delay(100);

  // Test servo
  Serial.print("Testing servo... ");
  LRServo.attach(LRServoPin);
  LRServo.write(90); // Move to center position
  delay(500);
  LRServo.write(0); // Move to left position
  delay(500);
  LRServo.write(180); // Move to right position
  delay(500);
  LRServo.write(90); // Reset to center
  Serial.println("Servo OK.");

  Serial.print("Testing servo up down... ");
  UDServo.attach(UDServoPin);
  UDServo.write(90); // Move to center position
  delay(500);
  UDServo.write(30); // Move to left position
  delay(500);
  UDServo.write(180); // Move to right position
  delay(500);
  UDServo.write(90); // Reset to center
  Serial.println("Servo OK.");

  // // Test laser detector
  // Serial.print("Testing laser receiver... ");
  // int laserDetected = digitalRead(laserReceiverLeftPin);
  // if (laserDetected == LOW) {
  //   Serial.println("Laser Receiver OK. Laser detected");
  // } else if (laserDetected == HIGH) {
  //   Serial.println("Laser Receiver OK. No Laser Detected");
  // } else {
  //   Serial.println("Laser Receiver FAILED");
  // }

  // Test motors
  Serial.print("Testing motors... ");
  Motor(aFwd, 150, 150); // Run motors at half speed forward
  delay(500);
  Motor(Stop, 0, 0);
  Serial.println("Motors OK.");

   hasInitialized = true; // Mark system as initialized


  for (int i = 0; i < 50; i++) {
    Serial.print('-');
  }
  Serial.println(); 
}


void moveServoHead() {
  LRServo.write(90); // Move to center position
  delay(1000);
  LRServo.write(45); // Move to left position
  delay(1000);
  LRServo.write(135); // Move to right position
  delay(1000);
  LRServo.write(90); // Reset to center
  delay(1000);
}
