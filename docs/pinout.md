# Pin Mapping

Default pin map for ESP32 DevKit (adjust as needed):

## RFID (MFRC522, SPI)
- SCK: GPIO 18
- MISO: GPIO 19
- MOSI: GPIO 23
- SS/SDA: GPIO 5
- RST: GPIO 27
- 3.3V and GND as required

## OLED (SSD1306, I2C)
- SDA: GPIO 21
- SCL: GPIO 22
- VCC/GND per module (3.3V recommended)

## PIR Sensor
- OUT: GPIO 13
- VCC: 5V or 3.3V (per module)
- GND

## Ultrasonic (HC-SR04)
- TRIG: GPIO 25
- ECHO: GPIO 26 (use divider or level shifter if 5V output)
- VCC: 5V
- GND

## Device Activation Output
- RELAY/MOSFET control: GPIO 32
- Use proper driver and flyback protection

## Optional Status LED
- LED: GPIO 2

## Notes
- Avoid GPIOs 34-39 for outputs (input-only).
- Verify SPI and I2C defaults with your board variant.
- Ensure common ground across all modules.
