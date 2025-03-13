#include "Adafruit_Keypad.h"

#define USE_DEEP_SLEEP
#define SLEEP_TIMEOUT  500  // 10mS intervals

RTC_DATA_ATTR int bootCount = 0;

#define COL_PIN_0 GPIO_NUM_2  // D0
#define COL_PIN_1 GPIO_NUM_3  // D1
#define COL_PIN_2 GPIO_NUM_5  // D3

#define ROW_PIN_0 GPIO_NUM_4   // D2
#define ROW_PIN_1 GPIO_NUM_6   // D4
#define ROW_PIN_2 GPIO_NUM_7   // D5
#define ROW_PIN_3 GPIO_NUM_21  // D6

#define COL_PIN_MASK ((1 << COL_PIN_0) + (1 << COL_PIN_1) + (1 << COL_PIN_2))

const byte ROWS = 4;  // Rows
const byte COLS = 3;  // Columns

// Define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { ROW_PIN_3, ROW_PIN_2, ROW_PIN_1, ROW_PIN_0 };  // Connect to the row pinouts of the keypad
byte colPins[COLS] = { COL_PIN_2, COL_PIN_1, COL_PIN_0 };             // Connect to the column pinouts of the keypad

// Initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

static uint16_t sleep_count = 0;  // Sleep after x seconds

#ifdef USE_DEEP_SLEEP
static bool sleep_permitted = true;  // False if error with sleep setup

// Prepare GPIO for sleep
void prep_for_wake(void) {
  // Allow cols to be pulled low when in deep sleep
  digitalWrite(ROW_PIN_0, HIGH);
  digitalWrite(ROW_PIN_1, HIGH);
  digitalWrite(ROW_PIN_2, HIGH);
  digitalWrite(ROW_PIN_3, HIGH);

  // Release outputs held during deep sleep
  gpio_hold_dis(ROW_PIN_0);
  gpio_hold_dis(ROW_PIN_1);
  gpio_hold_dis(ROW_PIN_2);
  gpio_hold_dis(ROW_PIN_3);
}

// Prepare GPIO for sleep
void prep_for_sleep(void) {
  pinMode(COL_PIN_0, INPUT);
  pinMode(COL_PIN_1, INPUT);
  pinMode(COL_PIN_2, INPUT);

  // Sleep - Rows are outputs
  pinMode(ROW_PIN_0, OUTPUT);
  pinMode(ROW_PIN_1, OUTPUT);
  pinMode(ROW_PIN_2, OUTPUT);
  pinMode(ROW_PIN_3, OUTPUT);

  // Allow cols to be pulled low when in deep sleep
  digitalWrite(ROW_PIN_0, LOW);
  digitalWrite(ROW_PIN_1, LOW);
  digitalWrite(ROW_PIN_2, LOW);
  digitalWrite(ROW_PIN_3, LOW);

  // Maintain outputs during deep sleep
  gpio_hold_en(ROW_PIN_0);
  gpio_hold_en(ROW_PIN_1);
  gpio_hold_en(ROW_PIN_2);
  gpio_hold_en(ROW_PIN_3);

  if (esp_deep_sleep_enable_gpio_wakeup(COL_PIN_MASK, ESP_GPIO_WAKEUP_GPIO_LOW) == ESP_OK) {
    Serial.println("Keypad Wake Mask : 0x" + String(COL_PIN_MASK, HEX));
  } else {
    Serial.println("Keypad Wake Error");
    sleep_permitted = false;
  }

  Serial.println("Deep Sleep!");
}

void do_deep_sleep() {
  prep_for_sleep();
  Serial.println("Going to sleep now");
  gpio_deep_sleep_hold_en();
  esp_deep_sleep_start();
}
#endif  // USE_DEEP_SLEEP

void setup() {
  Serial.begin(115200);

#ifdef USE_DEEP_SLEEP
  // Before starting the keypad, release held rows.
  prep_for_wake();
#endif //USE_DEEP_SLEEP

  customKeypad.begin();

  // See if any keys are still "pressed" - caused wake?
  customKeypad.tick();

  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    Serial.print((char)e.bit.KEY);
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.println(" pressed, wake key");
    } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      Serial.println(" released, wake key");
    }
  }

  Serial.println("Starting!");
}

void loop() {
  // Fast loop!
  customKeypad.tick();

  while (customKeypad.available()) {
    sleep_count = 0;  // Reset sleep count
    keypadEvent e = customKeypad.read();
    Serial.print((char)e.bit.KEY);
    if (e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    else if (e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
  }

  if (sleep_count++ > SLEEP_TIMEOUT) {
#ifdef USE_DEEP_SLEEP
    if (sleep_permitted) {
      do_deep_sleep();
    }
#endif
    Serial.println("Sleep!");
    delay(5000);  // Testing
    sleep_count = 0;
  }

  delay(10);
}