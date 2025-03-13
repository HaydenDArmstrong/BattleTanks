/*
 * Arduino Uno Serial Communication Receiver
 * Receives messages from ESP32 and prints to Serial Monitor
 * 
 * Connections:
 * Arduino RX (pin 0) <- ESP32 TX (GPIO 17)
 * Arduino TX (pin 1) <- ESP32 RX (GPIO 16)
 * Common GND between both boards
 */

#include <SoftwareSerial.h>

// Create a software serial port
// We use pins 2 and 3 because pins 0 and 1 are used for USB communication
SoftwareSerial espSerial(2, 3); // RX, TX

void setup() {
  // Initialize Serial Monitor 
  Serial.begin(9600);
  Serial.println("Arduino Uno Serial Receiver");
  
  // Initialize software serial for ESP32 communication
  espSerial.begin(9600);
  
  delay(1000); // Give time for serial initialization
}

void loop() {
  // Check if data is available from ESP32
  if (espSerial.available()) {
    // Read the incoming message
    String message = espSerial.readStringUntil('\n');
    
    // Print the message to Serial Monitor
    Serial.print("Message from ESP32: ");
    Serial.println(message);
  }
}
