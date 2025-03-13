# arduino_code
Arduino Test Code

## Deep Sleep Keypad
The columns are on RTC GPIO’s and with a sleep enable setting of ESP_GPIO_WAKEUP_GPIO_LOW, the columns are pulled high (internally) when the esp_deep_sleep_enable_gpio_wakeup function is called.
Multiple GPIO can wake the ESP32C3 by setting the appropriate bit mask (COL_PIN_MASK).

The rows’ state needs to be retained (LOW) during deep sleep and released at wake for the keypad library to work. When a button is pressed the associated (HIGH) column is pulled low for the wakeup.

Since the serial disconnects during deep sleep it’s a little tricky to detect the key presses using print statements.

There is a 5 second delay before sleep to allow programming - this is reset every key press so allows the device to be detected for programming by pressing a key repeatedly.

The keypad is connected directly to the XIAO ESP32C3 - no external components.

Tested with this keypad.

![LC_Keypad](https://github.com/user-attachments/assets/1f583514-181d-4db8-90d8-bae9ea7c5f31)
