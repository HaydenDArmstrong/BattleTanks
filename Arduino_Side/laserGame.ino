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


 void laserGame() {
    while (lives > 0) {
      // int laserLeft = digitalRead(laserReceiverLeftPin);
      // int laserRight = digitalRead(laserReceiverRightPin);
      // digitalWrite(laserTurret, HIGH); // Ensure turret is on

      // if (laserLeft == HIGH || laserRight == HIGH) {
      //   Serial.println("Laser detected!");
      //   Motor(Stop, 0, 0, 0, 0); // Stop robot
      //   flashLaserTurret();     // Flash turret to indicate hit
      //   delay(pauseAfterHit);   // Pause for effect
      //   laserLife();            // Decrement lives
      // } else {
      //   Serial.println("No laser detected.");
      //   delay(100);
      //   moveServoHead(); // Execute servo movement/   
            }
 }
