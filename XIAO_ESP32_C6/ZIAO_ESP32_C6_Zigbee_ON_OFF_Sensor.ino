#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee binary sensor device configuration */
#define BINARY_DEVICE_ENDPOINT_NUMBER 1

uint8_t button = GPIO_NUM_2;      // D2
uint8_t binaryPin = GPIO_NUM_21;  // D3

ZigbeeBinary zbBinarySensor = ZigbeeBinary(BINARY_DEVICE_ENDPOINT_NUMBER);

bool binaryStatus = false;
bool binaryWasLow = false;

void check_binary_input() {
  if (digitalRead(binaryPin) == LOW && !binaryWasLow) {  // Sensor activated
    // Debounce handling
    delay(100);
    if (digitalRead(binaryPin) == LOW) {
      binaryStatus = 0;
      zbBinarySensor.setBinaryInput(binaryStatus);
      zbBinarySensor.reportBinaryInput();
      binaryWasLow = true;
    }
  }

  if (digitalRead(binaryPin) == HIGH && binaryWasLow) {  // Sensor activated
    // Debounce handling
    delay(100);
    if (digitalRead(binaryPin) == HIGH) {
      binaryStatus = 1;
      zbBinarySensor.setBinaryInput(binaryStatus);
      zbBinarySensor.reportBinaryInput();
      binaryWasLow = false;  // Allow more low levels
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  // Init button switch
  pinMode(button, INPUT_PULLUP);
  pinMode(binaryPin, INPUT_PULLUP);

  // Optional: set Zigbee device name and model
  zbBinarySensor.setManufacturerAndModel("KaSaJa", "ZigbeeOnOffSensor");

  // Set up binary zone armed input (Security)
  zbBinarySensor.addBinaryInput();
  zbBinarySensor.setBinaryInputApplication(BINARY_INPUT_APPLICATION_TYPE_HVAC_FAN_STATUS);
  zbBinarySensor.setBinaryInputDescription("Binary Sensor");

  // Add endpoints to Zigbee Core
  Zigbee.addEndpoint(&zbBinarySensor);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Connected");
}

void loop() {
  // Checking button for factory reset and reporting
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }
  check_binary_input();
  delay(100);
}
