


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
  // int laserDetected = digitalRead(laserReceiverLeftPin) || digitalRead(laserReceiverRightPin);
  // Serial.println(laserDetected);
  // if (laserDetected == LOW) {
  //   Serial.println("Laser detected: Yes");
  // } else {
  //   Serial.println("Laser detected: No");
  // }

  // Print lives
  Serial.print("Lives remaining: ");
  Serial.println(lives);

  // Print a separator line
  for (int i = 0; i < 50; i++) {
    Serial.print('-');
  }
  Serial.println();
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
