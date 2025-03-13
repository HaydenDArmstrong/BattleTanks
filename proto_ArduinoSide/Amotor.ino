void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4) {
  analogWrite(PWM2A, Speed1);  // Motor PWM speed regulation
  analogWrite(PWM2B, Speed2);  // Motor PWM speed regulation
  analogWrite(PWM0A, Speed3);  // Motor PWM speed regulation
  analogWrite(PWM0B, Speed4);  // Motor PWM speed regulation

  digitalWrite(DIR_LATCH, LOW);            // DIR_LATCH sets the low level and writes the direction of motion in preparation
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir);  // Write Dir motion direction value
  digitalWrite(DIR_LATCH, HIGH);           // DIR_LATCH sets the high level and outputs the direction of motion
}