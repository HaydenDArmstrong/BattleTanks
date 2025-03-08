#include <Bluepad32.h>
#include <HardwareSerial.h>

// Create serial port for communication with Arduino
HardwareSerial SerialPort(2); // Use hardware serial port 2

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      
      // Send controller connection message to Arduino
      SerialPort.println("Controller connected!");
      
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      
      // Send controller disconnection message to Arduino
      SerialPort.println("Controller disconnected!");
      
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

// Process gamepad and send button states to Arduino
void processGamepad(ControllerPtr ctl) {
  // Store the previous button state to detect changes
  static uint16_t prevButtons = 0;
  static uint8_t prevDpad = 0;
  static int16_t prevAxisX = 0;
  static int16_t prevAxisY = 0;
  
  // Check if any button state has changed
  if (ctl->buttons() != prevButtons || ctl->dpad() != prevDpad || 
      abs(ctl->axisX() - prevAxisX) > 25 || abs(ctl->axisY() - prevAxisY) > 25) {
    
    // Send button state to Arduino
    SerialPort.print("BTN:");
    SerialPort.print(ctl->buttons(), HEX);
    SerialPort.print(",DPAD:");
    SerialPort.print(ctl->dpad(), HEX);
    SerialPort.print(",LX:");
    SerialPort.print(ctl->axisX());
    SerialPort.print(",LY:");
    SerialPort.println(ctl->axisY());
    
    // Also print to local serial for debugging
    Serial.print("Sent to Arduino - BTN:");
    Serial.print(ctl->buttons(), HEX);
    Serial.print(",DPAD:");
    Serial.print(ctl->dpad(), HEX);
    Serial.print(",LX:");
    Serial.print(ctl->axisX());
    Serial.print(",LY:");
    Serial.println(ctl->axisY());
    
    // Update previous states
    prevButtons = ctl->buttons();
    prevDpad = ctl->dpad();
    prevAxisX = ctl->axisX();
    prevAxisY = ctl->axisY();
  }
  
  // Process specific buttons (for your reference)
  
  // PS4 X button (0x0001)
  if ((ctl->buttons() & 0x0001) && !(prevButtons & 0x0001)) {
    SerialPort.println("X button pressed");
  }
  
  // PS4 Circle button (0x0002)
  if ((ctl->buttons() & 0x0002) && !(prevButtons & 0x0002)) {
    SerialPort.println("Circle button pressed");
  }
  
  // PS4 Square button (0x0004)
  if ((ctl->buttons() & 0x0004) && !(prevButtons & 0x0004)) {
    SerialPort.println("Square button pressed");
  }
  
  // PS4 Triangle button (0x0008)
  if ((ctl->buttons() & 0x0008) && !(prevButtons & 0x0008)) {
    SerialPort.println("Triangle button pressed");
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
         processGamepad(myController);
      }
      else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

// Arduino setup function
void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  
  // Initialize Serial2 for Arduino communication with RX on GPIO 16, TX on GPIO 17
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("ESP32 Controller to Arduino communication initialized");
  
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  
  // Send initial message to Arduino
  SerialPort.println("ESP32 controller system ready!");
}

// Arduino loop function
void loop() {
  // This call fetches all the controllers' data
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

  delay(20); // Shorter delay for more responsive controls
}
