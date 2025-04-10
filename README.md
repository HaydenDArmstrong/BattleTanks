# LaserTanks

## Battle Tank Fizz File
made a diagram in Fritzing to show how Battle tank wires.

## ControllerOutputToArduino
- Code to be uploaded to the **ESP32**.
- The ESP32 connects to the **controller** and outputs controller information, including:
  - Button press state
  - D-pad press state
  - Thumbstick axis data
- This is accomplished mainly through the **BluePad32** library.
- Data is sent to the **Arduino** through the transmission pin out and recieving pin in.

## ArduinoSide
- Code to be uploaded to the **Arduino**.
- The tank uses an **L293D motor** to power the **motors, laser emitter, and laser receiver**.
- This code includes:
  - Reading ESP32 controller information and acting on it (button functions, thumbstick mapping).
  - Initialization function to validate basic tank functionality.
  - Future game functionality development.

- The **PWMServo** library is used instead of the normal **Servo** library to prevent jitter when both the **servo and ESP32** are active. This issue arises due to **SoftwareSerial**, which uses interrupts whenever a character is sent, affecting servo motion.

- The **L293D motor shield** is mounted on the board and serves as the base for GPIO connections. The corresponding pin connections are as follows:

## Servo Pins
- **SERVO1** – Up/Down turret servo motor  
- **SERVO2** – Left/Right turret servo motor  

## DC Motor Wiring (Tread Movement)
- **Left Motor** → Connect to **M1**  
- **Right Motor** → Connect to **M2**  

### Left Motor (Nearest to USB Port)
- **Black (GND)** → First pin (ground)  
- **Red (PWR)** → Second pin (power)  
- **Middle GND slot** remains empty  

### Right Motor
- **Black (GND)** → Leftmost pin (next to middle GND)  
- **Red (PWR)** → Rightmost pin  


## Misc Wiring (Analog Pins)
- **A0**: *Laser Receiver?*  
- **A1**: *Laser Receiver?*  
- **A2**: *(Unused)*  
- **A3**: Laser emitter pin  
- **A4**: ESP32 Receiving IN (connect to ESP32 pin **17**)  
- **A5**: ESP32 Transmit OUT (connect to ESP32 pin **16**)  

## ESP32 Notes
- **ESP32 Pin 17** → **TX (Transmits)**  
- **ESP32 Pin 16** → **RX (Receives)**  
- **TX of ESP32 must connect to RX of the board and vice versa.**

### Power Requirements
- **ESP32 requires a separate and adequate power source** to avoid voltage issues.
- MAKE SURE THE LASER RECIEVER HAS THE PHOTORESISTOR IN THE RIGHT DIRECTION (NUB FACING TOWARDS PINS) OR SHIT GONNA BURN.

### Grounding
- **ESP32 must share a common ground with the board. Make sure to properly ground all the components.**

## TODO
- **Correct Servo Movement:** Ensure smooth and delay-free movement of the tank servos from controller input. *(The tank servos are controlled via the right thumbstick.)*
- **Remove Archaic Functions and Code:** Delete unused code, such as KB and BT functions, prototyping code, etc.
- **Develop an Advanced Laser Game:** The current laser game is basic and lacks necessary features.
- **Add a Life Screen:** Display remaining lives before the game ends. This is challenging due to the limited pinout of the **ESP32** and **motor shield**. However, with one extra pin available (if only two laser receivers are used), a small digital screen might be implemented using a single signal pin.
