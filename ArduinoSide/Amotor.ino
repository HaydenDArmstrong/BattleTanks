void Motor(int Dir, int SpeedL, int SpeedR) {
   analogWrite(PWML, SpeedL);  // Motor speed regulation from consts speedL and speedR in arduinoside.ino
  analogWrite(PWMR, SpeedR);  

  digitalWrite(DIR_LATCH, LOW);            // DIR_LATCH sets the low level and writes the direction of motion in preparation
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Write Dir motion direction value
  digitalWrite(DIR_LATCH, HIGH);           // DIR_LATCH sets the high level and outputs the direction of motion
}
