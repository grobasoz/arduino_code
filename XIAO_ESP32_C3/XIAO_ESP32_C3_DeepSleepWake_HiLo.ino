#include <Arduino.h>

RTC_DATA_ATTR int bootCount = 0;

#define WAKE_PIN_0 GPIO_NUM_2  // D0
#define WAKE_PIN_1 GPIO_NUM_4  // D2
#define WAKE_PIN_2 GPIO_NUM_5  // D3

#define WAKE_PIN_LOW_MASK ((1 << WAKE_PIN_0) + (1 << WAKE_PIN_2))
#define WAKE_PIN_HIGH_MASK ((1 << WAKE_PIN_1))

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

bool noSleepLow = false;
bool noSleepHigh = false;

void setup() {
  Serial.begin(115200);
  delay(1000);  //Take some time to open up the Serial Monitor

  if (esp_sleep_is_valid_wakeup_gpio(WAKE_PIN_0)) {
    Serial.println("Wake Pin 0 OK");
  }
  if (esp_sleep_is_valid_wakeup_gpio(WAKE_PIN_1)) {
    Serial.println("Wake Pin 1 OK");
  }
  if (esp_sleep_is_valid_wakeup_gpio(WAKE_PIN_2)) {
    Serial.println("Wake Pin 2 OK");
  }
  delay(2000);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  pinMode(WAKE_PIN_0, INPUT);
  pinMode(WAKE_PIN_1, INPUT);
  pinMode(WAKE_PIN_2, INPUT);

  if (esp_deep_sleep_enable_gpio_wakeup(WAKE_PIN_LOW_MASK, ESP_GPIO_WAKEUP_GPIO_LOW) == ESP_OK) {
    Serial.println("Button Wake Low Enabled, Mask = " + String(WAKE_PIN_LOW_MASK));
  } else {
    Serial.println("Button Wake Error");
    noSleepLow = true;
  }

  if(noSleepLow)
  {
    Serial.println("No Sleep Low!");
    delay(2000);
  }

  if (esp_deep_sleep_enable_gpio_wakeup(WAKE_PIN_HIGH_MASK, ESP_GPIO_WAKEUP_GPIO_HIGH) == ESP_OK) {
    Serial.println("Button Wake High Enabled, Mask = " + String(WAKE_PIN_HIGH_MASK));
  } else {
    Serial.println("Button Wake Error");
    noSleepHigh = true;
  }

  if(noSleepHigh)
  {
    Serial.println("No Sleep High!");
    delay(2000);
  }
}

bool state = false;
bool oldState = false;
uint8_t tick_count = 0;

void loop() {
  //This is not going to be called
  delay(1000);

  if (digitalRead(WAKE_PIN_0) == LOW) {
    Serial.println("Wake Pin 0 Low!");
  } else {
    Serial.println("Wake Pin 0 High!");
  }

  if (digitalRead(WAKE_PIN_1) == LOW) {
    Serial.println("Wake Pin 1 Low!");
  } else {
    Serial.println("Wake Pin 1 High!");
  }

  if (digitalRead(WAKE_PIN_2) == LOW) {
    Serial.println("Wake Pin 2 Low!");
  } else {
    Serial.println("Wake Pin 2 High!");
  }

  Serial.println("Tick! " + String(tick_count));

  if (tick_count++ > 5) {
    if ((noSleepLow == false) && (noSleepHigh == false))  {
      //Go to sleep now
      Serial.println("Going to sleep now");
      esp_deep_sleep_start();
      // esp_deep_sleep(5000000);  // 1 Sec wake
    }
    Serial.println("This will never be printed");
  }
}