void processCommand(char command) {
  switch (command) {
    case 'w': // Move Forward
      Motor(Move_Forward, Speed1, Speed2, Speed3, Speed4);
      Serial.println("Moving forward");
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

    // Games and special functions
    case '[': // Runs laser game
      // laserGame();
      break;
    case ']': // Restore lives
      lives = 3;
      break;
    case '`': // Display stats
      displayStats();
      break;
    case 't': // Runs init/test function
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
