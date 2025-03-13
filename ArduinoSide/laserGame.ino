// void laserLife() {
//   if (lives > 0) {
//     lives--;
//     Serial.print("Laser hit! Lives remaining: ");
//     Serial.print(lives);
//     Serial.println();

//     if (lives == 0) {
//       Serial.println("Game Over!");
//       // Add game-over logic here
//     }
//   } else {
//     Serial.println("No lives left. Game Over!");
//   }
// }

// // Constants for delays
// const int turretFlashOnDelay = 300;
// const int turretFlashOffDelay = 700;
// const int pauseAfterHit = 2000;

// void flashLaserTurret() {
//   for (int i = 0; i < 2; ++i) {
//     digitalWrite(laserTurret, LOW);
//     delay(turretFlashOffDelay);
//     digitalWrite(laserTurret, HIGH);
//     delay(turretFlashOnDelay);
//   }
// }

// void laserGame() {
//   while (lives > 0) {
//     int laserLeft = digitalRead(laserReceiverLeftPin);
//     int laserRight = digitalRead(laserReceiverRightPin);
//     digitalWrite(laserTurret, HIGH); // Ensure turret is on

//     if (laserLeft == HIGH || laserRight == HIGH) {
//       Serial.println("Laser detected!");
//       Motor(Stop, 0, 0, 0, 0); // Stop robot
//       flashLaserTurret();     // Flash turret to indicate hit
//       delay(pauseAfterHit);   // Pause for effect
//       laserLife();            // Decrement lives
//     } else {
//       Serial.println("No laser detected.");
//       delay(100);
//        moveServoHead(); // Execute servo movement
//     }

//     // Check for command to end game
//     if (Serial.available() > 0) {
//       char cmd = Serial.read();
//       if (cmd == ']' || cmd == 'S') {
//         break; // Exit game
//       }
//     }

//     // Check ESP32 controller
//     checkEspController();
//     if (newEspData) {
//       processEspData();
//       if (espData.indexOf("]") >= 0 || espData.indexOf("S") >= 0) {
//         break; // Exit game
//       }
//     }
//   }

//   // Game over logic
//   Motor(Stop, 0, 0, 0, 0);        // Ensure the robot is stopped
//   digitalWrite(laserTurret, LOW); // Turn off turret
//   Serial.println("Robot has died. Game Over!");
// }